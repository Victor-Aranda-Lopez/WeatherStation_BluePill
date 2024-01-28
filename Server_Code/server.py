import time
import datetime
import TCPHandler
import socket
import errno
from measureDB import Measure
READ_REAL_MEASURES = 1
payloadString = ""
hours_measure = [10,11,12,13,14,15,16,17,18,19,20,21,22,23] #UTC time
diff_minutes_with_exact_hour = 30

def calcNextMeasureSeconds(measured_hour : int):
    if measured_hour == None:
        measured_hour = -1 #accept all
    current_time = datetime.datetime.now() + datetime.timedelta(hours=1)
    for hour in hours_measure:
        possible_time = datetime.datetime.now() + datetime.timedelta(hours=1)
        possible_time = possible_time.replace(hour=hour,minute=0,second=0,microsecond=0)
        if possible_time > current_time and measured_hour < hour:
            return int(possible_time.timestamp())
    # next day
    possible_time = datetime.datetime.now() + datetime.timedelta(hours=1) + datetime.timedelta(days=1)
    possible_time = possible_time.replace(hour=hours_measure[0],minute=0,second=0,microsecond=0)
    return int(possible_time.timestamp())

def validMeasure():
    current_time = datetime.datetime.now() + datetime.timedelta(hours=1)
    for hour_to_measure in hours_measure:
        possible_time = datetime.datetime.now() + datetime.timedelta(hours=1)
        possible_time = possible_time.replace(hour=hour_to_measure,minute=0,second=0,microsecond=0)
        max_time = possible_time + datetime.timedelta(minutes=diff_minutes_with_exact_hour)
        min_time = possible_time - datetime.timedelta(minutes=diff_minutes_with_exact_hour)
        if(current_time > min_time and current_time < max_time):
            return hour_to_measure
    return None

tcpHandler = TCPHandler.TCPHandlerFactory.createConnectionHandler(READ_REAL_MEASURES)#1 for real
print("Server started")
while True:
    try:
        tcpHandler.waitForConnection()
        print("Connection accepted")
        headerString = tcpHandler.readHeader()
        # connection closed
        print("Header: {}",headerString)
        if len(headerString) == 0:
            continue
        # data not valid
        if len(headerString) != 6 or not headerString.isnumeric:
            tcpHandler.closeConnection()
            continue
        validHour = validMeasure()
        print("validHour: {}",validHour)
        if validHour != None:
            payloadLenght = int(headerString[2:6])
            payloadString = tcpHandler.readBody(payloadLenght)

            measures = payloadString.split(";")
            temperature_in = float(measures[0])/10
            print("temperature_in: {}",temperature_in)
            humidity_in = float(measures[1])/10
            print("humidity_in: {}",humidity_in)
            datetimeStamp = datetime.datetime.now() + datetime.timedelta(hours=1)
            print("datetimeStamp: {}",datetimeStamp)
            #datetimeStamp = datetimeStamp.replace(hour=validHour,minute=0,second=0,microsecond=0)
            pass
            #save measure
            Measure.create(humidity=humidity_in,temperature=temperature_in,dateAndTime=datetimeStamp)
        #respond time
        time.sleep(5)
        current_epoch_time = int((datetime.datetime.now() + datetime.timedelta(hours=1)).timestamp())
        responsePayload = "00"
        nextMeasureSeconds = calcNextMeasureSeconds(validHour)
        payload = str(current_epoch_time) + ";" +str(nextMeasureSeconds)
        payloadLenght = "{:04d}".format(len(payload))
        responsePayload += payloadLenght + payload
        tcpHandler.sendBytes(bytes(responsePayload, 'utf-8'))
        print("current_epoch_time: {}",current_epoch_time)
        print("nextMeasureSeconds: {}",nextMeasureSeconds)
        time.sleep(10)
        tcpHandler.closeConnection()
    except socket.error as e:
        if isinstance(e.args, tuple):
            print ("errno is %d" % e[0])
            if e[0] == errno.EPIPE:
               # remote peer disconnected
               print ("Detected remote disconnect")
            else:
               # determine and handle different error
               pass
        else:
            print ("socket error ", e)
        tcpHandler.closeConnection()
        break
    except IOError as e:
        # Hmmm, Can IOError actually be raised by the socket module?
        print ("Got IOError: ", e)
        break