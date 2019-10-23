import serial
import requests
import datetime
import math

CONFIG_SERIAL_PORT = "COM3"
lastestRuntimeMillis = 0
lastestRuntimeReal = 0
mainSerial = serial.Serial(CONFIG_SERIAL_PORT)
mainSerial.timeout = None
print(mainSerial.name + ' Opened!')

def HTTPGetRequest(params):
    requests.get("http://localhost/insert.php", params=params)

def RuntimeToRealtime(runtime):
    global lastestRuntimeMillis, lastestRuntimeReal
    if lastestRuntimeMillis == 0:
        lastestRuntimeMillis = int(runtime)
        lastestRuntimeReal = datetime.datetime.now()
        return lastestRuntimeReal.isoformat()
    else:
        lastestRuntimeMillis = int(runtime)
        lastestRuntimeReal += datetime.timedelta(0, math.floor((int(runtime) - lastestRuntimeMillis) / 1000))
        return lastestRuntimeReal.isoformat()

while(True):
    btInput = mainSerial.readline()
    btString = btInput.decode("ASCII")
    btString = btString.replace('\r\n', '')
    pbtString = btString.split(',')
    HTTPGetRequest({"KIND": "LOG", "DATA": btString, "TIME": datetime.datetime.now().isoformat()})
    if pbtString[0] == "STATIC":
        HTTPGetRequest({"KIND": "SENSOR", "DATA": pbtString[1] + "," + pbtString[2] + "," + pbtString[3] + "," + pbtString[4], "TIME": RuntimeToRealtime(pbtString[5])})
    elif pbtString[0] == "EVENT":
        HTTPGetRequest({"KIND": "SENSOR", "DATA": pbtString[2] + "," + pbtString[3], "TIME": RuntimeToRealtime(pbtString[1])})
mainSerial.close()