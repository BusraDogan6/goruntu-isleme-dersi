
#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    // Görüntüyü tek kanallı (grayscale) olarak oku
    cv::Mat image = cv::imread(
        "/content/img.jpeg",
        cv::IMREAD_GRAYSCALE
    );

    if (image.empty())
    {
        std::cout << "Resim okunamadi!" << std::endl;
        return -1;
    }

    std::cout << "Genislik  : " << image.cols << std::endl;
    std::cout << "Yukseklik: " << image.rows << std::endl;
    std::cout << "Kanal    : " << image.channels() << std::endl;

    // Orijinali bozmamak için kopyasını oluştur
    cv::Mat image6bit = image.clone();

    // SATIRLARI GEZ
    for (int row = 0; row < image6bit.rows; row++)
    {
        // SÜTUNLARI GEZ
        for (int col = 0; col < image6bit.cols; col++)
        {
            // Pikselin parlaklık değerini al
            uchar pixel = image6bit.at<uchar>(row, col);

            // 8 bit -> 6 bit nicemleme
            pixel = (pixel / 4) * 4;

            // Yeni değeri piksele yaz
            image6bit.at<uchar>(row, col) = pixel;
        }
    }

    // Sonuçları kaydet
    cv::imwrite("/content/original_gray.png", image);
    cv::imwrite("/content/image_6bit.png", image6bit);

    std::cout << "Islem tamamlandi." << std::endl;

    return 0;
}
