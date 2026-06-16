# ABOV TLS Sertifika Kurulumu — SLM320 (4G LTE + MQTT)

ESP32 akışının SLM320 modem karşılığı. Aynı `mqtt_certs.c` altyapısı ve aynı `.cert_flash` bölgesi kullanılır; hedef modem UFS dosya sistemidir.

---

## Problem ve çözüm özeti

SLM320 TLS MQTT için sertifikaların modem dosya sisteminde (`cacert.pem`, `client.pem`, `user_key.pem`) olması gerekir. Önceki kod her `SLM320_ConnectBroker()` çağrısında ABOV flash'tan tekrar yüklüyordu — prov sonrası ABOV silinince bu başarısız olurdu.

**Yapılanlar:**

1. Boot'ta `MqttCertProv_Run()` içinde SLM320'ye yükleme (`QFUPL`).
2. Buton ile ABOV flash silme (ESP32 ile ortak).
3. `SLM320_STATE_SSL_CFG` güncellendi: ABOV'da sertifika yoksa upload atlanır, modemde dosya doğrulanır.

---

## Değiştirilen / eklenen dosyalar

| Dosya | Ne yapıldı |
|-------|------------|
| `Libraries/MEIG_SLM3XX/slm320.c` | `SLM320_BootstrapAt`, `CertsUploadAll`, `CertsVerifyAll`; SSL_CFG mantığı |
| `Libraries/MEIG_SLM3XX/slm320.h` | Yeni API bildirimleri |
| `Libraries/cert_Lib/mqtt_cert_provision.c` | SLM320 yükleme dalı eklendi |
| `prv_user_code.c` | Ortak cert prov bloğu (ESP32 + SLM320) |
| `Libraries/cert_Lib/mqtt_certs.c` | Ortak (ESP32 ile paylaşılan) |

Sertifika `.inc` ve linker değişiklikleri ESP32 dokümanındakiyle aynıdır (`CERT_PROVISION_ESP32.md`).

---

## 1. Modem dosya eşlemesi

| ABOV kaynağı | `MqttCerts_Get*()` | SLM320 UFS | `QSSLCFG` |
|--------------|-------------------|------------|-----------|
| Root CA | `GetRootCA()` | `cacert.pem` | `"cacert"` |
| Client cert | `GetClientCert()` | `client.pem` | `"clientcert"` |
| Private key | `GetPrivateKey()` | `user_key.pem` | `"clientkey"` |

SSL context ID: `SLM320_SSL_CTX_ID` = `2` (`slm320.h`).

---

## 2. `slm320_upload_pem()` — tek dosya yükleme

Mevcut private fonksiyon; `QFUPL` AT komutu ile UFS'e yazar:

```c
static uint8_t slm320_upload_pem(const char *filename, const char *pem_data, size_t pem_len)
{
    snprintf(cmd_buf, sizeof(cmd_buf),
             "AT+QFUPL=\"%s\",%u,100\r\n", filename, (unsigned)pem_len);
    SLM320_SendCmd(cmd_buf);
    /* CONNECT bekle */
    SLM320_SendCmd(pem_data);           // ham PEM gönder
    SLM320_CheckResponse("+QFUPL:", 15000);
    return slm320_verify_cert_file(filename);  // AT+QFLST ile doğrula
}
```

`+CME ERROR: 407` → dosya zaten var → `QFLST` ile doğrulama yeterli.

---

## 3. Yeni public API — `slm320.h`

```c
#if MQTT_USE_TLS_CERTS
uint8_t SLM320_BootstrapAt(void);      /* PWR on, RDY/AT READY, ATE0 */
uint8_t SLM320_CertsUploadAll(void);   /* QFDEL + QFUPL x3 */
uint8_t SLM320_CertsVerifyAll(void);   /* QFLST x3 */
#endif
```

### 3.1 `SLM320_BootstrapAt()`

Prov sırasında tam connect state machine çalıştırmadan modem AT'ye hazır hale getirilir:

```c
uint8_t SLM320_BootstrapAt(void)
{
    SLM320_PowerOn();
    /* 15 sn içinde RDY / AT READY bekle */
    for (i = 0; i < 10; i++) {
        SLM320_SendCmd("AT\r\n");
        if (SLM320_CheckResponse("OK", 3000)) break;
        SYSTICK_Wait(500);
    }
    SLM320_SendCmd("ATE0\r\n");
    return at_ok;
}
```

