/*
 * BatteryReader.h
 *
 *  Created on: Oct 22, 2023
 *      Author: victor
 */
///////////////////////////////////////////////////////
//Battery reader functionality is not yet implemented//
///////////////////////////////////////////////////////


#ifndef INC_BATTERYREADER_H_
#define INC_BATTERYREADER_H_

#include "stm32f1xx.h"
#include "main.h"
class BatteryReader {
public:
	BatteryReader();
	void initBatteryLevel(ADC_HandleTypeDef hadc1);
	uint32_t getBatteryLevel();
private:
	ADC_HandleTypeDef battery_hadc1;
};



#endif /* INC_BATTERYREADER_H_ */

