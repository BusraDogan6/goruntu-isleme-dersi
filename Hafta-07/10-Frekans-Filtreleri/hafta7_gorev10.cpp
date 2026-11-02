
#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <fstream>
#include <random>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

typedef vector<vector<complex<double>>> KarmaMatris;


// =====================================================
// GAUSSIAN GURULTU EKLE
// =====================================================

Mat gaussianGurultuEkle(
    const Mat& resim,
    double kuvvet)
{
    Mat sonuc = resim.clone();

    random_device rd;
    mt19937 generator(rd());

    normal_distribution<double> dagilim(
        0.0,
        kuvvet
    );


    for (int y = 0; y < resim.rows; y++)
    {
        for (int x = 0; x < resim.cols; x++)
        {
            double gurultu =
                dagilim(generator);


            int yeniDeger =
                resim.at<uchar>(y, x)
                +
                gurultu;


            if (yeniDeger < 0)
                yeniDeger = 0;

            if (yeniDeger > 255)
                yeniDeger = 255;


            sonuc.at<uchar>(y, x) =
                yeniDeger;
        }
    }


    return sonuc;
}


// =====================================================
// SALT & PEPPER GURULTU EKLE
// =====================================================

Mat saltPepperGurultuEkle(
    const Mat& resim,
    double oran)
{
    Mat sonuc = resim.clone();

    random_device rd;
    mt19937 generator(rd());

    uniform_real_distribution<double> dagilim(
        0.0,
        1.0
    );


    for (int y = 0; y < resim.rows; y++)
    {
        for (int x = 0; x < resim.cols; x++)
        {
            double rastgele =
                dagilim(generator);


            if (rastgele < oran / 2.0)
            {
                // Pepper
                sonuc.at<uchar>(y, x) = 0;
            }
            else if (rastgele < oran)
            {
                // Salt
                sonuc.at<uchar>(y, x) = 255;
            }
        }
    }


    return sonuc;
}


// =====================================================
// 1 BOYUTLU DFT / IDFT
// =====================================================

vector<complex<double>> DFT1D(
    const vector<complex<double>>& veri,
    bool ters)
{
    int N = veri.size();

    vector<complex<double>> sonuc(N);


    for (int k = 0; k < N; k++)
    {
        complex<double> toplam(0.0, 0.0);


        for (int n = 0; n < N; n++)
        {
            double isaret;

            if (ters)
                isaret = 1.0;
            else
                isaret = -1.0;


            double aci =
                isaret *
                2.0 * M_PI *
                k * n / N;


            complex<double> w(
                cos(aci),
                sin(aci)
            );


            toplam +=
                veri[n] * w;
        }


        if (ters)
        {
            toplam /=
                (double)N;
        }


        sonuc[k] =
            toplam;
    }


    return sonuc;
}


// =====================================================
// 2 BOYUTLU DFT
// =====================================================

KarmaMatris DFT2D(
    const Mat& resim)
{
    int M = resim.rows;
    int N = resim.cols;


    KarmaMatris gecici(
        M,
        vector<complex<double>>(N)
    );


    KarmaMatris sonuc(
        M,
        vector<complex<double>>(N)
    );


    // Once satirlar
    for (int y = 0; y < M; y++)
    {
        vector<complex<double>> satir(N);


        for (int x = 0; x < N; x++)
        {
            satir[x] =
                complex<double>(
                    resim.at<uchar>(y, x),
                    0.0
                );
        }


        gecici[y] =
            DFT1D(
                satir,
                false
            );
    }


    // Sonra sutunlar
    for (int x = 0; x < N; x++)
    {
        vector<complex<double>> sutun(M);


        for (int y = 0; y < M; y++)
        {
            sutun[y] =
                gecici[y][x];
        }


        vector<complex<double>> sonucSutun =
            DFT1D(
                sutun,
                false
            );


        for (int y = 0; y < M; y++)
        {
            sonuc[y][x] =
                sonucSutun[y];
        }
    }


    return sonuc;
}


// =====================================================
// 2 BOYUTLU IDFT
// =====================================================

Mat IDFT2D(
    const KarmaMatris& F)
{
    int M = F.size();
    int N = F[0].size();


    KarmaMatris gecici(
        M,
        vector<complex<double>>(N)
    );


    KarmaMatris sonuc(
        M,
        vector<complex<double>>(N)
    );


    // Satir IDFT
    for (int y = 0; y < M; y++)
    {
        gecici[y] =
            DFT1D(
                F[y],
                true
            );
    }


    // Sutun IDFT
    for (int x = 0; x < N; x++)
    {
        vector<complex<double>> sutun(M);


        for (int y = 0; y < M; y++)
        {
            sutun[y] =
                gecici[y][x];
        }


        vector<complex<double>> sonucSutun =
            DFT1D(
                sutun,
                true
            );


        for (int y = 0; y < M; y++)
        {
            sonuc[y][x] =
                sonucSutun[y];
        }
    }


    Mat goruntu(
        M,
        N,
        CV_64F
    );


    for (int y = 0; y < M; y++)
    {
        for (int x = 0; x < N; x++)
        {
            goruntu.at<double>(y, x) =
                sonuc[y][x].real();
        }
    }


    return goruntu;
}


