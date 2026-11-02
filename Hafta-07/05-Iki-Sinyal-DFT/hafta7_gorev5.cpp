
#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;


// Sinus sinyali olusturur
void sinusOlustur(
    vector<double>& veri,
    double genlik,
    double frekans,
    double faz)
{
    int N = veri.size();

    for (int i = 0; i < N; i++)
    {
        double t = (double)i / N;

        veri[i] =
            genlik *
            sin(2.0 * M_PI * frekans * t + faz);
    }
}


// Sinyale gurultu ekler
void gurultuEkle(
    vector<double>& veri,
    double gurultuOrani)
{
    double maxGenlik = 0;


    for (int i = 0; i < veri.size(); i++)
    {
        double deger = abs(veri[i]);

        if (deger > maxGenlik)
        {
            maxGenlik = deger;
        }
    }


    double gurultuGenligi =
        maxGenlik * gurultuOrani;


    for (int i = 0; i < veri.size(); i++)
    {
        double rastgele =
            (double)rand() / RAND_MAX;


        rastgele =
            rastgele * 2.0 - 1.0;


        veri[i] +=
            rastgele * gurultuGenligi;
    }
}


// Manuel DFT
vector<complex<double>> DFT(
    const vector<double>& veri)
{
    int N = veri.size();

    vector<complex<double>> harmonikler(N);


    for (int k = 0; k < N; k++)
    {
        complex<double> toplam(0.0, 0.0);


        for (int n = 0; n < N; n++)
        {
            double aci =
                -2.0 * M_PI * k * n / N;


            complex<double> deger(
                cos(aci),
                sin(aci)
            );


            toplam +=
                veri[n] * deger;
        }


        harmonikler[k] = toplam;
    }


    return harmonikler;
}


// Sinyali txt dosyasina kaydet
void sinyalKaydet(
    const vector<double>& veri,
    string dosyaAdi)
{
    ofstream dosya(dosyaAdi);


    for (int i = 0; i < veri.size(); i++)
    {
        dosya
            << i << " "
            << veri[i]
            << endl;
    }


    dosya.close();
}


// DFT sonucunu kaydet
void dftKaydet(
    const vector<complex<double>>& harmonikler,
    string dosyaAdi)
{
    int N = harmonikler.size();

    ofstream dosya(dosyaAdi);


    for (int k = 0; k < N; k++)
    {
        double genlik =
            abs(harmonikler[k]) / N;


        dosya
            << k << " "
            << genlik
            << endl;
    }


    dosya.close();
}



int main()
{
    srand(time(0));


    int N = 256;


    vector<double> sinyal1(N);
    vector<double> sinyal2(N);


    // 1. sinyal
    double frekans1 = 50.0;
    double genlik1 = 12.0;


    // 2. sinyal
    double frekans2 = 30.0;
    double genlik2 = 5.0;


    double faz = 0.0;


    // Gurultu orani
    double gurultuOrani = 0.20;


    // Sinusleri olustur
    sinusOlustur(
        sinyal1,
        genlik1,
        frekans1,
        faz
    );


    sinusOlustur(
        sinyal2,
        genlik2,
        frekans2,
        faz
    );


    // Iki sinyale de gurultu ekle
    gurultuEkle(
        sinyal1,
        gurultuOrani
    );


    gurultuEkle(
        sinyal2,
        gurultuOrani
    );


    // Ayri ayri DFT
    vector<complex<double>> dft1 =
        DFT(sinyal1);


    vector<complex<double>> dft2 =
        DFT(sinyal2);


    // Iki sinyali topla
    vector<double> toplamSinyal(N);


    for (int i = 0; i < N; i++)
    {
        toplamSinyal[i] =
            sinyal1[i] + sinyal2[i];
    }


    // Toplam sinyalin DFT'si
    vector<complex<double>> toplamDFT =
        DFT(toplamSinyal);


    string klasor =
        "/content/goruntu-isleme-dersi/Hafta-07/05-Iki-Sinyal-DFT/";


    // Sinyalleri kaydet
    sinyalKaydet(
        sinyal1,
        klasor + "sinyal1.txt"
    );


    sinyalKaydet(
        sinyal2,
        klasor + "sinyal2.txt"
    );


    sinyalKaydet(
        toplamSinyal,
        klasor + "toplam_sinyal.txt"
    );


    // DFT sonuclarini kaydet
    dftKaydet(
        dft1,
        klasor + "sinyal1_dft.txt"
    );


    dftKaydet(
        dft2,
        klasor + "sinyal2_dft.txt"
    );


    dftKaydet(
        toplamDFT,
        klasor + "toplam_dft.txt"
    );


    cout << "1. sinyal:" << endl;
    cout << "Frekans = " << frekans1 << endl;
    cout << "Genlik = " << genlik1 << endl;


    cout << endl;


    cout << "2. sinyal:" << endl;
    cout << "Frekans = " << frekans2 << endl;
    cout << "Genlik = " << genlik2 << endl;


    cout << endl;


    cout << "Gurultu orani = "
         << gurultuOrani
         << endl;


    cout << "DFT analizleri tamamlandi."
         << endl;


    return 0;
}
