/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <math.h>
#include "config.h"
#include "framebuffer.h"
#include "colorcorr.h"
#include "control_uart.h"
#include "config.h"
#include "testimage.h"
#include "matrix.h"
#include "tm_stm32f4_delay.h"
#include "can.h"
#include "icons.h"

#define _DEMO_WITH_CAN

#define DELAYMIL 2000
int main(void)
{
	TM_DELAY_Init();
	framebuffer_init();
	colorcorr_init();
	//control_uart_init();
	matrix_init();

#if defined _DEMO_WITH_CAN
	ledInit();
	CAN_Config(CAN_500Kbps, 0x100, 0x200);
#endif

	for(;;)
	{
#if defined _DEMO_WITH_CAN
		switch(canGetData())
		{
			case 0:
				canTest();
				break;
			case 1:
				testimage_init();
				//Delayms(DELAYMIL);
				break;
			case 2:
				drawStringLoop();
				break;
			case 3:
				ugTest();
				break;
			case 4:
				colWheelTest();
				break;
			case 5:
				drawPlasma32x32();
				break;
			case 6:
				adaTest();
				break;
			case 7:
				adaTestShape32x32();
				break;
			case 8:
				icnDrawIcon(ARROW_RIGHT);
				break;
			case 9:
				icnDrawIcon(ARROW_LEFT);
				break;
			case 10:
				icnDrawIcon(NO_ENTER);
				break;


			default:
				break;
		};
#else
	testimage_init();
	Delayms(DELAYMIL);
	drawStringLoop();
	//Delayms(DELAYMIL);
	ugTest();
	Delayms(DELAYMIL);
#ifndef _LED64_64_1
	colWheelTest();
	Delayms(DELAYMIL);
#endif
	drawPlasma32x32();
	adaTest();
	adaTestShape32x32();
	Delayms(DELAYMIL);
	//control_uart_loop();
#endif
	}

	return 0;
}

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
