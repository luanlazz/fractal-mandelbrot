// MandelbrotFractal.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int findMandelbrot(double cr, double ci, int max_iterations)
{
	int i = 0;
	double zr = 0.0, zi = 0.0;
	while (i < max_iterations && zr * zr * zi * zi < 4.0)
	{
		double temp = zr * zr - zi * zi + cr;
		zi = 2.0 * zr * zi + ci;
		zr = temp;
		i++;
	}
	return i;
}

double mapToReal(int x, int imageWidth, double minR, double maxR)
{
	double range = maxR - minR;
	// [0, width]
	// [0, maxR - minR] - val * range / width
	// [minR, maxR] - last step + minR
	return x * (range / imageWidth) + minR;
}

double mapToImaginary(int y, int imageHeight, double minI, double maxI)
{
	double range = maxI - minI;
	return y * (range / imageHeight) + minI;
}

void gerateImage(unsigned int** lines, int imageWidth, int imageHeight) 
{
	// Open the output file, write the PPM header
	ofstream fout("output_image.ppm");
	fout << "P3" << endl; // "magic number" - PPM file
	fout << imageWidth << " " << imageHeight << endl; // Dimensions
	fout << "256" << endl; // max value of a pixel RGB

	int r, g, b;

	for (int y = 0; y < imageHeight; y++)
	{
		for (int x = 0; x < imageWidth; x++)
		{
			// map the resulting number an RGB value
			r = ((int)(lines[x][y] * sinf(lines[x][y])) % 256);
			g = ((lines[x][y] * 3) % 256);
			b = (lines[x][y] % 256);

			// output it to the image
			fout << r << " " << g << " " << b << " ";
		}
		fout << endl;
	}
	fout.close();
}

int main()
{
	int imageWidth = 1024;
	int imageHeight = 1024;
	double maxN = 30;
	double minR = -1.5;
	double maxR = 0.7;
	double minI = -1.0;
	double maxI = 1.0;
	unsigned int** lines = NULL;

	lines = (unsigned int**)malloc(imageHeight * sizeof(unsigned int*));
	for (int i = 0; i < imageWidth; i++)
	{
		lines[i] = (unsigned int*)malloc(imageWidth * sizeof(unsigned int));
	}

	// for every pixel
	for (int y = 0; y < imageHeight; y++) // rows
	{
		for (int x = 0; x < imageWidth; x++) // pixels in row
		{
			// find the real and imaginary values for c
			double cr = mapToReal(x, imageWidth, minR, maxR);
			double ci = mapToImaginary(y, imageHeight, minI, maxI);

			// find the number of iterations in the mandelbrot
			int n = findMandelbrot(cr, ci, maxN);

			lines[x][y] = n;
		}
	}

	gerateImage(lines, imageWidth, imageHeight);

	cout << "Finished!" << endl;
	return 0;
}
