/*
 * can.h
 *
 *  Created on: 2016. 8. 24.
 *      Author: HJ Park
 */

#ifndef CAN_H_
#define CAN_H_


typedef enum
{
	CAN_1Mbps,
	CAN_500Kbps,
	CAN_250Kbps
} CAN_BAUDRATE;

void CAN_Config(CAN_BAUDRATE bRate, uint32_t TxId, uint32_t RxId);

#endif /* CAN_H_ */
