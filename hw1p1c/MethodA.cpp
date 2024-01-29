#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

// Transfer function: simple linear function
uchar linearTransferFunction(uchar input, float a, float b) {
    int temp = static_cast<int>(a * input + b);
    return static_cast<uchar>(max(0, min(temp, 255)));
}
void RGBtoYUV(const Mat &src, Mat &dst) {
    dst = Mat::zeros(src.size(), src.type());
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            Vec3b rgb = src.at<Vec3b>(y, x);
            float R = rgb[2];
            float G = rgb[1];
            float B = rgb[0];
            uchar Y = saturate_cast<uchar>(0.257 * R + 0.504 * G + 0.098 * B + 16);
            uchar U = saturate_cast<uchar>(-0.148 * R - 0.291 * G + 0.439 * B + 128);
            uchar V = saturate_cast<uchar>(0.439 * R - 0.368 * G - 0.071 * B + 128);
            dst.at<Vec3b>(y, x) = Vec3b(Y, U, V);
        }
    }
}

void YUVtoRGB(const Mat &src, Mat &dst) {
    dst = Mat::zeros(src.size(), src.type());
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            Vec3b yuv = src.at<Vec3b>(y, x);
            float Y = yuv[0];
            float U = yuv[1];
            float V = yuv[2];
            uchar R = saturate_cast<uchar>(1.164 * (Y - 16) + 1.596 * (V - 128));
            uchar G = saturate_cast<uchar>(1.164 * (Y - 16) - 0.813 * (V - 128) - 0.391 * (U - 128));
            uchar B = saturate_cast<uchar>(1.164 * (Y - 16) + 2.018 * (U - 128));
            dst.at<Vec3b>(y, x) = Vec3b(R, G, B);
        }
    }
}
int main(int argc, char *argv[]) {
    FILE *file;
    int BytesPerPixel = 3;  
    int Width = 750;
    int Height = 422;
    
    if (argc < 3){
        cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
        cout << "program_name input_image.raw output_image.raw" << endl;
        return 0;
    }
    
    // Allocate image data array
    unsigned char* Imagedata = new unsigned char[Width * Height * BytesPerPixel];

    // Read image into image data matrix
    if (!(file=fopen(argv[1],"rb"))) {
        cout << "Cannot open file: " << argv[1] << endl;
        exit(1);
    }
    fread(Imagedata, sizeof(unsigned char), Width * Height * BytesPerPixel, file);
    fclose(file);

    Mat image(Height, Width, CV_8UC3, Imagedata);

    // Convert from RGB to YUV
    Mat imageYUV;
    RGBtoYUV(image, imageYUV);

    vector<Mat> channels(3);
    split(imageYUV, channels);

    // Apply transfer function to the Y channel
    float a = 1.5; // scale factor
    float b = 0;   // offset
    for (int y = 0; y < channels[0].rows; y++) {
        for (int x = 0; x < channels[0].cols; x++) {
            channels[0].at<uchar>(y, x) = linearTransferFunction(channels[0].at<uchar>(y, x), a, b);
        }
    }

    // Combine Y channel back with U and V channels
    merge(channels, imageYUV);

    // Convert back to RGB
    Mat resultImage;
    YUVtoRGB(imageYUV, resultImage);

    // Save the result image
    if (!(file = fopen(argv[2], "wb"))) {
        cout << "Cannot open file: " << argv[2] << endl;
        exit(1);
    }
    fwrite(resultImage.data, sizeof(unsigned char), Width * Height * BytesPerPixel, file);
    fclose(file);

    // Display the modified image
    imshow("Modified Image", resultImage);
    imwrite("ModifiedA.jpg", resultImage);

    waitKey(0);

    delete[] Imagedata;
    return 0;
}

