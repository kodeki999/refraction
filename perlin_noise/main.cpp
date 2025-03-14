#include <opencv2/opencv.hpp>
#include <math.h>
#include <algorithm>





/////////////////////////////////////////////// CODICE PERLIN NOISE (NON SVILLUPPATO DALLO STUDENTE) //////////////////////////////////////////
typedef struct {
	float x, y;
} vector2;

vector2 randomGradient(int ix, int iy) {
	// No precomputed gradients mean this works for any number of grid coordinates
	const unsigned w = 8 * sizeof(unsigned);
	const unsigned s = w / 2;
	unsigned a = ix, b = iy;
	a *= 3284157443;

	b ^= a << s | a >> w - s;
	b *= 1911520717;

	a ^= b << s | b >> w - s;
	a *= 2048419325;
	float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]

	// Create the vector from the angle
	vector2 v;
	v.x = sin(random);
	v.y = cos(random);

	return v;
}

// Computes the dot product of the distance and gradient vectors.
float dotGridGradient(int ix, int iy, float x, float y) {
	// Get gradient from integer coordinates
	vector2 gradient = randomGradient(ix, iy);

	// Compute the distance vector
	float dx = x - (float)ix;
	float dy = y - (float)iy;

	// Compute the dot-product
	return (dx * gradient.x + dy * gradient.y);
}

float interpolate(float a0, float a1, float w)
{
	return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

// Sample Perlin noise at coordinates x, y
float perlin(float x, float y) {

	// Determine grid cell corner coordinates
	int x0 = (int)x;
	int y0 = (int)y;
	int x1 = x0 + 1;
	int y1 = y0 + 1;

	// Compute Interpolation weights
	float sx = x - (float)x0;
	float sy = y - (float)y0;

	// Compute and interpolate top two corners
	float n0 = dotGridGradient(x0, y0, x, y);
	float n1 = dotGridGradient(x1, y0, x, y);
	float ix0 = interpolate(n0, n1, sx);

	// Compute and interpolate bottom two corners
	n0 = dotGridGradient(x0, y1, x, y);
	n1 = dotGridGradient(x1, y1, x, y);
	float ix1 = interpolate(n0, n1, sx);

	// Final step: interpolate between the two previously interpolated values, now in y
	float value = interpolate(ix0, ix1, sy);

	return value;
}

/////////////////////////////////////////////// FINE CODICE PERLIN NOISE (NON SVILLUPPATO DALLO STUDENTE) //////////////////////////////////////////


// Funzione clamp personalizzata
int clamp(int val, int minVal, int maxVal) {
	return std::max(minVal, std::min(val, maxVal));
}




int main() {
	cv::Mat img = cv::imread("C:/Users/alt_g/Desktop/PROGETTI_CODE/5_C++/img/marti.jpg");

	cv::Mat perling = cv::Mat::zeros(img.rows, img.cols, CV_8UC1);


	/////////////////////////////////////////////// CODICE PERLIN NOISE (NON SVILLUPPATO DALLO STUDENTE) //////////////////////////////////////////
	const int GRID_SIZE = 800;

	for (int x = 0; x < perling.rows; x++) {

		for (int y = 0; y < perling.cols; y++) {



			float val = 0;

			float freq = 1;
			float amp = 1;
			
			float set = 0.9;
			float step = 0.2;

			for (int i = 0; i < 12; i++)
			{
				val += perlin(x * freq / GRID_SIZE, y * freq / GRID_SIZE) * amp;
				freq *= step;
				amp *= set;
			}

			// Contrast
			val *= 2;

			// Clipping
			if (val > 1.0f)
				val = 1.0f;
			else if (val < -1.0f)
				val = -1.0f;

			// Convert 1 to -1 into 255 to 0
			int color = (int)(((val + 1.0f) * 0.5f) * 255);
			perling.at<uchar>(x, y) = color;

		}

	}
	/////////////////////////////////////////////// FINE CODICE PERLIN NOISE (NON SVILLUPPATO DALLO STUDENTE) //////////////////////////////////////////



	cv::Mat sobelimgx, sobelimgy, gausimgx, gausimgy, distbgr;

	int dx = 1;
	int dy = 0;
	int ksize = 9; // dimensione del kernel sobel
	int scale = 1;
	int delta = 0;
	double sigma1 = 50; // rende più "morbida" la distorsione all'umentare del parametro
	int n1 = 2 * std::round(2.5 * sigma1) + 1;


	cv::Sobel(perling, sobelimgx, CV_8UC1, dx, dy, ksize, scale, delta);
	cv::Sobel(perling, sobelimgy, CV_8UC1, dx - 1, dy + 1, ksize, scale, delta);
	cv::GaussianBlur(sobelimgx, gausimgx, cv::Size(n1, n1), sigma1);
	cv::GaussianBlur(sobelimgy, gausimgy, cv::Size(n1, n1), sigma1);

	std::vector<cv::Mat> newbgr(3);
	std::vector<cv::Mat> bgr(3);
	cv::split(img, bgr);
	float inc = 1.15; //fattore di scala della dispersione cromatica

	for (int k = 0; k < 3; k++) {
		cv::Mat temp = cv::Mat::zeros(img.rows, img.cols, CV_8UC1);
		for (int i = 0; i < img.rows; i++) {
			for (int j = 0; j < img.cols; j++) {
				int x = clamp(i + static_cast<int>(gausimgx.at<uchar>(i, j) * inc), 0, img.rows - 1);
				int y = clamp(j + static_cast<int>(gausimgy.at<uchar>(i, j) * inc), 0, img.cols - 1);
				temp.at<uchar>(i, j) = bgr[k].at<uchar>(x, y);
			}
		}
		inc -= 0.15; // Gestisce la scala della dispersione cromatica
		newbgr[k] = temp;
	}

	cv::merge(newbgr, distbgr);
	

	cv::namedWindow("sobel x", 0);
	cv::namedWindow("sobel y", 0);
	cv::namedWindow("perlin", 0);
	cv::namedWindow("distortion", 0);
	cv::namedWindow("gaus x", 0);
	cv::namedWindow("gaus y", 0);
	

	cv::imshow("perlin", perling);
	cv::imshow("sobel x", sobelimgx);
	cv::imshow("sobel y", sobelimgy);
	cv::imshow("gaus x", gausimgx);
	cv::imshow("gaus y", gausimgy);
	cv::imshow("distortion", distbgr);

	cv::waitKey(0);

	return 0;
}