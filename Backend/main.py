import serial

CONFIG_SERIAL_PORT = "COM3"

mainSerial = serial.Serial(CONFIG_SERIAL_PORT)
mainSerial.timeout = None
print(mainSerial.name + ' Opened!')

while(True):
    btInput = mainSerial.readline()
    btString = btInput.decode("ASCII")
    btString = btString.replace('\r\n', '')
    print(btString)
mainSerial.close()