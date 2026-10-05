
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
    // Resmi grayscale olarak açıyoruz
    Mat resim = imread(
        "/content/input_image",
        IMREAD_GRAYSCALE
    );

    if (resim.empty())
    {
        cout << "Resim okunamadi!" << endl;
        return -1;
    }

    cout << "Resim boyutu: "
         << resim.cols
         << " x "
         << resim.rows
         << endl;


    // Sonuç resmi
    Mat sonuc = resim.clone();


    // 3x3 ortalama filtresi
    int kernel[3][3] =
    {
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1}
    };


    // Kenarlarda 3x3 pencere tam oluşmadığı için
    // 1'den başlayıp sondan 1 önce bitiriyoruz
    for (int y = 1; y < resim.rows - 1; y++)
    {
        for (int x = 1; x < resim.cols - 1; x++)
        {
            int toplam = 0;


            // 3x3 pencereyi geziyoruz
            for (int ky = -1; ky <= 1; ky++)
            {
                for (int kx = -1; kx <= 1; kx++)
                {
                    int piksel =
                        resim.at<uchar>(
                            y + ky,
                            x + kx
                        );

                    toplam +=
                        piksel *
                        kernel[ky + 1][kx + 1];
                }
            }


            // 9 pikselin ortalaması
            int yeniDeger = toplam / 9;


            sonuc.at<uchar>(y, x) =
                yeniDeger;
        }
    }


    // Sonucu kaydet
    imwrite(
        "/content/average_filtered.png",
        sonuc
    );


    cout << "3x3 ortalama filtresi uygulandi."
         << endl;

    cout << "Sonuc: average_filtered.png"
         << endl;


    return 0;
}
