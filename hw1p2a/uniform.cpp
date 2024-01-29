#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

void ApplyUniformFilter(Mat &inputImage, Mat &outputImage) {
    // Assuming filterSize is odd and square
    int filterSize = 3;
    int pad = filterSize / 2;
    Mat paddedInput;
    
    // Create padded version of inputImage
    copyMakeBorder(inputImage, paddedInput, pad, pad, pad, pad, BORDER_REPLICATE);
    
    for (int i = pad; i < inputImage.rows + pad; ++i) {
        for (int j = pad; j < inputImage.cols + pad; ++j) {
            int sum = 0; 
            for (int fi = -pad; fi <= pad; ++fi) {
                for (int fj = -pad; fj <= pad; ++fj) {
                    sum += (int)paddedInput.at<uchar>(i + fi, j + fj); // Cast uchar to int
                }
            }
            outputImage.at<uchar>(i - pad, j - pad) = sum / (filterSize * filterSize);
        }
    }
}
void saveRawImage(const char* filename, const Mat& image) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        cerr << "Cannot open file: " << filename << endl;
        exit(1);
    }

    fwrite(image.data, sizeof(uchar), image.total(), file);
    fclose(file);
}

int main(int argc, char** argv) {
    // Check for proper syntax
    if (argc != 3) {
        cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
        cout << "usage: " << argv[0] << " input_image.raw output_image.jpg" << endl;
        return 0;
    }

    const int width = 768;
    const int height = 512;

    // Read image (filename specified by first argument) into Mat object
    Mat inputImage(height, width, CV_8UC1);
    FILE* file;
    if (!(file = fopen(argv[1], "rb"))) {
        cout << "Cannot open file: " << argv[1] <<endl;
        return 1;
    }
    fread(inputImage.data, sizeof(unsigned char), height * width, file);
    fclose(file);

    // Create output Mat object with the same size and type as inputImage
    Mat outputImage = Mat::zeros(inputImage.size(), inputImage.type());

    // Apply uniform filter to the input image
    ApplyUniformFilter(inputImage, outputImage);

    // Display the original and filtered images
    namedWindow("Original Image", WINDOW_AUTOSIZE);
    imshow("Original Image", inputImage);

    namedWindow("Filtered Image", WINDOW_AUTOSIZE);
    imshow("Filtered Image", outputImage);
    imwrite("uniform_fixed.jpg", outputImage);

    waitKey(0); // Wait for a keystroke in the window


    string rawOutputFilename = string(argv[2]);
    saveRawImage(rawOutputFilename.c_str(), outputImage);
    cout << "Saved filtered image as RAW file: " << rawOutputFilename << endl;

    return 0;
}
