#include "Image.h" 
#include <fstream> 
#include <iomanip> 
#include <stdexcept>
#include <algorithm> 
#include <cstring> 
#include <string>

using namespace std;

void Image::allocateMemory() {
	if (m_width == 0 || m_height == 0) {
		m_data = nullptr;
		return;
	}

	m_data = new unsigned char* [m_height];
	for (unsigned int i = 0;i < m_height;i++) {
		m_data[i] = new unsigned char[m_width];
		memset(m_data[i], 0, m_width * sizeof(unsigned char));
	}
}


void Image::copyData(const Image& other) {
	if (other.m_data == nullptr) {
		m_data = nullptr;
		return;
	}
	m_data = new unsigned char* [m_height];
	for (unsigned int i = 0;i < m_height;i++) {
		m_data[i] = new unsigned char[m_width];
		memcpy(m_data[i], other.m_data[i], m_width * sizeof(unsigned char));
	}
}

void Image::releaseMemory() {
	if (m_data != nullptr) {
		for (unsigned int i = 0;i < m_height;i++) {
			delete m_data[i];
		}
		delete[] m_data;
	}
	m_width = m_height = 0;
}

Image::Image():m_data(nullptr),m_width(0),m_height(0){}
Image::Image(unsigned int w, unsigned int h) :m_width(w), m_height(h) {
	allocateMemory();
}

Image::Image(const Image& other) :m_width(other.m_width), m_height(other.m_height) {
	copyData(other);
}
Image::~Image() {
	releaseMemory();
}

bool Image::load(const string& imagePath) {
	ifstream file(imagePath);
	if (!file.is_open()) {
		cerr << "Error:Could not open file " << imagePath << endl;
		return false;
	}
	string magicNumber; 
	file >> magicNumber; 
	if (magicNumber != "P2"){
		cerr << "Error:Not a valid ASCII PGM file(magic number should be P2)" << std::endl;
		return false;
	}
	char ch;
	while (file.get(ch)) {//ignoram comentariile si spatiile
		if (ch == '#') {
			file.ignore(numeric_limits<streamsize>::max(), '\n');//sare peste toata linia 
		}
		else if (!isspace(ch)) {//daca e caracter diferit de spatiu il pune inapoi 
			file.unget();
			break;
		}
	}
	unsigned int width, height, maxVal; 
	file >> width >> height >> maxVal;
	if (file.fail()) {
		cerr << "Error: Invalid PGM header" <<endl;
		return false;
	}
	cout << width << ' ' << height << ' ' << maxVal << '\n';
	releaseMemory();

	m_width = width;
	m_height = height;
	allocateMemory();

	for (unsigned int y = 0;y < m_height;y++) {
		for (unsigned int x = 0;x < m_width;x++) {
			unsigned int pixelValue; 
			file >> pixelValue; 
			if (file.fail()) {
				cerr << "Error: Invalid pixel data at (" << x << ", " << y << ")" <<endl;
				releaseMemory();
				return false;
			}
			//normalizare intre 0 si 255 
			if (maxVal != 255) {
			   pixelValue = static_cast<unsigned char>((pixelValue * 255) / maxVal);
			}
			m_data[y][x]= static_cast<unsigned char>(pixelValue);
		}
	}
	if (file.fail() && !file.eof()) {
		cerr << "Error: Incomplete pixel data" << endl;
		releaseMemory();
		return false;
	}
	return true;
}

bool Image::save(const string& imagePath) const{
	ofstream file(imagePath); 
	if (!file.is_open()) {
		cerr << "Error: Could not create file" << imagePath <<endl;
		return false;
	}
	file << "P2\n";
	file<<"#Created by Image Processing Application\n";
	file << m_width << " " << m_height << "\n";
	file << "255\n";

	const int maxLineLength = 70; 
	int currentLineLength = 0;

	for (unsigned int y = 0;y < m_height;y++) {
		for (unsigned int x = 0;x < m_width;x++) {
			//Convertire valoarea pixel la string 
			string pixelStr = to_string(static_cast<int>(m_data[y][x]));
			if (currentLineLength + pixelStr.length() + 1 > maxLineLength) {
				file << "\n";
				currentLineLength = 0;
			}
			else if (x != 0 || y != 0) {
				file << " ";
				currentLineLength++;
			}

			file << pixelStr;
			currentLineLength += pixelStr.length();
		}
		file << "\n";
		if (file.fail()) {
			cerr<< "Error: Failed to write image data" <<endl;
			return false;
		}

		return true;
	}
}

