# AimGuard
AimGuard: YOLOv8 ve MPU6050 kullanan, askeri eğitimler için akıllı bir güvenlik ve pozisyon algılama sistemi.

AimGuard: YOLOv8 ve MPU6050 ile Askeri Eğitim Güvenlik Sistemi

🎯 Proje Özeti
AimGuard, askeri eğitim süreçlerinde güvenliği artırmak ve kullanıcı hatalarını en aza indirmek için tasarlanmış teknoloji destekli bir sistemdir .

Sistem iki ana bileşenden oluşur:

Pozisyon Algılama: Silah üzerine yerleştirilen MPU6050 sensörü ve ESP tabanlı mikrodenetleyici, silahın pozisyonunu (eğimini) hassas bir şekilde algılar . Hatalı pozisyonlarda kullanıcıya anlık olarak LED ve buzzer ile görsel/işitsel geri bildirim verilir .

Hedef Tespiti: Bilgisayarlı görü teknikleri kullanılarak, Roboflow üzerinde etiketlenen özel bir veri seti ile YOLOv8 modeli eğitilmiştir . Bu model, "düşman" tespiti yapar ve bu bilgiyi ESP sistemine ileterek kullanıcıyı uyarır .

Tüm sensör verileri ve sistem durumu, MQTT protokolü aracılığıyla Adafruit IO platformuna gönderilerek gerçek zamanlı veri analizi ve görselleştirme sağlanır .

🛠️ Kullanılan Teknolojiler
Derin Öğrenme / Görüntü İşleme: Ultralytics YOLOv8 , Python , OpenCV , Roboflow (Veri Etiketleme)

Donanım / Gömülü Sistemler: NodeMCU V3 (ESP8266) , MPU6050 6 Eksen Sensörü , RGB LED , Buzzer

Platform & Protokoller: Adafruit IO , MQTT , PlatformIO


Not: Eğitilen modele drive linkinden erişebilirsiniz.
