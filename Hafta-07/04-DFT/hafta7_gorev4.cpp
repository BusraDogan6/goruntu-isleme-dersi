
#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <fstream>

using namespace std;


// Vektor boyutu 2'nin kuvveti mi kontrol eder
bool ikininKuvvetiMi(int N)
{
    if (N <= 0)
        return false;

    return (N & (N - 1)) == 0;
}


// DFT fonksiyonu
vector<complex<double>> DFT(const vector<double>& veri)
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


            complex<double> usluIfade(
                cos(aci),
                sin(aci)
            );


            toplam +=
                veri[n] * usluIfade;
        }


        harmonikler[k] = toplam;
    }


    return harmonikler;
}


// txt dosyasindan sinyal oku
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


// Harmonikleri dosyaya kaydet
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
            << harmonikler[k].real() << " "
            << harmonikler[k].imag() << " "
            << genlik
            << endl;
    }


    dosya.close();
}



int main()
{
    string klasor2 =
        "/content/goruntu-isleme-dersi/Hafta-07/02-Gurultu-Ekleme/";

    string cikis =
        "/content/goruntu-isleme-dersi/Hafta-07/04-DFT/";


    // Temiz ve gurultulu sinyalleri oku
    vector<double> temizVeri =
        veriOku(
            klasor2 + "temiz_sinus.txt"
        );

    vector<double> gurultuluVeri =
        veriOku(
            klasor2 + "gurultulu_sinus.txt"
        );


    if (temizVeri.empty() ||
        gurultuluVeri.empty())
    {
        cout << "Girdi dosyalari bulunamadi!" << endl;
        return -1;
    }


    if (!ikininKuvvetiMi(temizVeri.size()))
    {
        cout << "Vektor boyutu 2'nin kuvveti degil!" << endl;
        return -1;
    }


    cout << "Vektor boyutu: "
         << temizVeri.size()
         << endl;


    // Temiz sinyalin DFT'si
    vector<complex<double>> temizHarmonik =
        DFT(temizVeri);


    // Gurultulu sinyalin DFT'si
    vector<complex<double>> gurultuluHarmonik =
        DFT(gurultuluVeri);


    harmonikKaydet(
        temizHarmonik,
        cikis + "temiz_dft.txt"
    );


    harmonikKaydet(
        gurultuluHarmonik,
        cikis + "gurultulu_dft.txt"
    );


    // En guclu frekansi bul
    int temizMaxIndex = 1;
    int gurultuluMaxIndex = 1;


    for (int k = 1;
         k < temizHarmonik.size() / 2;
         k++)
    {
        if (abs(temizHarmonik[k]) >
            abs(temizHarmonik[temizMaxIndex]))
        {
            temizMaxIndex = k;
        }


        if (abs(gurultuluHarmonik[k]) >
            abs(gurultuluHarmonik[gurultuluMaxIndex]))
        {
            gurultuluMaxIndex = k;
        }
    }


    cout << "Temiz sinyal en guclu harmonik: "
         << temizMaxIndex
         << endl;


    cout << "Gurultulu sinyal en guclu harmonik: "
         << gurultuluMaxIndex
         << endl;


    cout << "DFT islemleri tamamlandi."
         << endl;


    return 0;
}
