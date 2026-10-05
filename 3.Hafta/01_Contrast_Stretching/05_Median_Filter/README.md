# Görev 5 - 5x5 Median Filter

Bu görevde Salt-and-Pepper gürültüsü bulunan tek kanallı bir görüntü
üzerinde 5x5 Median Filter uygulanmıştır.

İlk olarak OpenCV'nin hazır medianBlur fonksiyonu kullanılmıştır.
Daha sonra Median Filter hazır fonksiyon kullanılmadan manuel olarak yazılmıştır.

## Yapılan İşlemler

1. Salt-and-Pepper gürültülü görüntü açıldı.
2. OpenCV medianBlur fonksiyonu ile 5x5 Median Filter uygulandı.
3. Manuel Median Filter için görüntü piksel piksel gezildi.
4. Her piksel için 5x5 komşuluk alındı.
5. 25 piksel değeri bir listeye eklendi.
6. Değerler küçükten büyüğe sıralandı.
7. Ortadaki değer yani medyan seçildi.
8. Medyan değer sonuç görüntüsüne yazıldı.
9. Hazır ve manuel filtre sonuçları karşılaştırıldı.

## 5x5 Median Filter

Her piksel için 5x5 boyutunda toplam 25 piksel kullanılmıştır.

25 değer sıralandıktan sonra ortadaki değer:

index = 12

olarak alınmıştır.

## Sonuç

Median Filter, Salt-and-Pepper gürültüsündeki 0 ve 255 gibi
uç piksel değerlerini bastırmada başarılı olmuştur.

Ortalama filtresine göre kenarların daha iyi korunmasını sağlar.

## Dosyalar

- `salt_pepper.png` : Gürültülü giriş görüntüsü
- `median_filter.cpp` : C++ kaynak kodu
- `median_opencv.png` : OpenCV Median Filter sonucu
- `median_manual.png` : Manuel Median Filter sonucu

## Kullanılan Teknolojiler

- C++
- OpenCV
- Google Colab
