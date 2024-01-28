/*
 * BatteryReader.h
 *
 *  Created on: Oct 22, 2023
 *      Author: victor
 */
///////////////////////////////////////////////////////
//Battery reader functionality is not yet implemented//
///////////////////////////////////////////////////////

#define ENABLE_BATTERY_READER 0
#if ENABLE_BATTERY_READER
#ifndef INC_BATTERYREADER_H_
#define INC_BATTERYREADER_H_

#include "stm32f1xx.h"
#include "main.h"
class BatteryReader {
public:
	BatteryReader();
	uint32_t getBatteryLevel();
};



#endif /* INC_BATTERYREADER_H_ */

#endif
