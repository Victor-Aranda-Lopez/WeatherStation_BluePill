# IoT weather sensor system - proof of concept
## Introduction

This project is a simple proof of concept for a IoT weather sensor system that consist of microcontrollers (from now on "motes") and a MQTT server (from now on simply as "server") that collects all data. The motes use a battery as a energy supply.

Here I have created a simplification of that system with only 1 mote and the server. The motes use GSM (2G) technology because it is simpler, less enegy consuming, low data traffic is used, and 5G modules for IoT/microcontrollers are too expensive currently and 5G coverage is not widespread.

    Mote    <--   2G  -->    Server   
    
## Technologies used
- Stm32CubeIde - IDE to program the motes. (Side note: I tried many others like MbedStudio, uKeil, PlatformIO IDE but Stm32CubeIde allows much more flexibility, specially to archive low consumption)
- C++ -->  Programming language for the motes
- GSM --> Main transmission network
- MQTT --> Application protocol to transmit data
- ThingsBoard --> Open sourced program with MQTT server to receive, process and display data.
## Contents
Mote code can be found inside Microcontroller_Code folder
## Mote program flow
The mote will measure the temperature and humidity using the AM2320 sensor, and the battery. It enables the SIM900A module. Then it will be transmitted to the server with the 2G SIM900A module. The comunication between the microcontroller and the SIM900A module is done using AT commands. A custom library was made to make it simpler to use. 

A json payload with temperature, humidity and battery level is transmitted to the server using the MQTT protocol. In order to build the headers of the MQTT protocol, this library was used: https://github.com/eclipse-paho/paho.mqtt.embedded-c/tree/master/MQTTPacket .

Finally, after transmitting the payload, SIM900A module is powered off, and the mote will sleep using standby mode of STM32F103C8T6 for a fixed amount of time before repeating the process in a new weather measurement.
## Mote hardware
2 schemas have been uploaded to the Schema folder. 
- schema simple.pdf: it has been implemented and useful for testing environments, but nit for produccion since the battery is directly connected to the microcontorller and GSM module which will drain the battery and damage it it there is no undervoltage protection. You can see the photos of this schema in Images folder. Note: a capacitor has been aded parallel to R4 but it doesn't improve the accuracy.
- schema with solar charger module.pdf: suggested production schema with a module to automatically charge the battery with a solar panel and control the battery and protect it from overdischarge or overcharge.
## MQTT Application protocol
It is implementred MQTT 3 using usign either cliendID or pair user and password as credentials with the server
Refer to https://github.com/eclipse-paho/paho.mqtt.embedded-c/tree/master/MQTTPacket for more details, and in sim900a.cpp file, MQTT_publish_payload method
## Results in Thing ThingsBoard
In ![Images/ThingsBoard data.png](https://github.com/Victor-Aranda-Lopez/WeatherStation_BluePill/blob/main/Images/ThingsBoard%20data.png), you can find the data received by the server, and ready to be processed.

## Other characteristics
 - Mote uses standby HAL function to sleep using the least amount of energy (I measured 40uA)
 - Code checks that the SIM900A has connected to the network, and if it has establed TCP connection with the MQTT server, but it doesn't confirm if the MQTT payloads where received and processed by the server. See SIM900A::MQTT_publish_payload function comments.
 - When SIM900A transmits, it has spikes of 2000 mA of current, so I strongly recommend adding capacitors and a power supply capable of supporting them. The normal consumption is arround 50 mA
 - The purpose of powering off the SIM900A module is to remove any parasite energy consumption while the system is sleeping and save 50mA. This is accomplished with a transistor connected to a PIN in the microcontroller. See Mote hardware



## License

[MIT](https://choosealicense.com/licenses/mit/)


## Acknowledgements

 - [AM2320 library](https://github.com/SimpleMethod/STM32-AM2320/tree/master)
 - [MQTT paho library](https://github.com/eclipse-paho/paho.mqtt.embedded-c/tree/master/MQTTPacket)

