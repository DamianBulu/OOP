
#pragma once
#include "ImageProcessing.h"

class GammaCorrection : public ImageProcessing {
private:
    float gamma;

public:
    GammaCorrection(float g = 1.0f):gamma(g){}
    void process(const Image& src, Image& dst) override {
        dst = Image(src.width(), src.height());
        for (unsigned int y = 0; y < src.height(); ++y) {
            for (unsigned int x = 0; x < src.width(); ++x) {
                float normalized = src.at(x, y) / 255.0f;
                float corrected = std::pow(normalized, gamma);
                dst.at(x, y) = static_cast<unsigned char>(corrected * 255.0f);
            }
        }
    }
};