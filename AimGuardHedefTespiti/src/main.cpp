#include <Wire.h>  // I2C iletişimi için gerekli kütüphane
#include <MPU6050.h>  // MPU6050 ivme sensörü için kütüphane
#include <config.h>  // Wi-Fi ve Adafruit IO bağlantı bilgilerini içeren dosya

// MPU6050 ve Adafruit IO nesneleri
MPU6050 mpu;  // MPU6050 nesnesi tanımlanıyor
AdafruitIO_Feed *buzzerFeed = io.feed("buzzer");  // Adafruit IO üzerindeki buzzer veri akışı
AdafruitIO_Feed *mpuFeed = io.feed("mpu6050");  // MPU6050 verilerini göndermek için veri akışı

// Pin tanımları
const int buzzerPin = 16;  // Buzzer pini tanımlanıyor
const int soldakiKirmiziPin = D3, soldakiYesilPin = D4, soldakiMaviPin = D5;  // Soldaki LED pinleri
const int sagdakiKirmiziPin = D6, sagdakiYesilPin = D7, sagdakiMaviPin = D8;  // Sağdaki LED pinleri

// Zamanlama değişkenleri
unsigned long sonGonderme = 0;  // Veri gönderme zamanını tutan değişken
unsigned long hedefZaman = 0;   // Zamanlayıcı değişken
bool hedefAlgilandi = false;    // Hedef algılama durumunu tutan değişken

// LED'leri kapatma fonksiyonu
void ledleriKapat() {
  analogWrite(soldakiKirmiziPin, 0);
  analogWrite(soldakiYesilPin, 0);
  analogWrite(soldakiMaviPin, 0);
  analogWrite(sagdakiKirmiziPin, 0);
  analogWrite(sagdakiYesilPin, 0);
  analogWrite(sagdakiMaviPin, 0);
}

void setup() {
  Serial.begin(9600);  // Seri iletişim hızı 9600 baud olarak ayarlanır
  Wire.begin();  // I2C iletişimi başlatılır
  mpu.initialize();  // MPU6050 sensörü başlatılır
  
  // Pin modları çıkış olarak ayarlanır
  pinMode(buzzerPin, OUTPUT);
  pinMode(soldakiKirmiziPin, OUTPUT);
  pinMode(soldakiYesilPin, OUTPUT);
  pinMode(soldakiMaviPin, OUTPUT);
  pinMode(sagdakiKirmiziPin, OUTPUT);
  pinMode(sagdakiYesilPin, OUTPUT);
  pinMode(sagdakiMaviPin, OUTPUT);
  ledleriKapat();  // Tüm LED'ler kapatılır

  // Adafruit IO bağlantısı başlatılır
  io.connect();
  while (io.status() < AIO_CONNECTED) {  // Bağlantı sağlanana kadar beklenir
    delay(500);
  }
}

void loop() {
  io.run();  // Adafruit IO bağlantısını çalıştırır (süreklilik sağlar)

  // MPU6050 verilerini okuma
  int16_t ax, ay, az;  // X, Y, Z eksenindeki ivme verilerini tutar
  mpu.getAcceleration(&ax, &ay, &az);  // MPU6050'den ivme verileri alınır

  // Seri porttan hedef algılama mesajlarını okuma
  if (Serial.available()) {  // Seri portta veri varsa
    String command = Serial.readStringUntil('\n');  // Mesaj okunur
    if (command == "hedefAlgilandi") {  // Hedef algılandı mesajı
      hedefAlgilandi = true;
    } else if (command == "hedefAlgilanmadi") {  // Hedef algılanmadı mesajı
      hedefAlgilandi = false;
    }
  }

  // LED ve Buzzer kontrolü
  int buzzerDurumu = 0;  // Buzzer durumunu tutan değişken
  ledleriKapat();  // Önce tüm LED'leri kapatır

  if (ay > 3000) {  // Silah sola doğru eğimliyse
    buzzerDurumu = 1;
    digitalWrite(buzzerPin, HIGH);  // Buzzer aktif edilir
    analogWrite(sagdakiKirmiziPin, 255);  // Sağdaki kırmızı LED yanar
    analogWrite(soldakiMaviPin, 255);  // Soldaki mavi LED yanar
  } else if (ay < -3000) {  // Silah sağa doğru eğimliyse
    buzzerDurumu = 1;
    digitalWrite(buzzerPin, HIGH);  // Buzzer aktif edilir
    analogWrite(soldakiKirmiziPin, 255);  // Soldaki kırmızı LED yanar
    analogWrite(sagdakiMaviPin, 255);  // Sağdaki mavi LED yanar
  } else {  // Eğim doğru ise
    buzzerDurumu = 0;
    if (hedefAlgilandi) {  // Hedef algılandıysa
      analogWrite(soldakiKirmiziPin, 255);
      analogWrite(sagdakiKirmiziPin, 255);
      static unsigned long bipZaman = 0;  // Bip sesi zamanlaması için değişken
      if (millis() - bipZaman > 500) {  // 500 ms aralıklarla buzzer bip sesi verir
        digitalWrite(buzzerPin, !digitalRead(buzzerPin));
        bipZaman = millis();
      }
    } else {  // Hedef algılanmadıysa
      digitalWrite(buzzerPin, LOW);  // Buzzer kapatılır
      analogWrite(soldakiYesilPin, 255);  // Yeşil LED'ler yanar
      analogWrite(sagdakiYesilPin, 255);
    }
  }

  // Adafruit IO'ya veri gönderme işlemi
  unsigned long simdikiZaman = millis();
  if (simdikiZaman - sonGonderme >= 4000) {  // Her 4 saniyede bir veri gönder
    mpuFeed->save(ay);  // MPU6050 Y ekseni verisi gönderilir
    buzzerFeed->save(buzzerDurumu);  // Buzzer durumu gönderilir
    sonGonderme = simdikiZaman;  // Zamanlayıcı güncellenir
  }
}
