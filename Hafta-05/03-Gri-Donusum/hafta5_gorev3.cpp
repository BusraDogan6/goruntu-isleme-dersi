
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;
using namespace cv;


// Kendi gri tonlama fonksiyonumuz
Mat griyeCevir(const Mat& renkli)
{
    Mat gri(
        renkli.rows,
        renkli.cols,
        CV_8UC1
    );

    for (int y = 0; y < renkli.rows; y++)
    {
        for (int x = 0; x < renkli.cols; x++)
        {
            Vec3b piksel = renkli.at<Vec3b>(y, x);

            int B = piksel[0];
            int G = piksel[1];
            int R = piksel[2];

            // OpenCV'nin 8-bit gri donusumune uygun hesap
            int griDeger =
                (
                    B * 3735 +
                    G * 19235 +
                    R * 9798 +
                    16384
                ) >> 15;

            gri.at<uchar>(y, x) =
                (uchar)griDeger;
        }
    }

    return gri;
}


// İki görüntü arasındaki toplam mutlak hata
long long hataHesapla(
    const Mat& resim1,
    const Mat& resim2)
{
    long long toplamHata = 0;

    for (int y = 0; y < resim1.rows; y++)
    {
        for (int x = 0; x < resim1.cols; x++)
        {
            int deger1 =
                resim1.at<uchar>(y, x);

            int deger2 =
                resim2.at<uchar>(y, x);

            int fark = abs(deger1 - deger2);

            toplamHata += fark;
        }
    }

    return toplamHata;
}


int main()
{
    string giris =
        "/content/goruntu-isleme-dersi/Hafta-05/input/girdi.png";

    string cikis =
        "/content/goruntu-isleme-dersi/Hafta-05/03-Gri-Donusum/";


    Mat renkli =
        imread(giris, IMREAD_COLOR);

    if (renkli.empty())
    {
        cout << "Resim okunamadi!" << endl;
        return -1;
    }


    cout << "Resim boyutu: "
         << renkli.cols
         << " x "
         << renkli.rows
         << endl;


    // ---------------------------------
    // 1. OpenCV hazır fonksiyon
    // ---------------------------------

    Mat opencvGri;

    cvtColor(
        renkli,
        opencvGri,
        COLOR_BGR2GRAY
    );


    // ---------------------------------
    // 2. Kendi yazdığımız fonksiyon
    // ---------------------------------

    Mat kendiGri =
        griyeCevir(renkli);


    // ---------------------------------
    // Hata hesabı
    // ---------------------------------

    long long toplamHata =
        hataHesapla(
            opencvGri,
            kendiGri
        );


    cout << endl;
    cout << "Toplam mutlak hata: "
         << toplamHata
         << endl;


    // ---------------------------------
    // Fark görüntüsü
    // ---------------------------------

    Mat farkResmi;

    absdiff(
        opencvGri,
        kendiGri,
        farkResmi
    );


    // Sonuçları kaydet
    imwrite(
        cikis + "opencv_gri.png",
        opencvGri
    );

    imwrite(
        cikis + "kendi_gri.png",
        kendiGri
    );

    imwrite(
        cikis + "fark.png",
        farkResmi
    );


    // Hata değerini dosyaya yaz
    ofstream dosya(
        cikis + "hata.txt"
    );

    dosya << "Toplam mutlak hata: "
          << toplamHata
          << endl;

    dosya.close();


    cout << "Sonuclar kaydedildi." << endl;

    return 0;
}
