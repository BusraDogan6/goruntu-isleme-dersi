# Thread ile Parlaklık İşlemleri

Bu çalışmada grayscale görüntü 4 parçaya ayrılmıştır.

Her parçaya farklı parlaklık işlemi uygulanmıştır:

- Sol üst: +60
- Sağ üst: +30
- Sol alt: -30
- Sağ alt: -60

İlk uygulamada her parça ayrı bir thread tarafından işlenmiştir.

Daha sonra aynı 4 işlem:

- 1 thread ile sırayla
- 4 thread ile paralel

çalıştırılarak süre karşılaştırması yapılmıştır.

Colab ortamında CPU thread kapasitesi 2 olarak görüldüğü için 4 thread kullanımı her zaman hızlanma sağlamamıştır. Thread oluşturma, scheduling ve senkronizasyon maliyetleri nedeniyle bazı testlerde 4 thread daha yavaş çalışmıştır.
