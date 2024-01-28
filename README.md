# IoT weather sensor system - proof of concept
## Introduction

This project is a simple proof of concept for a IoT weather sensor system that consist of microcontrollers (from now on "motes") and a central server (from now on simply as "server") that collects all data.

Here I have created a simplification of that system with only 1 mote and the server. The motes use GSM (2G) technology because it is simpler, less enegy consuming, low data traffic is used, and 5G modules for IoT/microcontrollers are too expensive currently and 5G coverage is not spread.

   
    Mote    <--   2G  --> Router <--   Wifi  -->    Server   
    
## Technologies used
- Stm32CubeIde - IDE to program Motes. (Side note: I tried many others like MbedStudio, uKeil, PlatformIO IDE but Stm32CubeIde allows much more flexibility)
- C++ -  Programming language for the motes
- Python   -  Server programming language
- SQLLite3 - DB to store measurements in server
- GSM - Main transmission network
- Rasbian - Server OS (Raspberry 4B was used)

## Contents
Mote code can be found inside Microcontroller_Code folder, while server code in Server_Code
## General working flow
The mote will measure the temperature and humidity using the AM2321 sensor. Then it will be transmitted to teh server with the SIM900a module using AT commands. (A library was made to make it simpler to use). A custom application protocol (see application protocol section) is used since SIM900a is too simple that it does not support for modern protocols. (Recommendation: use a better communication module, because SIM900a gave me many headaches 😬).

Server will be listening to incomming messages. When a message is received and it is valid, temperature and humidity are stored in DB and a response wiht the current time and when the server wants the next measurement is sent to the mote.

Finally, the mote will received the current and next measurement time and it will sleep (using standby method of HAL) until next measurement.
## Mote hardware
//TODO
## Application protocol
Message from mote to server.
01{payload size : 4B}{temperature};{humidity}

payload size: size in bytes of the whole payload. This field has a size of 4B.
temperature: value measured. As interger
humidity: value measured. As interger

Response message from server to mote
00{payload size : 4B}{currentTime};{nextMeasureTime}

payload size: size in bytes of the whole payload. This field has a size of 4B.
currentTime: current time in miliseconds since epoch
nextMeasureTime: time for the next measurement in miliseconds since epoch

## Other functionalities
 - Mote uses standby HAL function to sleep using the least amount of energy (I measured 40uA)
 - Use killprocess.py if server doesn't unblock the port. Usefull when debuging

## License

MIT

**Free Software, Hell Yeah!**


