#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

// ... [其他函数保持不变]


int main(int argc, char *argv[]) {
    FILE *file;
    int BytesPerPixel = 3;
    int Width = 750;  // Image width
    int Height = 422; // Image height
    
    // Check for proper syntax
    if (argc < 3){
        cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
        cout << "program_name input_image.raw output_image.raw [BytesPerPixel = 1]" << endl;
        return 0;
    }
    
    // Check if image is grayscale or color
    if (argc < 3){
        cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
        cout << "program_name input_image.raw output_image.raw" << endl;
        return 0;
    }
    
    // Allocate image data array
    unsigned char* Imagedata = new unsigned char[Width * Height * BytesPerPixel];

    // Read image (filename specified by first argument) into image data matrix
    if (!(file=fopen(argv[1],"rb"))) {
        cout << "Cannot open file: " << argv[1] << endl;
        exit(1);
    }
    fread(Imagedata, sizeof(unsigned char), Width * Height * BytesPerPixel, file);
    fclose(file);

    Mat image(Height, Width, CV_8UC3, Imagedata); // Assuming 24-bit RGB color

    // Convert from RGB to YUV
    Mat imageYUV;
    cvtColor(image, imageYUV, COLOR_RGB2YUV);

    vector<Mat> channels(3);
    split(imageYUV, channels);

    // Apply CLAHE to the Y channel
    Ptr<CLAHE> clahe = createCLAHE();
    clahe->setClipLimit(2.0);  // 设置clip limit
    clahe->setTilesGridSize(Size(8, 8));  // 设置tile的尺寸
    Mat yChannelCLAHE;
    clahe->apply(channels[0], yChannelCLAHE);

    // Combine Y channel back with U and V channels
    channels[0] = yChannelCLAHE;
    Mat resultImage;
    merge(channels, imageYUV);

    // Convert back to RGB
    cvtColor(imageYUV, resultImage, COLOR_YUV2RGB);

    // Display and save results
    imshow("Original Image", image);
    imshow("Enhanced Image with CLAHE", resultImage);
    imwrite("enhanced_image_with_CLAHE.jpg", resultImage);

    waitKey(0);

    delete[] Imagedata;
    return 0;
}
