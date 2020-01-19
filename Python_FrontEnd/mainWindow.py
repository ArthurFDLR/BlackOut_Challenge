from PyQt5 import QtWidgets as Qtw
from PyQt5.QtGui import QColor, QPixmap
from PyQt5 import QtCore as Qt
from parseSerial import Parser
from parseSerial import MessageID
import Goose_GUI as GUI


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
            print(name + " : " + str(value))
            self.mess.setText(name + " : " + str(value))

class map_GUI(Qtw.QWidget):
    def __init__(self):
        super().__init__()
        self.delta_X = 0
        self.delta_Y = 0
        self.beacon = False

        self.initUI()

    def initUI(self):
        self.setWindowTitle("GPS Mais sans S")
        self.setGeometry(0, 0, 480, 640)
        label = Qtw.QLabel(self)
        self.beacon = GUI.beacon()
        GUI.create_frame_map(self.delta_X, self.delta_Y, self.beacon)
        pixmap = QPixmap('map.jpg')
        label.setPixmap(pixmap)
        self.resize(pixmap.width(), pixmap.height())
        self.show()

    def update(self, x: dict):
        for name, value in x.items():
            self.delta_X = x["Delta_X"]
            self.delta_Y = x["Delta_Y"]


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
    sendMessage=Qt.pyqtSignal(str)
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

        self.parserThread=Parser(self)
        self.parserThread.newData.connect(self.commWidget.update)
        self.parserThread.newDebug.connect(self.debugWidget.update)
        self.parserThread.newDebug.connect(print)
        self.parserThread.newMovement.connect(self.mapWidget.update)

        
        self.parserThread.start()
