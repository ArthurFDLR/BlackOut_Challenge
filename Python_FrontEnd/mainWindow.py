from PyQt5 import QtWidgets as Qtw
from PyQt5.QtGui import QColor, QPixmap
from PyQt5 import QtCore as Qt
from parseSerial import Parser
import Goose_GUI as GUI
from positionComputation import PositionComputation, PosEnum

class Comm(Qtw.QFrame):
    def __init__(self):
        super().__init__()
        self.setFrameShadow(Qtw.QFrame.Plain)
        self.setFrameShape(Qtw.QFrame.StyledPanel)
        self.myLayout=Qtw.QHBoxLayout(self)
        self.lab=Qtw.QLabel("Last byte received : ")
        self.myLayout.addWidget(self.lab)
        self.mess=Qtw.QLabel("")
        self.myLayout.addWidget(self.mess)

    def update(self, x : dict):
        for name,value in x.items():
            #print(name + " : " + str(value))
            self.mess.setText(name + " : " + str(value))

class initialisation_setting(Qtw.QWidget):
    def __init__(self):
        super().__init__()

    def initUI(self):
        self.setFrameShadow(Qtw.QFrame.Plain)
        self.setFrameShape(Qtw.QFrame.StyledPanel)
        self.myLayout = Qtw.QHBoxLayout(self)
        self.label = Qtw.QLabel("Settings")
        self.myLayout.addWidget(self.label)
        self.slider_x = Qtw.QSlider(Qt.Horizontal)
        self.slider_x.set
        self.slider_y = Qtw.QSlider(Qt.Horizontal)

class map_GUI(Qtw.QWidget):
    def __init__(self):
        super().__init__()
        self.beacon = False
        self.initUI()

    def updatePosition(self, pos : dict):
        self.posX = pos[PosEnum.POS_X]
        self.posY = pos[PosEnum.POS_y]
        self.theta = pos[PosEnum.POS_THETA]

    def initUI(self):
        self.setFrameShadow(Qtw.QFrame.Plain)
        self.setFrameShape(Qtw.QFrame.StyledPanel)
        self.myLayout = Qtw.QHBoxLayout(self)
        label = Qtw.QLabel(self)
        self.beacon = GUI.beacon()
        GUI.create_frame_map(self.posX, self.posY, self.theta, self.beacon)
        pixmap = QPixmap('map.jpg')
        label.setPixmap(pixmap)
        self.resize(pixmap.width(), pixmap.height())
        self.show()


class DebugMessage(Qtw.QFrame):
    def __init__(self):
        super().__init__()
        self.setFrameShadow(Qtw.QFrame.Plain)
        self.setFrameShape(Qtw.QFrame.StyledPanel)
        self.myLayout=Qtw.QHBoxLayout(self)
        self.mess=Qtw.QLabel("")
        self.myLayout.addWidget(self.mess)

    def update(self, message : str):
        self.mess.setText(message)

class MainWindow(Qtw.QWidget):
     

    def __init__(self):
        super().__init__()
        self.mainLayout=Qtw.QVBoxLayout(self)
        self.setLayout(self.mainLayout)
        self.commWidget=Comm()
        self.mainLayout.addWidget(self.commWidget)
        self.debugWidget=DebugMessage()
        self.mainLayout.addWidget(self.debugWidget)
        self.mapWidget=map_GUI()
        self.mainLayout.addWidget(self.mapWidget)

        self.sendButton = Qtw.QPushButton("Click click")
        self.mainLayout.addWidget(self.sendButton)
        self.sendButton.clicked.connect(lambda : self.sendMessage.emit("!"))

        self.posCompution = PositionComputation(self)

        self.parserThread=Parser(self)
        self.sendMessage.connect(self.parserThread.sendMessage) # When MainWindow emit message, the parser catch them and send on serial port
        self.parserThread.newData.connect(self.commWidget.update) # When parser emit newData, 
        self.parserThread.newDebug.connect(self.debugWidget.update)
        self.parserThread.newDebug.connect(print) # When parser emit message, print in console
        self.parserThread.newMovement.connect(self.mapWidget.updatePosition)

        self.parserThread.newData.connect(self.posCompution.dataReception)

        self.posCompution.newPosition.connect(self.mapWidget.updatePosition)

        
        self.parserThread.start()
