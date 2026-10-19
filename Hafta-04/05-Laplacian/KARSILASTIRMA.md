# Görev 5 - Laplacian, Sobel ve Prewitt Karşılaştırması

Bu çalışmada aynı gri tonlamalı görüntü üzerinde Prewitt, Sobel ve Laplacian
kenar tespit yöntemleri uygulanmıştır.

Filtreler uygulanmadan önce görüntü Gaussian filtre ile hafif şekilde
yumuşatılmıştır. Bu işlem görüntüdeki küçük gürültülerin kenar tespit
sonucunu fazla etkilemesini azaltmak için yapılmıştır.

## Prewitt

Prewitt operatörü birinci türev tabanlı bir kenar tespit yöntemidir.

Yatay ve dikey kenarlar için farklı 3x3 çekirdekler kullanılmaktadır.
Çekirdekteki komşu pikseller eşit ağırlıklara sahiptir.

Prewitt filtresi basit bir yapıya sahiptir ve görüntüdeki yatay ve
dikey yoğunluk değişimlerini tespit edebilir.

## Sobel

Sobel operatörü de birinci türev tabanlıdır ve yatay ile dikey kenarları
ayrı ayrı tespit eder.

Prewitt filtresinden farklı olarak bazı komşu piksellere 2 kat ağırlık
verilmektedir.

Bu nedenle Sobel sonucunda kenarlar bazı bölgelerde Prewitt sonucuna göre
daha belirgin görünmektedir.

## Laplacian

Laplacian operatörü ikinci türev tabanlı bir kenar tespit yöntemidir.

Bu çalışmada aşağıdaki 3x3 çekirdek kullanılmıştır:

0   1   0
1  -4   1
0   1   0

Prewitt ve Sobel filtrelerinden farklı olarak Laplacian için yatay ve
dikey olmak üzere iki ayrı filtre kullanılmamaktadır.

Laplacian görüntüdeki yoğunluk değişimlerini bütün yönlerde tespit etmeye
çalışır.

İkinci türev kullandığı için görüntüde bulunan gürültülere karşı daha
hassastır. Bu nedenle Laplacian filtresinden önce Gaussian filtre
uygulanmıştır.

## Sonuçların Karşılaştırılması

Prewitt filtresi basit yapısıyla yatay ve dikey kenarları başarılı bir
şekilde ortaya çıkarmıştır.

Sobel filtresinde orta bölgelere daha fazla ağırlık verilmesi nedeniyle
kenarların bazı bölgelerde Prewitt'e göre daha güçlü olduğu görülmüştür.

Laplacian ise belirli bir yöne bağlı olmadan görüntüdeki kenar değişimlerini
ortaya çıkarmıştır. Bu nedenle yatay ve dikey kenarlar tek bir sonuç
görüntüsünde görülebilmektedir.

Ancak Laplacian gürültüye Sobel ve Prewitt'e göre daha hassastır.
Gaussian yumuşatma uygulanması bu etkinin azaltılmasına yardımcı olmuştur.

Bu çalışmada filtre hesaplamaları CUDA kullanılarak GPU üzerinde
gerçekleştirilmiştir.
