#include "seamcarving.h"
#include <math.h>
void calc_energy(struct rgb_img* im, struct rgb_img** grad) {
	create_img(grad, im->height, im->width);
	for (int i = 0; i < (im->height); i++) {
		for (int j = 0; j < (im->width); j++) {
			int iup = i - 1;
			int jleft = j - 1;
			int idown = i + 1;
			int jright = j + 1;
			if (iup == -1) {
				iup = im->height - 1;
			}
			if (jleft == -1) {
				jleft = im->width - 1;
			}
			if (idown == im->height) {
				idown = 0;
			}
			if (jright == im->width) {
				jright = 0;
			}
			int rleft = get_pixel(im, i, jleft, 0);
			int rright = get_pixel(im, i, jright, 0);
			int rup = get_pixel(im, iup, j, 0);
			int rdown = get_pixel(im, idown, j, 0);
			int gleft = get_pixel(im, i, jleft, 1);
			int gright = get_pixel(im, i, jright, 1);
			int gup = get_pixel(im, iup, j, 1);
			int gdown = get_pixel(im, idown, j, 1);
			int bleft = get_pixel(im, i, jleft, 2);
			int bright = get_pixel(im, i, jright, 2);
			int bup = get_pixel(im, iup, j, 2);
			int bdown = get_pixel(im, idown, j, 2);

			int dx2 = (rleft - rright) * (rleft - rright) + (gleft - gright) * (gleft - gright) + (bleft - bright) * (bleft - bright);
			int dy2 = (rup - rdown) * (rup - rdown) + (gup - gdown) * (gup - gdown) + (bup - bdown) * (bup - bdown);
            
			int energy = sqrt(dx2 + dy2)/10;
			uint8_t nrg = (uint8_t)energy;
			set_pixel(*grad, i, j, nrg, nrg, nrg);
            
		}
	}
}



void dynamic_seam(struct rgb_img* grad, double** best_arr) {
	*best_arr = (double*)(malloc(sizeof(double) * (grad->height) * (grad->width)));
	for (int i = 0; i < grad->height; i++) {
		for (int j = 0; j < grad->width; j++) {
			if (i == 0) {
				(*best_arr)[i * (grad->width) + j] = get_pixel(grad, i, j, 0);
				continue;
			}
			double cur_min = 256;
			for (int index = (j - 1); index < (j + 2); index++) {
				if (index<0 || index >(grad->width) - 1) {
					continue;
				}
				if (cur_min > (get_pixel(grad, i, j, 0) + (*best_arr)[(i - 1) * (grad->width) + index])) {
					cur_min = (*best_arr)[(i - 1) * (grad->width) + index]+ (get_pixel(grad, i, j, 0));
				}
				
			}
			(*best_arr)[i * (grad->width) + j] = cur_min;
		}
	}

}
void remove_seam(struct rgb_img* src, struct rgb_img** dest, int* path) {
	create_img(dest, src->height, (src->width)-1);
	for (int i = 0; i < src->height; i++) {
		for (int j = 0; j < path[i]; j++) {
			int r = get_pixel(src, i, j, 0);
			int g = get_pixel(src, i, j, 1);
			int b = get_pixel(src, i, j, 2);
			set_pixel(*dest, i, j, r, g, b);
		}
		for (int j = path[i] + 1; j < src->width; j++){
			int r = get_pixel(src, i, j, 0);
			int g = get_pixel(src, i, j, 1);
			int b = get_pixel(src, i, j, 2);
			set_pixel(*dest, i, j-1, r, g, b);
		}
	}
	
}

void recover_path(double* best, int height, int width, int** path) {
	*path = (int*)(malloc(sizeof(int) * height));

	double cur_min = 256;
	int cur_min_index = -1;
	//find smallest energy//
	for (int i = 0; i < width; i++) {
		if (best[(height - 1) * width + i] < cur_min) {
			cur_min = best[(height - 1) * width + i];
			cur_min_index = i;
		}

	}
	(*path)[height - 1] = cur_min_index;
	for (int i = (height - 2); i > -1; i--) {
		cur_min = 256;
		int start_index = cur_min_index - 1;
		for (int j = start_index; j < start_index + 3; j++) {
			if (j< 0 || j >(width - 1)) {
				continue;
			}
			if (best[i * width + j] < cur_min) {
				cur_min = best[i * width + j];
				cur_min_index = j;
			}

		}
		(*path)[i] = cur_min_index;

	}

}



