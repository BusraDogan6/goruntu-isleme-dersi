
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
        "/content/goruntu-isleme-dersi/Hafta-06/input/girdi_esmerkezli.png";

    string cikis =
        "/content/goruntu-isleme-dersi/Hafta-06/05-Es-Merkezli-Cemberler/";

    Mat resim = imread(giris);

    if (resim.empty())
    {
        cout << "Resim okunamadi!" << endl;
        return -1;
    }


    int genislik = resim.cols;
    int yukseklik = resim.rows;


    // ----------------------------
    // 1. Gri
    // ----------------------------

    Mat gri;

    cvtColor(
        resim,
        gri,
        COLOR_BGR2GRAY
    );


    // ----------------------------
    // 2. Canny
    // ----------------------------

    Mat canny;

    Canny(
        gri,
        canny,
        50,
        150
    );


    // ----------------------------
    // 3. Median
    // ----------------------------

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


    // ----------------------------
    // 4. Sin ve cos tablosu
    // ----------------------------

    int aciAdimi = 4;

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


    // ----------------------------
    // 5. Yarıçap aralığı
    // ----------------------------

    int minYaricap = 10;
    int maxYaricap = 120;
    int yaricapAdimi = 2;


    int yaricapSayisi =
        ((maxYaricap - minYaricap)
        / yaricapAdimi) + 1;


    cout << "Yaricap sayisi: "
         << yaricapSayisi
         << endl;


    // ----------------------------
    // 6. 3 boyutlu oylama matrisi
    //
    // [yaricap][merkezY][merkezX]
    // ----------------------------

    size_t hucreSayisi =
        (size_t)yaricapSayisi *
        yukseklik *
        genislik;


    vector<unsigned short> oylama(
        hucreSayisi,
        0
    );


    // Aynı merkeze farklı yarıçaplardan
    // gelen oyları burada topluyoruz.
    vector<int> merkezOylari(
        genislik * yukseklik,
        0
    );


    cout << "3 boyutlu oylama matrisi olusturuldu."
         << endl;


    // ----------------------------
    // 7. Oylama
    // ----------------------------

    for (int y = 0; y < yukseklik; y++)
    {
        for (int x = 0; x < genislik; x++)
        {
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


                        // 3D oylama
                        oylama[index]++;


                        // Aynı merkeze farklı yarıçaplardan
                        // gelen oyları direkt oylama sırasında topluyoruz
                        int merkezIndex =
                            merkezY * genislik +
                            merkezX;

                        merkezOylari[merkezIndex]++;
                    }
                }
            }
        }
    }


    cout << "Oylama tamamlandi." << endl;


    // ----------------------------
    // 8. En güçlü ortak merkezi bul
    // ----------------------------

    int enYuksekMerkezOy = 0;

    int enIyiX = 0;
    int enIyiY = 0;


    for (int y = 0; y < yukseklik; y++)
    {
        for (int x = 0; x < genislik; x++)
        {
            int oy =
                merkezOylari[
                    y * genislik + x
                ];


            if (oy > enYuksekMerkezOy)
            {
                enYuksekMerkezOy = oy;

                enIyiX = x;
                enIyiY = y;
            }
        }
    }


    cout << "Es merkez: "
         << enIyiX
         << ", "
         << enIyiY
         << endl;

    cout << "Merkez oy sayisi: "
         << enYuksekMerkezOy
         << endl;


    // ----------------------------
    // 9. Merkez oylama görüntüsü
    // ----------------------------

    Mat merkezOylamaResmi(
        yukseklik,
        genislik,
        CV_8UC1,
        Scalar(0)
    );


    for (int y = 0; y < yukseklik; y++)
    {
        for (int x = 0; x < genislik; x++)
        {
            int oy =
                merkezOylari[
                    y * genislik + x
                ];


            int deger = 0;


            if (enYuksekMerkezOy > 0)
            {
                deger =
                    oy *
                    255 /
                    enYuksekMerkezOy;
            }


            merkezOylamaResmi.at<uchar>(y, x) =
                (uchar)deger;
        }
    }


    imwrite(
        cikis + "03_esmerkez_oylama.png",
        merkezOylamaResmi
    );


    // ----------------------------
    // 10. Seçilen merkez için
    //     yarıçap oylarını bul
    // ----------------------------

    vector<int> yaricapOylari(
        yaricapSayisi,
        0
    );


    int enYuksekYaricapOy = 0;


    for (int rIndex = 0;
         rIndex < yaricapSayisi;
         rIndex++)
    {
        size_t index =
            (
                (size_t)rIndex *
                yukseklik +
                enIyiY
            )
            * genislik +
            enIyiX;


        yaricapOylari[rIndex] =
            oylama[index];


        if (yaricapOylari[rIndex] >
            enYuksekYaricapOy)
        {
            enYuksekYaricapOy =
                yaricapOylari[rIndex];
        }
    }


    cout << "En yuksek yaricap oyu: "
         << enYuksekYaricapOy
         << endl;


    // ----------------------------
    // 11. Yarıçap oylarını
    //     resim olarak göster
    // ----------------------------

    Mat yaricapGrafik(
        400,
        600,
        CV_8UC3,
        Scalar(255, 255, 255)
    );


    int grafikGenislik =
        yaricapGrafik.cols;


    for (int i = 1;
         i < yaricapSayisi;
         i++)
    {
        int x1 =
            (i - 1) *
            grafikGenislik /
            yaricapSayisi;


        int x2 =
            i *
            grafikGenislik /
            yaricapSayisi;


        int y1 =
            399;

        int y2 =
            399;


        if (enYuksekYaricapOy > 0)
        {
            y1 =
                399 -
                yaricapOylari[i - 1] *
                350 /
                enYuksekYaricapOy;


            y2 =
                399 -
                yaricapOylari[i] *
                350 /
                enYuksekYaricapOy;
        }


        line(
            yaricapGrafik,
            Point(x1, y1),
            Point(x2, y2),
            Scalar(0, 0, 0),
            2
        );
    }


    imwrite(
        cikis + "04_yaricap_oylari.png",
        yaricapGrafik
    );


    // ----------------------------
    // 12. Yarıçap eşik değeri
    // ----------------------------

    int esik =
        (int)(
            enYuksekYaricapOy * 0.60
        );


    if (esik < 8)
        esik = 8;


    cout << "Yaricap esigi: "
         << esik
         << endl;


    // ----------------------------
    // 13. Aynı merkezdeki
    //     yarıçap zirvelerini bul
    // ----------------------------

    vector<int> bulunanYaricaplar;


    for (int rIndex = 1;
         rIndex < yaricapSayisi - 1;
         rIndex++)
    {
        int oy =
            yaricapOylari[rIndex];


        if (oy < esik)
            continue;


        // Yarıçap ekseninde yerel maksimum
        if (oy >= yaricapOylari[rIndex - 1] &&
            oy >= yaricapOylari[rIndex + 1])
        {
            int yaricap =
                minYaricap +
                rIndex *
                yaricapAdimi;


            bulunanYaricaplar.push_back(
                yaricap
            );
        }
    }


    // ----------------------------
    // 14. Eş merkezli çemberleri çiz
    // ----------------------------

    Mat sonuc =
        resim.clone();


    Point merkez(
        enIyiX,
        enIyiY
    );


    for (int i = 0;
         i < bulunanYaricaplar.size();
         i++)
    {
        int yaricap =
            bulunanYaricaplar[i];


        circle(
            sonuc,
            merkez,
            yaricap,
            Scalar(0, 255, 0),
            2
        );
    }


    // Ortak merkez kırmızı
    circle(
        sonuc,
        merkez,
        4,
        Scalar(0, 0, 255),
        -1
    );


    imwrite(
        cikis + "05_esmerkezli_sonuc.png",
        sonuc
    );


    // ----------------------------
    // 15. Sonuçları yaz
    // ----------------------------

    ofstream dosya(
        cikis + "sonuclar.txt"
    );


    dosya
        << "Es merkez X: "
        << enIyiX
        << endl;

    dosya
        << "Es merkez Y: "
        << enIyiY
        << endl;

    dosya
        << "Merkez oy sayisi: "
        << enYuksekMerkezOy
        << endl;

    dosya
        << "Yaricap esigi: "
        << esik
        << endl;

    dosya
        << "Bulunan es merkezli cember sayisi: "
        << bulunanYaricaplar.size()
        << endl;


    dosya << "Yaricaplar: ";


    for (int yaricap :
         bulunanYaricaplar)
    {
        dosya
            << yaricap
            << " ";
    }


    dosya << endl;

    dosya.close();


    cout << "Bulunan es merkezli cember sayisi: "
         << bulunanYaricaplar.size()
         << endl;


    cout << "Yaricaplar: ";


    for (int yaricap :
         bulunanYaricaplar)
    {
        cout
            << yaricap
            << " ";
    }


    cout << endl;

    cout << "Sonuclar kaydedildi."
         << endl;


    return 0;
}
