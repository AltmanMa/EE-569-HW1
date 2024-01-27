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

// Function to apply transfer function to the image
void applyTransferFunction(unsigned char* imageData, int width, int height, int BytesPerPixel, float a, float b) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < BytesPerPixel; k++) {
                int index = (i * width + j) * BytesPerPixel + k;
                imageData[index] = linearTransferFunction(imageData[index], a, b);
            }
        }
    }
}

int main(int argc, char *argv[]) {
// Define file pointer and variables
    FILE *file;
    int BytesPerPixel;
    int Width = 596;  // Image width
    int Height = 340; // Image height
    
    // Check for proper syntax
    if (argc < 3){
        cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
        cout << "program_name input_image.raw output_image.raw [BytesPerPixel = 1]" << endl;
        return 0;
    }
    
    // Check if image is grayscale or color
    if (argc < 4){
        BytesPerPixel = 1; // Default is grey image
    } else {
        BytesPerPixel = atoi(argv[3]);
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

    // Define the transfer function parameters
    float a = 1.5; // scale factor
    float b = 0;   // offset

    // 应用传递函数到图像
    applyTransferFunction(Imagedata, Width, Height, BytesPerPixel, a, b);

    // 创建 Mat 对象用于显示和保存修改后的图像
    Mat modifiedImage(Height, Width, CV_8UC1, Imagedata);
    imshow("Modified Image", modifiedImage);
    imwrite("modified_image.jpg", modifiedImage); // 保存修改后的图像

    // 绘制传递函数图像
    Mat transferFunctionPlot = Mat::zeros(256, 256, CV_8UC1);
    for (int i = 0; i < 256; i++) {
        int output = linearTransferFunction(i, a, b);
        line(transferFunctionPlot, Point(i, 255), Point(i, 255 - output), Scalar(255), 1);
    }
    imshow("Transfer Function", transferFunctionPlot);
    imwrite("transfer_function.jpg", transferFunctionPlot); // 保存传递函数图像

    waitKey(0);

    delete[] Imagedata;
    return 0;
}
