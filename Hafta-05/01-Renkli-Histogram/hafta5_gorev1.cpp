
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
    string yol = "/content/goruntu-isleme-dersi/Hafta-05/input/girdi.png";

    Mat resim = imread(yol, IMREAD_COLOR);

    if (resim.empty())
    {
        cout << "Resim okunamadi!" << endl;
        return -1;
    }

    int maviHist[256] = {0};
    int yesilHist[256] = {0};
    int kirmiziHist[256] = {0};

    // Görüntüdeki bütün pikselleri gez
    for (int y = 0; y < resim.rows; y++)
    {
        for (int x = 0; x < resim.cols; x++)
        {
            Vec3b piksel = resim.at<Vec3b>(y, x);

            int mavi = piksel[0];
            int yesil = piksel[1];
            int kirmizi = piksel[2];

            maviHist[mavi]++;
            yesilHist[yesil]++;
            kirmiziHist[kirmizi]++;
        }
    }

    cout << "RGB kanal histogramlari hesaplandi." << endl;

    // İlk birkaç değeri kontrol amaçlı yazdır
    cout << endl;
    cout << "Blue kanal ilk 10 deger:" << endl;

    for (int i = 0; i < 10; i++)
    {
        cout << i << " : " << maviHist[i] << endl;
    }

    cout << endl;
    cout << "Green kanal ilk 10 deger:" << endl;

    for (int i = 0; i < 10; i++)
    {
        cout << i << " : " << yesilHist[i] << endl;
    }

    cout << endl;
    cout << "Red kanal ilk 10 deger:" << endl;

    for (int i = 0; i < 10; i++)
    {
        cout << i << " : " << kirmiziHist[i] << endl;
    }

    return 0;
}
