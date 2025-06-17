#pragma once 
#include "ImageProcessing.h"
#include <algorithm>

using namespace std;

class BrightnessContrastAdjustment : public ImageProcessing {
private:
    float alpha; // Contrast control
    int beta;    // Brightness control

public:
    BrightnessContrastAdjustment(float a = 1.0f, int b = 0): alpha(a),beta(b){}
    void process(const Image& src, Image& dst) override {
        dst = Image(src.width(), src.height());

        for (unsigned int y = 0; y < src.height(); ++y) {
            for (unsigned int x = 0; x < src.width(); ++x) {
                float val = alpha * src.at(x, y) + beta;
                dst.at(x, y) = static_cast<unsigned char>(clamp(val, 0.0f, 255.0f));
            }
        }
    }
};