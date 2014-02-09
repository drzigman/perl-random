#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ieee754.h>

int main( int argc, char *argv[] ) {
    if ( argc != 2 ) {
        printf("usage: %s <Xn input value>\n", argv[0]);
        return 1;
    }

    uint64_t xn = strtoull( argv[1], NULL, 10 );

    unsigned short int xsubi[3];

    xsubi[0] = xn & 0xffff;
    xsubi[1] = (xn >> 16) & 0xffff;
    xsubi[2] = (xn >> 32) & 0xffff;

    union ieee754_double temp;

    temp.ieee.negative = 0;
    temp.ieee.exponent = IEEE754_DOUBLE_BIAS;
    temp.ieee.mantissa0 = (xsubi[2] << 4) | (xsubi[1] >> 12);
    temp.ieee.mantissa1 = ((xsubi[1] & 0xfff) << 20) | (xsubi[0] << 4);

    printf("%f\n", temp.d - 1.0);
}
