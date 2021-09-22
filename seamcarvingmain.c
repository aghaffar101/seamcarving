#include "c_img.h"
#include "seamcarving.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(void){
    struct rgb_img * pic1;
    struct rgb_img *grad;
  
    calc_energy(pic1, &grad);
  
    print_grad(grad);

    return 0;
}