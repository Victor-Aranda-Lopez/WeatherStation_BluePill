/*
 * am2320.h
 *
 *  Created on: 04.08.2019
 *      Author: SimpleMethod
 *  Updated on: 01.10.2020
 *Copyright 2019 SimpleMethod
 *
 *Permission is hereby granted, free of charge, to any person obtaining a copy of
 *this software and associated documentation files (the "Software"), to deal in
 *the Software without restriction, including without limitation the rights to
 *use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *of the Software, and to permit persons to whom the Software is furnished to do
 *so, subject to the following conditions:
 *
 *The above copyright notice and this permission notice shall be included in all
 *copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *THE SOFTWARE.
 ******************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif

// all of your legacy C code here


#ifndef INC_AM2320_H_
#define INC_AM2320_H_


#include "main.h"

#define AM2320_ADDRESS 0x5C<<1



typedef struct {
	I2C_HandleTypeDef* i2c_handle;
	uint8_t device_address;
	uint8_t data[8];
} Am2320_HandleTypeDef;


Am2320_HandleTypeDef am2320_Init(I2C_HandleTypeDef* i2c_handle,uint8_t device_address);
uint8_t am2320_ReadValue(Am2320_HandleTypeDef *am2320);
void am2320_GetTemperatureAndHumidity(Am2320_HandleTypeDef *am2320, float *temperature, float *humidity);

#endif /* INC_AM2320_H_ */
#ifdef __cplusplus
}
#endif
