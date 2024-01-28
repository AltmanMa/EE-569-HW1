#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

using namespace std;
using namespace cv;

// Function to read a RAW image file into a Mat object
Mat readRawImage(const char* filename, int width, int height) {
    FILE* file = fopen(filename, "rb");
    cout << "Fopen success. Continuing..." << endl;

    if (!file) {
        cerr << "Cannot open file: " << filename << endl;
        exit(1);
    }

    Mat image = Mat(height, width, CV_8UC1);
    fread(image.data, sizeof(uchar), width * height, file);
    fclose(file);

    return image;
}

// Function to compute MSE between two images
double computeMSE(const Mat& img1, const Mat& img2) {
    if (img1.size() != img2.size() || img1.type() != img2.type()) {
        cerr << "Error: Images must have the same dimensions and type" << endl;
        exit(1);
    }

    double mse = 0;
    for (int i = 0; i < img1.rows; ++i) {
        for (int j = 0; j < img1.cols; ++j) {
            mse += pow(img1.at<uchar>(i, j) - img2.at<uchar>(i, j), 2);
        }
    }
    mse /= (img1.rows * img1.cols);
    return mse;
}

// Function to compute PSNR
double computePSNR(const Mat& img1, const Mat& img2) {
    double mse = computeMSE(img1, img2);
    if (mse == 0) {
        return INFINITY;
    }
    return 10 * log10(pow(255, 2) / mse);
}

int main(int argc, char** argv) {
    cout << "Number of arguments: " << argc << endl;
    for (int i = 0; i < argc; ++i) {
        cout << "Argument " << i << ": " << argv[i] << endl;
    }

    if (argc != 5) {
        cout << "Usage: " << argv[0] << " original_image.raw denoised_image.raw [width height]" << endl;
        return 0;
    }

    int width = atoi(argv[3]);
    int height = atoi(argv[4]);

    Mat originalImage = readRawImage(argv[1], width, height);
    Mat denoisedImage = readRawImage(argv[2], width, height);
    
    cout << "Arguments are correct. Continuing..." << endl;

    double psnr = computePSNR(originalImage, denoisedImage);
    cout << "Calculation finished. Continuing..." << endl;

    cout << "PSNR: " << psnr << " dB" << endl;

    return 0;
}
