
# Hafta 09 - Ödev 2
## Kırmızı Çember İçeren Trafik İşaretlerinin Tespiti

Bu çalışmada Kaggle üzerindeki Germany, UK ve USA trafik işaretleri veri seti kullanılmıştır.

Amaç kırmızı çember içeren trafik işaretlerini tespit etmektir.

### Kullanılan yöntemler

1. Görüntüler RGB, HSV ve CIE Lab renk uzaylarında incelendi.
2. Her renk uzayında kırmızı bölgeler için ayrı maske oluşturuldu.
3. Üç renk uzayından en az ikisinin kırmızı kabul ettiği pikseller ortak maskeye alındı.
4. Maskedeki küçük gürültüler morfolojik işlemler ile temizlendi.
5. Hough Circle algoritması ile dairesel bölgeler tespit edildi.
6. Hough tarafından bulunan çemberlerin çevresindeki kırmızı piksel oranı kontrol edildi.
7. Yeterli miktarda kırmızı içeren çemberler trafik işareti olarak kabul edildi.

### Kullanılan renk uzayları

- RGB
- HSV
- CIE Lab

### Hough

OpenCV `HoughCircles` fonksiyonu kullanılmıştır.

Yanlış çember tespitlerini azaltmak için Hough parametreleri daha seçici ayarlanmış ve çember çevresindeki kırmızı piksel oranı ayrıca kontrol edilmiştir.

### Sonuç

Farklı renk uzaylarının birlikte kullanılması yalnızca renk bilgisinden kaynaklanan hataları azaltmıştır. Hough Circle ile şekil bilgisi de kullanılarak kırmızı çemberli trafik işaretlerinin daha güvenilir şekilde tespit edilmesi sağlanmıştır.
