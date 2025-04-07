#ifndef __GSM_H__
#define __GSM_H__
#include <string>

using namespace std;

class SIM900A
{
public:
    SIM900A();
    bool init(void);
    void disconnect();
    void shutdown(void);
    bool MQTT_publish_payload(char * serverAddress, char * serverPort, char  * topic, char  * message);
};


#endif // __AM2321_H__


