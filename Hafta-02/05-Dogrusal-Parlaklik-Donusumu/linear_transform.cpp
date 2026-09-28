
#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    // Görüntüyü tek kanallı grayscale olarak oku
    cv::Mat image = cv::imread(
        "/content/img.jpeg",
        cv::IMREAD_GRAYSCALE
    );

    if (image.empty())
    {
        std::cout << "Resim okunamadi!" << std::endl;
        return -1;
    }

    // İki farklı sonuç görüntüsü
    cv::Mat result075 = image.clone();
    cv::Mat result075plus20 = image.clone();

    // Tüm pikselleri satır-sütun dolaş
    for (int row = 0; row < image.rows; row++)
    {
        for (int col = 0; col < image.cols; col++)
        {
            uchar pixel =
                image.at<uchar>(row, col);

            // 1. işlem:
            // newPixel = 0.75 * pixel
            double value1 =
                0.75 * pixel;

            result075.at<uchar>(row, col) =
                cv::saturate_cast<uchar>(value1);


            // 2. işlem:
            // newPixel = 0.75 * pixel + 20
            double value2 =
                0.75 * pixel + 20;

            result075plus20.at<uchar>(row, col) =
                cv::saturate_cast<uchar>(value2);
        }
    }

    // Sonuçları kaydet
    cv::imwrite(
        "/content/result_075.png",
        result075
    );

    cv::imwrite(
        "/content/result_075_plus20.png",
        result075plus20
    );

    std::cout << "Islem tamamlandi." << std::endl;

    return 0;
}
