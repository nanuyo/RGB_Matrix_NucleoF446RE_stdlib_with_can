/*
 * ug_test.c
 *
 *  Created on: 2016. 8. 17.
 *      Author: HJ Park
 */

#include "ugui.h"
#include "framebuffer.h"
#include "colorcorr.h"
#include "tm_stm32f4_delay.h"

#define DELAYMIL 2000

#include "no2.h"
//#include "larrow.h"
#include "Right3Green64x64.h"
#include "imageX565.h"
#include "icons.h"

#if 0
#include "right_arrow64_64.h" //NOK
#include "right_arrow_6464_blue.h" //NOK
#include "arrow-right64_64_blue.h" //NOK
#include "arrow-left64x64.bmp.h"//NOK
#include "arrow.h"//NOK
#include "alarm_arrow_right_93571.h"//NOK
#include "larrow.h"
#include "no2.h" //Good
#include "image565.h"
#include "imageX565.h"
#include "syj888.h"
#include "jy888.h"
#include "no888.h"
#include "cancel888.h"
#endif


/* GUI structure */
UG_GUI gui;

const UG_BMP NoEnter32 =
{
   (void*)NoEnter32Data,
   NOENTER_BMPWIDTH,
   NOENTER_BMPHEIGHT,
   BMP_BPP_32,
   BMP_RGB888
};

#if 0
const UG_BMP LeftArrow32 =
{
   (void*)LeftArrow32Data,
   LARROW_BMPWIDTH,
   LARROW_BMPHEIGHT,
   BMP_BPP_32,
   BMP_RGB888
};
#else
const UG_BMP RightArrow64x64 =
{
   (void*)RightArrow32Data,
   RARROW_BMPWIDTH,
   RARROW_BMPHEIGHT,
   BMP_BPP_32,
   BMP_RGB888
};
#endif

const UG_BMP logo =
{
   (void*)image16,
   BMPWIDTH16,
   BMPHEIGHT16,
   BMP_BPP_16,
   BMP_RGB565
};





#define _WHITE_TO_BLACK

void ugDrawPixelRGB(UG_S16 x, UG_S16 y, UG_COLOR color) {
	//uint32_t offset=(x + (y*MATRIX_WIDTH))*3; // bug : first line is not disappeared
	uint32_t offset=(x%MATRIX_WIDTH + ((y%MATRIX_HEIGHT)*MATRIX_WIDTH))*3;

#if defined (_WHITE_TO_BLACK)
	if ( ((color & 0xf0f0f0) == 0xf0f0f0) )
	{
		framebuffer_write(offset,0);
		framebuffer_write(offset+1,0);
		framebuffer_write(offset+2,0);
	}else
	{
		framebuffer_write(offset,colorcorr_lookup((color&0xff0000)>>16));
		framebuffer_write(offset+1,colorcorr_lookup((color&0x00ff00)>>8));
		framebuffer_write(offset+2,colorcorr_lookup(color&0xff));
	}
#else
	framebuffer_write(offset,colorcorr_lookup((color&0xff0000)>>16));
	framebuffer_write(offset+1,colorcorr_lookup((color&0x00ff00)>>8));
	framebuffer_write(offset+2,colorcorr_lookup(color&0xff));
#endif
}

void ugReverseDrawPixelRGB(UG_S16 x, UG_S16 y, UG_COLOR color) {

	uint32_t offset=(((MATRIX_WIDTH-1) - x%MATRIX_WIDTH) + ((y%MATRIX_HEIGHT)*MATRIX_WIDTH))*3;

#if defined (_WHITE_TO_BLACK)
	if ( ((color & 0xf0f0f0) == 0xf0f0f0) )
	{
		framebuffer_write(offset,0);
		framebuffer_write(offset+1,0);
		framebuffer_write(offset+2,0);
	}else
	{
		framebuffer_write(offset,colorcorr_lookup((color&0xff0000)>>16));
		framebuffer_write(offset+1,colorcorr_lookup((color&0x00ff00)>>8));
		framebuffer_write(offset+2,colorcorr_lookup(color&0xff));
	}
#else
	framebuffer_write(offset,colorcorr_lookup((color&0xff0000)>>16));
	framebuffer_write(offset+1,colorcorr_lookup((color&0x00ff00)>>8));
	framebuffer_write(offset+2,colorcorr_lookup(color&0xff));
#endif


}
#define MOVING_DELAY 0

