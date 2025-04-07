#ifndef __UTIL_FUNCTIONS__
#define __UTIL_FUNCTIONS__
#include <string>
#include <string.h>
#include <vector>
#include <time.h>
#include "stm32f1xx.h"
#include <main.h>
#include "sim900a.h"

using namespace std;
void initDebugLED();
void toggleDebugLED();
void createMessage (Measure measures);
void sleepForNextMeasure();
void enablePeripherals();
void disablePeripherals();
bool sendMeasuresToServer (Measure measures);
#endif