### 3.2 `SLM320_CertsUploadAll()`

```c
uint8_t SLM320_CertsUploadAll(void)
{
    SLM320_SendCmd("AT+QFDEL=\"cacert.pem\"\r\n");
    SLM320_SendCmd("AT+QFDEL=\"client.pem\"\r\n");
    SLM320_SendCmd("AT+QFDEL=\"user_key.pem\"\r\n");

    if (!slm320_upload_pem("cacert.pem",  MqttCerts_GetRootCA(),     MqttCerts_GetRootCALen()))     return 0;
    if (!slm320_upload_pem("client.pem",  MqttCerts_GetClientCert(), MqttCerts_GetClientCertLen())) return 0;
    if (!slm320_upload_pem("user_key.pem", MqttCerts_GetPrivateKey(), MqttCerts_GetPrivateKeyLen())) return 0;
    return 1;
}
```

### 3.3 `SLM320_CertsVerifyAll()`

ABOV silindikten sonra connect sırasında modemde dosyaların hâlâ olduğunu kontrol eder:

```c
uint8_t SLM320_CertsVerifyAll(void)
{
    if (!slm320_verify_cert_file("cacert.pem"))   return 0;
    if (!slm320_verify_cert_file("client.pem"))   return 0;
    if (!slm320_verify_cert_file("user_key.pem")) return 0;
    return 1;
}
```

---

## 4. `mqtt_cert_provision.c` — SLM320 dalı

`EMPA_SLM320_4G` tanımlıysa ESP32 dalından sonra (veya tek başına) çalışır:

```c
#if defined(EMPA_SLM320_4G)
    DebugFramework_PutsLine("[CERT] ABOV sertifikalari SLM320'ye yukleniyor...");
    if (SLM320_BootstrapAt() == 0U) {
        DebugFramework_PutsLine("[CERT] SLM320 AT hazir degil");
        return -1;
    }
    if (SLM320_CertsUploadAll() == 0U) {
        DebugFramework_PutsLine("[CERT] SLM320 yukleme basarisiz");
        return -1;
    }
    DebugFramework_PutsLine("[CERT] SLM320 yukleme tamam");
#endif

/* Ortak: log ONCESI → buton → MqttCerts_EraseFlash() → log SONRASI */
```

SLM320-only build'de `MqttCertProv_Run(NULL, 0)` çağrılır (`prv_user_code.c`).

---

## 5. `SLM320_STATE_SSL_CFG` — connect sırasındaki mantık

Eski kod her seferinde ABOV'dan upload ediyordu. Yeni kod:

```c
case SLM320_STATE_SSL_CFG:
{
    if (MqttCerts_HasEmbedded() != 0U)
    {
        /* ABOV'da hâlâ sertifika var (prov atlanmış veya ilk connect) */
        if (!SLM320_CertsUploadAll())
            return SLM320_ERROR;
    }
    else
    {
        /* Prov sonrası normal yol: modemde dosya olmalı */
        if (!SLM320_CertsVerifyAll()) {
            slm320_log("[SLM320][SSL] Modem cert files missing\r\n");
            return SLM320_ERROR;
        }
    }

    /* QSSLCFG: cacert, clientcert, clientkey, seclevel, sslversion, sni ... */
    snprintf(cmd_buf, sizeof(cmd_buf),
             "AT+QSSLCFG=\"cacert\",%u,\"cacert.pem\"\r\n", SLM320_SSL_CTX_ID);
    ...
    snprintf(cmd_buf, sizeof(cmd_buf),
             "AT+QMTCFG=\"ssl\",%u,1,%u\r\n", SLM320_MQTT_CLIENT_ID, SLM320_SSL_CTX_ID);
    slm320_state = SLM320_STATE_MQTT_OPEN;
}
```

PDP aktif olduktan sonra (`QIACT`) bu state'e girilir.

---

## 6. `prv_user_code.c` — boot sırası

```c
#if defined(EMPA_SLM320_4G)
    SLM320_Init(NULL);
#endif

/* ESP32 test (varsa) ... */

#if (defined(EMPA_ESP32_MQTT_AWS) || defined(EMPA_SLM320_4G)) && MQTT_USE_TLS_CERTS
    if (MqttCerts_HasEmbedded() != 0U)
#if defined(EMPA_ESP32_MQTT_AWS)
        MqttCertProv_Run(mqttPacketBuffer, MQTT_DATA_PACKET_BUFF_SIZE);
#else
        MqttCertProv_Run(NULL, 0U);
#endif
    else { /* sertifika yok log */ }
#endif

#if defined(EMPA_SLM320_4G)
    SLM320_ConnectBroker();
#endif
```

