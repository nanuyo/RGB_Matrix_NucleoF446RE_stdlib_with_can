/*
 * plasma_32x32.c
 *
 *  Created on: 2016. 8. 16.
 *      Author: HJ Park
 */

#include "config.h"
#include "stm32f4xx.h"
#include "tm_stm32f4_delay.h"

static const int8_t sinetab[256] = {
     0,   2,   5,   8,  11,  15,  18,  21,
    24,  27,  30,  33,  36,  39,  42,  45,
    48,  51,  54,  56,  59,  62,  65,  67,
    70,  72,  75,  77,  80,  82,  85,  87,
    89,  91,  93,  96,  98, 100, 101, 103,
   105, 107, 108, 110, 111, 113, 114, 116,
   117, 118, 119, 120, 121, 122, 123, 123,
   124, 125, 125, 126, 126, 126, 126, 126,
   127, 126, 126, 126, 126, 126, 125, 125,
   124, 123, 123, 122, 121, 120, 119, 118,
   117, 116, 114, 113, 111, 110, 108, 107,
   105, 103, 101, 100,  98,  96,  93,  91,
    89,  87,  85,  82,  80,  77,  75,  72,
    70,  67,  65,  62,  59,  56,  54,  51,
    48,  45,  42,  39,  36,  33,  30,  27,
    24,  21,  18,  15,  11,   8,   5,   2,
     0,  -3,  -6,  -9, -12, -16, -19, -22,
   -25, -28, -31, -34, -37, -40, -43, -46,
   -49, -52, -55, -57, -60, -63, -66, -68,
   -71, -73, -76, -78, -81, -83, -86, -88,
   -90, -92, -94, -97, -99,-101,-102,-104,
  -106,-108,-109,-111,-112,-114,-115,-117,
  -118,-119,-120,-121,-122,-123,-124,-124,
  -125,-126,-126,-127,-127,-127,-127,-127,
  -128,-127,-127,-127,-127,-127,-126,-126,
  -125,-124,-124,-123,-122,-121,-120,-119,
  -118,-117,-115,-114,-112,-111,-109,-108,
  -106,-104,-102,-101, -99, -97, -94, -92,
   -90, -88, -86, -83, -81, -78, -76, -73,
   -71, -68, -66, -63, -60, -57, -55, -52,
   -49, -46, -43, -40, -37, -34, -31, -28,
   -25, -22, -19, -16, -12,  -9,  -6,  -3
};
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

const float radius1  = 16.3, radius2  = 23.0, radius3  = 40.8, radius4  = 44.2,
            centerx1 = 16.1, centerx2 = 11.6, centerx3 = 23.4, centerx4 =  4.1,
            centery1 =  8.7, centery2 =  6.5, centery3 = 14.0, centery4 = -2.9;
float       angle1   =  0.0, angle2   =  0.0, angle3   =  0.0, angle4   =  0.0;
long        hueShift =  0;
#if 1

void drawPlasma32x32() {
  int           x1, x2, x3, x4, y1, y2, y3, y4, sx1, sx2, sx3, sx4;
  unsigned char x, y;
  long          value;

  for(int i=50;i>0;i--)
  	{

  sx1 = (int)(cos(angle1) * radius1 + centerx1);
  sx2 = (int)(cos(angle2) * radius2 + centerx2);
  sx3 = (int)(cos(angle3) * radius3 + centerx3);
  sx4 = (int)(cos(angle4) * radius4 + centerx4);
  y1  = (int)(sin(angle1) * radius1 + centery1);
  y2  = (int)(sin(angle2) * radius2 + centery2);
  y3  = (int)(sin(angle3) * radius3 + centery3);
  y4  = (int)(sin(angle4) * radius4 + centery4);



  for(y=0; y<MATRIX_HEIGHT; y++) {
    x1 = sx1; x2 = sx2; x3 = sx3; x4 = sx4;
    for(x=0; x<MATRIX_WIDTH; x++) {
#if 1
      value = hueShift
        + (int8_t)pgm_read_byte(sinetab + (uint8_t)((x1 * x1 + y1 * y1) >> 2))
        + (int8_t)pgm_read_byte(sinetab + (uint8_t)((x2 * x2 + y2 * y2) >> 2))
        + (int8_t)pgm_read_byte(sinetab + (uint8_t)((x3 * x3 + y3 * y3) >> 3))
        + (int8_t)pgm_read_byte(sinetab + (uint8_t)((x4 * x4 + y4 * y4) >> 3));
#else
      value = hueShift
           + (sinetab [(x1 * x1 + y1 * y1) >> 2])
           + (sinetab [(x2 * x2 + y2 * y2) >> 2])
           + (sinetab [(x3 * x3 + y3 * y3) >> 3])
           + (sinetab [(x4 * x4 + y4 * y4) >> 3]);
#endif
      colWheelDrawPixel(x, y, ColorHSV(value * 3, 255, 255, 1));
      x1--; x2--; x3--; x4--;
    }
    y1--; y2--; y3--; y4--;
  }

  angle1 += 0.03;
  angle2 -= 0.07;
  angle3 += 0.13;
  angle4 -= 0.15;
  hueShift += 2;
  framebuffer_swap();

	}
}

#endif
