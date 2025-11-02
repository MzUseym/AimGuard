# Gerekli kütüphaneleri içe aktarıyoruz
from ultralytics import YOLO  # YOLO modeli için Ultralytics kütüphanesi
import cv2  # OpenCV kütüphanesi, kamera görüntüsü işleme için kullanılır
import serial  # Seri port iletişimi için kullanılır (ESP8266 ile iletişim)
import time  # Zamanlayıcı işlemleri için kullanılır

# YOLO modeli yükleniyor
model = YOLO('best.pt')  # Daha önce eğitilmiş YOLO modeli 'best.pt' dosyasından yükleniyor

# ESP8266 ile seri iletişim kuruluyor
try:
    esp = serial.Serial('COM4', 9600, timeout=1)  # ESP8266 ile 'COM4' portu üzerinden 9600 baud hızında seri iletişim başlatılıyor
    time.sleep(2)  # Seri bağlantının oturması için 2 saniye bekleniyor
except serial.SerialException as e:  # Bağlantı hatası olursa bu blok çalışır
    print("ESP ile bağlantı kurulamadı:", e)  # Hata mesajı ekrana yazdırılır
    exit()  # Program durdurulur

# Dahili kamera başlatılıyor
cap = cv2.VideoCapture(0)  # OpenCV kullanarak bilgisayarın dahili kamerası başlatılıyor (kamera ID: 0)

print("Hedef algılama başlatıldı...")  # Kullanıcıya sistemin başlatıldığı bildiriliyor

try:
    # Kamera sürekli açık olduğu sürece döngü çalışır
    while cap.isOpened():
        ret, frame = cap.read()  # Kameradan bir kare (frame) okunur
        if not ret:  # Kare okunamazsa döngüyü sonlandır
            break

        # YOLO modeli ile hedef tespiti yapılır
        results = model(frame)  # Kameradan alınan kare, YOLO modeline gönderilerek hedefler tespit edilir
        annotated_frame = results[0].plot()  # Tespit edilen hedeflerin çerçeveleri kare üzerinde çizilir
        cv2.imshow("Hedef Algılama", annotated_frame)  # Tespit edilen hedeflerin işlendiği görüntü ekrana gösterilir

        # Hedef algılanıp algılanmadığını kontrol eden değişken
        detected = False
        for detection in results[0].boxes:  # Tespit edilen nesnelerin kutuları (bounding boxes) üzerinde döner
            conf = detection.conf  # Tespit edilen nesnenin güven skoru (confidence score)
            if conf > 0.6:  # Güven eşiği 0.6'dan büyükse hedef algılanmış kabul edilir
                detected = True
                print(f"Hedef algılandı: Güven {conf}")  # Algılama durumu ve güven skoru ekrana yazdırılır
                esp.write(b"hedefAlgilandi\n")  # Seri port üzerinden ESP8266'ya "hedef algılandı" mesajı gönderilir
                break  # Hedef algılandıktan sonra döngüden çıkılır

        if not detected:  # Eğer hiçbir hedef algılanmadıysa:
            esp.write(b"hedefAlgilanmadi\n")  # Seri port üzerinden "hedef algılanmadı" mesajı gönderilir

        # 'q' tuşuna basılırsa döngü sonlandırılır
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
except Exception as e:  # Program çalışırken bir hata oluşursa:
    print("Bir hata oluştu:", e)  # Hata mesajı ekrana yazdırılır
finally:
    # Kaynaklar serbest bırakılıyor
    cap.release()  # Kamera kapatılır
    cv2.destroyAllWindows()  # Açık olan tüm pencere kapatılır
    esp.close()  # Seri port bağlantısı kapatılır
