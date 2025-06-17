#pragma once 
#include <iostream>

using namespace std;

class Point {
public :
	int x;
	int y;
	Point(int x = 0, int y = 0):x(x),y(y){}

	friend ostream& operator<<(ostream& os, const Point& pt) {
		os << "(" << pt.x << "," << pt.y << ")";
		return os;
	}
	friend istream& operator>>(istream& is, Point& pt) {
		is >> pt.x >> pt.y;
		return is;
	}
};