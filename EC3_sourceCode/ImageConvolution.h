#pragma once 
#include "ImageProcessing.h"
#include <algorithm> 

class ImageConvolution : public ImageProcessing {
private:
    float** kernel;
    int kernelWidth;
    int kernelHeight;
    float (*scalingFunction)(float);

public:
    ImageConvolution(float** k, int kw, int kh, float (*sf)(float)) : kernel(k), kernelWidth(kw), kernelHeight(kh), scalingFunction(sf) {}
    ~ImageConvolution() {
        for (int i = 0; i < kernelHeight; ++i) {
            delete[] kernel[i];
        }
        delete[] kernel;
    }
    void process(const Image& src, Image& dst) override {
        dst = Image(src.width(), src.height());
        int kCenterX = kernelWidth / 2;
        int kCenterY = kernelHeight / 2;

        for (unsigned int y = 0; y < src.height(); ++y) {
            for (unsigned int x = 0; x < src.width(); ++x) {
                float sum = 0.0f;

                for (int ky = 0; ky < kernelHeight; ++ky) {
                    for (int kx = 0; kx < kernelWidth; ++kx) {
                        int imgX = x + kx - kCenterX;
                        int imgY = y + ky - kCenterY;
                        imgX = std::clamp(imgX, 0, static_cast<int>(src.width()) - 1);
                        imgY = std::clamp(imgY, 0, static_cast<int>(src.height()) - 1);

                        sum += kernel[ky][kx] * src.at(imgX, imgY);
                    }
                }

                float scaled = scalingFunction(sum);
                dst.at(x, y) = static_cast<unsigned char>(std::clamp(scaled, 0.0f, 255.0f));
            }
        }
    }
};