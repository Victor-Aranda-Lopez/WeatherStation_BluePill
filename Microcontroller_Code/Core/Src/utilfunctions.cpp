#include "utilfunctions.h"
#include <stdio.h>
#include <ctype.h>
#include "stm32f1xx.h"
#include <vector>
#include <string.h>
#include "BatteryReader.h"
using namespace std;

extern Measure measures;
extern SIM900A sim900a;
extern AM2321 tempHumSensor;
extern bool takeMeasureBool;
extern char* toPrint;
extern int secondsToWait;
extern long nextMeasureTimeSeconds;
extern long currentTimeSeconds;
extern bool startNewSeriesOfMeasurements ;
extern UART_HandleTypeDef huart2;
extern RTC_HandleTypeDef hrtc;

#define PAYLOAD_HEADER_LENGHT 6
#define MOCK_MEASURES 0
#define DEFAULT_WAIT_TIME_NO_SERVER_RESPONSE_SECONDS 15

void initDebugLED(){
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOC_CLK_ENABLE();
	/*Configure GPIO pin : PC13 */
	  GPIO_InitStruct.Pin = GPIO_PIN_13;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_PULLUP;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

}
void toggleDebugLED(){
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}
void DebugPrint(const char *text)
{
  HAL_UART_Transmit(&huart2, (uint8_t *)text, strlen(text), 100U);
}
void DebugPrint(string text)
{
  HAL_UART_Transmit(&huart2, (uint8_t *)text.c_str(), text.size(), 100U);
}
string createMessage (Measure measures, bool newSeriesOfMeasurements = false) {
	char payload[50];
	int lenght_payload = snprintf(payload, sizeof(payload),"%d;%d", (int)(measures.temperature*10),(int)(measures.humidity*10));

    char s_message[50];
    s_message[0]='0';
    s_message[1]='1';
    char lenght_buffer[6];
    snprintf(lenght_buffer,sizeof(lenght_buffer),"%04d",lenght_payload);
    strncat (&s_message[2], &lenght_buffer[0], sizeof(lenght_buffer));
    strncat (&s_message[6], &payload[0], lenght_payload);
    string out_string = s_message;

    DebugPrint("ss_message: ");
    DebugPrint(out_string.c_str());
    DebugPrint("\r\n");
    return s_message;
}
bool processResponse(string response) {
    if(response.length()==0)//TODO: more checks
        return false;

    int payloadLenght = atoi(response.substr(2,4).c_str());
    if(payloadLenght==0)
        return false;
    string payload = response.substr(PAYLOAD_HEADER_LENGHT,PAYLOAD_HEADER_LENGHT+payloadLenght);

    std::vector<std::string> seglist;
    seglist = splitStringByDelimiter(payload,";");//%3B
    for(int i=0;i<(int)seglist.size();i++) {
        if(i==0) {//Current time
			char * pEnd;
			currentTimeSeconds = strtol(seglist[0].c_str(), &pEnd, 10);
        }
        if(i==1) {//Next measure
            char * pEnd;
            nextMeasureTimeSeconds = strtol(seglist[1].c_str(), &pEnd, 10);
        }
    }
    return true;
}
bool sendMeasuresToServerAndSync (Measure measures) {
    if(sim900a.connectToServer()){
    	DebugPrint("Connected to server\r\n");
    }else{
    	DebugPrint("ERROR: Not connected to server\r\n");
        return false;
    }

    DebugPrint("Reading temp and hum...\r\n");
    string message_s = createMessage(measures).c_str();

    DebugPrint("Sending: ");
    DebugPrint(message_s.c_str());
    DebugPrint("\r\n");
    if(sim900a.sendPayload(message_s)){
        DebugPrint("Sent payload to server\r\n");
    }else{
        DebugPrint("ERROR: payload not sent to server\r\n");
        return false;
    }
    //Wait seconds for server response is made
    HAL_Delay(4000);
    //Response
    processResponse(sim900a.getLastResponse());//"0000211701515024;1701518400"
    //Disconnect
    HAL_Delay(500);
    if (sim900a.disconnect()){
        DebugPrint("Disconnected from to server\r\n\r\n");
    }else{
        DebugPrint("ERROR: Not disconnected from server\r\n\r\n");
    }
    if (nextMeasureTimeSeconds != 0 && currentTimeSeconds != 0) {
    	return true;
    }else{
    	return false;
    }
}

void sleepForNextMeasure() {
      /* Clear standby flag */
   __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
   /* Clear wakeup flag */
   __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

  if(nextMeasureTimeSeconds == 0) {
      nextMeasureTimeSeconds = DEFAULT_WAIT_TIME_NO_SERVER_RESPONSE_SECONDS;
  }
	HAL_RTC_WaitForSynchro(&hrtc);
	int secondsSleeping = (nextMeasureTimeSeconds-currentTimeSeconds);
	RTC_TimeTypeDef  stimestructure;
	RTC_DateTypeDef sDate;
	RTC_AlarmTypeDef salarmstructure;
	//Set time to 0 by default
	HAL_RTC_WaitForSynchro(&hrtc);
	stimestructure.Hours = 0;
	stimestructure.Minutes = 0;
	stimestructure.Seconds = 0;
	if(HAL_RTC_SetTime(&hrtc,&stimestructure,RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
	sDate.WeekDay = RTC_WEEKDAY_MONDAY;
	sDate.Month = RTC_MONTH_JANUARY;
	sDate.Date = 1;
	sDate.Year = 0;
	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}

  /* Set Alarm - RTC Alarm Generation: Alarm on Hours, Minutes and Seconds */
	int h(secondsSleeping/3600);
	int min(secondsSleeping/60 - h*60);
	int sec(secondsSleeping - (h*60 + min)*60);
  salarmstructure.Alarm = RTC_ALARM_A;
  salarmstructure.AlarmTime.Hours = h;
  salarmstructure.AlarmTime.Minutes = min;
  salarmstructure.AlarmTime.Seconds = sec;
  if(HAL_RTC_SetAlarm_IT(&hrtc,&salarmstructure,RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
    DebugPrint("Entering in STANDBY mode\r\n");
    HAL_SuspendTick();
    HAL_PWR_EnterSTANDBYMode();
}

vector<string> splitStringByDelimiter(string s,char* delimiter){
    vector<string> res;
    int pos = 0;
    while(pos < sizeof(s)){
        pos = s.find(delimiter);
        res.push_back(s.substr(0,pos));
        s.erase(0,pos+1); // +3 3 is the length of the delimiter, "%20"
    }
    return res;
}

void configRetrialOfConnection(){
	//Rest time to 0 to indicate that connection to server failed
	//So, when config standby, it will only sleep DEFAULT_WAIT_TIME_NO_SERVER_RESPONSE_SECONDS
	currentTimeSeconds = 0;
}

//Activates peripherals at 5V
void enablePeripherals(){
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	  /* GPIO Ports Clock Enable */
	  __HAL_RCC_GPIOC_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();

	  /*Configure GPIO pin Output Level */

	  /*Configure GPIO pin : PA8 */
	  GPIO_InitStruct.Pin = GPIO_PIN_8;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);

}
void disablePeripherals(){
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	  __HAL_RCC_GPIOC_CLK_DISABLE();
	  __HAL_RCC_GPIOA_CLK_DISABLE();
	  __HAL_RCC_GPIOB_CLK_DISABLE();
}
