#pragma once 

#include "Point.h" 
#include <iostream> 

class Rectangle {
public:
	unsigned int x;
	unsigned int y;
	unsigned int width;
	unsigned int height;
	Rectangle(unsigned int x = 0, unsigned int y = 0, unsigned int w = 0, unsigned int h = 0) :x(x), y(y), width(w), height(h){}
	Rectangle(Point tl, Point br):x(tl.x),y(tl.y),width(br.x-tl.x),height(br.y-tl.y){}


	Rectangle operator+(const Point& pt) const {
		return Rectangle(x + pt.x, y + pt.y, width, height);
	}
	Rectangle operator-(const Point& pt) const {
		return Rectangle(x - pt.x, y - pt.y, width, height);
	}
	Rectangle operator&(const Rectangle& other)const {
		unsigned int x1 = std::max(x, other.x);
		unsigned int y1 = std::max(y, other.y);
		unsigned int x2 = std::max(x+width, other.x+other.width);
		unsigned int y2 = std::max(y+height, other.y+other.height);

		if (x2 > x1 && y2 > y1) {
			return Rectangle(x1, y1, x2 - x1, y2 - y1);
		}
		return Rectangle();
	}
	Rectangle operator|(const Rectangle& other) const {
		unsigned int x1 = std::min(x, other.x);
		unsigned int y1 = std::min(y, other.y);
		unsigned int x2 = std::max(x + width, other.x + other.width);
		unsigned int y2 = std::max(y + height, other.y + other.height);

		return Rectangle(x1, y1, x2 - x1, y2 - y1);
	}

	friend std::ostream& operator<<(std::ostream& os, const
		Rectangle& rect){
		os << "Rectangle(x=" << rect.x << ", y=" << rect.y
			<< ", width=" << rect.width << ", height=" << rect.height << ")";
		return os;
	}
	friend std::istream& operator>>(std::istream& is, Rectangle& rect) {
		is >> rect.x >> rect.y >> rect.width >> rect.height;
		return is;
	}

};