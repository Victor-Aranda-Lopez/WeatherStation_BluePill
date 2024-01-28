#ifndef __GSM_H__
#define __GSM_H__
#include <string>
//#include "Serial.h"
using namespace std;

void uart2_handler(void);


class SIM900A
{
public:
    SIM900A();
    
    bool init(void);

    bool connectToServer(void);
    bool sendPayload(string payload);
    bool disconnect();
    string getLastResponse();
    void shutdown(void);
private:
    string waitForResponse(float time);
    string sendGSM(string cmd,float wait_time=0.5);
    void callback_rx();
};


#endif // __AM2321_H__


