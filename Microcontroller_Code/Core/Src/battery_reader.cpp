/*
 * BatteryReader.cpp
 *
 *  Created on: Oct 22, 2023
 *      Author: victor
 */
///////////////////////////////////////////////////////
//Battery reader functionality is not yet implemented//
///////////////////////////////////////////////////////
#include <battery_reader.h>
#include "math.h"
#include "stm32f1xx.h"



BatteryReader::BatteryReader() {
}

/**
 * Set up Analog In
 */
void BatteryReader::initBatteryLevel(ADC_HandleTypeDef hadc1) {
	battery_hadc1 = hadc1;
	HAL_ADC_Start(&battery_hadc1);
	HAL_Delay(300);
}

/**
 * Returns value measured in mV.
 * Important, watch out for input voltages over 3.3V in pin that do not tolerate that
 */
uint32_t BatteryReader::getBatteryLevel() {
	HAL_ADC_PollForConversion(&battery_hadc1, 1000);
	uint32_t val  = HAL_ADC_GetValue(&battery_hadc1);
	return (uint32_t) val*3.3*1000/4096;   //Return in mV
}

