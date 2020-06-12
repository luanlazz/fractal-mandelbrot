// MandelbrotFractal.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include <iostream>
#include <fstream>
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

int main()
{
	// get the required input values from file
	ifstream fin("input.txt");
	int imageWidth, imageHeight, maxN;
	double minR, maxR, minI, maxI;
	if (!fin)
	{
		cout << "Could not open file" << endl;
		cin.ignore();
		return 0;
	}

	fin >> imageWidth >> imageHeight >> maxN;
	fin >> minR >> maxR >> minI >> maxI;
	fin.close();

	// Open the output file, write the PPM header
	ofstream fout("output_image.ppm");
	fout << "P3" << endl; // "magic number" - PPM file
	fout << imageWidth << " " << imageHeight << endl; // Dimensions
	fout << "256" << endl; // max value of a pixel RGB

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

			// map the resulting number an RGB value
			int r = ((int)(n * sinf(n)) % 256);
			int g = ((n * 3) % 256);
			int b = (n % 256);

			// output it to the image
			fout << r << " " << g << " " << b << " ";
		}
		fout << endl;
	}
	fout.close();
	cout << "Finished!" << endl;	
	return 0;
}