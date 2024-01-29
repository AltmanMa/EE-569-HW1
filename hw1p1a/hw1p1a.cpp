// This sample code reads in image data from a RAW image file and 
// writes it into another file

// NOTE:	The code assumes that the image is of size 256 x 256 and is in the
//			RAW format. You will need to make corresponding changes to
//			accommodate images of different sizes and/or types

#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;
bool isRed(int i, int j) {
    return i % 2 != 0 && j % 2 == 0;
}

bool isBlue(int i, int j) {
    return i % 2 == 0 && j % 2 != 0;
}

bool isGreen(int i, int j) {
    return !isRed(i, j) && !isBlue(i, j);
}

int main(int argc, char *argv[])

{
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel;
	int Width = 420;
	int Height = 288;
	
	// Check for proper syntax
	if (argc < 3){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name input_image.raw output_image.raw [BytesPerPixel = 1] [Size = 420*288]" << endl;
		return 0;
	}
	
	// Check if image is grayscale or color
	if (argc < 4) {
    	BytesPerPixel = 1; // 默认为灰度图像
	} else {
    	BytesPerPixel = atoi(argv[3]);
    // 检查是否指定了图像尺寸
    	if (argc >= 5) {
        	Width = atoi(argv[4]);
        	if (argc >= 6) {
            	Height = atoi(argv[5]);
        	}
    	}
	}
	
	// Allocate image data array
	unsigned char*** Imagedata = new unsigned char**[Height];
    for (int i = 0; i < Height; ++i) {
        Imagedata[i] = new unsigned char*[Width];
        for (int j = 0; j < Width; ++j) {
            Imagedata[i][j] = new unsigned char[BytesPerPixel];
        }
    }

    unsigned char*** RGBImagedata = new unsigned char**[Height];
    for (int i = 0; i < Height; ++i) {
        RGBImagedata[i] = new unsigned char*[Width];
        for (int j = 0; j < Width; ++j) {
            RGBImagedata[i][j] = new unsigned char[3];
        }
    }

	// 打开文件进行读取
	if (!(file = fopen(argv[1], "rb"))) {
    	cout << "Cannot open file: " << argv[1] << endl;
    	exit(1);
	}

	// 根据图像的宽度、高度和每像素字节大小读取数据
	for (int i = 0; i < Height; i++) {
    	for (int j = 0; j < Width; j++) {
        	fread(Imagedata[i][j], sizeof(unsigned char), BytesPerPixel, file);
    	}
	}

	fclose(file);

	for (int i = 0; i < Height; ++i) {
    	for (int j = 0; j < Width; ++j) {
        	if (isRed(i, j)) {
				RGBImagedata[i][j][0] = Imagedata[i][j][0];
				float blue = 0.0;
				float green = 0.0;
				int countblue = 0;
				int countgreen = 0;
				if(i>0){
					green += Imagedata[i-1][j][0];
					countgreen++;
					if(j>0){
						blue += Imagedata[i-1][j-1][0];
						countblue++;
					}
					if(j<Width-1){
						blue += Imagedata[i-1][j+1][0];
						countblue++;
					}
				}
				if(i<Height-1){
					green += Imagedata[i+1][j][0];
					countgreen++;
					if(j>0){
						blue += Imagedata[i+1][j-1][0];
						countblue++;
					}
					if(j<Width-1){
						blue += Imagedata[i+1][j+1][0];
						countblue++;
					}
				}
				if(j>0){
					green += Imagedata[i][j-1][0];
					countgreen++;
				}
				if(j<Width-1){
					green += Imagedata[i][j+1][0];
					countgreen++;
				}
				if (countgreen != 0){
					RGBImagedata[i][j][1] = green/countgreen;
					}
				if (countblue != 0) {
					RGBImagedata[i][j][2] = blue/countblue;
					}

            // Interpolate missing green and blue values for a red pixel
            // Be careful at the image boundaries
        	} else if (isBlue(i, j)) {
				RGBImagedata[i][j][2] = Imagedata[i][j][0];
				float red = 0.0;
				float green = 0.0;
				int countred = 0;
				int countgreen = 0;
				if(i>0){
					green += Imagedata[i-1][j][0];
					countgreen++;
					if(j>0){
						red += Imagedata[i-1][j-1][0];
						countred++;
					}
					if(j<Width-1){
						red += Imagedata[i-1][j+1][0];
						countred++;
					}
				}
				if(i<Height-1){
					green += Imagedata[i+1][j][0];
					countgreen++;
					if(j>0){
						red += Imagedata[i+1][j-1][0];
						countred++;
					}
					if(j<Width-1){
						red += Imagedata[i+1][j+1][0];
						countred++;
					}
				}
				if(j>0){
					green += Imagedata[i][j-1][0];
					countgreen++;
				}
				if(j<Width-1){
					green += Imagedata[i][j+1][0];
					countgreen++;
				}
				if (countgreen != 0) {
					RGBImagedata[i][j][1] = green/countgreen;
					}
				if (countred != 0) {
					RGBImagedata[i][j][0] = red/countred;
					}
            	// Interpolate missing green and red values for a blue pixel
            	// Be careful at the image boundaries
        	} else {
				RGBImagedata[i][j][1] = Imagedata[i][j][0];
				float red = 0.0;
				float blue = 0.0;
				int countred = 0;
				int countblue = 0;
				if(i>0){
					red+= Imagedata[i-1][j][0];
					countred++;
				}
				if(i<Height-1){
					red += Imagedata[i+1][j][0];
					countred++;
				}
				if(j>0){
					blue += Imagedata[i][j-1][0];
					countblue++;
				}
				if(j<Width-1){
					blue += Imagedata[i][j+1][0];
					countblue++;
				}
				if (countblue != 0) {
					RGBImagedata[i][j][2] = blue/countblue;
					}
				if (countred != 0) {
					RGBImagedata[i][j][0] = red/countred;
				}
            	// Interpolate missing red and blue values for a green pixel
            	// Be careful at the image boundaries
            	// Remember to differentiate between green pixels in red rows and blue rows
        	}
    	}
	}

	// Write image data (filename specified by second argument) from image data matrix

	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	for (int i = 0; i < Height; ++i) {
    	for (int j = 0; j < Width; ++j) {
        	fwrite(RGBImagedata[i][j], sizeof(unsigned char), 3, file);
    	}
	}

	fclose(file);
	for (int i = 0; i < Height; ++i) {
    	for (int j = 0; j < Width; ++j) {
        	delete[] RGBImagedata[i][j];
    	}
    	delete[] RGBImagedata[i];
	}
	delete[] RGBImagedata;

	return 0;
}
