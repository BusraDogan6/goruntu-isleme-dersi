
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>

using namespace std;
using namespace cv;

int main()
{
    string klasor =
        "/content/goruntu-isleme-dersi/Hafta-08/04-Ten-Rengi-RGB-HSV-YCbCr/";

    Mat resim =
        imread(
            klasor + "input/insan.jpg",
            IMREAD_COLOR
        );

    if (resim.empty())
    {
        cout << "Resim bulunamadi!" << endl;
        return -1;
    }


    // ----------------------------------
    // Renk uzaylarini olustur
    // ----------------------------------

    Mat rgb;
    Mat hsv;
    Mat ycbcr;

    cvtColor(
        resim,
        rgb,
        COLOR_BGR2RGB
    );

    cvtColor(
        resim,
        hsv,
        COLOR_BGR2HSV
    );

    // OpenCV'de kanal sirasi Y, Cr, Cb
    cvtColor(
        resim,
        ycbcr,
        COLOR_BGR2YCrCb
    );


    // Sonuc goruntuleri
    Mat rgbSonuc =
        resim.clone();

    Mat hsvSonuc =
        resim.clone();

    Mat ycbcrSonuc =
        resim.clone();


    // Ten maskeleri
    Mat rgbMaske(
        resim.rows,
        resim.cols,
        CV_8UC1,
        Scalar(0)
    );

    Mat hsvMaske(
        resim.rows,
        resim.cols,
        CV_8UC1,
        Scalar(0)
    );

    Mat ycbcrMaske(
        resim.rows,
        resim.cols,
        CV_8UC1,
        Scalar(0)
    );


    // ==================================
    // TUM PIKSELLERI GEZ
    // ==================================

    for (int y = 0; y < resim.rows; y++)
    {
        for (int x = 0; x < resim.cols; x++)
        {

            // ==================================
            // 1 - RGB
            // ==================================

            Vec3b rgbPiksel =
                rgb.at<Vec3b>(y, x);

            int R = rgbPiksel[0];
            int G = rgbPiksel[1];
            int B = rgbPiksel[2];

            int enBuyuk =
                max(R, max(G, B));

            int enKucuk =
                min(R, min(G, B));


            bool rgbTen =
                R > 95 &&
                G > 40 &&
                B > 20 &&
                (enBuyuk - enKucuk) > 15 &&
                abs(R - G) > 15 &&
                R > G &&
                R > B;


            if (rgbTen)
            {
                // Ten bolgesi beyaz
                rgbMaske.at<uchar>(y, x) = 255;
            }
            else
            {
                // Ten olmayan bolge KIRMIZI
                // BGR sirasi: 0, 0, 255
                rgbSonuc.at<Vec3b>(y, x) =
                    Vec3b(0, 0, 255);
            }



            // ==================================
            // 2 - HSV
            // ==================================

            Vec3b hsvPiksel =
                hsv.at<Vec3b>(y, x);

            int H = hsvPiksel[0];
            int S = hsvPiksel[1];
            int V = hsvPiksel[2];


            bool hsvTen =
                H >= 0 &&
                H <= 25 &&
                S >= 30 &&
                S <= 180 &&
                V >= 50;


            if (hsvTen)
            {
                hsvMaske.at<uchar>(y, x) = 255;
            }
            else
            {
                hsvSonuc.at<Vec3b>(y, x) =
                    Vec3b(0, 0, 255);
            }



            // ==================================
            // 3 - YCbCr
            // ==================================

            Vec3b ycbcrPiksel =
                ycbcr.at<Vec3b>(y, x);

            int Y =
                ycbcrPiksel[0];

            int Cr =
                ycbcrPiksel[1];

            int Cb =
                ycbcrPiksel[2];


            bool ycbcrTen =
                Cr >= 133 &&
                Cr <= 173 &&
                Cb >= 77 &&
                Cb <= 127;


            if (ycbcrTen)
            {
                ycbcrMaske.at<uchar>(y, x) = 255;
            }
            else
            {
                ycbcrSonuc.at<Vec3b>(y, x) =
                    Vec3b(0, 0, 255);
            }
        }
    }


    // ----------------------------------
    // Sonuclari kaydet
    // ----------------------------------

    imwrite(
        klasor + "01_RGB_maske.bmp",
        rgbMaske
    );

    imwrite(
        klasor + "02_RGB_sonuc.bmp",
        rgbSonuc
    );

    imwrite(
        klasor + "03_HSV_maske.bmp",
        hsvMaske
    );

    imwrite(
        klasor + "04_HSV_sonuc.bmp",
        hsvSonuc
    );

    imwrite(
        klasor + "05_YCbCr_maske.bmp",
        ycbcrMaske
    );

    imwrite(
        klasor + "06_YCbCr_sonuc.bmp",
        ycbcrSonuc
    );


    cout << "RGB ten tespiti tamamlandi." << endl;
    cout << "HSV ten tespiti tamamlandi." << endl;
    cout << "YCbCr ten tespiti tamamlandi." << endl;

    cout << endl;

    cout << "Ten olmayan bolgeler kirmizi yapildi."
         << endl;


    return 0;
}
