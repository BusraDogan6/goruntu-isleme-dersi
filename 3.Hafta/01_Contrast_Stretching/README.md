# Görev 1 - Linear Contrast Stretching

Bu çalışmada tek kanallı (grayscale) görüntüler üzerinde doğrusal kontrast germe uygulanmıştır.

## Yapılan İşlemler

1. Görüntü grayscale olarak okunmuştur.
2. Görüntünün tüm pikselleri matris üzerinde tek tek gezilmiştir.
3. Histogram oluşturulmuştur.
4. Minimum ve maksimum piksel değerleri bulunmuştur.
5. Minimum değer 0'a, maksimum değer 255'e taşınmıştır.
6. Linear Contrast Stretching uygulanmıştır.
7. İşlem öncesi ve sonrası histogramlar oluşturulmuştur.

## Formül

Yeni Piksel = ((Eski Piksel - Min) / (Max - Min)) * 255

## Testler

Çalışma iki farklı görüntü üzerinde test edilmiştir.

### Test 1

- input_image.jpg
- contrast_stretched.png
- original_histogram.png
- stretched_histogram.png

### Test 2

- input_image.jpg
- contrast_stretched.png
- original_histogram.png
- stretched_histogram.png

## Kullanılan Teknolojiler

- C++
- OpenCV
- Google Colab
