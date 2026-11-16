
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>

using namespace std;
using namespace cv;


// RGB -> HSI
Mat RGBdenHSI(const Mat& rgb)
{
    Mat hsi(rgb.rows, rgb.cols, CV_8UC3);

    for (int y = 0; y < rgb.rows; y++)
    {
        for (int x = 0; x < rgb.cols; x++)
        {
            Vec3b p = rgb.at<Vec3b>(y, x);

            double R = p[0] / 255.0;
            double G = p[1] / 255.0;
            double B = p[2] / 255.0;

            double I = (R + G + B) / 3.0;

            double toplam = R + G + B;
            double S = 0.0;

            if (toplam > 0)
            {
                double minDeger = min(R, min(G, B));
                S = 1.0 - (3.0 * minDeger / toplam);
            }

            double pay =
                0.5 * ((R - G) + (R - B));

            double payda =
                sqrt(
                    (R - G) * (R - G) +
                    (R - B) * (G - B)
                );

            double H = 0.0;

            if (payda > 0)
            {
                double deger = pay / payda;

                deger = max(-1.0, min(1.0, deger));

                double theta = acos(deger);

                if (B <= G)
                    H = theta;
                else
                    H = 2.0 * CV_PI - theta;

                H = H / (2.0 * CV_PI);
            }

            hsi.at<Vec3b>(y, x)[0] =
                saturate_cast<uchar>(H * 255);

            hsi.at<Vec3b>(y, x)[1] =
                saturate_cast<uchar>(S * 255);

            hsi.at<Vec3b>(y, x)[2] =
                saturate_cast<uchar>(I * 255);
        }
    }

    return hsi;
}


int main()
{
    string klasor =
        "/content/goruntu-isleme-dersi/Hafta-08/03-Renk-Uzaylari/";

    Mat bgr =
        imread(
            klasor + "input/rgb_girdi.bmp",
            IMREAD_COLOR
        );

    if (bgr.empty())
    {
        cout << "Resim bulunamadi!" << endl;
        return -1;
    }


    // OpenCV BGR okudugu icin once RGB
    Mat rgb;

    cvtColor(
        bgr,
        rgb,
        COLOR_BGR2RGB
    );


    // RGB -> HSV
    Mat hsv;

    cvtColor(
        rgb,
        hsv,
        COLOR_RGB2HSV
    );


    // RGB -> HSI
    Mat hsi =
        RGBdenHSI(rgb);


    // RGB -> YUV
    Mat yuv;

    cvtColor(
        rgb,
        yuv,
        COLOR_RGB2YUV
    );


    // RGB -> CIE Lab
    Mat lab;

    cvtColor(
        rgb,
        lab,
        COLOR_RGB2Lab
    );


    // Hepsi 3 kanalli BMP olarak kaydediliyor
    imwrite(
        klasor + "RGB.bmp",
        rgb
    );

    imwrite(
        klasor + "HSV.bmp",
        hsv
    );

    imwrite(
        klasor + "HSI.bmp",
        hsi
    );

    imwrite(
        klasor + "YUV.bmp",
        yuv
    );

    imwrite(
        klasor + "LAB.bmp",
        lab
    );


    cout << "RGB kanal: "
         << rgb.channels()
         << endl;

    cout << "HSV kanal: "
         << hsv.channels()
         << endl;

    cout << "HSI kanal: "
         << hsi.channels()
         << endl;

    cout << "YUV kanal: "
         << yuv.channels()
         << endl;

    cout << "LAB kanal: "
         << lab.channels()
         << endl;


    cout << endl;
    cout << "Tum renk uzaylari 3 kanalli BMP olarak kaydedildi."
         << endl;


    return 0;
}
