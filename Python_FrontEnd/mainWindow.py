from PyQt5 import QtWidgets as Qtw
from PyQt5.QtGui import QColor
from PyQt5 import QtCore as Qt
from parseSerial import Parser
from parseSerial import MessageID


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
    sendMessage=Qt.pyqtSignal(MessageID,int,int,int,int)
    def __init__(self):
        super().__init__()
        self.mainLayout=Qtw.QVBoxLayout(self)
        self.setLayout(self.mainLayout)
        self.commWidget=Comm()
        self.mainLayout.addWidget(self.commWidget)
        self.debugWidget=DebugMessage()
        self.mainLayout.addWidget(self.debugWidget)

        self.parserThread=Parser(self)
        self.parserThread.newData.connect(self.commWidget.update)
        self.parserThread.newDebug.connect(self.debugWidget.update)
        
        self.parserThread.start()
