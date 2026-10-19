# Görev 6 - 7x7 Laplacian of Gaussian (LoG)

Bu çalışmada gri tonlamalı görüntü üzerinde 7x7 boyutunda
Laplacian of Gaussian (LoG) çekirdeği kullanılarak kenar tespiti yapılmıştır.

LoG filtresi Gaussian yumuşatma ve Laplacian kenar tespit işlemlerinin
birleşiminden oluşmaktadır. Gaussian işlemi görüntüdeki küçük gürültüleri
azaltırken Laplacian işlemi yoğunluk değişimlerini kullanarak kenarları
ortaya çıkarmaktadır.

## Karşılaştırma

LoG sonucu, Prewitt ve Sobel filtrelerinin Gaussian uygulanmış ve
uygulanmamış sonuçları ile karşılaştırılmıştır.

Gaussian uygulanmadan kullanılan Prewitt ve Sobel filtrelerinde görüntüdeki
küçük detaylar ve gürültüler daha fazla kenar olarak algılanabilmektedir.

Gaussian filtresi uygulandıktan sonra Prewitt ve Sobel sonuçlarında küçük
gürültülerin azaldığı ve ana kenarların daha temiz şekilde görülebildiği
gözlemlenmiştir.

Sobel filtresi orta komşulara daha yüksek ağırlık verdiği için Prewitt'e
göre bazı kenarları daha güçlü göstermektedir.

LoG filtresi ise belirli bir yatay veya dikey yön aramadan görüntüdeki
yoğunluk değişimlerini bütün yönlerde tespit etmektedir.

7x7 çekirdek kullanıldığı için LoG daha geniş bir komşuluk bölgesini
değerlendirmektedir. Bu durum küçük gürültülerin etkisinin azaltılmasına
yardımcı olurken bazı ince detayların da yumuşamasına neden olabilir.

## Sonuç

Prewitt ve Sobel basit ve yönlü kenar tespit yöntemleridir.
Gaussian filtre ile birlikte kullanıldıklarında gürültüye karşı daha
başarılı sonuç vermektedirler.

LoG filtresi ise Gaussian ve Laplacian işlemlerini tek bir çekirdekte
birleştirerek hem yumuşatma hem de kenar tespiti yapmaktadır.

Bu çalışmada kenar filtrelerinin hesaplamaları CUDA kullanılarak GPU
üzerinde gerçekleştirilmiştir.
