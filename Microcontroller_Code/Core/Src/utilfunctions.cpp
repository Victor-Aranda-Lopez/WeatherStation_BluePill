#include "utilfunctions.h"
#include "stm32f1xx.h"
using namespace std;

/*----------------------------------------------------------
 * Elements to configure before connecting to server
 */
//MQTT server configuration
char * mqtt_server = "1.2.3.4"; //dns or ip
char * mqtt_server_port = "1234";

// Write MQTT topic where data is published
char * topic = "mqttdemo";
//------------------------------------------------------


extern Measure measures;
extern SIM900A sim900a;
extern bool takeMeasureBool;
extern long nextMeasureTimeSeconds;
extern long currentTimeSeconds;
extern UART_HandleTypeDef huart2;
extern RTC_HandleTypeDef hrtc;

#define DEFAULT_WAIT_TIME_NEXT_MEASURE 5

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
/*
 * Creates json based on all measures made.
 * Modify it for you needs.
 */
void createMessage (char* buf, Measure measures) {
    char s_message[60]={0};
    strcat (s_message, "{\"temperature\":");
    char con[8];
    snprintf(con,sizeof(con), "%.4f", measures.temperature);
    strcat (s_message, con);
    strcat (s_message, ",\"humidity\":");
    snprintf(con,sizeof(con), "%.4f", measures.humidity);
    strcat (s_message, con);
    strcat (s_message, ",\"battery\":");
    snprintf(con,sizeof(con), "%ld", measures.battery);
    strcat (s_message, con);
    strcat (s_message, "}");

    strcpy(buf,s_message);
}

bool sendMeasuresToServer (Measure measures) {
	char payload[60];
	createMessage(payload,measures);
	return sim900a.MQTT_publish_payload(mqtt_server, mqtt_server_port, topic, payload);
}

void sleepForNextMeasure() {
      /* Clear standby flag */
   __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
   /* Clear wakeup flag */
   __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

	HAL_RTC_WaitForSynchro(&hrtc);
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
	int secondsSleeping = DEFAULT_WAIT_TIME_NEXT_MEASURE;

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
    HAL_SuspendTick();
    HAL_PWR_EnterSTANDBYMode();
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

	  // __HAL_RCC_GPIOA_FORCE_RESET();
	  // __HAL_RCC_GPIOB_FORCE_RESET();
	  // __HAL_RCC_GPIOC_FORCE_RESET();
	  __HAL_RCC_GPIOC_CLK_DISABLE();
	  __HAL_RCC_GPIOA_CLK_DISABLE();
	  __HAL_RCC_GPIOB_CLK_DISABLE();
}