void icnDrawIcon(SIGN_ICON icon) {

switch(icon){
case ARROW_RIGHT:
	UG_Init(&gui,(void(*)(UG_S16,UG_S16,UG_COLOR))ugDrawPixelRGB,MATRIX_WIDTH,MATRIX_HEIGHT);
	//UG_Init(&gui,(void(*)(UG_S16,UG_S16,UG_COLOR))ugReverseDrawPixelRGB,MATRIX_WIDTH,MATRIX_HEIGHT);
	for(int i = 0 ; i < MATRIX_WIDTH ; i ++)
	//for(int i = (MATRIX_WIDTH-1) ; i >= 0 ; i --)

		{
			UG_DrawBMP888(i, 0, &RightArrow64x64/*&LeftArrow32*/ );
				framebuffer_swap();
				//Delayms(MOVING_DELAY);
		}
	break;
case ARROW_LEFT:
	//UG_Init(&gui,(void(*)(UG_S16,UG_S16,UG_COLOR))ugDrawPixelRGB,MATRIX_WIDTH,MATRIX_HEIGHT);
	UG_Init(&gui,(void(*)(UG_S16,UG_S16,UG_COLOR))ugReverseDrawPixelRGB,MATRIX_WIDTH,MATRIX_HEIGHT);
	for(int i = 0 ; i < MATRIX_WIDTH ; i ++)
	//for(int i = (MATRIX_WIDTH-1) ; i >= 0 ; i --)
	{
		UG_DrawBMP888(i, 0, &RightArrow64x64/*&LeftArrow32*/ );
		framebuffer_swap();
		//Delayms(MOVING_DELAY);
	}
	break;
case NO_ENTER:
	UG_Init(&gui,(void(*)(UG_S16,UG_S16,UG_COLOR))ugDrawPixelRGB,MATRIX_WIDTH,MATRIX_HEIGHT);
	for(int i = 0 ; i < MATRIX_WIDTH ; i ++)
	{
		UG_DrawBMP888(i, 0, &NoEnter32 );
		framebuffer_swap();
	//	Delayms(MOVING_DELAY);
	}
	break;
default:
	break;
}
}


void ugTest(){

	UG_Init(&gui,(void(*)(UG_S16,UG_S16,UG_COLOR))ugReverseDrawPixelRGB/*ugDrawPixelRGB*/,MATRIX_WIDTH,MATRIX_HEIGHT);
#ifdef _LED64_64_1
#if 1
	//for(int i = (MATRIX_WIDTH-1) ; i >= 0 ; i --)
	for(int i = 0 ; i < MATRIX_WIDTH ; i ++)
	{
		UG_DrawBMP888(i, 0, &NoEnter32 );
			framebuffer_swap();
			Delayms(50);
	}
#else
	UG_DrawBMP888(21*2, 0, &logo32 );
	framebuffer_swap();
	Delayms(200);
	UG_DrawBMP888(21, 0, &logo32 );
		framebuffer_swap();
		Delayms(200);
		UG_DrawBMP888(0, 0, &logo32 );
				framebuffer_swap();
				Delayms(200);
#endif


#else
	UG_FillScreen( C_WHITE );
	UG_FontSelect( &FONT_6X8 );
	UG_FillFrame( 0, 0, 10, 10, C_RED);
	UG_FillRoundFrame( 11, 0, 20,10, 5, C_YELLOW);
    UG_DrawMesh( 21, 0, 30, 10, C_GREEN);
	UG_DrawFrame( 0, 11, 10, 20, C_MAGENTA );
	UG_DrawRoundFrame( 11, 11, 20, 20, 5, C_BLUE );
    UG_DrawPixel(11 , 11, C_BLACK );

	UG_DrawCircle( 21, 11, 5, C_CYAN );
	UG_FillCircle( 0, 21, 5, C_VIOLET );

	UG_DrawArc( 11, 21, 5, 2, C_LIME );

    UG_DrawLine( 0, 0/*31*/, 31, 31, C_GOLD);

	UG_PutString(0, 23, "Test" );

	UG_PutChar( 'T', 0, 23, C_BROWN, C_ORANGE );


	if(MATRIX_WIDTH>32){
		UG_DrawBMP( MATRIX_PANEL_WIDTH, 0, &logo );
	}
	else{
		framebuffer_swap();
		Delayms(DELAYMIL);
		UG_DrawBMP( 0, 0, &logo );
	}

	framebuffer_swap();
#endif
}
