
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;
using namespace cv;


// İki renkli görüntü arasındaki ortalama mutlak fark
double ortalamaHataHesapla(
    const Mat& resim1,
    const Mat& resim2)
{
    long long toplamFark = 0;

    for (int y = 0; y < resim1.rows; y++)
    {
        for (int x = 0; x < resim1.cols; x++)
        {
            Vec3b p1 = resim1.at<Vec3b>(y, x);
            Vec3b p2 = resim2.at<Vec3b>(y, x);

            toplamFark += abs((int)p1[0] - (int)p2[0]);
            toplamFark += abs((int)p1[1] - (int)p2[1]);
            toplamFark += abs((int)p1[2] - (int)p2[2]);
        }
    }

    double pikselSayisi =
        resim1.rows *
        resim1.cols *
        3.0;

    return toplamFark / pikselSayisi;
}


int main()
{
    string inputKlasor =
        "/content/goruntu-isleme-dersi/Hafta-05/05-Xray-Renklendirme/input/";

    string outputKlasor =
        "/content/goruntu-isleme-dersi/Hafta-05/05-Xray-Renklendirme/output/";


    vector<string> dosyalar;

    // Input klasöründeki resimleri al
    glob(
        inputKlasor + "*",
        dosyalar,
        false
    );


    if (dosyalar.empty())
    {
        cout << "Input klasorunde resim bulunamadi!" << endl;
        return -1;
    }


    ofstream rapor(
        outputKlasor + "rapor.txt"
    );


    rapor << "5. Hafta - Gorev 5" << endl;
    rapor << "X-Ray gri donusum ve renklendirme" << endl;
    rapor << "-----------------------------------" << endl << endl;


    int sayac = 1;

    for (string dosya : dosyalar)
    {
        if (sayac > 5)
            break;


        Mat orijinal =
            imread(
                dosya,
                IMREAD_COLOR
            );


        if (orijinal.empty())
        {
            cout << "Resim okunamadi: "
                 << dosya
                 << endl;

            continue;
        }


        // ------------------------------
        // 1. RGB/BGR -> GRAY
        // ------------------------------

        Mat gri;

        cvtColor(
            orijinal,
            gri,
            COLOR_BGR2GRAY
        );


        // ------------------------------
        // 2. Gri görüntüyü renklendir
        // ------------------------------

        Mat renkli;

        applyColorMap(
            gri,
            renkli,
            COLORMAP_TURBO
        );


        // ------------------------------
        // 3. Orijinal ile fark
        // ------------------------------

        double hata =
            ortalamaHataHesapla(
                orijinal,
                renkli
            );


        // ------------------------------
        // Sonuçları kaydet
        // ------------------------------

        string numara =
            to_string(sayac);


        imwrite(
            outputKlasor +
            "xray_" +
            numara +
            "_gri.png",
            gri
        );


        imwrite(
            outputKlasor +
            "xray_" +
            numara +
            "_renkli.png",
            renkli
        );


        // Orijinal görüntüyü de karşılaştırma için kaydet
        imwrite(
            outputKlasor +
            "xray_" +
            numara +
            "_orijinal.png",
            orijinal
        );


        rapor
            << "Goruntu "
            << sayac
            << endl;

        rapor
            << "Ortalama mutlak fark: "
            << hata
            << endl;

        rapor
            << endl;


        cout
            << "Goruntu "
            << sayac
            << " tamamlandi. Hata: "
            << hata
            << endl;


        sayac++;
    }


    rapor << endl;
    rapor << "Renk paleti: TURBO" << endl;

    rapor << "Bu renk paletini secme nedenim: "
          << "Gri goruntudeki yogunluk farklarini "
          << "daha belirgin gostermesi ve nesnelerin "
          << "sinirlarini daha kolay ayirt etmeye yardimci olmasidir."
          << endl;

    rapor << endl;

    rapor << "Yorum: "
          << "Orijinal renkli X-ray goruntusu ile sonradan "
          << "renklendirilen goruntu birebir ayni degildir. "
          << "Bunun nedeni gri tonlamaya gecildiginde orijinal "
          << "renk bilgisinin kaybolmasidir. TURBO ile yapilan "
          << "renklendirme gri seviye yogunluklarina gore yapay "
          << "renk atamasi yapmaktadir."
          << endl;


    rapor.close();


    cout << endl;
    cout << "Tum goruntuler islendi." << endl;

    return 0;
}
