
#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <array>
#include <chrono>
#include <fstream>
#include <iomanip>

using Clock = std::chrono::steady_clock;

const int LEVELS = 256;
const int TEST_COUNT = 100;

using Histogram = std::array<long long, LEVELS>;


// Bir görüntü parçasının histogramını hesaplar
void calculateHistogram(
    const cv::Mat& region,
    Histogram& histogram)
{
    histogram.fill(0);

    for (int row = 0; row < region.rows; row++)
    {
        for (int col = 0; col < region.cols; col++)
        {
            uchar pixel = region.at<uchar>(row, col);

            histogram[pixel]++;
        }
    }
}


// Dört histogramı birleştirir
Histogram mergeHistograms(
    const Histogram& h1,
    const Histogram& h2,
    const Histogram& h3,
    const Histogram& h4)
{
    Histogram mainHistogram;
    mainHistogram.fill(0);

    for (int i = 0; i < LEVELS; i++)
    {
        mainHistogram[i] =
            h1[i] +
            h2[i] +
            h3[i] +
            h4[i];
    }

    return mainHistogram;
}


int main()
{
    // Görüntüyü grayscale olarak oku
    cv::Mat image = cv::imread(
        "/content/img.jpeg",
        cv::IMREAD_GRAYSCALE
    );

    if (image.empty())
    {
        std::cout << "Resim okunamadi!" << std::endl;
        return -1;
    }


    std::cout
        << "Resim boyutu: "
        << image.cols << " x "
        << image.rows
        << std::endl;

    std::cout
        << "CPU thread kapasitesi: "
        << std::thread::hardware_concurrency()
        << std::endl;

    std::cout
        << "Test sayisi: "
        << TEST_COUNT
        << std::endl;


    // ------------------------------------------------
    // Resmi 4 parçaya böl
    // ------------------------------------------------

    int halfWidth =
        image.cols / 2;

    int halfHeight =
        image.rows / 2;


    cv::Mat p1 = image(
        cv::Rect(
            0,
            0,
            halfWidth,
            halfHeight
        )
    );

    cv::Mat p2 = image(
        cv::Rect(
            halfWidth,
            0,
            image.cols - halfWidth,
            halfHeight
        )
    );

    cv::Mat p3 = image(
        cv::Rect(
            0,
            halfHeight,
            halfWidth,
            image.rows - halfHeight
        )
    );

    cv::Mat p4 = image(
        cv::Rect(
            halfWidth,
            halfHeight,
            image.cols - halfWidth,
            image.rows - halfHeight
        )
    );


    // Son histogramları saklamak için
    Histogram singleMainHistogram;
    Histogram multiMainHistogram;

    double singleTotal = 0.0;
    double multiTotal = 0.0;


    // ==================================================
    // 1 THREAD TESTİ
    // ==================================================

    for (int test = 0; test < TEST_COUNT; test++)
    {
        Histogram h1, h2, h3, h4;

        auto start = Clock::now();

        // Tek thread bütün parçaları sırayla hesaplıyor
        calculateHistogram(p1, h1);
        calculateHistogram(p2, h2);
        calculateHistogram(p3, h3);
        calculateHistogram(p4, h4);

        Histogram mainHistogram =
            mergeHistograms(
                h1, h2, h3, h4
            );

        auto end = Clock::now();

        singleTotal +=
            std::chrono::duration<double, std::milli>(
                end - start
            ).count();

        singleMainHistogram = mainHistogram;
    }


    // ==================================================
    // 4 THREAD TESTİ
    // ==================================================

    for (int test = 0; test < TEST_COUNT; test++)
    {
        Histogram h1, h2, h3, h4;

        auto start = Clock::now();

        // Her parça ayrı thread
        std::thread t1(
            calculateHistogram,
            std::cref(p1),
            std::ref(h1)
        );

        std::thread t2(
            calculateHistogram,
            std::cref(p2),
            std::ref(h2)
        );

        std::thread t3(
            calculateHistogram,
            std::cref(p3),
            std::ref(h3)
        );

        std::thread t4(
            calculateHistogram,
            std::cref(p4),
            std::ref(h4)
        );


        t1.join();
        t2.join();
        t3.join();
        t4.join();


        Histogram mainHistogram =
            mergeHistograms(
                h1, h2, h3, h4
            );

        auto end = Clock::now();


        multiTotal +=
            std::chrono::duration<double, std::milli>(
                end - start
            ).count();

        multiMainHistogram = mainHistogram;
    }


    // ==================================================
    // ORTALAMA SÜRELER
    // ==================================================

    double singleAverage =
        singleTotal / TEST_COUNT;

    double multiAverage =
        multiTotal / TEST_COUNT;

    double speedup =
        singleAverage / multiAverage;


    // Histogramlar aynı mı?
    int differentBins = 0;

    for (int i = 0; i < LEVELS; i++)
    {
        if (singleMainHistogram[i] !=
            multiMainHistogram[i])
        {
            differentBins++;
        }
    }


    std::cout
        << std::fixed
        << std::setprecision(4);


    std::cout
        << "\n====================================\n";

    std::cout
        << "HISTOGRAM SURE KARSILASTIRMASI\n";

    std::cout
        << "====================================\n";


    std::cout
        << "1 Thread ortalama: "
        << singleAverage
        << " ms"
        << std::endl;


    std::cout
        << "4 Thread ortalama: "
        << multiAverage
        << " ms"
        << std::endl;


    std::cout
        << "Hizlanma orani: "
        << speedup
        << " kat"
        << std::endl;


    std::cout
        << "Farkli histogram kutusu: "
        << differentBins
        << std::endl;


    // Yüzdelik fark
    double percentage;

    if (multiAverage < singleAverage)
    {
        percentage =
            ((singleAverage - multiAverage)
             / singleAverage) * 100.0;

        std::cout
            << "4 thread yaklasik %"
            << percentage
            << " daha hizli."
            << std::endl;
    }
    else
    {
        percentage =
            ((multiAverage - singleAverage)
             / singleAverage) * 100.0;

        std::cout
            << "4 thread yaklasik %"
            << percentage
            << " daha yavas."
            << std::endl;
    }


    // ==================================================
    // SONUÇLARI TXT DOSYASINA KAYDET
    // ==================================================

    std::ofstream file(
        "/content/histogram_timing.txt"
    );

    file
        << std::fixed
        << std::setprecision(4);

    file
        << "Histogram Sure Karsilastirmasi\n\n";

    file
        << "Resim boyutu: "
        << image.cols
        << " x "
        << image.rows
        << "\n";

    file
        << "CPU thread kapasitesi: "
        << std::thread::hardware_concurrency()
        << "\n";

    file
        << "Test sayisi: "
        << TEST_COUNT
        << "\n\n";

    file
        << "1 Thread ortalama: "
        << singleAverage
        << " ms\n";

    file
        << "4 Thread ortalama: "
        << multiAverage
        << " ms\n";

    file
        << "Hizlanma orani: "
        << speedup
        << " kat\n";

    file
        << "Farkli histogram kutusu: "
        << differentBins
        << "\n";

    file.close();


    return 0;
}
