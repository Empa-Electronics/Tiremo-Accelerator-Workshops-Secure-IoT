# ABOV TLS Sertifika Kurulumu — ESP32 (WiFi + MQTT)

Bu doküman, ABOV MCU üzerindeki TLS sertifikalarının ESP32 modülüne bir kez yüklenip ardından MCU flash'ından kalıcı olarak silinmesi akışını ve ilgili kodu açıklar.

---

## Problem ve çözüm özeti

**İstenen davranış:**

1. Firmware ile gelen sertifikalar varsa → ESP32'ye yükle.
2. Kullanıcı butona 3+ saniye basınca → sertifikaları ABOV flash'ından **kalıcı** sil.
3. Reset sonrası ABOV'da sertifika kalmamalı → doğrudan broker bağlantısına geç (ESP32'deki sertifikalar kullanılır).
4. IDE ile yeni firmware yüklenince → sertifikalar tekrar gelsin, kurulum yeniden yapılabilsin.

**Eski sorunlar:**

- Sadece RAM siliniyordu → reset'te sertifikalar geri geliyordu.
- Code-flash'taki `.rodata` bölgesini silmek MCU'yu kilitleyebiliyordu.
- `CONFIG_HAL_FMC` kapalıyken veya MCUbrew generate sonrası linker hatası oluşuyordu.

**Uygulanan çözüm:**

- Sertifikalar izole `.cert_flash` linker bölümüne alındı.
- Silme, HAL FMC modülüne bağımlı olmadan doğrudan CFMC register erişimiyle yapılıyor (`RAMFUNC`).
- Yükleme boot'ta `MqttCertProv_Run()` ile yapılıyor; `MQTT_ConnectBroker()` içinde tekrar yüklenmiyor.

---

## Değiştirilen / eklenen dosyalar

| Dosya | Ne yapıldı |
|-------|------------|
| `certificates/mqtt_rootCA.inc` | `__attribute__((section(".cert_flash")))` eklendi |
| `certificates/mqtt_certificate.inc` | Aynı |
| `certificates/mqtt_private.inc` | Aynı |
| `Framework/.../gcc_a34g43x.ld` | `.cert_flash` section (512 B hizalı) |
| `Libraries/cert_Lib/mqtt_certs.c` | Okuma, silme, log API |
| `Libraries/cert_Lib/mqtt_certs.h` | Public fonksiyon bildirimleri |
| `Libraries/cert_Lib/mqtt_cert_provision.c` | ESP32 yükleme + buton + silme |
| `Libraries/cert_Lib/mqtt_cert_provision.h` | `MqttCertProv_Run()` |
| `Libraries/MQTT_Library/mqtt_core.c` | Cert upload adımı atlandı |
| `prv_user_code.c` | Boot akışına cert prov eklendi |
| `config/app_config.h` | `APP_BTN_LONG_PRESS_MS` (3000 ms) |

---

## 1. Sertifika kaynağı — `.inc` dosyaları

PEM verileri artık normal `.rodata` yerine özel section'da:

```c
// certificates/mqtt_rootCA.inc
static const char __attribute__((section(".cert_flash"))) s_mqtt_root_ca[] =
"-----BEGIN CERTIFICATE-----\n"
...
"-----END CERTIFICATE-----\n";
```

Aynı pattern `mqtt_certificate.inc` ve `mqtt_private.inc` için de geçerli.

---

## 2. Linker — izole flash bölgesi

`gcc_a34g43x.ld` içine `.text`'ten sonra, firmware kodundan ayrı bir bölüm eklendi:

```ld
.cert_flash ALIGN(512) :
{
    __cert_flash_start__ = .;
    KEEP(*(.cert_flash))
    . = ALIGN(512);
    __cert_flash_end__ = .;
} > FLASH
```

- **512 byte hizalama:** ABOV code-flash sayfa boyutu (`STARTUP_PAGE_SIZE = 0x200`).
- **`__cert_flash_start__` / `__cert_flash_end__`:** Runtime silme bu aralığı kullanır.
- Sadece bu sayfalar silinir; `.text` ve diğer `.rodata` dokunulmaz.

---

## 3. `mqtt_certs.c` — okuma ve silme

### 3.1 Geçerlilik kontrolü

Flash silindikten sonra tüm byte'lar `0xFF` olur. PEM geçerliliği ilk byte ile kontrol edilir:

