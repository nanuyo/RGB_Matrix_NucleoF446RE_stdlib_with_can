/*
 * colorwheel.c
 *
 *  Created on: 2016. 8. 16.
 *      Author: HJ Park
 */





#include "framebuffer.h"
#include "colorcorr.h"
#include <math.h>
#include <arm_math.h>

//#define _ADA_GAMMA_CORRECTION  /* Darker than colorcorr_lookup*/

#ifdef _ADA_GAMMA_CORRECTION
static const uint8_t pgamma[] = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,
  0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
  0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
  0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
  0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x02,
  0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
  0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
  0x02,0x02,0x02,0x02,0x02,0x03,0x03,0x03,
  0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
  0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x04,
  0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
  0x04,0x04,0x04,0x04,0x04,0x04,0x05,0x05,
  0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,
  0x05,0x05,0x05,0x06,0x06,0x06,0x06,0x06,
  0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x07,
  0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,
  0x07,0x07,0x08,0x08,0x08,0x08,0x08,0x08,
  0x08,0x08,0x08,0x08,0x09,0x09,0x09,0x09,
  0x09,0x09,0x09,0x09,0x09,0x0a,0x0a,0x0a,
  0x0a,0x0a,0x0a,0x0a,0x0a,0x0a,0x0b,0x0b,
  0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0c,0x0c,
  0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0d,0x0d,
  0x0d,0x0d,0x0d,0x0d,0x0d,0x0e,0x0e,0x0e,
  0x0e,0x0e,0x0e,0x0e,0x0f,0x0f,0x0f,0x0f
};
#endif

void colWheelDrawPixel(int32_t x, int32_t y, uint32_t color ) {
	//int32_t offset=(x + (y*MATRIX_WIDTH))*3;
	uint32_t offset=(x%MATRIX_WIDTH + ((y%MATRIX_HEIGHT)*MATRIX_WIDTH))*3;
	uint8_t R8,G8,B8;
#ifdef COL565
	uint8_t R5,G6,B5;

    R5 = (color&0xf800)>>11;
    G6 = (color&0x07E0)>>5;
    B5 = color&0x1f;
	#if 1
		R8 = ( R5 << 3) | (R5 >> 2);
		G8 = ( G6 << 3) | (G6 >> 2);
		B8 = ( B5 << 3) | (B5 >> 2);
	#else
		R8 = ( R5 * 527 + 23 ) >> 6;
		G8 = ( G6 * 259 + 33 ) >> 6;
		B8 = ( B5 * 527 + 23 ) >> 6;
	#endif

#else
	R8 = (color>>16)&0xff;
	G8 = (color>>8)&0xff;
	B8 = (color&0xff);
#endif

#ifdef _ADA_GAMMA_CORRECTION
	framebuffer_write(offset,R8);
	framebuffer_write(offset+1,G8);
	framebuffer_write(offset+2,B8);

#else
	framebuffer_write(offset,colorcorr_lookup(R8));
	framebuffer_write(offset+1,colorcorr_lookup(G8));
	framebuffer_write(offset+2,colorcorr_lookup(B8));
#endif

}

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
uint32_t ColorHSV(long hue, uint8_t sat, uint8_t val, uint8_t gflag) {

  uint8_t  r, g, b, lo;
  uint16_t s1, v1;

  // Hue
  hue %= 1536;             // -1535 to +1535
  if(hue < 0) hue += 1536; //     0 to +1535
  lo = hue & 255;          // Low byte  = primary/secondary color mix
  switch(hue >> 8) {       // High byte = sextant of colorwheel
    case 0 : r = 255     ; g =  lo     ; b =   0     ; break; // R to Y
    case 1 : r = 255 - lo; g = 255     ; b =   0     ; break; // Y to G
    case 2 : r =   0     ; g = 255     ; b =  lo     ; break; // G to C
    case 3 : r =   0     ; g = 255 - lo; b = 255     ; break; // C to B
    case 4 : r =  lo     ; g =   0     ; b = 255     ; break; // B to M
    default: r = 255     ; g =   0     ; b = 255 - lo; break; // M to R
  }

  // Saturation: add 1 so range is 1 to 256, allowig a quick shift operation
  // on the result rather than a costly divide, while the type upgrade to int
  // avoids repeated type conversions in both directions.
  s1 = sat + 1;
  r  = 255 - (((255 - r) * s1) >> 8);
  g  = 255 - (((255 - g) * s1) >> 8);
  b  = 255 - (((255 - b) * s1) >> 8);

#ifdef _ADA_GAMMA_CORRECTION

  // Value (brightness) & 16-bit color reduction: similar to above, add 1
  // to allow shifts, and upgrade to int makes other conversions implicit.
  v1 = val + 1;
  if(gflag) { // Gamma-corrected color?
    r = pgm_read_byte(&pgamma[(r * v1) >> 8]); // Gamma correction table maps
    g = pgm_read_byte(&pgamma[(g * v1) >> 8]); // 8-bit input to 4-bit output
    b = pgm_read_byte(&pgamma[(b * v1) >> 8]);
  } else { // linear (uncorrected) color
    r = (r * v1) >> 12; // 4-bit results
    g = (g * v1) >> 12;
    b = (b * v1) >> 12;
  }
#endif

#ifdef COL565
  return (r << 12) | ((r & 0x8) << 8) | // 4/4/4 -> 5/6/5
         (g <<  7) | ((g & 0xC) << 3) |
         (b <<  1) | ( b        >> 3);
#else
  return ((r << 16)&0xff0000) | ((g <<  8)&0xff00) | (b&0xff );
#endif
}

void colWheelTest(){
	int      x, y, hue;
	  float    dx, dy, d;
	  uint8_t  sat, val;
	  uint32_t c;
//for(;;)
{
	  for(y=0; y < MATRIX_WIDTH; y++) {
	    dy = 15.5 - (float)y;
	    for(x=0; x < MATRIX_HEIGHT; x++) {
	      dx = 15.5 - (float)x;
	      d  = dx * dx + dy * dy;
	      if(d <= (16.5 * 16.5)) { // Inside the circle(ish)?
	        hue = (int)((atan2(-dy, dx) + PI) * 1536.0 / (PI * 2.0));

	        d = sqrt(d);
	        if(d > 15.5) {
	          // Do a little pseudo anti-aliasing along perimeter
	          sat = 255;
	          val = (int)((1.0 - (d - 15.5)) * 255.0 + 0.5);
	        } else
	        {
	          // White at center
	          sat = (int)(d / 15.5 * 255.0 + 0.5);
	          val = 255;
	        }
	        c = ColorHSV(hue, sat, val, 1);
	      } else {
	        c = 0;
	      }
	      if(MATRIX_WIDTH>32){
	    	     colWheelDrawPixel(x, y, c);
	    	     colWheelDrawPixel(x+MATRIX_PANEL_WIDTH, y, c);
	      }
	      else{
	    	  	 colWheelDrawPixel(x, y, c);
	     }


	    }

	  }
		 framebuffer_swap();
}
}

