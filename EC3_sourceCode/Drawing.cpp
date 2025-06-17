#include "Drawing.h"
#include <algorithm>
#include <cmath>

void Drawing::drawCircle(Image& img, Point center, int radius, unsigned char color) {
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        img.at(center.x + x, center.y + y) = color;
        img.at(center.x + y, center.y + x) = color;
        img.at(center.x - y, center.y + x) = color;
        img.at(center.x - x, center.y + y) = color;
        img.at(center.x - x, center.y - y) = color;
        img.at(center.x - y, center.y - x) = color;
        img.at(center.x + y, center.y - x) = color;
        img.at(center.x + x, center.y - y) = color;

        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

void Drawing::drawLine(Image& img, Point p1, Point p2, unsigned char color) {
    int dx = abs(p2.x - p1.x);
    int dy = -abs(p2.y - p1.y);
    int sx = p1.x < p2.x ? 1 : -1;
    int sy = p1.y < p2.y ? 1 : -1;
    int err = dx + dy;

    while (true) {
        if (p1.x >= 0 && p1.x < img.width() && p1.y >= 0 && p1.y < img.height()) {
            img.at(p1.x, p1.y) = color;
        }

        if (p1.x == p2.x && p1.y == p2.y) break;

        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            p1.x += sx;
        }
        if (e2 <= dx) {
            err += dx;
            p1.y += sy;
        }
    }
}

void Drawing::drawRectangle(Image& img, Rectangle r, unsigned char color) {
    Point p1(r.x, r.y);
    Point p2(r.x + r.width, r.y);
    Point p3(r.x + r.width, r.y + r.height);
    Point p4(r.x, r.y + r.height);

    drawLine(img, p1, p2, color);
    drawLine(img, p2, p3, color);
    drawLine(img, p3, p4, color);
    drawLine(img, p4, p1, color);
}

void Drawing::drawRectangle(Image& img, Point tl, Point br, unsigned char color) {
    drawRectangle(img, Rectangle(tl, br), color);
}