# Görev 7 - Canny Kenar Tespiti ve Karşılaştırma

Bu çalışmada OpenCV kütüphanesinde bulunan Canny kenar tespit algoritması
kullanılarak görüntüdeki kenarlar tespit edilmiştir.

Canny uygulanmadan önce görüntü Gaussian filtre ile hafif şekilde
yumuşatılmıştır. Böylece küçük gürültülerin kenar olarak algılanmasının
azaltılması amaçlanmıştır.

Canny fonksiyonunda alt eşik değeri 50, üst eşik değeri ise 150 olarak
kullanılmıştır.

## Çalışma Süreleri

Aynı görüntü üzerinde ölçülen çalışma süreleri:

Canny: 7.06938 ms
Prewitt CUDA: 0.26288 ms
Sobel CUDA: 0.0424 ms
Laplacian CUDA: 0.02976 ms
LoG CUDA: 0.120672 ms


Prewitt, Sobel, Laplacian ve LoG filtreleri CUDA kullanılarak GPU üzerinde
çalıştırılmıştır. Canny ise OpenCV'nin Canny fonksiyonu ile CPU üzerinde
çalıştırılmıştır.

Bu nedenle süre karşılaştırmasında kullanılan donanımın farklı olduğu
dikkate alınmalıdır.

## Sonuçların Görsel Karşılaştırılması

### Prewitt

Prewitt basit ve hızlı bir kenar tespit yöntemidir. Yatay ve dikey
yoğunluk değişimlerini başarılı şekilde tespit etmektedir. Ancak ince
gürültüler de kenar olarak algılanabilir.

### Sobel

Sobel filtresi Prewitt'e benzer ancak orta komşulara daha fazla ağırlık
verir. Bu nedenle bazı kenarlar Prewitt sonucuna göre daha belirgin
görünmektedir.

### Laplacian

Laplacian yön bağımsız olarak kenarları tespit etmektedir. İkinci türev
tabanlı olduğu için görüntüdeki küçük değişimlere ve gürültülere karşı
daha hassastır.

### LoG

LoG, Gaussian yumuşatma ile Laplacian işlemini birleştirir. Gürültünün
etkisini azaltarak bütün yönlerdeki kenar değişimlerini ortaya çıkarmaya
çalışır.

### Canny

Canny sonucunda kenarların diğer yöntemlere göre daha ince ve daha
sürekli olduğu gözlemlenmiştir. Alt ve üst eşik kullanılması sayesinde
zayıf ve gereksiz kenarların bir kısmı elenmektedir.

Canny yalnızca gradyan büyüklüğüne bakmaz. Kenar inceltme ve eşikleme
adımları da kullandığı için sonuç görüntüsü genellikle daha temizdir.

## Doğruluk Değerlendirmesi

Bu çalışmada gerçek kenarları gösteren bir ground truth görüntüsü
bulunmadığı için sayısal bir doğruluk yüzdesi hesaplanmamıştır.

Bu nedenle doğruluk karşılaştırması görsel olarak yapılmıştır.

Görsel değerlendirmede Canny'nin gereksiz kenarları azaltma, kenarları
ince gösterme ve ana kenarların sürekliliğini koruma açısından diğer
yöntemlere göre daha başarılı olduğu görülmüştür.

Prewitt ve Sobel daha basit ve hızlı yöntemlerdir. Laplacian ve LoG ise
yön bağımsız kenar tespiti yapabilmektedir.

## Sonuç

Çalışma sonucunda her algoritmanın farklı avantajlara sahip olduğu
görülmüştür.

Prewitt ve Sobel basit ve hızlıdır. Laplacian bütün yönlerdeki yoğunluk
değişimlerini algılar. LoG gürültüyü azaltma ve kenar tespitini birleştirir.

Canny ise eşikleme ve kenar inceltme işlemleri sayesinde görsel olarak
daha temiz ve sürekli kenarlar üretmiştir.
