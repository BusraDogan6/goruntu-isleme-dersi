# Görev 4 - Sobel ve Prewitt Operatörlerinin Karşılaştırılması

Bu çalışmada aynı gri tonlamalı görüntü üzerinde Prewitt ve Sobel kenar tespit operatörleri uygulanmıştır. Filtreler uygulanmadan önce görüntü Gaussian filtre ile hafif şekilde yumuşatılmıştır. Böylece görüntüde bulunan küçük gürültülerin kenar tespit sonucunu fazla etkilemesi azaltılmıştır.

## Prewitt Operatörü

Prewitt operatörü görüntüdeki yatay ve dikey yoğunluk değişimlerini tespit etmek için kullanılan 3x3 boyutunda bir kenar tespit filtresidir.

Dikey Prewitt çekirdeği:

-1  0  1  
-1  0  1  
-1  0  1  

Yatay Prewitt çekirdeği:

-1 -1 -1  
 0  0  0  
 1  1  1  

Prewitt filtresi komşu pikseller arasındaki parlaklık farkını kullanarak kenarları ortaya çıkarmaktadır. Çekirdekte kullanılan ağırlıklar birbirine eşittir. Bu nedenle yapısı daha basit ve hesaplama maliyeti daha düşüktür.

## Sobel Operatörü

Sobel operatörü de Prewitt gibi görüntüdeki yatay ve dikey kenarları bulmak için kullanılan 3x3 boyutunda bir filtredir.

Dikey Sobel çekirdeği:

-1  0  1  
-2  0  2  
-1  0  1  

Yatay Sobel çekirdeği:

-1 -2 -1  
 0  0  0  
 1  2  1  

Sobel filtresinde orta satır veya orta sütundaki piksel değerlerine 2 kat ağırlık verilmektedir. Bu özellik Sobel filtresinin kenar bölgelerine daha fazla önem vermesini sağlar.

## Karşılaştırma

Yapılan deneylerde hem Prewitt hem de Sobel operatörlerinin görüntüdeki yatay ve dikey kenarları başarılı bir şekilde tespit ettiği görülmüştür.

Prewitt filtresi daha basit bir çekirdeğe sahiptir. Tüm komşu pikseller eşit ağırlıkta değerlendirildiği için hesaplama işlemi daha sadedir. Ancak görüntüde gürültü bulunduğunda bazı küçük değişimler de kenar olarak algılanabilir.

Sobel filtresinde ise orta piksellere daha yüksek ağırlık verilmesi nedeniyle kenarlar daha belirgin hale gelmektedir. Bu nedenle Sobel sonucu bazı bölgelerde Prewitt sonucuna göre daha güçlü ve daha net görünmektedir.

Her iki filtre uygulanmadan önce Gaussian filtre kullanılması gürültünün azaltılmasına yardımcı olmuştur. Böylece gereksiz küçük kenarların oluşması azaltılmıştır.

Bu çalışmada filtre işlemleri CUDA kullanılarak GPU üzerinde gerçekleştirilmiştir. Görüntü önce GPU belleğine aktarılmış, her piksel CUDA threadleri tarafından işlenmiş ve sonuç tekrar CPU belleğine alınarak OpenCV ile kaydedilmiştir.

## Sonuç

Prewitt ve Sobel operatörleri benzer amaçlarla kullanılan kenar tespit yöntemleridir. Prewitt daha basit bir yapıya sahipken, Sobel operatörü ağırlıklı çekirdeği sayesinde kenar değişimlerini daha güçlü şekilde ortaya çıkarabilmektedir.

Bu nedenle gürültünün ve küçük parlaklık değişimlerinin bulunduğu görüntülerde Sobel operatörü genellikle daha belirgin sonuçlar verebilir. Prewitt ise basitliği ve düşük hesaplama maliyeti nedeniyle temel kenar tespit işlemlerinde kullanılabilir.
