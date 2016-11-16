/**
  ******************************************************************************
  * @file    can.c
  * @author  hj park
  * @version V1.0
  * @date    01-December-2016
  * @brief   Can function.
  ******************************************************************************
*/

#include "stm32f4xx.h"
#include "stm32f4xx_can.h"
#include "can.h"

#define RX_TEST
//#define BUTTON_TEST

#if 1
 #define CANx                       CAN1
  #define CAN_CLK                    RCC_APB1Periph_CAN1
  #define CAN_RX_PIN                 GPIO_Pin_11
  #define CAN_TX_PIN                 GPIO_Pin_12
  #define CAN_GPIO_PORT              GPIOA
  #define CAN_GPIO_CLK               RCC_AHB1Periph_GPIOA
  #define CAN_AF_PORT                GPIO_AF_CAN1
  #define CAN_RX_SOURCE              GPIO_PinSource11
  #define CAN_TX_SOURCE              GPIO_PinSource12
#else
 #define CANx                       CAN1
  #define CAN_CLK                    RCC_APB1Periph_CAN1
  #define CAN_RX_PIN                 GPIO_Pin_8
  #define CAN_TX_PIN                 GPIO_Pin_9
  #define CAN_GPIO_PORT              GPIOB
  #define CAN_GPIO_CLK               RCC_AHB1Periph_GPIOB
  #define CAN_AF_PORT                GPIO_AF_CAN1
  #define CAN_RX_SOURCE              GPIO_PinSource8
  #define CAN_TX_SOURCE              GPIO_PinSource9
#endif

#ifdef BUTTON_TEST
#define KEY_PRESSED     0x00
#define KEY_NOT_PRESSED 0x01
#endif

#define TX_STD_ID 0x100
#define RX_STD_ID 0x200

CAN_InitTypeDef        CAN_InitStructure;
CAN_FilterInitTypeDef  CAN_FilterInitStructure;
CanTxMsg TxMessage;
CanRxMsg RxMessage;
uint8_t ubKeyNumber = 0x0;


uint32_t TxStdId = TX_STD_ID;
uint32_t RxStdId = RX_STD_ID;

#ifdef RX_TEST
static void NVIC_Config(void);
#endif


void Init_RxMes(CanRxMsg *RxMessage);

#define KEY_BUTTON_PIN                   GPIO_Pin_13
#define KEY_BUTTON_GPIO_PORT             GPIOC
#define KEY_BUTTON_GPIO_CLK              RCC_AHB1Periph_GPIOC
#define KEY_BUTTON_EXTI_LINE             EXTI_Line13
#define KEY_BUTTON_EXTI_PORT_SOURCE      EXTI_PortSourceGPIOC
#define KEY_BUTTON_EXTI_PIN_SOURCE       EXTI_PinSource13
#define KEY_BUTTON_EXTI_IRQn             EXTI15_10_IRQn

/**
  * @brief  This function handles CAN1 RX0 request.
  * @param  None
  * @retval None
  */
void CAN1_RX0_IRQHandler(void)
{
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

  if ((RxMessage.StdId == RxStdId)&&(RxMessage.IDE == CAN_ID_STD) && (RxMessage.DLC == 1))
  {
    //LED_Display(RxMessage.Data[0]);
    GPIO_ToggleBits(GPIOA, GPIO_Pin_5);
    ubKeyNumber = RxMessage.Data[0];
  }
}

uint8_t canGetData(void)
{
	return ubKeyNumber;
}

uint8_t canPutData(uint32_t TxId, uint8_t data)
{
	CanTxMsg TxMsg;

	TxMsg.StdId = TxId;
	TxMsg.ExtId = 0x01;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_STD;
	TxMsg.DLC = 1;
	TxMsg.Data[0] = data;
    CAN_Transmit(CANx, &TxMsg);
}

uint32_t STM_EVAL_PBGetState(void)
{
  return GPIO_ReadInputDataBit(KEY_BUTTON_GPIO_PORT, KEY_BUTTON_PIN);
}

void ledInit(void)
{
	  GPIO_InitTypeDef  GPIO_InitStructure;

		  /* GPIOA Peripheral clock enable */
		  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

		  /* Configure PA5 in output pushpull mode */
		  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		  GPIO_Init(GPIOA, &GPIO_InitStructure);
#ifdef BUTTON_TEST
		  /* Enable the BUTTON Clock */
		  RCC_AHB1PeriphClockCmd(KEY_BUTTON_GPIO_CLK, ENABLE);
		  /* Configure Button pin as input */
		  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		  GPIO_InitStructure.GPIO_Pin = KEY_BUTTON_PIN;
		  GPIO_Init(KEY_BUTTON_GPIO_PORT, &GPIO_InitStructure);
	#endif
}

