// Based on https://github.com/josch/mandelbrot (used with permission)
#include "colors.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static uint16_t rgb_to_565(uint8_t r, uint8_t g, uint8_t b)
{
	// fedcba98 76543210
	// rrrrrrrr              .
	//      ggg ggggg        |  inputs
	//             bbbbbbbb  '
	// rrrrrggg gggbbbbb     <  output
	return
		((r & 0xf8) << 8) |
		((g & 0xfc) << 3) |
		((b & 0xf8) >> 3);
}

void render_mandelbrot(int width, int height, uint16_t *image,
		long double centerx, long double centery, long double magn)
{
	long double eps = 1e-17;
	long double Q1LOG2 = 1.44269504088896340735992468100189213742664595415299;
	long double LOG2 = 0.69314718055994530941723212145817656807550013436026;
	int x, y;
	long double bailout = 128;
	long double logLogBailout = log(log(bailout));
	int foundperiods = 0;
	long maxiter = width * sqrt(magn);
	long double x0d = 4 / magn / width;
	long double x2 = -2 / magn + centerx;
	long double y1d = -4 / magn / width;
	long double y2 = 2 / magn * height / width + centery;
	int imgidx = 0;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			long double px = x*x0d + x2;
			long double py = y*y1d + y2;
			// no Main bulb or Cardoid check to be faster
			long double zx = px;
			long double zy = py;
			long i;
			// Initial maximum period to detect.
			int check = 3;
			// Maximum period doubles every iterations:
			int whenupdate = 10;
			// Period history registers.
			long double hx = 0;
			long double hy = 0;
			long double xx, yy;
			bool inside = true;
			for (i = 1; i <= maxiter; i++) {
				xx = zx * zx;
				yy = zy * zy;
				if (xx + yy > bailout) {
					inside = false;
					break;
				}
				// iterate
				zy = 2 * zx * zy + py;
				zx = xx - yy + px;

				// periodicity check
				long double d = zx - hx;
				if (d > 0.0 ? d < eps : d > -eps) {
					d = zy - hy;
					if (d > 0.0 ? d < eps : d > -eps) {
						// Period found.
						foundperiods++;
						break;
					}
				}
				if ((i & check) == 0) {
					if (--whenupdate == 0) {
						whenupdate = 10;
						check <<= 1;
						check++;
					}
					// period = 0;
					hx = zx;
					hy = zy;
				}
			}

			if (inside) {
				image[imgidx++] = 0;
			} else {
				long double r = sqrtl(zx*zx + zy*zy);
				long double c = i - 1.28 + (logLogBailout - logl(logl(r))) * Q1LOG2;
				int idx = fmodl((logl(c/64+1)/LOG2+0.45), 1)*GRADIENTLENGTH + 0.5;
				const unsigned char *rgb = colors[idx];
				image[imgidx++] = rgb_to_565(rgb[0], rgb[1], rgb[2]);
			}
		}
	}
}
