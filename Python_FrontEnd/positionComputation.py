from PyQt5.QtCore import QObject,pyqtSignal,QThread
from PyQt5 import QtWidgets as Qtw
import time
from enum import Enum
import math

class PosEnum(Enum):
    POS_X = 1
    POS_Y = 2
    POS_THETA = 3
    DELTA_X = 4
    DELTA_Y = 5
    DELTA_THETA = 6

class PositionComputation(Qtw.QWidget):
    newPosition = pyqtSignal(dict)
 

    def __init__(self,mainWindow):
        super().__init__()

        self.dictDeltaIn = {
            PosEnum.DELTA_X : 0.0,
            PosEnum.DELTA_Y : 0.0,
            PosEnum.DELTA_THETA : 0.0
        }

        self.dictPosOut = {
            PosEnum.POS_X : 0.0,
            PosEnum.POS_Y : 0.0,
            PosEnum.POS_THETA : 0.0
        }

    def dataReception(self, x : dict): #Begin new position calculation if dX, dY, dTh received
        if (('dX' in x) and ('dTh' in x)) : # whole new set of data incomming
            self.dictDeltaIn[PosEnum.DELTA_X] = x['dX']
            self.dictDeltaIn[PosEnum.DELTA_THETA] = x['dTh']
            print("new data")
            
            self.computePosition()

    def posCalibrationReception(self, x : dict):
        self.dictPosOut[PosEnum.POS_THETA] = x[PosEnum.POS_THETA]
        self.dictPosOut[PosEnum.POS_X] = x[PosEnum.POS_X]
        self.dictPosOut[PosEnum.POS_Y] = x[PosEnum.POS_Y]

        self.newPosition.emit(self.dictPosOut)
    
    def computePosition(self):
        
        ## POSITION COMPUTATION ##
        self.dictPosOut[PosEnum.POS_THETA] += self.dictDeltaIn[PosEnum.DELTA_THETA]
        self.dictPosOut[PosEnum.POS_X] += math.sin(math.radians(self.dictPosOut[PosEnum.POS_THETA])) * self.dictDeltaIn[PosEnum.DELTA_X]
        self.dictPosOut[PosEnum.POS_Y] += math.cos(math.radians(self.dictPosOut[PosEnum.POS_THETA])) * self.dictDeltaIn[PosEnum.DELTA_X]

        self.newPosition.emit(self.dictPosOut)