int canTest(void)
{
#ifndef BUTTON_TEST
	 // while(1)
	  {
		TimingDelay(180000000);

		TxMessage.Data[0] = ubKeyNumber;
		CAN_Transmit(CANx, &TxMessage);
		while((CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP0) !=RESET) || \
							  (CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP1) !=RESET) || \
							  (CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP2) !=RESET));

		//GPIO_ToggleBits(GPIOA, GPIO_Pin_5);
	  }
#else
	  while(1)
	  {
		while(STM_EVAL_PBGetState() == KEY_PRESSED)
		{
		if(ubKeyNumber == 0x4)
		  {
			ubKeyNumber = 0x00;
		  }
		  else
		  {
			ubKeyNumber++;
			TxMessage.Data[0] = ubKeyNumber;
			CAN_Transmit(CANx, &TxMessage);
			/* Wait until one of the mailboxes is empty */
			while((CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP0) !=RESET) || \
				  (CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP1) !=RESET) || \
				  (CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP2) !=RESET));

			while(STM_EVAL_PBGetState() != KEY_NOT_PRESSED)
			{
			}
		  }
		}
	  }
#endif

}

/**
  * @brief  Configures the CAN.
  * @param  None
  * @retval None
  */
void CAN_Config(CAN_BAUDRATE bRate, uint32_t TxId, uint32_t RxId)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  TxStdId = TxId;
  RxStdId = RxId;

#ifdef RX_TEST
  /* NVIC configuration */
  NVIC_Config();
#endif

  /* CAN GPIOs configuration **************************************************/

  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(CAN_GPIO_CLK, ENABLE);

  /* Connect CAN pins to AF9 */
  GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_RX_SOURCE, CAN_AF_PORT);
  GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_TX_SOURCE, CAN_AF_PORT);

  /* Configure CAN RX and TX pins */
  GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN | CAN_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed/*GPIO_Speed_50MHz*/;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStructure);

  /* CAN configuration ********************************************************/
  /* Enable CAN clock */
  RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);

  /* CAN register init */
  CAN_DeInit(CANx);

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = DISABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;


  /* CAN Baudrate = 1 MBps (CAN clocked at 30 MHz) */
  if(bRate == CAN_1Mbps) //not working
  {
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
  CAN_InitStructure.CAN_BS1 = CAN_BS1_1tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
  CAN_InitStructure.CAN_Prescaler = 9;
  }else if(bRate == CAN_500Kbps ){ //working
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
  CAN_InitStructure.CAN_BS1 = CAN_BS1_2tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
  CAN_InitStructure.CAN_Prescaler = 15;
  }else if(bRate == CAN_250Kbps ) { //working
   CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
   CAN_InitStructure.CAN_BS1 = CAN_BS1_2tq;
   CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
   CAN_InitStructure.CAN_Prescaler = 30;
  }

   CAN_Init(CANx, &CAN_InitStructure);

  /* CAN filter init */
  CAN_FilterInitStructure.CAN_FilterNumber = 0;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

  /* Transmit Structure preparation */
  TxMessage.StdId = TxStdId;
  TxMessage.ExtId = 0x01;
  TxMessage.RTR = CAN_RTR_DATA;
  TxMessage.IDE = CAN_ID_STD;
  TxMessage.DLC = 1;

#ifdef RX_TEST
  RxMessage.StdId = RxStdId;
  RxMessage.ExtId = 0x01;
  RxMessage.RTR = CAN_RTR_DATA;
  RxMessage.IDE = CAN_ID_STD;
  RxMessage.DLC = 1;
  RxMessage.FMI = 0;
  RxMessage.Data[0] = 0;
#endif

#ifdef RX_TEST
  /* Enable FIFO 0 message pending Interrupt */
  CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
#endif
}





static void NVIC_Config(void)
{
  NVIC_InitTypeDef  NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Initializes the Rx Message.
  * @param  RxMessage: pointer to the message to initialize
  * @retval None
  */
void Init_RxMes(CanRxMsg *RxMessage)
{
 /* uint8_t ubCounter = 0;

  RxMessage->StdId = 0x00;
  RxMessage->ExtId = 0x00;
  RxMessage->IDE = CAN_ID_STD;
  RxMessage->DLC = 0;
  RxMessage->FMI = 0;
  for (ubCounter = 0; ubCounter < 8; ubCounter++)
  {
    RxMessage->Data[ubCounter] = 0x00;
  }*/
}
