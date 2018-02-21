import serial
import struct
import serial.tools.list_ports

outputFile = '18V.txt'
startSignal = 's'
arduinoStartSignal = 'a'
intSignal = 'i'
finishSignal = 'f'
dataList = []

def find_com_port():
    """
    Finds COM port connected to Arduino
    :return:
    """

    # Find all connected COM ports
    ports = serial.tools.list_ports.comports()

    for port in ports:
        if 'Arduino' in port.description:
            return port
    return 0

"""
Main function
"""
port = 0

# while (True):
#     port = find_com_port()
    # if port == 0:
    #     print('Cannot open COM port')
    # else:
    #     print(port)
    #     break

port = find_com_port()
ser = serial.Serial()
ser.port = 'COM4'
ser.baudrate = 9600
ser.open()

"""
Wait for start signal from arduino
"""
# while(True):
#     if ser.inWaiting():
#         input = ser.read().decode("utf-8")
#         if input == arduinoStartSignal:
#             ser.write(startSignal.encode("utf-8"))
#             break

while(True):
    test = ser.read().decode("utf-8")
    print(test)
    if test == finishSignal:
        break
    elif test == intSignal:
        bytes = ser.read(2)
        val = struct.unpack('>H', bytes)[0]
        dataList.append(val)

f = open(outputFile, 'w')

for element in dataList:
    f.write(str(element) + '\n')
f.close()

