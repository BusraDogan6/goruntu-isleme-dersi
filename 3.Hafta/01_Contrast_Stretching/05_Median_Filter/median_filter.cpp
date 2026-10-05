
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
using namespace cv;

int main()
{
    // Gürültülü resmi açıyoruz
    Mat resim = imread(
        "/content/salt_pepper.png",
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


    // ---------------------------------
    // 1 - OpenCV hazır Median Filter
    // ---------------------------------

    Mat hazirSonuc;

    medianBlur(
        resim,
        hazirSonuc,
        5
    );


    imwrite(
        "/content/median_opencv.png",
        hazirSonuc
    );


    cout << "OpenCV median filtre tamamlandi."
         << endl;



    // ---------------------------------
    // 2 - Kendi Median Filtremiz
    // ---------------------------------

    Mat manuelSonuc = resim.clone();


    // 5x5 olduğu için merkezin etrafında
    // 2 piksel sağ-sol ve yukarı-aşağı geziyoruz
    for (int y = 2; y < resim.rows - 2; y++)
    {
        for (int x = 2; x < resim.cols - 2; x++)
        {
            vector<int> degerler;


            // 5x5 alanı geziyoruz
            for (int ky = -2; ky <= 2; ky++)
            {
                for (int kx = -2; kx <= 2; kx++)
                {
                    int piksel =
                        resim.at<uchar>(
                            y + ky,
                            x + kx
                        );

                    degerler.push_back(piksel);
                }
            }


            // 25 değeri küçükten büyüğe sırala
            sort(
                degerler.begin(),
                degerler.end()
            );


            // 25 sayının ortasındaki değer
            // index olarak 12
            int medyan = degerler[12];


            manuelSonuc.at<uchar>(y, x) =
                medyan;
        }
    }


    imwrite(
        "/content/median_manual.png",
        manuelSonuc
    );


    cout << "Manuel median filtre tamamlandi."
         << endl;


    return 0;
}
