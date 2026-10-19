
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <cmath>

using namespace std;
using namespace cv;


// 1 - Gaussian 5x5
void gaussian5x5(Mat resim, Mat& sonuc)
{
    sonuc = resim.clone();

    int kernel[5][5] =
    {
        {1, 4, 7, 4, 1},
        {4, 16, 26, 16, 4},
        {7, 26, 41, 26, 7},
        {4, 16, 26, 16, 4},
        {1, 4, 7, 4, 1}
    };

    for (int y = 2; y < resim.rows - 2; y++)
    {
        for (int x = 2; x < resim.cols - 2; x++)
        {
            int toplam = 0;

            for (int ky = -2; ky <= 2; ky++)
            {
                for (int kx = -2; kx <= 2; kx++)
                {
                    int piksel =
                        resim.at<uchar>(y + ky, x + kx);

                    toplam += piksel *
                        kernel[ky + 2][kx + 2];
                }
            }

            sonuc.at<uchar>(y, x) =
                toplam / 273;
        }
    }
}


// 2 - Sobel Gradient
void sobelGradient(Mat resim, Mat& magnitude, Mat& angle)
{
    magnitude = Mat::zeros(
        resim.rows,
        resim.cols,
        CV_32F
    );

    angle = Mat::zeros(
        resim.rows,
        resim.cols,
        CV_32F
    );

    int sobelX[3][3] =
    {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    int sobelY[3][3] =
    {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}
    };

    float pi = 3.14159265;

    for (int y = 1; y < resim.rows - 1; y++)
    {
        for (int x = 1; x < resim.cols - 1; x++)
        {
            int gx = 0;
            int gy = 0;

            for (int ky = -1; ky <= 1; ky++)
            {
                for (int kx = -1; kx <= 1; kx++)
                {
                    int piksel =
                        resim.at<uchar>(y + ky, x + kx);

                    gx += piksel *
                        sobelX[ky + 1][kx + 1];

                    gy += piksel *
                        sobelY[ky + 1][kx + 1];
                }
            }

            float buyukluk =
                sqrt((float)(gx * gx + gy * gy));

            float aci =
                atan2((float)gy, (float)gx)
                * 180.0 / pi;

            if (aci < 0)
                aci += 180;

            magnitude.at<float>(y, x) = buyukluk;
            angle.at<float>(y, x) = aci;
        }
    }
}


// 3 - Non Maximum Suppression
void nonMaximumSuppression(
    Mat magnitude,
    Mat angle,
    Mat& sonuc)
{
    sonuc = Mat::zeros(
        magnitude.rows,
        magnitude.cols,
        CV_32F
    );

    for (int y = 1; y < magnitude.rows - 1; y++)
    {
        for (int x = 1; x < magnitude.cols - 1; x++)
        {
            float aci = angle.at<float>(y, x);
            float mevcut = magnitude.at<float>(y, x);

            float komsu1 = 0;
            float komsu2 = 0;

            if ((aci >= 0 && aci < 22.5) ||
                (aci >= 157.5 && aci <= 180))
            {
                komsu1 = magnitude.at<float>(y, x - 1);
                komsu2 = magnitude.at<float>(y, x + 1);
            }
            else if (aci >= 22.5 && aci < 67.5)
            {
                komsu1 = magnitude.at<float>(y - 1, x + 1);
                komsu2 = magnitude.at<float>(y + 1, x - 1);
            }
            else if (aci >= 67.5 && aci < 112.5)
            {
                komsu1 = magnitude.at<float>(y - 1, x);
                komsu2 = magnitude.at<float>(y + 1, x);
            }
            else
            {
                komsu1 = magnitude.at<float>(y - 1, x - 1);
                komsu2 = magnitude.at<float>(y + 1, x + 1);
            }

            if (mevcut >= komsu1 &&
                mevcut >= komsu2)
            {
                sonuc.at<float>(y, x) = mevcut;
            }
        }
    }
}


