# Handle Rcat serial io
import serial
import io
import threading
import time
import pandas as pd 


class SerialIO:

    def __init__(self):
        self.ser = None
        self.dataBuffer = []
        self.thread = None
        self.active = False
        self.lastcall = pd.Timestamp.now()

        self.commands = [b'q', b'w', b'e', b'a', b's', b'd', b'x' ]

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

    def disconnect(self):
        self.active = False

        try:
            if self.ser:
                self.ser.close()
        except:
            print("Serial close problem")

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

    def status(self):
        return self.active

    def write(self, char):
        #if self.active:
        print(char)
        self.ser.write(char)