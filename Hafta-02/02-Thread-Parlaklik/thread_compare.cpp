
#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <chrono>

using Clock = std::chrono::steady_clock;


// Bir görüntü bölgesinin parlaklığını değiştirir
void changeBrightness(cv::Mat region, int value)
{
    for (int row = 0; row < region.rows; row++)
    {
        for (int col = 0; col < region.cols; col++)
        {
            int pixel = region.at<uchar>(row, col);

            int newPixel = pixel + value;

            region.at<uchar>(row, col) =
                cv::saturate_cast<uchar>(newPixel);
        }
    }
}


// Görüntüyü 4 parçaya ayırır
void getFourParts(
    cv::Mat& image,
    cv::Mat& p1,
    cv::Mat& p2,
    cv::Mat& p3,
    cv::Mat& p4)
{
    int halfWidth = image.cols / 2;
    int halfHeight = image.rows / 2;

    // Sol üst
    p1 = image(
        cv::Rect(
            0,
            0,
            halfWidth,
            halfHeight
        )
    );

    // Sağ üst
    p2 = image(
        cv::Rect(
            halfWidth,
            0,
            image.cols - halfWidth,
            halfHeight
        )
    );

    // Sol alt
    p3 = image(
        cv::Rect(
            0,
            halfHeight,
            halfWidth,
            image.rows - halfHeight
        )
    );

    // Sağ alt
    p4 = image(
        cv::Rect(
            halfWidth,
            halfHeight,
            image.cols - halfWidth,
            image.rows - halfHeight
        )
    );
}


int main()
{
    cv::Mat original = cv::imread(
        "/content/img.jpeg",
        cv::IMREAD_GRAYSCALE
    );

    if (original.empty())
    {
        std::cout << "Resim okunamadi!" << std::endl;
        return -1;
    }

    std::cout << "Resim boyutu: "
              << original.cols << " x "
              << original.rows << std::endl;

    std::cout << "CPU thread kapasitesi: "
              << std::thread::hardware_concurrency()
              << std::endl;


    // ==================================================
    // 1 THREAD
    // ==================================================

    cv::Mat singleImage = original.clone();

    cv::Mat s1, s2, s3, s4;

    getFourParts(
        singleImage,
        s1, s2, s3, s4
    );

    auto singleStart = Clock::now();

    // Tek thread 4 parçayı sırayla işler
    changeBrightness(s1, +60);
    changeBrightness(s2, +30);
    changeBrightness(s3, -30);
    changeBrightness(s4, -60);

    auto singleEnd = Clock::now();

    double singleTime =
        std::chrono::duration<double, std::milli>(
            singleEnd - singleStart
        ).count();


    // ==================================================
    // 4 THREAD
    // ==================================================

    cv::Mat multiImage = original.clone();

    cv::Mat m1, m2, m3, m4;

    getFourParts(
        multiImage,
        m1, m2, m3, m4
    );

    auto multiStart = Clock::now();

    std::thread t1(changeBrightness, m1, +60);
    std::thread t2(changeBrightness, m2, +30);
    std::thread t3(changeBrightness, m3, -30);
    std::thread t4(changeBrightness, m4, -60);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    auto multiEnd = Clock::now();

    double multiTime =
        std::chrono::duration<double, std::milli>(
            multiEnd - multiStart
        ).count();


    // ==================================================
    // SONUÇ
    // ==================================================

    double speedup =
        singleTime / multiTime;

    std::cout << "\n=============================\n";

    std::cout << "1 THREAD - 4 PARCA SIRAYLA:\n";
    std::cout << singleTime << " ms\n";

    std::cout << "\n4 THREAD - 4 PARCA PARALEL:\n";
    std::cout << multiTime << " ms\n";

    std::cout << "\nHizlanma orani: "
              << speedup
              << " kat\n";


    // Sonuç görüntülerini kaydet
    cv::imwrite(
        "/content/single_result.png",
        singleImage
    );

    cv::imwrite(
        "/content/multi_result.png",
        multiImage
    );


    // İki sonucun aynı olduğunu kontrol et
    cv::Mat difference;

    cv::absdiff(
        singleImage,
        multiImage,
        difference
    );

    int differentPixels =
        cv::countNonZero(difference);

    std::cout << "\nFarkli piksel sayisi: "
              << differentPixels
              << std::endl;

    return 0;
}
