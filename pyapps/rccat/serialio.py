# Handle Rcat serial io
import serial
import requests
import io
import threading
import time
import pandas as pd 
import math
import urllib.parse


class SerialIO:

    def __init__(self):
        self.ser = None
        self.dataBuffer = []
        self.thread = None
        self.active = False
        self.lastcall = pd.Timestamp.now()



    @property
    def readData(self):
        return pd.DataFrame(self.dataBuffer, columns=['time',
                                                        'steer_rec', 'acc_rec',
                                                        'ax', 'ay', 'az', 'a_tot',
                                                        'pitch', 'roll',
                                                        'dist',
                                                        'speed',
                                                        'steer_out',
                                                        'acc_out',
                                                        'state'
                                                        ])

    def connect(self, deviceName="/dev/ttyUSB0"):

        if deviceName.startswith("/dev"):
            try:
                self.ser = serial.Serial(
                    deviceName, 115200, timeout=None, xonxoff=False, rtscts=False, dsrdtr=False)
                self.thread = threading.Thread(target=self.readSerial)
                self.thread.daemon = True
                self.active = True
                self.thread.start()

                return ""
            except:
                self.active = False
                return "No connection"
        if deviceName.startswith("http://"):
            try:
                self.getUrl = urllib.parse.urljoin(deviceName, "/get")
                self.writeUrl = urllib.parse.urljoin(deviceName, "/write")
                self.thread = threading.Thread(target=self.readHTTP)
                self.thread.daemon = True
                self.active = True
                self.thread.start()

                return ""
            except:
                self.active = False
                return "No connection"


    def disconnect(self):
        self.active = False

        try:
            if self.ser:
                self.ser.close()
        except:
            print("Serial close problem")

    def importData(self, binaryData):
        self.active  = True

        for l in  binaryData.split(b"\r\n"):
            el = l.split(b"\t")
            try:
                if len(el) == 13:
                    dataLines = [int(x) for x in el]
                    self.dataBuffer.append([0, *dataLines])
            except:
                pass

    def getDataString(self):
        """Create a tab separated mas data array
        """
        return "\r\n".join([ "\t".join([str(x)for x in l[1:]]) for l in self.dataBuffer])

    def clearBuffer(self):
        self.dataBuffer = []


    def readSerial(self):

        while self.active:
            try:
                l = self.ser.readline().decode("utf-8")

                strings = l.replace("\r\n", "").split("\t")
                vals = [int(s) for s in strings]
                if len(vals)  == 13:
                    self.dataBuffer.append([pd.Timestamp.now(), *vals])
            except:
                pass


    def readHTTP(self):
        while self.active:
            try:
                #collect data from url and parse it
                r = requests.get(self.getUrl)
                self.importData(r.content)
                #the buffer is (32kB) is sufficient for a few seconds
                time.sleep(1.0)
            except:
                pass


    def status(self):
        return self.active

    def write(self, char):
        if 'writeUrl' in self.__dict__:
            requests.post(self.writeUrl, char)
        if 'ser' in self.__dict__ and self.ser is not None:
            self.ser.write(char)


    def driveCommand(self, steer, accel):

        steer_byte = int(max(-125, min(steer, 125)) + 128)
        accel_byte = int(max(-125, min(accel, 125)) + 128)
        
        commands = b's' + bytes([steer_byte, accel_byte])
        self.write(commands)


    def stopDrive(self):
        self.ser.write(b'x')