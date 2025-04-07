#include "sim900a.h"
#include <stdio.h>
#include "stm32f1xx.h"
#include <vector>
#include <string.h>
#include "utilfunctions.h"
#include "MQTTPacket/src/MQTTPacket.h"

/*------------------------
 * Elements to configure SIM900 to your aplication
 */
//Write clientId of the sensor for MQTT
char *  mqtt_sensor_clientId =  "123456789";//  Change with random ID
char *  mqtt_sensor_username =  "123456789";//  Change with random ID
char *  mqtt_sensor_password =  "123456789";//  Change with random ID
// Write your network provider 
char ATCSTT[] = "AT+CSTT=\"xxxx\"\r\n";
//-------------


#define BUFF_LEN 1000
char * end_of_line = "\r\n";
  //Write you server port
              //You also need to write your APN in line 56
string latest_response;
char CONNECT_OK []= "CONNECT OK";
char CLOSE_OK []= "CLOSE OK";
char SEND_OK []= "SEND OK";
char STATE_IP_STATUS [] = "STATE: IP STATUS";
char OK[] = "OK";
char AT[] = "AT\r\n";
char ATCPIN[] = "AT+CPIN?\r\n";
char ATCREG[] = "AT+CREG?\r\n";
char ATCGATT[] = "AT+CGATT?\r\n";
char ATCIPSHUT[] = "AT+CIPSHUT\r\n";
char ATCIPSTATUS[] = "AT+CIPSTATUS\r\n";
char ATCIPMUX0[] = "AT+CIPMUX=0\r\n";
char ATCIICR[] = "AT+CIICR\r\n";
char ATCIFSR[] = "AT+CIFSR\r\n";
//char ATCIPSTART_CONNECT_TCP[] = "AT+CIPSTART=\"TCP\",\"<server IP or dns>\",<server port>\r\n";
char ATCIPSEND[] = "AT+CIPSEND\r\n";

char ATCIPCLOSE[] = "AT+CIPCLOSE\r\n";

char reply[600]= { 0 };
extern uint8_t rxData[8];
extern UART_HandleTypeDef huart1;
extern char my_uart_buffer[RX_GPRS_BUFFER_SIZE];
extern int my_uart_buffer_index;
uint8_t * mqtt_message;
unsigned char mqttMessage_header[127];
MQTTPacket_connectData data;

SIM900A::SIM900A() {
	data = MQTTPacket_connectData_initializer;
}

bool SIM900A::init(void){
	//Wait 8 sec for GSM to connect to network (arbitrary)
	memset(reply, '\0', sizeof(reply));
	HAL_Delay(2000);
	HAL_UART_Transmit (&huart1, (uint8_t *) AT, strlen(AT), HAL_MAX_DELAY);
	HAL_UART_Receive(&huart1,(uint8_t *) reply,20,1000);
	if(!strstr(reply,OK))
		return false;
	HAL_Delay(1000);
	HAL_UART_Transmit (&huart1, (uint8_t *) ATCPIN, strlen(ATCPIN), HAL_MAX_DELAY);
	HAL_Delay(1000);
	HAL_UART_Transmit (&huart1, (uint8_t *) ATCREG, strlen(ATCREG), HAL_MAX_DELAY);
	HAL_Delay(1000);
	HAL_UART_Transmit (&huart1, (uint8_t *) ATCGATT, strlen(ATCGATT), HAL_MAX_DELAY);
	HAL_Delay(1000);
	HAL_UART_Transmit (&huart1, (uint8_t *) ATCIPSHUT, strlen(ATCIPSHUT), HAL_MAX_DELAY);
	HAL_Delay(2000);
	HAL_UART_Transmit (&huart1, (uint8_t *) ATCIPSTATUS, strlen(ATCIPSTATUS), HAL_MAX_DELAY);
	HAL_Delay(1000);
	HAL_UART_Transmit (&huart1, (uint8_t *) ATCIPMUX0, strlen(ATCIPMUX0), HAL_MAX_DELAY);
	HAL_Delay(1000);
	HAL_UART_Transmit (&huart1, (uint8_t *) ATCSTT, strlen(ATCSTT), HAL_MAX_DELAY);
	HAL_Delay(1000);
	HAL_UART_Transmit (&huart1, (uint8_t *) ATCIICR, strlen(ATCIICR), HAL_MAX_DELAY);
	HAL_Delay(5000);
	HAL_UART_Transmit (&huart1, (uint8_t *) ATCIFSR, strlen(ATCIFSR), HAL_MAX_DELAY);
	HAL_Delay(1000);
	HAL_UART_Receive(&huart1,(uint8_t *) reply,200,1000);
	memset(reply, '\0', sizeof(reply));
	HAL_UART_Transmit (&huart1, (uint8_t *) ATCIPSTATUS, strlen(ATCIPSTATUS), HAL_MAX_DELAY);
	HAL_Delay(1000);
	HAL_UART_Receive(&huart1,(uint8_t *) reply,200,5000);
	//TODO: test this :find "STATE:IP STATUS" string in reply to confirm the return value
	return strstr(reply,STATE_IP_STATUS);
}

