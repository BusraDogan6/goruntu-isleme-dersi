# Görev 4 - 3x3 Ortalama Filtresi

Bu görevde gürültülü ve tek kanallı bir görüntü üzerinde
3x3 ortalama filtresi uygulanmıştır.

Filtre görüntü üzerinde konvolüsyon mantığı ile gezdirilmiştir.
Hazır blur, boxFilter veya filter2D fonksiyonları kullanılmamıştır.

## Kullanılan Filtre

3x3 ortalama filtresi:

1 1 1
1 1 1
1 1 1

Her piksel için 3x3 komşuluktaki 9 piksel toplanmış
ve sonuç 9'a bölünmüştür.

## Yapılan İşlemler

1. Gürültülü görüntü grayscale olarak açıldı.
2. Görüntünün pikselleri tek tek gezildi.
3. Her pikselin 3x3 komşuluğu alındı.
4. 9 pikselin toplamı hesaplandı.
5. Toplam 9'a bölündü.
6. Bulunan ortalama değer merkez piksele yazıldı.
7. Filtrelenmiş görüntü kaydedildi.

## Sonuç

Ortalama filtresi görüntüdeki ani piksel değişimlerini azaltarak
gürültüyü bastırmıştır. Bununla birlikte görüntüde bir miktar
bulanıklaşma oluşmuştur.

## Dosyalar

- `input_image.png` : Gürültülü giriş görüntüsü
- `average_filter.cpp` : C++ kaynak kodu
- `average_filtered.png` : 3x3 ortalama filtresi uygulanmış görüntü

## Kullanılan Teknolojiler

- C++
- OpenCV
- Google Colab