**Önemli:** `SLM320_Init()` cert prov'dan **önce** çağrılmalı (UART/GPIO hazır).

---

## Akış diyagramı

```
Boot
  │
  ├─ SLM320_Init()
  │
  ├─ MqttCerts_HasEmbedded() ?
  │     ├─ EVET → MqttCertProv_Run() [SLM320 dalı]
  │     │         ├─ SLM320_BootstrapAt()
  │     │         ├─ SLM320_CertsUploadAll()   (QFUPL → UFS)
  │     │         ├─ MqttCerts_LogStorage(ONCESI)
  │     │         ├─ Buton 3+ sn
  │     │         ├─ MqttCerts_EraseFlash()    (ABOV .cert_flash)
  │     │         └─ MqttCerts_LogStorage(SONRASI)
  │     └─ HAYIR → log + MqttCerts_LogFlashIfPresent()
  │
  └─ SLM320_ConnectBroker()
        ├─ APN, QIACT, DNS ...
        └─ SSL_CFG → CertsVerifyAll() → QSSLCFG → QMTOPEN → QMTCONN
```

---

## ESP32 + SLM320 birlikte

İkisi de `app_config.h`'de açıksa `MqttCertProv_Run()` sırası:

1. `Wifi_MqttCertsUpload2()` → ESP32 NVS
2. `SLM320_CertsUploadAll()` → modem UFS
3. Tek buton basışı → `MqttCerts_EraseFlash()` (bir kez)

Her iki modeme yükleme **silmeden önce** tamamlanmalıdır.

---

## Yapılandırma

`config/app_config.h`:
```c
#define EMPA_SLM320_4G
#define APP_BTN_LONG_PRESS_MS       3000U
```

`config/mqtt_device_config.h`:
```c
#define MQTT_USE_TLS_CERTS          1
```

`config/network_config.h` — APN (`CELLULAR_APN` vb.)

`config/board_config.h` — `SLM320_UART_ID`, PWR / PWRKEY pinleri

---

## Beklenen log örnekleri

**İlk kurulum:**
```
[SLM320] Driver init OK
[CERT] ABOV sertifikalari SLM320'ye yukleniyor...
[CERT] SLM320 yukleme tamam
[CERT] --- ONCESI ---
...
[CERT] Flash sertifikalari kalici olarak silindi
[SLM320] Starting 4G LTE connection...
[SLM320] TLS ready
[SLM320] MQTT broker connected!
```

**Reset sonrası:**
```
[CERT] ABOV'da sertifika yok
[SLM320] Starting 4G LTE connection...
[SLM320] TLS ready
```

---

## Hata ayıklama

| Log | Kod yeri | Anlam |
|-----|----------|-------|
| `[CERT] SLM320 AT hazir degil` | `mqtt_cert_provision.c` | `SLM320_BootstrapAt()` başarısız |
| `[CERT] SLM320 yukleme basarisiz` | `mqtt_cert_provision.c` | `SLM320_CertsUploadAll()` |
| `[SLM320][SSL] QFUPL CONNECT not received` | `slm320_upload_pem()` | Modem CONNECT vermedi |
| `[SLM320][SSL] Modem cert files missing` | `SLM320_STATE_SSL_CFG` | ABOV silindi, modemde dosya yok |
| `[CERT] ABOV'da sertifika yok` + ek satır yok | `MqttCerts_LogFlashIfPresent()` | ABOV flash silme OK |

---

## Ortak kod referansı

SLM320 ve ESP32 aynı modülleri paylaşır:

| Modül | Dosya |
|-------|-------|
| Flash okuma / silme | `mqtt_certs.c` |
| Prov orkestrasyon | `mqtt_cert_provision.c` |
| PEM kaynağı | `certificates/mqtt_*.inc` |
| Linker bölümü | `gcc_a34g43x.ld` → `.cert_flash` |

Detaylı flash silme ve `.cert_flash` tasarımı için: `CERT_PROVISION_ESP32.md`.

---

## Yeniden kurulum

Yeni firmware flash → `.cert_flash` dolu → `HasEmbedded() == 1` → `CertsUploadAll()` içindeki `QFDEL` eski modem dosyalarını siler, yenilerini yazar → prov tekrarlanır.