// 4 - Hysteresis
void hysteresis(
    Mat nms,
    Mat& sonuc,
    float dusukEsik,
    float yuksekEsik)
{
    sonuc = Mat::zeros(
        nms.rows,
        nms.cols,
        CV_8UC1
    );

    // strong - weak ayir
    for (int y = 1; y < nms.rows - 1; y++)
    {
        for (int x = 1; x < nms.cols - 1; x++)
        {
            float deger = nms.at<float>(y, x);

            if (deger >= yuksekEsik)
                sonuc.at<uchar>(y, x) = 255;

            else if (deger >= dusukEsik)
                sonuc.at<uchar>(y, x) = 75;
        }
    }

    // weak kenarlari kontrol et
    bool degisim = true;

    while (degisim)
    {
        degisim = false;

        for (int y = 1; y < sonuc.rows - 1; y++)
        {
            for (int x = 1; x < sonuc.cols - 1; x++)
            {
                if (sonuc.at<uchar>(y, x) == 75)
                {
                    bool strongVar = false;

                    for (int ky = -1; ky <= 1; ky++)
                    {
                        for (int kx = -1; kx <= 1; kx++)
                        {
                            if (sonuc.at<uchar>(
                                y + ky,
                                x + kx) == 255)
                            {
                                strongVar = true;
                            }
                        }
                    }

                    if (strongVar)
                    {
                        sonuc.at<uchar>(y, x) = 255;
                        degisim = true;
                    }
                }
            }
        }
    }

    // kalan weak pikselleri sil
    for (int y = 0; y < sonuc.rows; y++)
    {
        for (int x = 0; x < sonuc.cols; x++)
        {
            if (sonuc.at<uchar>(y, x) == 75)
                sonuc.at<uchar>(y, x) = 0;
        }
    }
}


int main()
{
    string giris =
        "/content/goruntu-isleme-dersi/Hafta-04/input/girdi.png";

    string klasor =
        "/content/goruntu-isleme-dersi/Hafta-04/Odev-Canny-Sifirdan/";

    Mat resim =
        imread(giris, IMREAD_GRAYSCALE);

    if (resim.empty())
    {
        cout << "Resim okunamadi!" << endl;
        return -1;
    }

    cout << "Resim boyutu: "
         << resim.cols << " x "
         << resim.rows << endl;


    // 1 Gaussian
    Mat gaussian;

    gaussian5x5(resim, gaussian);

    imwrite(
        klasor + "01_gaussian.png",
        gaussian
    );

    cout << "Gaussian tamamlandi." << endl;


    // 2 Sobel
    Mat magnitude;
    Mat angle;

    sobelGradient(
        gaussian,
        magnitude,
        angle
    );

    Mat gradientGoster;

    double minDeger;
    double maxDeger;

    minMaxLoc(
        magnitude,
        &minDeger,
        &maxDeger
    );

    magnitude.convertTo(
        gradientGoster,
        CV_8UC1,
        255.0 / maxDeger
    );

    imwrite(
        klasor + "02_gradient.png",
        gradientGoster
    );

    cout << "Sobel gradient tamamlandi." << endl;


    // 3 NMS
    Mat nms;

    nonMaximumSuppression(
        magnitude,
        angle,
        nms
    );

    Mat nmsGoster;

    minMaxLoc(
        nms,
        &minDeger,
        &maxDeger
    );

    nms.convertTo(
        nmsGoster,
        CV_8UC1,
        255.0 / maxDeger
    );

    imwrite(
        klasor + "03_nms.png",
        nmsGoster
    );

    cout << "Non Maximum Suppression tamamlandi."
         << endl;


    // 4 Hysteresis
    Mat cannySonuc;

    hysteresis(
        nms,
        cannySonuc,
        50,
        150
    );

    imwrite(
        klasor + "04_canny_manual.png",
        cannySonuc
    );

    cout << "Hysteresis tamamlandi." << endl;
    cout << "Canny islemi tamamlandi." << endl;

    return 0;
}
