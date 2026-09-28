# Histogram Süre Karşılaştırması

Bu çalışmada aynı görüntünün histogramı iki farklı yöntemle hesaplanmıştır:

- 1 thread ile 4 parça sırayla işlendi.
- 4 thread ile 4 parça paralel işlendi.

Her iki yöntemde de aynı histogram elde edilmiştir.

Colab ortamında kullanılabilir CPU thread sayısı sınırlı olduğundan 4 thread kullanımı her zaman hızlanma sağlamamıştır. Thread oluşturma, scheduling ve senkronizasyon maliyetleri toplam süreyi etkileyebilir.
