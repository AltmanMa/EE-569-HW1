#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace cv;
using namespace std;

// Function to apply bilateral filter
void ApplyBilateralFilter(const Mat &src, Mat &dst, int d, double sigmaColor, double sigmaSpace) {
    dst = Mat(src.size(), src.type());
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            double iFiltered = 0;
            double wP = 0;
            // Go over the window
            for (int k = -d; k <= d; k++) {
                for (int l = -d; l <= d; l++) {
                    int newI = min(max(i + k, 0), src.rows - 1);
                    int newJ = min(max(j + l, 0), src.cols - 1);
                    double gi = exp(-((k * k + l * l) / (2 * sigmaSpace * sigmaSpace)));
                    double gs = exp(-((src.at<uchar>(i, j) - src.at<uchar>(newI, newJ)) * (src.at<uchar>(i, j) - src.at<uchar>(newI, newJ)) / (2 * sigmaColor * sigmaColor)));
                    double w = gi * gs;
                    iFiltered += src.at<uchar>(newI, newJ) * w;
                    wP += w;
                }
            }
            dst.at<uchar>(i, j) = round(iFiltered / wP);
        }
    }
}

int main(int argc, char *argv[]) {
    // Check for proper syntax
    if (argc < 5) {
        cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
        cout << "usage: " << argv[0] << " input_image.raw output_image.raw width height" << endl;
        return -1;
    }

    const int Width = atoi(argv[3]);  // Image width
    const int Height = atoi(argv[4]); // Image height
    
    // Allocate image data array for a grayscale image
    unsigned char* Imagedata = new unsigned char[Width * Height];

    // Read image (filename specified by first argument) into image data matrix
    FILE *file;
    if (!(file = fopen(argv[1], "rb"))) {
        cout << "Cannot open file: " << argv[1] << endl;
        return -1;
    }
    fread(Imagedata, sizeof(unsigned char), Width * Height, file);
    fclose(file);

    // Initialize Mat object from array
    Mat image(Height, Width, CV_8UC1, Imagedata);

    // Apply bilateral filter
    Mat filteredImage;
    // You need to choose these parameters depending on your noise and image characteristics
    double sigmaColor = 25.0; // This value will vary depending on your image
    double sigmaSpace = 25.0; // This value will vary depending on your image
    ApplyBilateralFilter(image, filteredImage, 5, sigmaColor, sigmaSpace);

    // Save the filtered image to a raw file
    if (!(file = fopen(argv[2], "wb"))) {
        cout << "Cannot open file: " << argv[2] << endl;
        return -1;
    }
    fwrite(filteredImage.data, sizeof(unsigned char), Width * Height, file);
    fclose(file);
    cout << "Filtered image saved to " << argv[2] << endl;

    // Display the original and filtered images
    namedWindow("Original Image", WINDOW_AUTOSIZE);
    imshow("Original Image", image);
    namedWindow("Filtered Image", WINDOW_AUTOSIZE);
    imshow("Filtered Image", filteredImage);
    imwrite("Bilateral_fixed.jpg", filteredImage);

    waitKey(0);

    // Cleanup
    delete[] Imagedata;

    return 0;
}

