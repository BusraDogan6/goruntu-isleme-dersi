
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;


// Kendi calcHist fonksiyonumuz
void calcHist(const cv::Mat& kanal, int histogram[256])
{
    // Önce histogramı sıfırla
    for (int i = 0; i < 256; i++)
    {
        histogram[i] = 0;
    }

    // Kanalın bütün piksellerini gez
    for (int y = 0; y < kanal.rows; y++)
    {
        for (int x = 0; x < kanal.cols; x++)
        {
            int pikselDegeri = kanal.at<uchar>(y, x);

            histogram[pikselDegeri]++;
        }
    }
}


// Histogramı resim olarak çizmek için
cv::Mat histogramCiz(
    int histogram[256],
    cv::Scalar renk)
{
    int genislik = 512;
    int yukseklik = 400;

    cv::Mat histResmi(
        yukseklik,
        genislik,
        CV_8UC3,
        cv::Scalar(255, 255, 255)
    );

    // En büyük değeri bul
    int maxDeger = 0;

    for (int i = 0; i < 256; i++)
    {
        if (histogram[i] > maxDeger)
        {
            maxDeger = histogram[i];
        }
    }


    int binGenislik = 2;


    // Histogramı çiz
    for (int i = 1; i < 256; i++)
    {
        int oncekiY =
            yukseklik -
            (histogram[i - 1] * (yukseklik - 20) / maxDeger);

        int simdikiY =
            yukseklik -
            (histogram[i] * (yukseklik - 20) / maxDeger);


        cv::line(
            histResmi,
            cv::Point((i - 1) * binGenislik, oncekiY),
            cv::Point(i * binGenislik, simdikiY),
            renk,
            2
        );
    }

    return histResmi;
}


int main()
{
    string giris =
        "/content/goruntu-isleme-dersi/Hafta-05/input/girdi.png";

    string cikis =
        "/content/goruntu-isleme-dersi/Hafta-05/02-CalcHist/";


    cv::Mat resim =
        cv::imread(giris, cv::IMREAD_COLOR);


    if (resim.empty())
    {
        cout << "Resim okunamadi!" << endl;
        return -1;
    }


    cout << "Resim boyutu: "
         << resim.cols
         << " x "
         << resim.rows
         << endl;


    // OpenCV renk sırası BGR
    vector<cv::Mat> kanallar;

    cv::split(resim, kanallar);


    cv::Mat maviKanal = kanallar[0];
    cv::Mat yesilKanal = kanallar[1];
    cv::Mat kirmiziKanal = kanallar[2];


    int maviHist[256];
    int yesilHist[256];
    int kirmiziHist[256];


    // Kendi yazdığımız calcHist fonksiyonunu
    // her kanal için ayrı ayrı çalıştırıyoruz

    ::calcHist(maviKanal, maviHist);

    cout << "Mavi kanal histogrami hesaplandi." << endl;


    ::calcHist(yesilKanal, yesilHist);

    cout << "Yesil kanal histogrami hesaplandi." << endl;


    ::calcHist(kirmiziKanal, kirmiziHist);

    cout << "Kirmizi kanal histogrami hesaplandi." << endl;


    // Histogram görüntülerini oluştur
    cv::Mat maviHistogramResmi =
        histogramCiz(
            maviHist,
            cv::Scalar(255, 0, 0)
        );


    cv::Mat yesilHistogramResmi =
        histogramCiz(
            yesilHist,
            cv::Scalar(0, 180, 0)
        );


    cv::Mat kirmiziHistogramResmi =
        histogramCiz(
            kirmiziHist,
            cv::Scalar(0, 0, 255)
        );


    // Sonuçları kaydet
    cv::imwrite(
        cikis + "hist_mavi.png",
        maviHistogramResmi
    );

    cv::imwrite(
        cikis + "hist_yesil.png",
        yesilHistogramResmi
    );

    cv::imwrite(
        cikis + "hist_kirmizi.png",
        kirmiziHistogramResmi
    );


    cout << endl;
    cout << "Tum histogramlar kaydedildi." << endl;

    return 0;
}
