
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace std;


// Gaussian filtre
void gaussianFiltre(
    const vector<double>& giris,
    vector<double>& cikis)
{
    int N = giris.size();

    cikis.resize(N);

    double kernel[5] =
    {
        1.0 / 16.0,
        4.0 / 16.0,
        6.0 / 16.0,
        4.0 / 16.0,
        1.0 / 16.0
    };


    cikis[0] = giris[0];
    cikis[1] = giris[1];

    cikis[N - 2] = giris[N - 2];
    cikis[N - 1] = giris[N - 1];


    for (int i = 2; i < N - 2; i++)
    {
        double toplam = 0;

        for (int k = -2; k <= 2; k++)
        {
            toplam +=
                giris[i + k] *
                kernel[k + 2];
        }

        cikis[i] = toplam;
    }
}


// Median filtre
void medianFiltre(
    const vector<double>& giris,
    vector<double>& cikis)
{
    int N = giris.size();

    cikis.resize(N);


    cikis[0] = giris[0];
    cikis[1] = giris[1];

    cikis[N - 2] = giris[N - 2];
    cikis[N - 1] = giris[N - 1];


    for (int i = 2; i < N - 2; i++)
    {
        vector<double> pencere;

        for (int k = -2; k <= 2; k++)
        {
            pencere.push_back(
                giris[i + k]
            );
        }

        sort(
            pencere.begin(),
            pencere.end()
        );

        cikis[i] = pencere[2];
    }
}


int main()
{
    string girisDosya =
        "/content/goruntu-isleme-dersi/Hafta-07/02-Gurultu-Ekleme/gurultulu_sinus.txt";

    string cikisKlasor =
        "/content/goruntu-isleme-dersi/Hafta-07/03-Gauss-Median-Filtre/";


    ifstream dosya(girisDosya);

    if (!dosya.is_open())
    {
        cout << "Gurultulu veri bulunamadi!" << endl;
        return -1;
    }


    vector<double> gurultuluVeri;

    int index;
    double deger;


    while (dosya >> index >> deger)
    {
        gurultuluVeri.push_back(deger);
    }

    dosya.close();


    vector<double> gaussianSonuc;
    vector<double> medianSonuc;


    gaussianFiltre(
        gurultuluVeri,
        gaussianSonuc
    );


    medianFiltre(
        gaussianSonuc,
        medianSonuc
    );


    ofstream gaussDosya(
        cikisKlasor + "gaussian_sonuc.txt"
    );

    ofstream medianDosya(
        cikisKlasor + "median_sonuc.txt"
    );


    for (int i = 0; i < gurultuluVeri.size(); i++)
    {
        gaussDosya
            << i << " "
            << gaussianSonuc[i]
            << endl;

        medianDosya
            << i << " "
            << medianSonuc[i]
            << endl;
    }


    gaussDosya.close();
    medianDosya.close();


    cout << "Okunan veri sayisi: "
         << gurultuluVeri.size()
         << endl;

    cout << "Gaussian filtre uygulandi." << endl;

    cout << "Median filtre uygulandi." << endl;

    cout << "Sonuclar kaydedildi." << endl;


    return 0;
}
