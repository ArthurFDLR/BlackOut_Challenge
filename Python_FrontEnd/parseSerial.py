import serial
from PyQt5.QtCore import QObject,pyqtSignal,QThread
import time
from struct import unpack
from binascii import unhexlify

from ctypes import *

class Parser(QThread):
    newDebug = pyqtSignal(str)
    newData = pyqtSignal(dict)
    #newMovement = pyqtSignal(dict)  #  {"DeltaX" : deltaX, "DeltaY" : deltaY}

    def __init__(self,mainWindow):
        super().__init__()

        #Read port configuration 
        file = open("setup.txt","r")
        port = file.readline()[:-1]
        baudrate = int(file.readline())
        file.close()

        #init serial communication
        self.ser=serial.Serial()
        self.ser.port=port
        self.ser.baudrate=baudrate
        
        self.stop=False
    
    def decode_float(self,s):   #return unpack('<f', unhexlify(s))[0]
        s = s[6:8] + s[4:6] + s[2:4] + s[0:2] # reverse the byte order
        i = int(s, 16)                   # convert from hex to a Python int
        cp = pointer(c_int(i))           # make this into a c integer
        fp = cast(cp, POINTER(c_float))  # cast the int pointer to a float pointer
        return fp.contents.value         # dereference the pointer, get the float
        

    def sendMessage(self, char : str):
        self.ser.write(char.encode('utf-8'))
        print("send")
    
    def readNamedValue(self):
        if (self.ser.read()).decode() == '@':
            floatStr = ""
            nameStr = ""
            nameChar = (self.ser.read()).decode()
            while (nameChar != '#'):
                nameStr += nameChar
                nameChar = (self.ser.read()).decode()
            for i in range(8): # read incomming float
                floatStr += (self.ser.read()).decode()
        return nameStr, self.decode_float(floatStr)

    def run(self):              # type data : '*i@name1#value1@name2#value2...'
        self.connectPort()
        while not self.stop:
            if self.ser.inWaiting()>0:
                x = (self.ser.read()).decode()
                if x == '*': # New values incomming
                    namedValues = {}
                    for i in range(int((self.ser.read()).decode())): #Get number of values comming
                        name, value = self.readNamedValue()
                        namedValues[name] = value
                    self.newData.emit(namedValues)
                
                if x == '|':    # New message
                    message = ""
                    messageChar = (self.ser.read()).decode()
                    while messageChar != '\n':
                        message += messageChar
                        messageChar = (self.ser.read()).decode()
                    self.newDebug.emit(message)

    
    def stop(self):
        stop=True
        self.ser.close()

    def connectPort(self):
        try:
            pass
            self.ser.open()
        except:
            print("Wrong COM")
            exit()