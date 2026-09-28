# Klasördeki Görüntüleri 1024x768 Boyutuna Yeniden Ölçekleme

Bu çalışmada C++ ve OpenCV kullanılarak bir klasör içerisindeki görüntü dosyaları okunmuş ve her görüntü 1024x768 çözünürlüğüne yeniden ölçeklenmiştir.

## Kullanılan Yapılar

- C++
- OpenCV
- `std::filesystem`
- `cv::imread()`
- `cv::resize()`
- `cv::imwrite()`

## İşlem Adımları

1. Görüntülerin bulunduğu klasör tarandı.
2. `.jpg`, `.jpeg`, `.png` ve `.bmp` uzantılı dosyalar seçildi.
3. Her görüntü `cv::imread()` ile okundu.
4. Görüntüler `cv::resize()` kullanılarak 1024x768 boyutuna getirildi.
5. İşlenmiş görüntüler çıktı olarak kaydedildi.

## Sonuç

Klasör içerisindeki görüntüler başarıyla okunmuş ve 1024x768 çözünürlüğüne yeniden ölçeklenmiştir.
