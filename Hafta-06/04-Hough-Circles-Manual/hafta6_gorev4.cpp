
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

using namespace std;
using namespace cv;

int main()
{
    string giris =
        "/content/goruntu-isleme-dersi/Hafta-06/input/girdi_cember.png";

    string cikis =
        "/content/goruntu-isleme-dersi/Hafta-06/04-Hough-Circles-Manual/";

    Mat resim = imread(giris);

    if (resim.empty())
    {
        cout << "Resim okunamadi!" << endl;
        return -1;
    }


    int genislik = resim.cols;
    int yukseklik = resim.rows;


    // Gri
    Mat gri;

    cvtColor(
        resim,
        gri,
        COLOR_BGR2GRAY
    );


    // Canny
    Mat canny;

    Canny(
        gri,
        canny,
        50,
        150
    );


    // Median
    Mat medianSonuc;

    medianBlur(
        canny,
        medianSonuc,
        3
    );


    imwrite(
        cikis + "01_canny.png",
        canny
    );

    imwrite(
        cikis + "02_median.png",
        medianSonuc
    );


    // -----------------------------
    // Sin ve cos tablosu
    // -----------------------------

    int aciAdimi = 2;

    int aciSayisi =
        360 / aciAdimi;


    vector<float> sinDegerleri(aciSayisi);
    vector<float> cosDegerleri(aciSayisi);


    for (int i = 0; i < aciSayisi; i++)
    {
        int derece =
            i * aciAdimi;

        float radyan =
            derece * CV_PI / 180.0f;


        sinDegerleri[i] =
            sin(radyan);

        cosDegerleri[i] =
            cos(radyan);
    }


    cout << "Sin ve cos tablolari hazir." << endl;


    // -----------------------------
    // Yarıçap değerleri
    // -----------------------------

    int minYaricap = 10;
    int maxYaricap = 100;
    int yaricapAdimi = 2;


    int yaricapSayisi =
        ((maxYaricap - minYaricap)
        / yaricapAdimi) + 1;


    cout << "Yaricap sayisi: "
         << yaricapSayisi
         << endl;


    // -----------------------------
    // 3 boyutlu oylama matrisi
    //
    // [yaricap][merkezY][merkezX]
    // -----------------------------

    size_t hucreSayisi =
        (size_t)yaricapSayisi *
        yukseklik *
        genislik;


    // unsigned short yeterli
    // çünkü bir hücrenin oy sayısı çok büyük değil
    vector<unsigned short> oylama(
        hucreSayisi,
        0
    );


    cout << "3 boyutlu oylama matrisi olusturuldu."
         << endl;


    // -----------------------------
    // HOUGH OYLAMA
    // -----------------------------

    for (int y = 0; y < yukseklik; y++)
    {
        for (int x = 0; x < genislik; x++)
        {
            // Sadece kenar pikseli
            if (medianSonuc.at<uchar>(y, x) == 0)
                continue;


            for (int rIndex = 0;
                 rIndex < yaricapSayisi;
                 rIndex++)
            {
                int yaricap =
                    minYaricap +
                    rIndex * yaricapAdimi;


                for (int aci = 0;
                     aci < aciSayisi;
                     aci++)
                {
                    int merkezX =
                        cvRound(
                            x -
                            yaricap *
                            cosDegerleri[aci]
                        );


                    int merkezY =
                        cvRound(
                            y -
                            yaricap *
                            sinDegerleri[aci]
                        );


                    if (merkezX >= 0 &&
                        merkezX < genislik &&
                        merkezY >= 0 &&
                        merkezY < yukseklik)
                    {
                        size_t index =
                            (
                                (size_t)rIndex *
                                yukseklik +
                                merkezY
                            )
                            * genislik +
                            merkezX;


                        oylama[index]++;
                    }
                }
            }
        }
    }


    cout << "Oylama tamamlandi." << endl;


    // -----------------------------
    // En yüksek oyu bul
    // -----------------------------

    int enYuksekOy = 0;


    for (size_t i = 0;
         i < oylama.size();
         i++)
    {
        if (oylama[i] > enYuksekOy)
        {
            enYuksekOy =
                oylama[i];
        }
    }


    cout << "En yuksek oy: "
         << enYuksekOy
         << endl;


    // -----------------------------
    // Oylama matrisini görüntüle
    //
    // 3D olduğu için her merkezde
    // yarıçaplar arasındaki en yüksek
    // oyu kullanıyoruz.
    // -----------------------------

    Mat oylamaResmi(
        yukseklik,
        genislik,
        CV_8UC1,
        Scalar(0)
    );


    for (int y = 0; y < yukseklik; y++)
    {
        for (int x = 0; x < genislik; x++)
        {
            int maxOy = 0;


            for (int rIndex = 0;
                 rIndex < yaricapSayisi;
                 rIndex++)
            {
                size_t index =
                    (
                        (size_t)rIndex *
                        yukseklik +
                        y
                    )
                    * genislik +
                    x;


                if (oylama[index] > maxOy)
                {
                    maxOy =
                        oylama[index];
                }
            }


            int deger = 0;


            if (enYuksekOy > 0)
            {
                deger =
                    maxOy *
                    255 /
                    enYuksekOy;
            }


            oylamaResmi.at<uchar>(y, x) =
                (uchar)deger;
        }
    }


    imwrite(
        cikis + "03_oylama_matrisi.png",
        oylamaResmi
    );


    // -----------------------------
    // Bir yarıçap dilimini de kaydet
    // -----------------------------

    int ornekRIndex =
        yaricapSayisi / 2;


    int ornekYaricap =
        minYaricap +
        ornekRIndex *
        yaricapAdimi;


    Mat yaricapDilim(
        yukseklik,
        genislik,
        CV_8UC1,
        Scalar(0)
    );


    for (int y = 0; y < yukseklik; y++)
    {
        for (int x = 0; x < genislik; x++)
        {
            size_t index =
                (
                    (size_t)ornekRIndex *
                    yukseklik +
                    y
                )
                * genislik +
                x;


            int deger = 0;


            if (enYuksekOy > 0)
            {
                deger =
                    oylama[index] *
                    255 /
                    enYuksekOy;
            }


            yaricapDilim.at<uchar>(y, x) =
                (uchar)deger;
        }
    }


    imwrite(
        cikis + "04_oylama_yaricap_dilimi.png",
        yaricapDilim
    );


    // -----------------------------
    // Eşik
    // -----------------------------

    int esik =
        (int)(enYuksekOy * 0.75);


    if (esik < 10)
        esik = 10;


    cout << "Esik degeri: "
         << esik
         << endl;


    // -----------------------------
    // Çemberleri bul
    // -----------------------------

    Mat sonuc =
        resim.clone();


    int bulunanCember = 0;


    for (int rIndex = 1;
         rIndex < yaricapSayisi - 1;
         rIndex++)
    {
        for (int y = 1;
             y < yukseklik - 1;
             y++)
        {
            for (int x = 1;
                 x < genislik - 1;
                 x++)
            {
                size_t index =
                    (
                        (size_t)rIndex *
                        yukseklik +
                        y
                    )
                    * genislik +
                    x;


                int oy =
                    oylama[index];


                if (oy < esik)
                    continue;


                // Yakın hücreler içinde maksimum mu?
                bool maksimum = true;


                for (int dr = -1; dr <= 1; dr++)
                {
                    for (int dy = -1; dy <= 1; dy++)
                    {
                        for (int dx = -1; dx <= 1; dx++)
                        {
                            if (dr == 0 &&
                                dy == 0 &&
                                dx == 0)
                                continue;


                            size_t komsuIndex =
                                (
                                    (size_t)(rIndex + dr) *
                                    yukseklik +
                                    (y + dy)
                                )
                                * genislik +
                                (x + dx);


                            if (oylama[komsuIndex] > oy)
                            {
                                maksimum = false;
                            }
                        }
                    }
                }


                if (!maksimum)
                    continue;


                int yaricap =
                    minYaricap +
                    rIndex *
                    yaricapAdimi;


                Point merkez(
                    x,
                    y
                );


                // Çember yeşil
                circle(
                    sonuc,
                    merkez,
                    yaricap,
                    Scalar(0, 255, 0),
                    2
                );


                // Merkez kırmızı
                circle(
                    sonuc,
                    merkez,
                    3,
                    Scalar(0, 0, 255),
                    -1
                );


                bulunanCember++;
            }
        }
    }


    imwrite(
        cikis + "05_hough_manual_sonuc.png",
        sonuc
    );


    cout << "Bulunan cember sayisi: "
         << bulunanCember
         << endl;


    // Sonuç dosyası
    ofstream dosya(
        cikis + "sonuclar.txt"
    );


    dosya
        << "Min yaricap: "
        << minYaricap
        << endl;

    dosya
        << "Max yaricap: "
        << maxYaricap
        << endl;

    dosya
        << "Aci adimi: "
        << aciAdimi
        << endl;

    dosya
        << "En yuksek oy: "
        << enYuksekOy
        << endl;

    dosya
        << "Esik: "
        << esik
        << endl;

    dosya
        << "Bulunan cember sayisi: "
        << bulunanCember
        << endl;

    dosya
        << "Ornek oylama yaricapi: "
        << ornekYaricap
        << endl;


    dosya.close();


    cout << "Sonuclar kaydedildi." << endl;

    return 0;
}