// =====================================================
// FREKANSLARI MERKEZE AL
// =====================================================

KarmaMatris frekansKaydir(
    const KarmaMatris& F)
{
    int M = F.size();
    int N = F[0].size();


    KarmaMatris sonuc(
        M,
        vector<complex<double>>(N)
    );


    for (int y = 0; y < M; y++)
    {
        for (int x = 0; x < N; x++)
        {
            int yeniY =
                (y + M / 2) % M;

            int yeniX =
                (x + N / 2) % N;


            sonuc[yeniY][yeniX] =
                F[y][x];
        }
    }


    return sonuc;
}


// =====================================================
// LOW PASS
//
// Dusuk frekanslari koru.
// Yuksek frekanslari kaldir.
// =====================================================

void lowPass(
    KarmaMatris& F,
    double D0)
{
    int M = F.size();
    int N = F[0].size();

    int merkezY = M / 2;
    int merkezX = N / 2;


    for (int y = 0; y < M; y++)
    {
        for (int x = 0; x < N; x++)
        {
            double dx =
                x - merkezX;

            double dy =
                y - merkezY;


            double D =
                sqrt(
                    dx * dx +
                    dy * dy
                );


            if (D > D0)
            {
                F[y][x] =
                    complex<double>(0.0, 0.0);
            }
        }
    }
}


// =====================================================
// HIGH PASS
//
// Dusuk frekanslari kaldir.
// Yuksek frekanslari koru.
// =====================================================

void highPass(
    KarmaMatris& F,
    double D0)
{
    int M = F.size();
    int N = F[0].size();

    int merkezY = M / 2;
    int merkezX = N / 2;


    for (int y = 0; y < M; y++)
    {
        for (int x = 0; x < N; x++)
        {
            double dx =
                x - merkezX;

            double dy =
                y - merkezY;


            double D =
                sqrt(
                    dx * dx +
                    dy * dy
                );


            if (D < D0)
            {
                F[y][x] =
                    complex<double>(0.0, 0.0);
            }
        }
    }
}


// =====================================================
// BAND PASS
//
// D1 ile D2 arasini koru.
// =====================================================

void bandPass(
    KarmaMatris& F,
    double D1,
    double D2)
{
    int M = F.size();
    int N = F[0].size();

    int merkezY = M / 2;
    int merkezX = N / 2;


    for (int y = 0; y < M; y++)
    {
        for (int x = 0; x < N; x++)
        {
            double dx =
                x - merkezX;

            double dy =
                y - merkezY;


            double D =
                sqrt(
                    dx * dx +
                    dy * dy
                );


            if (
                D < D1 ||
                D > D2
            )
            {
                F[y][x] =
                    complex<double>(0.0, 0.0);
            }
        }
    }
}


// =====================================================
// BAND REJECT
//
// D1-D2 arasini kaldir.
// Diger frekanslari koru.
// =====================================================

void bandReject(
    KarmaMatris& F,
    double D1,
    double D2)
{
    int M = F.size();
    int N = F[0].size();

    int merkezY = M / 2;
    int merkezX = N / 2;


    for (int y = 0; y < M; y++)
    {
        for (int x = 0; x < N; x++)
        {
            double dx =
                x - merkezX;

            double dy =
                y - merkezY;


            double D =
                sqrt(
                    dx * dx +
                    dy * dy
                );


            if (
                D >= D1 &&
                D <= D2
            )
            {
                F[y][x] =
                    complex<double>(0.0, 0.0);
            }
        }
    }
}


// =====================================================
// NORMAL GORUNTU KAYDET
// =====================================================

void normalKaydet(
    const Mat& resim,
    string dosyaAdi)
{
    Mat sonuc(
        resim.rows,
        resim.cols,
        CV_8UC1
    );


    for (int y = 0; y < resim.rows; y++)
    {
        for (int x = 0; x < resim.cols; x++)
        {
            double deger =
                resim.at<double>(y, x);


            if (deger < 0)
                deger = 0;

            if (deger > 255)
                deger = 255;


            sonuc.at<uchar>(y, x) =
                round(deger);
        }
    }


    imwrite(
        dosyaAdi,
        sonuc
    );
}