void create_ATCIPSTART_CONNECT_TCP_command(char * command, char * serverAddress, char * serverPort) {
	strcat (command, "AT+CIPSTART=\"TCP\",\"");
	strcat (command, serverAddress);
	strcat (command, "\",");
	strcat (command, serverPort);
	strcat (command, end_of_line);
}

/**
 * Send MQTT payload to server. Inspured from https://github.com/eclipse-paho/paho.mqtt.embedded-c/tree/master/MQTTPacket
 */
bool SIM900A::MQTT_publish_payload(char * serverAddress, char * serverPort, char  * topic, char  * payload) {
	//Clean reply buffer
	memset(reply, '\0', sizeof(reply));
	HAL_UART_Receive(&huart1,(uint8_t *) reply,200,1000);
	memset(reply, '\0', sizeof(reply));
	//Prepare AT-CIPSTART_CONNECT TCP command
	char command_buffer[80];
	create_ATCIPSTART_CONNECT_TCP_command(command_buffer, serverAddress, serverPort);

	HAL_UART_Transmit (&huart1, (uint8_t *) command_buffer, strlen(command_buffer), HAL_MAX_DELAY);
	HAL_UART_Receive(&huart1,(uint8_t *) reply,200,2000);
	if(!strstr(reply,CONNECT_OK)){
		return false;
	}

    MQTTString topicString = MQTTString_initializer;
	data.keepAliveInterval = 20;
	data.cleansession = 1;
	data.MQTTVersion = 3;
	//Choose either clientID or pair user and password
	data.clientID.cstring = mqtt_sensor_clientId;
	data.username.cstring = mqtt_sensor_username;
	data.password.cstring = mqtt_sensor_password;

	unsigned char buf[200]={0};
	unsigned char recv[200]={0};
	int buflen = sizeof(buf);
	int payloadlen = strlen(payload);
	int len = 0;

	char end = 0x1A;
	len = MQTTSerialize_connect(buf, buflen, &data);
	buf[len]= end;
	len++;
	HAL_UART_Transmit (&huart1, (uint8_t *) ATCIPSEND, strlen(ATCIPSEND), HAL_MAX_DELAY);
	HAL_Delay(1500);
	HAL_UART_Transmit (&huart1, (uint8_t *) buf, buflen, HAL_MAX_DELAY);
	HAL_Delay(3000);

	string ret;
	len = 0;
	topicString.cstring = topic;
	len += MQTTSerialize_publish((unsigned char *)(buf + len), buflen - len, 0, 0, 0, 0, topicString, (unsigned char *)payload, payloadlen);
	//TODO: add disconnect (optional)
	//len += MQTTSerialize_disconnect((unsigned char *)(buf + len), buflen - len);
	buf[len]= end;
	len++;
	HAL_UART_Transmit (&huart1, (uint8_t *) ATCIPSEND, strlen(ATCIPSEND), HAL_MAX_DELAY);
	HAL_Delay(1500);
	HAL_UART_Transmit (&huart1, (uint8_t *) buf, buflen, HAL_MAX_DELAY);
	HAL_Delay(3000);
	//TODO: check in HAL_UART_Receive that contains "SEND OK"
	/* HAL_UART_Receive(&huart1,(uint8_t *) reply,200,2000);
	   if(!strstr(reply,SEND_OK)){
			disconnect();
			return false;
	   }
	*/
	disconnect();
	//It is assumed that it has gone well
	return true;
}


void SIM900A::disconnect(){
	HAL_UART_Transmit (&huart1, (uint8_t *) ATCIPCLOSE, strlen(ATCIPCLOSE), HAL_MAX_DELAY);
}

void SIM900A::shutdown(){
	HAL_UART_Transmit (&huart1, (uint8_t *) ATCIPSHUT, strlen(ATCIPSHUT), HAL_MAX_DELAY);
	HAL_Delay(1000);
    toggleDebugLED();
}
