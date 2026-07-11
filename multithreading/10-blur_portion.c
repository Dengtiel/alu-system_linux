#include <stdlib.h>
#include "multithreading.h"

/**
 * blur_pixel - blur a single pixel using convolution kernel
 * @portion: pointer to blur portion data
 * @x: x position of pixel
 * @y: y position of pixel
 */
static void blur_pixel(blur_portion_t const *portion, size_t x, size_t y)
{
	size_t ki, kj, half;
	size_t px, py;
	float r, g, b, factor, kval;
	img_t const *img;
	kernel_t const *kernel;

	img = portion->img;
	kernel = portion->kernel;
	half = kernel->size / 2;
	r = 0;
	g = 0;
	b = 0;
	factor = 0;

	for (ki = 0; ki < kernel->size; ki++)
	{
		for (kj = 0; kj < kernel->size; kj++)
		{
			if (y + ki < half || x + kj < half)
				continue;
			py = y + ki - half;
			px = x + kj - half;
			if (py >= img->h || px >= img->w)
				continue;
			kval = kernel->matrix[ki][kj];
			r += img->pixels[py * img->w + px].r * kval;
			g += img->pixels[py * img->w + px].g * kval;
			b += img->pixels[py * img->w + px].b * kval;
			factor += kval;
		}
	}
	portion->img_blur->pixels[y * img->w + x].r = r / factor;
	portion->img_blur->pixels[y * img->w + x].g = g / factor;
	portion->img_blur->pixels[y * img->w + x].b = b / factor;
}

/**
 * blur_portion - blurs a portion of an image
 * @portion: pointer to the blur portion data structure
 */
void blur_portion(blur_portion_t const *portion)
{
	size_t x, y;

	for (y = portion->y; y < portion->y + portion->h; y++)
	{
		for (x = portion->x; x < portion->x + portion->w; x++)
			blur_pixel(portion, x, y);
	}
}