// =====================================================
// HIGH PASS VE BAND PASS GORSELLESTIRME
// =====================================================

void normalizeKaydet(
    const Mat& resim,
    string dosyaAdi)
{
    Mat normalizeSonuc;
    Mat sonuc8;


    normalize(
        resim,
        normalizeSonuc,
        0,
        255,
        NORM_MINMAX
    );


    normalizeSonuc.convertTo(
        sonuc8,
        CV_8UC1
    );


    imwrite(
        dosyaAdi,
        sonuc8
    );
}


// =====================================================
// BUTUN FILTRELERI BIR RESME UYGULA
// =====================================================

void filtreleriUygula(
    const Mat& resim,
    string isim,
    string cikis)
{
    cout
        << isim
        << " icin DFT hesaplaniyor..."
        << endl;


    KarmaMatris F =
        DFT2D(
            resim
        );


    KarmaMatris merkezli =
        frekansKaydir(
            F
        );


    // Filtre parametreleri
    double D0 =
        16.0;

    double D1 =
        8.0;

    double D2 =
        22.0;


    // ---------------------------
    // LOW PASS
    // ---------------------------

    KarmaMatris low =
        merkezli;


    lowPass(
        low,
        D0
    );


    low =
        frekansKaydir(
            low
        );


    Mat lowSonuc =
        IDFT2D(
            low
        );


    normalKaydet(
        lowSonuc,
        cikis + isim + "_low_pass.png"
    );


    // ---------------------------
    // HIGH PASS
    // ---------------------------

    KarmaMatris high =
        merkezli;


    highPass(
        high,
        D0
    );


    high =
        frekansKaydir(
            high
        );


    Mat highSonuc =
        IDFT2D(
            high
        );


    normalizeKaydet(
        highSonuc,
        cikis + isim + "_high_pass.png"
    );


    // ---------------------------
    // BAND PASS
    // ---------------------------

    KarmaMatris bandP =
        merkezli;


    bandPass(
        bandP,
        D1,
        D2
    );


    bandP =
        frekansKaydir(
            bandP
        );


    Mat bandPSonuc =
        IDFT2D(
            bandP
        );


    normalizeKaydet(
        bandPSonuc,
        cikis + isim + "_band_pass.png"
    );


    // ---------------------------
    // BAND REJECT
    // ---------------------------

    KarmaMatris bandR =
        merkezli;


    bandReject(
        bandR,
        D1,
        D2
    );


    bandR =
        frekansKaydir(
            bandR
        );


    Mat bandRSonuc =
        IDFT2D(
            bandR
        );


    normalKaydet(
        bandRSonuc,
        cikis + isim + "_band_reject.png"
    );


    cout
        << isim
        << " tamamlandi."
        << endl;
}


// =====================================================
// MAIN
// =====================================================

int main()
{
    string klasor =
        "/content/goruntu-isleme-dersi/Hafta-07/10-Frekans-Filtreleri/";


    Mat resim =
        imread(
            klasor + "input/girdi.png"
        );


    if (resim.empty())
    {
        cout << "Yeni fotograf bulunamadi!"
             << endl;

        return -1;
    }


    // Gri tona cevir
    Mat gri;

    cvtColor(
        resim,
        gri,
        COLOR_BGR2GRAY
    );


    // Manuel DFT icin 64x64
    Mat orijinal;

    resize(
        gri,
        orijinal,
        Size(64, 64)
    );


    // Yeni resme yeni gurultuler ekle
    double gaussianKuvvet =
        25.0;

    double saltPepperOran =
        0.10;


    Mat gaussian =
        gaussianGurultuEkle(
            orijinal,
            gaussianKuvvet
        );


    Mat saltPepper =
        saltPepperGurultuEkle(
            orijinal,
            saltPepperOran
        );


    // Girisleri kaydet
    imwrite(
        klasor + "01_orijinal.png",
        orijinal
    );


    imwrite(
        klasor + "02_gaussian_gurultulu.png",
        gaussian
    );


    imwrite(
        klasor + "03_salt_pepper_gurultulu.png",
        saltPepper
    );


    cout
        << "Gaussian gurultu kuvveti: "
        << gaussianKuvvet
        << endl;


    cout
        << "Salt & Pepper gurultu orani: "
        << saltPepperOran
        << endl;


    cout << endl;


    // Filtreleri uygula

    filtreleriUygula(
        orijinal,
        "orijinal",
        klasor
    );


    filtreleriUygula(
        gaussian,
        "gaussian",
        klasor
    );


    filtreleriUygula(
        saltPepper,
        "salt_pepper",
        klasor
    );


    cout << endl;

    cout << "Tum islemler tamamlandi."
         << endl;


    return 0;
}