Image& Image::operator=(const Image& other) {
	if (this != &other) {
		releaseMemory();
		m_width = other.m_width;
		m_height = other.m_height;
		copyData(other);
	}
	return *this;
}
Image Image::operator+(const Image& other) const {
	if (m_width != other.m_width || m_height != other.m_height) {
		throw std::invalid_argument("Images must have the same dimensions for addition");
	}

	Image result(m_width, m_height);
	for (unsigned int y = 0; y < m_height; ++y) {
		for (unsigned int x = 0; x < m_width; ++x) {
			int sum = m_data[y][x] + other.m_data[y][x];
			result.m_data[y][x] = static_cast<unsigned char>(std::clamp(sum, 0, 255));
		}
	}
	return result;
}

Image Image::operator-(const Image& other) const {
	if (m_width != other.m_width || m_height != other.m_height) {
		throw invalid_argument("Images must have the same dimensions for subtraction");
	}

	Image result(m_width, m_height);
	for (unsigned int y = 0; y < m_height; ++y) {
		for (unsigned int x = 0; x < m_width; ++x) {
			int diff = m_data[y][x] - other.m_data[y][x];
			result.m_data[y][x] = static_cast<unsigned char>(std::clamp(diff, 0, 255));
		}
	}
	return result;
}
Image Image::operator+(unsigned char scalar) const {
	Image result(m_width, m_height);
	for (unsigned int y = 0; y < m_height; ++y) {
		for (unsigned int x = 0; x < m_width; ++x) {
			int sum = m_data[y][x] + scalar;
			result.m_data[y][x] = static_cast<unsigned char>(std::clamp(sum, 0, 255));
		}
	}
	return result;
}

Image Image::operator-(unsigned char scalar) const {
	Image result(m_width, m_height);
	for (unsigned int y = 0; y < m_height; ++y) {
		for (unsigned int x = 0; x < m_width; ++x) {
			int diff = m_data[y][x] - scalar;
			result.m_data[y][x] = static_cast<unsigned char>(std::clamp(diff, 0, 255));
		}
	}
	return result;
}

Image Image::operator*(float scalar) const {
	Image result(m_width, m_height);
	for (unsigned int y = 0; y < m_height; ++y) {
		for (unsigned int x = 0; x < m_width; ++x) {
			float product = m_data[y][x] * scalar;
			result.m_data[y][x] = static_cast<unsigned char>(std::clamp(product, 0.0f, 255.0f));
		}
	}
	return result;
}
unsigned char& Image::at(unsigned int x, unsigned int y) const {
	if (x >= m_width || y >= m_height) {
		throw out_of_range("Pixel coordinates out of range");
	}
	return m_data[y][x];
}
unsigned char& Image::at(Point pt) const {
	return at(pt.x, pt.y);
}

unsigned char* Image::row(int y) {
	if (y < 0 || static_cast<unsigned int>(y) >= m_height) {
		throw out_of_range("Row index out of range");
	}
	return m_data[y];
}

bool Image::isEmpty() const {
	return m_data == nullptr;
}

Size Image::size() const {
	return Size(m_width, m_height);
}
unsigned int Image::width() const {
	return m_width;
}

unsigned int Image::height() const {
	return m_height;
}
void Image::release() {
	releaseMemory();
}

bool Image::getROI(Image& roiImg, Rectangle roiRect) {
	return getROI(roiImg, roiRect.x, roiRect.y, roiRect.width, roiRect.height);
}
bool Image::getROI(Image& roiImg, unsigned int x, unsigned int y,
	unsigned int width, unsigned int height) {
	if (x + width > m_width || y + height > m_height) {
		return false;
	}

	roiImg = Image(width, height);
	for (unsigned int i = 0; i < height; ++i) {
		for (unsigned int j = 0; j < width; ++j) {
			roiImg.at(j, i) = at(x + j, y + i);
		}
	}
	return true;
}

Image Image::zeros(unsigned int width, unsigned int height) {
	Image result(width, height);
	for (unsigned int y = 0; y < height; ++y) {
		for (unsigned int x = 0; x < width; ++x) {
			result.at(x, y) = 0;
		}
	}
	return result;
}

Image Image::ones(unsigned int width, unsigned int height) {
	Image result(width, height);
	for (unsigned int y = 0; y < height; ++y) {
		for (unsigned int x = 0; x < width; ++x) {
			result.at(x, y) = 255;
		}
	}
	return result;
}
const unsigned char* Image::row(int y) const {
	if (y < 0 || static_cast<unsigned int>(y) >= m_height) {
		throw std::out_of_range("Row index out of range");
	}
	return m_data[y];
}
ostream& operator<<(std::ostream& os, const Image& img) {
	for (unsigned int y = 0; y < img.height(); ++y) {
		for (unsigned int x = 0; x < img.width(); ++x) {
			os << std::setw(3) << static_cast<int>(img.row(y)[x]) << " ";
		}
		os << "\n";
	}
	return os;
}