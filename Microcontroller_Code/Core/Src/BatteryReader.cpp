/*
 * BatteryReader.cpp
 *
 *  Created on: Oct 22, 2023
 *      Author: victor
 */
///////////////////////////////////////////////////////
//Battery reader functionality is not yet implemented//
///////////////////////////////////////////////////////
#include "BatteryReader.h"
#if ENABLE_BATTERY_READER
#include "math.h"
#define BATTERY_VOL_MAX 7.6
#define BATTERY_VOL_MIN 7.0
#define PULL_UP_RESISTOR 10460000
#define PULL_DOWN_RESISTOR 5600000

extern ADC_HandleTypeDef hadc1;
BatteryReader::BatteryReader() {
}

uint32_t BatteryReader::getBatteryLevel() {
	HAL_ADC_Start(&hadc1);
   // Poll ADC1 Peripheral & TimeOut = 1mSec
	HAL_ADC_PollForConversion(&hadc1, 50);
   // Read The ADC Conversion Result & Map It To PWM DutyCycle
	uint32_t getValueADC = HAL_ADC_GetValue(&hadc1);//Needs low resistance (for high A) and high ADC freq
	HAL_ADC_Stop(&hadc1);
	float voltage = 3.3f * getValueADC / 4096.0f;
	//TODO
	float vol_conterted = getValueADC * 3.3 / pow(2,32);

	float in_max_vol = BATTERY_VOL_MAX*PULL_DOWN_RESISTOR/(PULL_DOWN_RESISTOR + PULL_UP_RESISTOR);
	float in_min_vol = BATTERY_VOL_MIN*PULL_DOWN_RESISTOR/(PULL_DOWN_RESISTOR + PULL_UP_RESISTOR);
	float in_real_vol = (vol_conterted - in_min_vol)/(in_max_vol - in_min_vol);
	//My setup:
	//     BATT V
	//   10.46MOhm
	//     ADC
	//    5.6MOhm
	//     0V
	return (uint32_t)in_real_vol;
}

#endif