```c
static uint8_t cert_pem_valid(const char *p)
{
    return ((uint8_t)p[0] != 0xFFU && p[0] == '-') ? 1U : 0U;
}
```

`0xFF` → silinmiş. `0x2D` (`'-'`) → `-----BEGIN CERTIFICATE-----` başlangıcı.

### 3.2 Flash'tan RAM'e yükleme

```c
static void mqtt_certs_load(void)
{
    for (i = 0U; i < 3U; i++)
    {
        const char *src = s_slots[i].flash_src;  // .cert_flash pointer

        if (cert_pem_valid(src) == 0U) { /* len = 0 */ continue; }

        memcpy(s_slots[i].buf, src, srcLen);
        s_slots[i].len = srcLen;
    }
}
```

Slot tablosu: `mqtt_rootCA`, `mqtt_certificate`, `mqtt_private_key`.

### 3.3 `MqttCerts_HasEmbedded()`

Üç sertifikanın da RAM'de dolu olup olmadığını döner. Boot'ta prov kararı buradan verilir:

```c
uint8_t MqttCerts_HasEmbedded(void)
{
    mqtt_certs_load();
    return (s_slots[0].len > 0U && s_slots[1].len > 0U && s_slots[2].len > 0U)
           ? 1U : 0U;
}
```

### 3.4 Flash silme — `MqttCerts_EraseFlash()`

HAL `CONFIG_HAL_FMC` **gerekmez**. Fonksiyon RAM'den çalışır (`RAMFUNC` → `.code_section_on_ram`):

```c
RAMFUNC static int cert_fmc_erase_page(uint32_t page_addr)
{
    CFMC_Type *cfmc = CFMC;
    cfmc->CONF |= CFMC_CONF_WTEN_Msk;       // write enable
    cfmc->FLSKEY = 0x01234567;              // PE unlock sequence
    cfmc->FLSKEY = 0x12345678;
    cfmc->FLSKEY = 0x23456789;
    cfmc->CTRL |= CFMC_CTRL_PERS_Msk;       // 512 B page erase
    *(volatile uint32_t *)addr = 0U;        // erase trigger
    /* busy wait + lock */
}
```

`MqttCerts_EraseFlash()` tüm `.cert_flash` aralığını 512'şer byte sayfa sayfa siler, RAM buffer'ları da temizler.

### 3.5 Log fonksiyonları

| Fonksiyon | Ne zaman |
|-----------|----------|
| `MqttCerts_LogStorage(phase)` | Prov öncesi/sonrası — RAM + FW adres, byte0, PEM başlığı |
| `MqttCerts_LogFlashIfPresent()` | `[CERT] ABOV'da sertifika yok` sonrası — flash'ta veri varsa basar, `0xFF` ise **sessiz** |

`printf` formatı: `%08X` (newlib-nano `%lX` desteklemez).

---

## 4. `mqtt_cert_provision.c` — ESP32 yükleme

`MqttCertProv_Run()` ESP32 dalı (`EMPA_ESP32_MQTT_AWS` açıksa):

```c
DebugFramework_PutsLine("[CERT] ABOV sertifikalari ESP32'ye yukleniyor...");
if (Wifi_MqttCertsUpload2(buffer, POLLING_MODE) != FUNC_OK)
    return -1;

MqttCerts_LogStorage("ONCESI");
prov_wait_long_press();                    // UserButton, APP_BTN_LONG_PRESS_MS
MqttCerts_EraseFlash();
MqttCerts_LogStorage("SONRASI");
```

`prov_wait_long_press()` → `UserButton_ConsumeLongPress()` + `SYSTICK_Wait(10)`.

---

## 5. `mqtt_core.c` — ESP32'ye AT ile yükleme

`Wifi_MqttCertsUpload2()` ESP-AT MQTT PKI namespace'ine yazar:

```c
wifi_sysmfg_write_binary(buffer, "mqtt_ca",   "mqtt_ca",   MqttCerts_GetRootCA(),     ...);
wifi_sysmfg_write_binary(buffer, "mqtt_cert", "mqtt_cert", MqttCerts_GetClientCert(), ...);
wifi_sysmfg_write_binary(buffer, "mqtt_key",  "mqtt_key",  MqttCerts_GetPrivateKey(), ...);
```

