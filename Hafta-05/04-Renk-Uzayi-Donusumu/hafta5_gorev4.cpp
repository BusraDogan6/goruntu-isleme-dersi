
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;
using namespace cv;


// Function pointer için tip tanımı
typedef Mat (*DonusumFonksiyonu)(const Mat&);


// BGR -> RGB dönüşümünü kendimiz yapıyoruz
Mat bgrToRgb(const Mat& resim)
{
    Mat sonuc(
        resim.rows,
        resim.cols,
        CV_8UC3
    );

    for (int y = 0; y < resim.rows; y++)
    {
        for (int x = 0; x < resim.cols; x++)
        {
            Vec3b piksel = resim.at<Vec3b>(y, x);

            int B = piksel[0];
            int G = piksel[1];
            int R = piksel[2];

            sonuc.at<Vec3b>(y, x)[0] = R;
            sonuc.at<Vec3b>(y, x)[1] = G;
            sonuc.at<Vec3b>(y, x)[2] = B;
        }
    }

    return sonuc;
}


// RGB -> BGR dönüşümü
Mat rgbToBgr(const Mat& resim)
{
    Mat sonuc(
        resim.rows,
        resim.cols,
        CV_8UC3
    );

    for (int y = 0; y < resim.rows; y++)
    {
        for (int x = 0; x < resim.cols; x++)
        {
            Vec3b piksel = resim.at<Vec3b>(y, x);

            int R = piksel[0];
            int G = piksel[1];
            int B = piksel[2];

            sonuc.at<Vec3b>(y, x)[0] = B;
            sonuc.at<Vec3b>(y, x)[1] = G;
            sonuc.at<Vec3b>(y, x)[2] = R;
        }
    }

    return sonuc;
}


// 3 kanallı iki görüntünün toplam mutlak hatası
long long hataHesapla(
    const Mat& resim1,
    const Mat& resim2)
{
    long long toplamHata = 0;

    for (int y = 0; y < resim1.rows; y++)
    {
        for (int x = 0; x < resim1.cols; x++)
        {
            Vec3b p1 = resim1.at<Vec3b>(y, x);
            Vec3b p2 = resim2.at<Vec3b>(y, x);

            for (int kanal = 0; kanal < 3; kanal++)
            {
                toplamHata += abs(
                    (int)p1[kanal] -
                    (int)p2[kanal]
                );
            }
        }
    }

    return toplamHata;
}


int main()
{
    string giris =
        "/content/goruntu-isleme-dersi/Hafta-05/input/girdi.png";

    string cikis =
        "/content/goruntu-isleme-dersi/Hafta-05/04-Renk-Uzayi-Donusumu/";


    Mat resim =
        imread(giris, IMREAD_COLOR);

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


    // ----------------------------------
    // BGR -> RGB
    // ----------------------------------

    DonusumFonksiyonu donusum;

    // Function pointer bizim fonksiyonumuzu gösteriyor
    donusum = bgrToRgb;

    Mat kendiRgb =
        donusum(resim);


    // OpenCV ile aynı dönüşüm
    Mat opencvRgb;

    cvtColor(
        resim,
        opencvRgb,
        COLOR_BGR2RGB
    );


    long long rgbHata =
        hataHesapla(
            kendiRgb,
            opencvRgb
        );


    // ----------------------------------
    // RGB -> BGR
    // ----------------------------------

    donusum = rgbToBgr;

    Mat kendiBgr =
        donusum(kendiRgb);


    Mat opencvBgr;

    cvtColor(
        opencvRgb,
        opencvBgr,
        COLOR_RGB2BGR
    );


    long long bgrHata =
        hataHesapla(
            kendiBgr,
            opencvBgr
        );


    // ----------------------------------
    // Fark görüntüleri
    // ----------------------------------

    Mat farkRgb;
    Mat farkBgr;

    absdiff(
        kendiRgb,
        opencvRgb,
        farkRgb
    );

    absdiff(
        kendiBgr,
        opencvBgr,
        farkBgr
    );


    // Sonuçları kaydet
    imwrite(
        cikis + "fark_rgb.png",
        farkRgb
    );

    imwrite(
        cikis + "fark_bgr.png",
        farkBgr
    );

    imwrite(
        cikis + "geri_donusturulmus_bgr.png",
        kendiBgr
    );


    // Hataları dosyaya yaz
    ofstream dosya(
        cikis + "hata.txt"
    );

    dosya
        << "BGR -> RGB hata: "
        << rgbHata
        << endl;

    dosya
        << "RGB -> BGR hata: "
        << bgrHata
        << endl;

    dosya.close();


    cout << endl;

    cout << "BGR -> RGB hata: "
         << rgbHata
         << endl;

    cout << "RGB -> BGR hata: "
         << bgrHata
         << endl;

    cout << endl;
    cout << "Sonuclar kaydedildi." << endl;

    return 0;
}
