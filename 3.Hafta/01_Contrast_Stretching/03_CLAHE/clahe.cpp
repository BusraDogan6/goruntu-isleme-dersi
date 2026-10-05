
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;


// Kendi yazdığımız CLAHE
Mat kendiCLAHE(Mat resim, int parcaX, int parcaY, double clipLimit)
{
    int satir = resim.rows;
    int sutun = resim.cols;

    // Her parça için 256 elemanlı dönüşüm tablosu
    vector<vector<vector<uchar>>> tablo(
        parcaY,
        vector<vector<uchar>>(
            parcaX,
            vector<uchar>(256)
        )
    );


    // Her parçanın histogramını hesapla
    for (int py = 0; py < parcaY; py++)
    {
        for (int px = 0; px < parcaX; px++)
        {
            int yBas = py * satir / parcaY;
            int ySon = (py + 1) * satir / parcaY;

            int xBas = px * sutun / parcaX;
            int xSon = (px + 1) * sutun / parcaX;


            int histogram[256] = {0};


            // Histogramı kendimiz çıkarıyoruz
            for (int y = yBas; y < ySon; y++)
            {
                for (int x = xBas; x < xSon; x++)
                {
                    int deger = resim.at<uchar>(y, x);

                    histogram[deger]++;
                }
            }


            int toplam =
                (ySon - yBas) *
                (xSon - xBas);


            // Histogram için sınır değeri
            int sinir =
                (int)(
                    clipLimit *
                    toplam /
                    256.0
                );

            if (sinir < 1)
                sinir = 1;


            // Sınırdan fazla olan değerleri kes
            int fazla = 0;

            for (int i = 0; i < 256; i++)
            {
                if (histogram[i] > sinir)
                {
                    fazla += histogram[i] - sinir;

                    histogram[i] = sinir;
                }
            }


            // Kesilen değerleri histogramın tamamına dağıt
            int eklenecek = fazla / 256;
            int kalan = fazla % 256;


            for (int i = 0; i < 256; i++)
            {
                histogram[i] += eklenecek;
            }


            for (int i = 0; i < kalan; i++)
            {
                histogram[i]++;
            }


            // CDF hesapla
            int cdf[256] = {0};

            cdf[0] = histogram[0];


            for (int i = 1; i < 256; i++)
            {
                cdf[i] =
                    cdf[i - 1] +
                    histogram[i];
            }


            // Her gri seviye için yeni değeri hesapla
            for (int i = 0; i < 256; i++)
            {
                double yeni =
                    ((double)cdf[i] / toplam)
                    * 255.0;


                if (yeni < 0)
                    yeni = 0;

                if (yeni > 255)
                    yeni = 255;


                tablo[py][px][i] =
                    (uchar)round(yeni);
            }
        }
    }


    Mat sonuc(
        satir,
        sutun,
        CV_8UC1
    );


    // Parçaların yaklaşık genişlik ve yüksekliği
    double parcaGenislik =
        (double)sutun / parcaX;

    double parcaYukseklik =
        (double)satir / parcaY;


    // Parçaların arasında sert geçiş olmasın diye
    // komşu parçaların değerlerini karıştırıyoruz
    for (int y = 0; y < satir; y++)
    {
        for (int x = 0; x < sutun; x++)
        {
            int eski =
                resim.at<uchar>(y, x);


            double gx =
                (x + 0.5) / parcaGenislik - 0.5;

            double gy =
                (y + 0.5) / parcaYukseklik - 0.5;


            int x1 = floor(gx);
            int y1 = floor(gy);

            int x2 = x1 + 1;
            int y2 = y1 + 1;


            double dx = gx - x1;
            double dy = gy - y1;


            if (x1 < 0)
            {
                x1 = 0;
                dx = 0;
            }

            if (y1 < 0)
            {
                y1 = 0;
                dy = 0;
            }

            if (x2 >= parcaX)
            {
                x2 = parcaX - 1;
                dx = 0;
            }

            if (y2 >= parcaY)
            {
                y2 = parcaY - 1;
                dy = 0;
            }


            double ust =
                tablo[y1][x1][eski] * (1 - dx)
                +
                tablo[y1][x2][eski] * dx;


            double alt =
                tablo[y2][x1][eski] * (1 - dx)
                +
                tablo[y2][x2][eski] * dx;


            double yeni =
                ust * (1 - dy)
                +
                alt * dy;


            sonuc.at<uchar>(y, x) =
                saturate_cast<uchar>(yeni);
        }
    }


    return sonuc;
}



int main()
{
    // Resmi tek kanallı aç
    Mat resim = imread(
        "/content/input_image",
        IMREAD_GRAYSCALE
    );


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



    // --------------------------
    // OpenCV hazır CLAHE
    // --------------------------

    Ptr<CLAHE> clahe =
        createCLAHE(2.0, Size(8, 8));


    Mat hazirSonuc;

    clahe->apply(
        resim,
        hazirSonuc
    );


    imwrite(
        "/content/clahe_opencv.png",
        hazirSonuc
    );


    cout << "OpenCV CLAHE tamamlandi."
         << endl;



    // --------------------------
    // Kendi CLAHE kodumuz
    // --------------------------

    Mat manuelSonuc =
        kendiCLAHE(
            resim,
            8,
            8,
            2.0
        );


    imwrite(
        "/content/clahe_manual.png",
        manuelSonuc
    );


    cout << "Manuel CLAHE tamamlandi."
         << endl;


    return 0;
}
