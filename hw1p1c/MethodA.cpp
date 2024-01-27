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

int main(int argc, char *argv[]) {
    FILE *file;
    int BytesPerPixel = 3;  // Assuming 24-bit RGB color image
    int Width = 750;        // Image width (to be set according to your image)
    int Height = 422;       // Image height (to be set according to your image)
    
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
    cvtColor(image, imageYUV, COLOR_RGB2YUV);

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
    cvtColor(imageYUV, resultImage, COLOR_YUV2RGB);

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

