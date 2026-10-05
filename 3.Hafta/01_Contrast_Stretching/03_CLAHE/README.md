# Görev 3 - CLAHE

Bu görevde düşük kontrastlı ve tek kanallı bir görüntü üzerinde
CLAHE (Contrast Limited Adaptive Histogram Equalization) işlemi uygulanmıştır.

İlk olarak OpenCV içerisinde bulunan hazır CLAHE fonksiyonu kullanılmıştır.
Daha sonra CLAHE işlemi hazır fonksiyon kullanılmadan tekrar yazılmıştır.

## Yapılan İşlemler

1. Görüntü grayscale olarak açıldı.
2. OpenCV'nin hazır CLAHE fonksiyonu uygulandı.
3. Görüntü küçük bölgelere ayrıldı.
4. Her bölgenin histogramı hesaplandı.
5. Histogram değerleri clip limit ile sınırlandırıldı.
6. Fazla kalan değerler histogram üzerine tekrar dağıtıldı.
7. CDF hesaplandı.
8. Yeni piksel değerleri oluşturuldu.
9. Manuel CLAHE sonucu elde edildi.
10. Hazır CLAHE ve manuel CLAHE sonuçları karşılaştırıldı.

## Kullanılan CLAHE Ayarları

- Clip Limit: 2.0
- Grid Size: 8 x 8

## Dosyalar

- `input_image.png` : Orijinal düşük kontrastlı görüntü
- `clahe.cpp` : C++ kodu
- `clahe_opencv.png` : OpenCV CLAHE sonucu
- `clahe_manual.png` : Manuel CLAHE sonucu

## Kullanılan Teknolojiler

- C++
- OpenCV
- Google Colab
