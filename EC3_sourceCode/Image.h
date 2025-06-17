#pragma once  

#include <iostream>
#include "Size.h" 
#include "Point.h" 
#include "Rectangle.h" 



using namespace std;

class Image {
private:
	unsigned char** m_data;
	unsigned int m_width;
	unsigned int m_height;

	void allocateMemory();
	void copyData(const Image& other);
	void releaseMemory();

public:
	Image(); 
	Image(unsigned int w, unsigned int h);
	Image(const Image& other);
	~Image();

	bool load(const string& imagePath);
	bool save(const string& imagePath) const;

	Image& operator=(const Image& other);
	Image operator+(const Image& other)const;
	Image operator-(const Image& other) const;
	Image operator+(unsigned char scalar) const;
	Image operator-(unsigned char scalar) const;
	Image operator*(float scalar) const;

	unsigned char& at(unsigned int c, unsigned int y)const;
	unsigned char& at(Point pt) const;

	unsigned char* row(int y);
	const unsigned char* row(int y) const;
	bool isEmpty() const; 
	Size size() const;

	unsigned int width() const;
	unsigned int height() const;
	void release(); 

	bool getROI(Image& roiImg, Rectangle roiRect);
	bool getROI(Image& roiImg, unsigned int x, unsigned int y, unsigned int width,unsigned int height);

	static Image zeros(unsigned int width, unsigned int height);
	static Image ones(unsigned int width, unsigned int height);

	friend ostream& operator<<(ostream& os, const Image& img);
};