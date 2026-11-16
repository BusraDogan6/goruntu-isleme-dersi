
#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <fstream>

using namespace std;
using namespace cv;
using namespace chrono;

int main()
{
    string klasor =
        "/content/goruntu-isleme-dersi/Hafta-08/02-Piksel-Yari/";

    Mat resim =
        imread(
            klasor + "input/girdi_4k.bmp",
            IMREAD_COLOR
        );

    if (resim.empty())
    {
        cout << "Resim bulunamadi!" << endl;
        return -1;
    }

    cout << "Resim boyutu: "
         << resim.cols
         << " x "
         << resim.rows
         << endl;

    cout << "Kanal sayisi: "
         << resim.channels()
         << endl;


    // Orijinali korumak icin kopya
    Mat sonuc =
        resim.clone();


    // ====================================
    // SADECE DONGULERIN SURESI BASLIYOR
    // ====================================

    auto baslangic =
        high_resolution_clock::now();


    for (int y = 0; y < sonuc.rows; y++)
    {
        for (int x = 0; x < sonuc.cols; x++)
        {
            Vec3b& piksel =
                sonuc.at<Vec3b>(y, x);

            // Blue
            piksel[0] =
                piksel[0] / 2;

            // Green
            piksel[1] =
                piksel[1] / 2;

            // Red
            piksel[2] =
                piksel[2] / 2;
        }
    }


    auto bitis =
        high_resolution_clock::now();

    // ====================================
    // DONGU SURESI BITTI
    // ====================================


    double sureMs =
        duration<double, milli>(
            bitis - baslangic
        ).count();


    double sureUs =
        duration<double, micro>(
            bitis - baslangic
        ).count();


    // Kaydetme sure hesabina dahil degil
    imwrite(
        klasor + "yarim_piksel_degerleri.bmp",
        sonuc
    );


    cout << endl;

    cout << "Tum piksel degerleri yariya dusuruldu."
         << endl;

    cout << "Sadece dongulerin suresi: "
         << sureMs
         << " ms"
         << endl;

    cout << "Sadece dongulerin suresi: "
         << sureUs
         << " mikro saniye"
         << endl;


    // Sureyi txt dosyasina kaydet
    ofstream dosya(
        klasor + "sure.txt"
    );

    dosya
        << "Resim boyutu: 3840x2160"
        << endl;

    dosya
        << "Kanal sayisi: 3"
        << endl;

    dosya
        << "Dongu suresi (ms): "
        << sureMs
        << endl;

    dosya
        << "Dongu suresi (mikro saniye): "
        << sureUs
        << endl;

    dosya.close();


    return 0;
}
