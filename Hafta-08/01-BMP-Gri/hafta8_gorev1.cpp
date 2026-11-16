
#include <iostream>
#include <opencv2/opencv.hpp>
#include <filesystem>

using namespace std;
using namespace cv;

int main()
{
    string klasor =
        "/content/goruntu-isleme-dersi/Hafta-08/01-BMP-Gri/";

    string giris =
        klasor + "input/girdi.png";


    // Fotografı renkli olarak oku
    Mat resim =
        imread(
            giris,
            IMREAD_COLOR
        );


    if (resim.empty())
    {
        cout << "Resim bulunamadi!" << endl;
        return -1;
    }


    // 1000x1000 boyutuna getir
    Mat renkli1000;

    resize(
        resim,
        renkli1000,
        Size(1000, 1000)
    );


    // Bitmap olarak kaydet
    string renkliDosya =
        klasor + "renkli_1000x1000.bmp";


    imwrite(
        renkliDosya,
        renkli1000
    );


    // Kaydettigimiz BMP dosyasini tekrar ac
    Mat renkliBMP =
        imread(
            renkliDosya,
            IMREAD_COLOR
        );


    // Gri tona donustur
    Mat gri;

    cvtColor(
        renkliBMP,
        gri,
        COLOR_BGR2GRAY
    );


    // Gri resmi BMP olarak kaydet
    string griDosya =
        klasor + "gri_1000x1000.bmp";


    imwrite(
        griDosya,
        gri
    );


    // Diskteki gercek dosya boyutlari
    uintmax_t renkliBoyut =
        filesystem::file_size(
            renkliDosya
        );


    uintmax_t griBoyut =
        filesystem::file_size(
            griDosya
        );


    // Bilgileri yazdir
    cout << "RENKLI RESIM" << endl;

    cout << "Cozunurluk: "
         << renkliBMP.cols
         << " x "
         << renkliBMP.rows
         << endl;

    cout << "Kanal sayisi: "
         << renkliBMP.channels()
         << endl;

    cout << "Disk boyutu: "
         << renkliBoyut
         << " byte"
         << endl;


    cout << endl;


    cout << "GRI RESIM" << endl;

    cout << "Cozunurluk: "
         << gri.cols
         << " x "
         << gri.rows
         << endl;

    cout << "Kanal sayisi: "
         << gri.channels()
         << endl;

    cout << "Disk boyutu: "
         << griBoyut
         << " byte"
         << endl;


    cout << endl;


    cout << "Renkli / Gri dosya boyutu orani: "
         << (double)renkliBoyut / griBoyut
         << endl;


    return 0;
}
