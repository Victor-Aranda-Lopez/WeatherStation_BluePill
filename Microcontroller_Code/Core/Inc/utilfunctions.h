#ifndef __UTIL_FUNCTIONS__
#define __UTIL_FUNCTIONS__
#include <string>
#include <string.h>
#include <vector>
#include <time.h>
#include "stm32f1xx.h"
#include <main.h>
#include "sim900a.h"
#include "AM2321.h"

using namespace std;
void initDebugLED();
void toggleDebugLED();
string createMessage (Measure measures, bool newSeriesOfMeasurements);
bool processResponse(string response);
bool sendMeasuresToServerAndSync (Measure measures);
void sleepForNextMeasure();
bool initServerMeasurements();
vector<string> splitStringByDelimiter(string s,char* delimiter);
void DebugPrint(const char *text);
void DebugPrint(string text);
void configRetrialOfConnection();
void enablePeripherals();
void disablePeripherals();
#endif
