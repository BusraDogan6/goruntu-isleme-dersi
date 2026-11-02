
#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <fstream>

using namespace std;


// ----------------------------
// TXT dosyasindan sinyal oku
// ----------------------------
vector<double> veriOku(string dosyaAdi)
{
    ifstream dosya(dosyaAdi);

    vector<double> veri;

    int index;
    double deger;

    while (dosya >> index >> deger)
    {
        veri.push_back(deger);
    }

    dosya.close();

    return veri;
}


// ----------------------------
// DFT
// ----------------------------
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


// ----------------------------
// Frekans filtresi
// Esik altindaki harmonikleri sil
// ----------------------------
void frekansFiltre(
    vector<complex<double>>& harmonikler,
    double esik)
{
    int N = harmonikler.size();


    for (int k = 0; k < N; k++)
    {
        // Normalize harmonik genligi
        double genlik =
            abs(harmonikler[k]) / N;


        if (genlik < esik)
        {
            harmonikler[k] =
                complex<double>(0.0, 0.0);
        }
    }
}


// ----------------------------
// IDFT
// Frekans uzayindan tekrar
// zaman uzayina donus
// ----------------------------
vector<double> IDFT(
    const vector<complex<double>>& harmonikler)
{
    int N = harmonikler.size();

    vector<double> sonuc(N);


    for (int n = 0; n < N; n++)
    {
        complex<double> toplam(0.0, 0.0);


        for (int k = 0; k < N; k++)
        {
            double aci =
                2.0 * M_PI * k * n / N;


            complex<double> deger(
                cos(aci),
                sin(aci)
            );


            toplam +=
                harmonikler[k] * deger;
        }


        toplam = toplam / (double)N;


        sonuc[n] = toplam.real();
    }


    return sonuc;
}


// ----------------------------
// Sinyali kaydet
// ----------------------------
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


// ----------------------------
// Harmonikleri kaydet
// ----------------------------
void harmonikKaydet(
    const vector<complex<double>>& harmonikler,
    string dosyaAdi)
{
    ofstream dosya(dosyaAdi);

    int N = harmonikler.size();


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


// ----------------------------
// Bir sinyali filtreleyen fonksiyon
// ----------------------------
void sinyalFiltrele(
    const vector<double>& sinyal,
    double esik,
    string isim,
    string cikisKlasor)
{
    // DFT
    vector<complex<double>> harmonikler =
        DFT(sinyal);


    // Filtre uygulanmadan once kaydet
    harmonikKaydet(
        harmonikler,
        cikisKlasor + isim + "_dft_once.txt"
    );


    // Esik altindaki harmonikleri sil
    frekansFiltre(
        harmonikler,
        esik
    );


    // Filtrelenmis harmonikleri kaydet
    harmonikKaydet(
        harmonikler,
        cikisKlasor + isim + "_dft_sonra.txt"
    );


    // IDFT ile sinyali geri olustur
    vector<double> filtreliSinyal =
        IDFT(harmonikler);


    sinyalKaydet(
        filtreliSinyal,
        cikisKlasor + isim + "_filtreli.txt"
    );


    cout << isim
         << " filtrelendi."
         << endl;
}


int main()
{
    string gorev5 =
        "/content/goruntu-isleme-dersi/Hafta-07/05-Iki-Sinyal-DFT/";

    string cikis =
        "/content/goruntu-isleme-dersi/Hafta-07/06-Frekans-Filtresi/";


    // 5. gorev sinyallerini oku
    vector<double> sinyal1 =
        veriOku(
            gorev5 + "sinyal1.txt"
        );

    vector<double> sinyal2 =
        veriOku(
            gorev5 + "sinyal2.txt"
        );

    vector<double> toplamSinyal =
        veriOku(
            gorev5 + "toplam_sinyal.txt"
        );


    if (sinyal1.empty() ||
        sinyal2.empty() ||
        toplamSinyal.empty())
    {
        cout << "5. gorev sinyalleri bulunamadi!"
             << endl;

        return -1;
    }


    // Harmonik genligi icin esik
    double esik = 1.0;


    cout << "Frekans filtresi esigi: "
         << esik
         << endl;


    // 50 Hz - Genlik 12
    sinyalFiltrele(
        sinyal1,
        esik,
        "sinyal1",
        cikis
    );


    // 30 Hz - Genlik 5
    sinyalFiltrele(
        sinyal2,
        esik,
        "sinyal2",
        cikis
    );


    // Iki sinyalin toplami
    sinyalFiltrele(
        toplamSinyal,
        esik,
        "toplam",
        cikis
    );


    cout << endl;

    cout << "Tum sinyaller filtrelendi."
         << endl;

    cout << "IDFT ile zaman uzayina geri donuldu."
         << endl;


    return 0;
}
