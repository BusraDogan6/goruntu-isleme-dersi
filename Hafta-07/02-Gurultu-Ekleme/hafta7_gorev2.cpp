
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

void sinusOlustur(vector<double>& veri,
                  double genlik,
                  double faz,
                  double frekans)
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


void gurultuEkle(vector<double>& veri,
                 double gurultuOrani)
{
    if (gurultuOrani < 0)
        gurultuOrani = 0;

    if (gurultuOrani > 1)
        gurultuOrani = 1;


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

        double gurultu =
            rastgele * gurultuGenligi;

        veri[i] =
            veri[i] + gurultu;
    }
}


int main()
{
    srand(time(0));

    int N = 256;

    vector<double> temizVeri(N);

    double genlik = 2.0;
    double faz = M_PI / 4.0;
    double frekans = 5.0;

    sinusOlustur(
        temizVeri,
        genlik,
        faz,
        frekans
    );


    vector<double> gurultuluVeri =
        temizVeri;

    double gurultuOrani = 0.30;

    gurultuEkle(
        gurultuluVeri,
        gurultuOrani
    );


    string klasor =
        "/content/goruntu-isleme-dersi/Hafta-07/02-Gurultu-Ekleme/";


    ofstream temizDosya(
        klasor + "temiz_sinus.txt"
    );

    ofstream gurultuluDosya(
        klasor + "gurultulu_sinus.txt"
    );


    for (int i = 0; i < N; i++)
    {
        temizDosya
            << i << " "
            << temizVeri[i]
            << endl;

        gurultuluDosya
            << i << " "
            << gurultuluVeri[i]
            << endl;
    }


    temizDosya.close();
    gurultuluDosya.close();


    cout << "Vektor boyutu: " << N << endl;
    cout << "Gurultu orani: " << gurultuOrani << endl;
    cout << "Gurultulu veri olusturuldu." << endl;

    return 0;
}
