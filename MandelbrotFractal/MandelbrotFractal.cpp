// MandelbrotFractal.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//
#define HAVE_STRUCT_TIMESPEC

#include <iostream>
#include <fstream>
#include <string>
#include <pthread.h>

using namespace std;

int imageWidth;
int imageHeight;
int numThreads;

typedef struct threadData
{
	int** lines;
	int yStart;
	int yEnd;
	int imageWidth;
	int imageHeight;
	int threadNum;
} threadData;

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

void *processLines(void* arg)
{
	double maxN = 5000;
	double minR = -1.5;
	double maxR = 0.5;
	double minI = -1.0;
	double maxI = 1.0;

	threadData* tData = (threadData*)arg;

	fprintf(stderr, "Inicio Thread %i: processando de %i a %i\n", 
		tData->threadNum,
		tData->yStart,
		tData->yEnd);

	// for every pixel
	for (int y = tData->yStart; y < tData->yEnd; y++) // rows
	{
		for (int x = 0; x < tData->imageWidth; x++) // pixels in row
		{
			// find the real and imaginary values for c
			double cr = mapToReal(x, tData->imageWidth, minR, maxR);
			double ci = mapToImaginary(y, tData->imageHeight, minI, maxI);

			// find the number of iterations in the mandelbrot
			int n = findMandelbrot(cr, ci, maxN);

			tData->lines[y][x] = n;
		}
	}

	fprintf(stderr, "Thread %i finalizou \n", tData->threadNum);

	return 0;
}

void gerateImage(int** lines, int imageWidth, int imageHeight)
{
	// Open the output file, write the PPM header
	ofstream fout("output_image.ppm");
	fout << "P3" << endl; // "magic number" - PPM file
	fout << imageWidth << " " << imageHeight << endl; // Dimensions
	fout << "256" << endl; // max value of a pixel RGB

	int r, g, b;

	fprintf(stderr, "Gerando imagem...\n");

	for (int y = 0; y < imageHeight; y++)
	{
		for (int x = 0; x < imageWidth; x++)
		{
			// map the resulting number an RGB value
			r = ((int)(lines[y][x] * sinf(lines[y][x])) % 256);
			g = ((lines[y][x] * 3) % 256);
			b = (lines[y][x] % 256);

			// output it to the image
			fout << r << " " << g << " " << b << " ";
		}
		fout << endl;
	}
	fout.close();
}

int main()
{
	imageWidth = 1024;
	imageHeight = 1024;
	numThreads = 6;

	int i;

	int** lines = NULL;

	lines = (int**)malloc(imageHeight * sizeof(unsigned int*));
	for (i = 0; i < imageWidth; i++)
	{
		lines[i] = (int*)malloc(imageWidth * sizeof(unsigned int));
	}

	threadData *tData = (threadData*)malloc((int)numThreads * sizeof(threadData));

	pthread_t *threads = (pthread_t*)malloc(numThreads * sizeof(pthread_t));

	int linesPerThread = imageHeight / numThreads;

	for (i = 0; i < numThreads; i++)
	{
		tData[i].threadNum = i + 1;
		tData[i].lines = lines;
		tData[i].imageWidth = imageWidth;
		tData[i].imageHeight = imageHeight;
		tData[i].yStart = i * linesPerThread;
		tData[i].yEnd = (i == numThreads - 1) ? imageHeight : (i + 1) * linesPerThread;

		fprintf(stderr, "Criando thread %i\n", tData[i].threadNum);

		pthread_create(&(threads[i]), NULL, processLines, &(tData[i]));
	}

	fprintf(stderr, "Main esperando as Threads terminarem\n");

	for (i = 0; i < numThreads; i++)
	{
		fprintf(stderr, "Join da Thread %i\n", i + 1);

		pthread_join(threads[i], NULL);
	}

	fprintf(stderr, "Finalizou join das Threads\n");

	gerateImage(lines, imageWidth, imageHeight);

	for (i = 0; i < imageHeight; i++)
	{
		free(lines[i]);
	}

	free(lines);

	fprintf(stderr, "Finalizou!\n");

	return 0;
}
