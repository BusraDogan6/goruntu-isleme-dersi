
#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <array>
#include <string>

const int LEVELS = 256;

using Histogram = std::array<long long, LEVELS>;


// Bir parçanın histogramını hesaplar
void calculateHistogram(
    const cv::Mat& region,
    Histogram& histogram)
{
    histogram.fill(0);

    for (int row = 0; row < region.rows; row++)
    {
        for (int col = 0; col < region.cols; col++)
        {
            uchar pixel =
                region.at<uchar>(row, col);

            histogram[pixel]++;
        }
    }
}


// Histogramı görsel olarak çizer
void drawHistogram(
    const Histogram& histogram,
    const std::string& filename)
{
    const int width = 512;
    const int height = 400;

    cv::Mat histImage(
        height,
        width,
        CV_8UC3,
        cv::Scalar(255, 255, 255)
    );

    long long maxValue = 0;

    for (int i = 0; i < LEVELS; i++)
    {
        if (histogram[i] > maxValue)
            maxValue = histogram[i];
    }

    if (maxValue == 0)
        return;

    for (int i = 0; i < LEVELS; i++)
    {
        int x1 = i * 2;
        int x2 = x1 + 1;

        int barHeight =
            static_cast<int>(
                (static_cast<double>(histogram[i]) /
                 maxValue) *
                (height - 20)
            );

        cv::rectangle(
            histImage,
            cv::Point(x1, height - 1),
            cv::Point(x2, height - barHeight),
            cv::Scalar(0, 0, 0),
            cv::FILLED
        );
    }

    cv::imwrite(filename, histImage);
}


int main()
{
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


    int halfWidth =
        image.cols / 2;

    int halfHeight =
        image.rows / 2;


    // Resmi 4 parçaya ayır
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


    Histogram h1, h2, h3, h4;


    // 4 thread paralel histogram hesabı
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


    // Ana histogramı oluştur
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


    // Histogram görsellerini oluştur
    drawHistogram(
        h1,
        "/content/part1_hist.png"
    );

    drawHistogram(
        h2,
        "/content/part2_hist.png"
    );

    drawHistogram(
        h3,
        "/content/part3_hist.png"
    );

    drawHistogram(
        h4,
        "/content/part4_hist.png"
    );

    drawHistogram(
        mainHistogram,
        "/content/main_hist.png"
    );


    // Kontrol
    long long total = 0;

    for (int i = 0; i < LEVELS; i++)
    {
        total += mainHistogram[i];
    }

    std::cout
        << "Histogram toplam piksel: "
        << total
        << std::endl;

    std::cout
        << "Resim toplam piksel: "
        << image.total()
        << std::endl;

    std::cout
        << "Histogramlar olusturuldu."
        << std::endl;

    return 0;
}