`MQTT_ConnectBroker()` state machine'de WiFi+NTP sonrası doğrudan `MQTT_INIT_STATE_MQTT_USER_CONFIG`'e geçilir — `MQTT_INIT_STATE_MQTT_CERT_UPLOAD` **atlanır** (yükleme zaten boot'ta yapıldı).

---

## 6. `prv_user_code.c` — boot entegrasyonu

Sertifika bloğu ESP32 ve SLM320 için ortaktır; ESP32 connect'ten önce çalışır:

```c
#if defined(EMPA_ESP32_MQTT_AWS)
    ESP32_AT_Test_WithRecovery();
    MqttPort_ABOV_Init();   /* Wifi_MqttCertsUpload2 icin g_port — cert prov'dan ONCE */
#endif

#if (defined(EMPA_ESP32_MQTT_AWS) || defined(EMPA_SLM320_4G)) && MQTT_USE_TLS_CERTS
    if (MqttCerts_HasEmbedded() != 0U)
        MqttCertProv_Run(mqttPacketBuffer, MQTT_DATA_PACKET_BUFF_SIZE);
    ...
#endif

#if defined(EMPA_ESP32_MQTT_AWS)
    MQTT_ConnectBroker();
#endif
```

---

## Akış diyagramı

```
Boot
  │
  ├─ ESP32_AT_Test_WithRecovery()
  ├─ MqttPort_ABOV_Init()          ← cert upload öncesi (g_port gerekli)
  │
  ├─ MqttCerts_HasEmbedded() ?
  │     ├─ EVET → MqttCertProv_Run()
  │     │         ├─ Wifi_MqttCertsUpload2()     → ESP32 NVS (mqtt_ca/cert/key)
  │     │         ├─ MqttCerts_LogStorage(ONCESI)
  │     │         ├─ Buton 3+ sn
  │     │         ├─ MqttCerts_EraseFlash()      → .cert_flash → 0xFF
  │     │         └─ MqttCerts_LogStorage(SONRASI)
  │     └─ HAYIR → log + MqttCerts_LogFlashIfPresent()
  │
  └─ MQTT_ConnectBroker()   (ESP32'deki PKI ile TLS)
```

---

## Yapılandırma

`config/app_config.h`:
```c
#define EMPA_ESP32_MQTT_AWS
#define APP_BTN_LONG_PRESS_MS       3000U
```

`config/mqtt_device_config.h`:
```c
#define MQTT_USE_TLS_CERTS          1
```

---

## Beklenen log örnekleri

**İlk kurulum (sertifika var):**
```
[CERT] ABOV sertifikalari ESP32'ye yukleniyor...
[MQTT] Uploading CA cert...
[MQTT] Uploading client cert...
[MQTT] Uploading private key...
[CERT] ESP32 yukleme tamam
[CERT] --- ONCESI ---
[CERT] ONCESI mqtt_rootCA        RAM @0x... FW @0x... VALID
...
[CERT] ABOV sertifikalari silmek icin butona 3+ saniye basin
[CERT] Flash siliniyor: 0x........ - 0x........
[CERT] Flash sertifikalari kalici olarak silindi
[CERT] --- SONRASI ---
...
[MQTT] Connecting to broker...
```

**Reset sonrası (silme başarılı):**
```
[CERT] ABOV'da sertifika yok
[MQTT] Connecting to broker...
```
(Ek FW log satırı yok = flash temiz.)

---

## Yeniden kurulum

IDE ile firmware flash → `.cert_flash` yeniden programlanır → `MqttCerts_HasEmbedded() == 1` → kurulum tekrarlanır.

---

## Public API özeti (`mqtt_certs.h`)

```c
const char *MqttCerts_GetRootCA(void);
const char *MqttCerts_GetClientCert(void);
const char *MqttCerts_GetPrivateKey(void);
size_t MqttCerts_GetRootCALen(void);
size_t MqttCerts_GetClientCertLen(void);
size_t MqttCerts_GetPrivateKeyLen(void);
uint8_t MqttCerts_HasEmbedded(void);
int     MqttCerts_EraseFlash(void);
void    MqttCerts_LogStorage(const char *phase);
void    MqttCerts_LogFlashIfPresent(void);
```

```c
int MqttCertProv_Run(char *buffer, uint16_t bufSize);  // mqtt_cert_provision.h
```
