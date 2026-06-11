# Geliştirme Ortamı Kurulumu

Bu kılavuz, Tiremo® Accelerator Workshops aktivitelerine katılabilmek için gerekli olan tüm yazılım araçlarının kurulum adımlarını içermektedir. Aktiviteye başlamadan önce aşağıdaki tüm araçların kurulumunu tamamlayınız.

---

## Gereksinimler

| Araç | Açıklama |
|------|----------|
| **eMStudio32** | ABOV mikrodenetleyiciler için entegre geliştirme ortamı (IDE) |
| **MCUBrew32** | Proje oluşturma ve yapılandırma aracı |
| **aFlasher32** | Binary dosyalarını karta yüklemek için flash programlama aracı |
| **Tera Term** | Seri port üzerinden haberleşme için terminal emülatörü |

---

## 1. eMStudio32 Kurulumu

eMStudio32, ABOV mikrodenetleyicileri için geliştirilmiş entegre bir geliştirme ortamıdır (IDE). Kod yazma, derleme ve hata ayıklama işlemlerini tek bir arayüz üzerinden gerçekleştirmenizi sağlar.

### Kaynaklar

- 🌐 **Resmi İndirme Sayfası:** [ABOV Tools & Support — eMStudio32](https://www.abov.co.kr/en/tools_support/debug_tools.php?category=eMStudio32)
- 📖 **Kullanım Kılavuzu:** [eMStudio32 Manual & Release Notes](https://abov.atlassian.net/wiki/spaces/ES2/pages/1558413356/Manual+Release)
- 📋 **Kurulum Adımları:** [eMStudio32 Installation Guide](https://abov.atlassian.net/wiki/spaces/ES2/pages/1752858627/Installation)
- 📄 **Yerel Döküman:** [ES2 Installation (PDF)](Document/ES2-Installation-200526-125944.pdf)

### Kurulum Adımları

1. Yukarıdaki resmi indirme sayfasına gidin ve kurulum dosyasını indirin.
2. İndirilen kurulum dosyasını çalıştırın.
3. [Kurulum dökümanında](https://abov.atlassian.net/wiki/spaces/ES2/pages/1752858627/Installation) belirtilen adımları takip ederek kurulumu tamamlayın.

---

## 2. MCUBrew32 Kurulumu

MCUBrew32, ABOV mikrodenetleyici projeleri için kod üretimi ve çevre birim yapılandırması yapmanızı sağlayan bir araçtır.

### Kaynaklar

- 🌐 **Resmi İndirme Sayfası:** [ABOV Tools & Support — MCUBrew32](https://www.abov.co.kr/en/tools_support/debug_tools.php?category=mcubrew32)
- 📖 **Kullanım Kılavuzu:** [MCUBrew32 User Guide](https://abov.atlassian.net/wiki/spaces/MCUBrew321/pages/760250452/Manual+Release)
- 📋 **Kurulum Adımları:** [MCUBrew32 Installation and Getting Started](https://abov.atlassian.net/wiki/spaces/MCUBrew321/pages/1379565598/Installation+and+Getting+Started)
- 📄 **Yerel Döküman:** [MCUBrew32 Installation (PDF)](Document/MCUBrew321-Installing%20and%20uninstalling%20the%20MCUBrew32%20program-200526-123146.pdf)

### Kurulum Adımları

1. Yukarıdaki resmi indirme sayfasına gidin ve kurulum dosyasını indirin.
2. İndirilen kurulum dosyasını çalıştırın ve kurulum adımlarını takip edin.
3. Kurulum dökümanının **6. adımında** yer alan **"Installation Complete"** ekranı göründüğünde kurulum tamamlanmış demektir.

---

## 3. aFlasher32 Kurulumu

aFlasher32, derlenmiş binary dosyalarını Tiremo®Cortex kartına yüklemek için kullanılan flash programlama aracıdır.

### Kurulum Adımları

**Adım 1 —** [ABOV Tools & Support — aFlasher32](https://www.abov.co.kr/en/tools_support/debug_tools.php?category=aflasher32) sayfasına gidin.

![aFlasher32 İndirme Sayfası](Document/aFlasher32/aFlasher_image1.png)

**Adım 2 —** *All Tools & Support (Downloadable)* alanında **Executable File** sütunundaki **aFlasher32 Executable** `.exe` dosyasını indirin.

**Adım 3 —** İndirilen `.zip` dosyasından `.exe` kurulum dosyasını çıkartın ve setup programını çalıştırın.

**Adım 4 —** Aşağıdaki adımları takip ederek kurulumu tamamlayın:

**→** Açılan kurulum ekranında **Next** butonuna basın.

![aFlasher32 Kurulum Adım 1](Document/aFlasher32/aFlasher_image2.png)

**→** Lisans sözleşmesini kabul edin.

![aFlasher32 Lisans Sözleşmesi](Document/aFlasher32/aFlasher_image3.png)

**→** Kurulum konumunu seçin ve **Install** butonuna basın.

![aFlasher32 Kurulum Konumu](Document/aFlasher32/aFlasher_image4.png)

**→** **Finish** butonuna basarak kurulumu tamamlayın.

![aFlasher32 Kurulum Tamamlandı](Document/aFlasher32/aFlasher_image5.png)

---

## 4. Tera Term Kurulumu

Tera Term, seri port üzerinden Tiremo®Cortex ile haberleşmek için kullanılan bir terminal emülatörüdür. Bu workshop'ta Tera Term kullanılacaktır; ancak dilerseniz tercih ettiğiniz herhangi bir seri terminal uygulamasını kullanabilirsiniz.

### Kurulum Adımları

**Adım 1 —** [https://teratermproject.github.io/index-en.html](https://teratermproject.github.io/index-en.html) adresine gidin.

**Adım 2 —** **Download** alanından en son sürüme tıklayın.

![Tera Term İndirme Sayfası](Document/TeraTerm/TeraTerm_image1.png)

**Adım 3 —** Açılan sayfada **installer** alanından kurulum dosyasını indirin.

![Tera Term Installer](Document/TeraTerm/TeraTerm_image2.png)

**Adım 4 —** İndirilen `.exe` dosyasını çalıştırın ve kurulum adımlarını tamamlayın.

---

## Kurulum Tamamlandı ✓

Tüm araçların kurulumu tamamlandıktan sonra aktiviteye geçebilirsiniz.

### ↳ [Aktiviteye Başla — Tiremo®Cortex ile Veri Toplama ve MQTT Haberleşmesi](Uygulama.md)

---

<p align="center">
  <sub>© Empa Electronics — Tiremo® Accelerator Workshops</sub>
</p>
