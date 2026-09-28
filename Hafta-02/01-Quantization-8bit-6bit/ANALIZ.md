# 8-Bit → 6-Bit Parlaklık Nicemleme

Bu çalışmada tek kanallı grayscale görüntünün parlaklık seviyeleri 8 bitten 6 bite düşürülmüştür.

8-bit grayscale görüntüde 256 farklı parlaklık seviyesi bulunur:

0 - 255

6-bit görüntüde ise:

2^6 = 64

farklı parlaklık seviyesi bulunur.

Bu nedenle her piksel için aşağıdaki işlem uygulanmıştır:

newPixel = (pixel / 4) * 4

Örneğin:

173 / 4 = 43  
43 * 4 = 172

Bu işlem sonucunda 8-bit görüntü fiziksel olarak yine 8-bit formatta saklanmaktadır. Ancak görüntüde yalnızca 64 farklı parlaklık seviyesi kullanılmaktadır.

8-bit ve 6-bit görüntüler gözle karşılaştırıldığında fark oldukça az görülmüştür. Bunun nedeni her pikselde oluşabilecek maksimum parlaklık farkının yalnızca 3 olmasıdır.
