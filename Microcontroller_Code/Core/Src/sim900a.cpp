#include "sim900a.h"
#include <stdio.h>
#include <ctype.h>
#include "stm32f1xx.h"
#include "stm32f1xx_it.h"
#include <vector>
#include <string.h>
#include "utilfunctions.h"
#define BUFF_LEN 1000
char x;
string result;
string end_of_line = "\r\n";
string serverAddress = "XXXXXXXX"; //Write you server address
string serverPort = "XXXX";  //Write you server port
              //You allso need to write your APN in line 56
string latest_response;
string CONNECT_OK = "CONNECT OK";
string CLOSE_OK = "CLOSE OK";
string SEND_OK = "SEND OK";
string STATE_IP_STATUS = "STATE: IP STATUS";
extern uint8_t rxData[8];
extern UART_HandleTypeDef huart1;
extern char my_uart_buffer[RX_GPRS_BUFFER_SIZE];
extern int my_uart_buffer_index;

string SIM900A::waitForResponse(float time) {
    HAL_Delay(time*1000);
    memset(my_uart_buffer+my_uart_buffer_index, '\0', RX_GPRS_BUFFER_SIZE-my_uart_buffer_index);
    char ret[RX_GPRS_BUFFER_SIZE];
    strcpy(ret, my_uart_buffer);
    memset(my_uart_buffer, '\0', RX_GPRS_BUFFER_SIZE);
    my_uart_buffer_index = 0;
    return ret;
}
string SIM900A::sendGSM(string cmd, float wait_time) {
	vector<uint8_t> myVector(cmd.begin(), cmd.end());
	HAL_UART_Transmit (&huart1, &myVector[0], cmd.size(), HAL_MAX_DELAY);
	HAL_UART_Receive_IT(&huart1, rxData, 1);
    return waitForResponse(wait_time);
}

SIM900A::SIM900A() {
}

bool SIM900A::init(void){
	//Wait 8 sec for GSM to connect to network (arbitrary)
	HAL_Delay(8000);
    sendGSM("AT\r\n");// all is well
    sendGSM("AT+CPIN?\r\n");//check if SIM is unlocked
    sendGSM("AT+CREG?\r\n");//checks if SIM is registered or not
    sendGSM("AT+CGATT?\r\n",1);//GPRS is attached or not
    sendGSM("AT+CIPSHUT\r\n",2);//Reset the IP session if any
    sendGSM("AT+CIPSTATUS\r\n");//Check if the IP stack is initialized
    sendGSM("AT+CIPMUX=0\r\n");//multi connection mode
    
    sendGSM("AT+CSTT=\"XXXXXX\"\r\n");//Write your APN name
    sendGSM("AT+CIICR\r\n",5);//Bring up the wireless. The response to this might take some time
    sendGSM("AT+CIFSR\r\n");//Get the local IP address
    return sendGSM("AT+CIPSTATUS\r\n",5).find(STATE_IP_STATUS) != string::npos;//current connection status: STATE:IP STATUS
}
bool SIM900A::connectToServer() {
    return sendGSM("AT+CIPSTART=\"TCP\",\""+serverAddress+"\"," + serverPort +"\r\n",2).find(CONNECT_OK) != string::npos;
}

bool SIM900A::sendPayload(string payload) {
    char end = 0x1A;
    sendGSM("AT+CIPSEND\r\n",1);//Request initiation of data sending
    return sendGSM(payload+end,3).find(SEND_OK) != string::npos;//Ctrl+Z key combination to indicate end of data sending ((char)26);// ASCII code of CTRL+Z)   
}

bool SIM900A::disconnect(){
    return sendGSM("AT+CIPCLOSE\r\n",2).find(CLOSE_OK) != string::npos;
}

void SIM900A::shutdown(){
    sendGSM("AT+CIPSHUT\r\n");//To close the connection
    waitForResponse(1);
    toggleDebugLED();
}
string SIM900A::getLastResponse(){
    return waitForResponse(2);
}
