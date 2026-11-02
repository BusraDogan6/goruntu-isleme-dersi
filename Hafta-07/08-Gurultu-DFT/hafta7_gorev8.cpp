
#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <fstream>
#include <random>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


// -------------------------------------------------
// Gaussian gurultu ekleme
// Kuvvet parametresi standart sapmadir
// -------------------------------------------------

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
                + gurultu;


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


// -------------------------------------------------
// Salt & Pepper gurultu ekleme
// oran 0 ile 1 arasinda olmalidir
// -------------------------------------------------

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


            // Pepper -> siyah
            if (rastgele < oran / 2.0)
            {
                sonuc.at<uchar>(y, x) = 0;
            }

            // Salt -> beyaz
            else if (rastgele < oran)
            {
                sonuc.at<uchar>(y, x) = 255;
            }
        }
    }


    return sonuc;
}


// -------------------------------------------------
// Manuel 2 Boyutlu DFT
// -------------------------------------------------

vector<vector<complex<double>>> DFT2D(
    const Mat& resim)
{
    int M = resim.rows;
    int N = resim.cols;


    vector<vector<complex<double>>> harmonikler(
        M,
        vector<complex<double>>(N)
    );


    for (int u = 0; u < M; u++)
    {
        for (int v = 0; v < N; v++)
        {
            complex<double> toplam(
                0.0,
                0.0
            );


            for (int x = 0; x < M; x++)
            {
                for (int y = 0; y < N; y++)
                {
                    double aci =
                        -2.0 * M_PI *
                        (
                            ((double)u * x / M)
                            +
                            ((double)v * y / N)
                        );


                    complex<double> harmonik(
                        cos(aci),
                        sin(aci)
                    );


                    double piksel =
                        resim.at<uchar>(x, y);


                    toplam +=
                        piksel * harmonik;
                }
            }


            harmonikler[u][v] =
                toplam;
        }
    }


    return harmonikler;
}


// -------------------------------------------------
// Harmonik genlik matrisini TXT olarak kaydet
// -------------------------------------------------

void harmonikKaydet(
    const vector<vector<complex<double>>>& harmonikler,
    string dosyaAdi)
{
    ofstream dosya(dosyaAdi);


    int satir =
        harmonikler.size();

    int sutun =
        harmonikler[0].size();


    for (int u = 0; u < satir; u++)
    {
        for (int v = 0; v < sutun; v++)
        {
            double genlik =
                abs(harmonikler[u][v]);


            dosya
                << genlik;


            if (v < sutun - 1)
            {
                dosya << " ";
            }
        }


        dosya << endl;
    }


    dosya.close();
}



int main()
{
    string klasor =
        "/content/goruntu-isleme-dersi/Hafta-07/08-Gurultu-DFT/";


    Mat resim =
        imread(
            klasor + "input/girdi.png",
            IMREAD_GRAYSCALE
        );


    if (resim.empty())
    {
        cout << "Resim bulunamadi!" << endl;
        return -1;
    }


    // Manuel DFT cok maliyetli oldugu icin
    // resmi 64x64 yapiyoruz

    Mat orijinal;

    resize(
        resim,
        orijinal,
        Size(64, 64)
    );


    // --------------------------------------
    // Gaussian gurultu
    // --------------------------------------

    double gaussianKuvvet = 25.0;


    Mat gaussianGurultulu =
        gaussianGurultuEkle(
            orijinal,
            gaussianKuvvet
        );


    // --------------------------------------
    // Salt & Pepper gurultu
    // --------------------------------------

    double saltPepperOran = 0.10;


    Mat saltPepperGurultulu =
        saltPepperGurultuEkle(
            orijinal,
            saltPepperOran
        );


    // Resimleri kaydet

    imwrite(
        klasor + "01_orijinal.png",
        orijinal
    );


    imwrite(
        klasor + "02_gaussian_gurultulu.png",
        gaussianGurultulu
    );


    imwrite(
        klasor + "03_salt_pepper_gurultulu.png",
        saltPepperGurultulu
    );


    cout << "Gaussian gurultu kuvveti: "
         << gaussianKuvvet
         << endl;


    cout << "Salt & Pepper gurultu orani: "
         << saltPepperOran
         << endl;


    // --------------------------------------
    // Orijinal goruntu DFT
    // --------------------------------------

    cout << endl;
    cout << "Orijinal goruntu DFT hesaplaniyor..."
         << endl;


    vector<vector<complex<double>>> orijinalDFT =
        DFT2D(orijinal);


    harmonikKaydet(
        orijinalDFT,
        klasor + "orijinal_harmonik.txt"
    );


    cout << "Orijinal DFT tamamlandi."
         << endl;


    // --------------------------------------
    // Gaussian gurultulu goruntu DFT
    // --------------------------------------

    cout << endl;
    cout << "Gaussian gurultulu goruntu DFT hesaplaniyor..."
         << endl;


    vector<vector<complex<double>>> gaussianDFT =
        DFT2D(gaussianGurultulu);


    harmonikKaydet(
        gaussianDFT,
        klasor + "gaussian_harmonik.txt"
    );


    cout << "Gaussian DFT tamamlandi."
         << endl;


    // --------------------------------------
    // Salt & Pepper gurultulu goruntu DFT
    // --------------------------------------

    cout << endl;
    cout << "Salt & Pepper goruntu DFT hesaplaniyor..."
         << endl;


    vector<vector<complex<double>>> saltPepperDFT =
        DFT2D(saltPepperGurultulu);


    harmonikKaydet(
        saltPepperDFT,
        klasor + "salt_pepper_harmonik.txt"
    );


    cout << "Salt & Pepper DFT tamamlandi."
         << endl;


    cout << endl;
    cout << "Tum islemler tamamlandi."
         << endl;


    return 0;
}
