#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <vector>

using namespace std;

void ApplyUniformFilter(unsigned char** imageData, unsigned char** filteredData, int height, int width) {
    int filterSize = 3;
    int filterArea = filterSize * filterSize;
    int pad = filterSize / 2;

    // Loop over each pixel in the image
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int sum = 0;

            // Loop over the filter area
            for (int fi = -pad; fi <= pad; ++fi) {
                for (int fj = -pad; fj <= pad; ++fj) {
                    // Check for image boundary
                    int ci = min(max(i + fi, 0), height - 1);
                    int cj = min(max(j + fj, 0), width - 1);

                    sum += imageData[ci][cj];
                }
            }
            
            // Assign the computed average to the central pixel
            filteredData[i][j] = sum / filterArea;
        }
    }
}

int main(int argc, char *argv[]) {
    // Check for proper syntax
    if (argc != 3) {
        cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
        cout << "usage: " << argv[0] << " input_image.raw output_image.raw" << endl;
        return 0;
    }

    const int width = 768;
    const int height = 512;
    const int bytesPerPixel = 1;

    // Allocate memory for the image data arrays
    unsigned char** imageData = new unsigned char*[height];
    unsigned char** filteredData = new unsigned char*[height];
    for (int i = 0; i < height; ++i) {
        imageData[i] = new unsigned char[width];
        filteredData[i] = new unsigned char[width];
    }

    // Read image (filename specified by first argument) into image data matrix
    FILE *file;
    if (!(file = fopen(argv[1], "rb"))) {
        cout << "Cannot open file: " << argv[1] << endl;
        exit(1);
    }
    for (int i = 0; i < height; ++i) {
        fread(imageData[i], sizeof(unsigned char), width, file);
    }
    fclose(file);

    // Apply the uniform filter
    ApplyUniformFilter(imageData, filteredData, height, width);

    // Write image data (filename specified by second argument) from image data matrix
    if (!(file = fopen(argv[2], "wb"))) {
        cout << "Cannot open file: " << argv[2] << endl;
        exit(1);
    }
    for (int i = 0; i < height; ++i) {
        fwrite(filteredData[i], sizeof(unsigned char), width, file);
    }
    fclose(file);

    // Free memory
    for (int i = 0; i < height; ++i) {
        delete[] imageData[i];
        delete[] filteredData[i];
    }
    delete[] imageData;
    delete[] filteredData;

    return 0;
}
