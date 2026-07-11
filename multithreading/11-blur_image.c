#include <stdlib.h>
#include <pthread.h>
#include "multithreading.h"

#define NUM_THREADS 16

/**
 * thread_blur - thread entry point to blur a portion
 * @arg: pointer to blur_portion_t
 *
 * Return: NULL
 */
static void *thread_blur(void *arg)
{
	blur_portion(arg);
	return (NULL);
}

/**
 * blur_image - blurs entire image using multiple threads
 * @img_blur: destination blurred image
 * @img: source image
 * @kernel: convolution kernel to use
 */
void blur_image(img_t *img_blur, img_t const *img, kernel_t const *kernel)
{
	pthread_t threads[NUM_THREADS];
	blur_portion_t portions[NUM_THREADS];
	size_t i;
	size_t rows_per_thread;

	rows_per_thread = img->h / NUM_THREADS;

	for (i = 0; i < NUM_THREADS; i++)
	{
		portions[i].img = img;
		portions[i].img_blur = img_blur;
		portions[i].kernel = kernel;
		portions[i].x = 0;
		portions[i].w = img->w;
		portions[i].y = i * rows_per_thread;
		if (i == NUM_THREADS - 1)
			portions[i].h = img->h - portions[i].y;
		else
			portions[i].h = rows_per_thread;
		pthread_create(&threads[i], NULL, thread_blur, &portions[i]);
	}

	for (i = 0; i < NUM_THREADS; i++)
		pthread_join(threads[i], NULL);
}
