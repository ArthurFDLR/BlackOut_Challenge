from PyQt5 import QtWidgets as Qtw
from PyQt5.QtGui import QColor, QPixmap, QPainter, QPen, QBrush
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

class initialisation_settings(Qtw.QFrame):
    newPositionCalibration = Qt.pyqtSignal(dict)

    def __init__(self, mainWindow):
        super().__init__()
        self.parent_window = mainWindow
        self.initUI()

    def initUI(self):
        self.setFrameShadow(Qtw.QFrame.Plain)
        self.setFrameShape(Qtw.QFrame.StyledPanel)
        self.myLayout = Qtw.QGridLayout(self)
        self.label = Qtw.QLabel("Settings (X,Y,Theta) :")
        self.myLayout.addWidget(self.label, 2, 1)
        self.slider_x = Qtw.QSlider(Qt.Qt.Horizontal)
        self.myLayout.addWidget(self.slider_x, 2, 2)
        self.slider_x.setMinimum(0)
        self.slider_x.setMaximum(3833)
        self.slider_x.setValue(3833//2)
        self.slider_y = Qtw.QSlider(Qt.Qt.Horizontal)
        self.myLayout.addWidget(self.slider_y, 2, 3)
        self.slider_y.setMinimum(0)
        self.slider_y.setMaximum(3277)
        self.slider_y.setValue(3277//2)
        self.dial_theta = Qtw.QDial()
        self.myLayout.addWidget(self.dial_theta, 2, 4)
        self.dial_theta.setMinimum(-180)
        self.dial_theta.setMaximum(180)
        self.dial_theta.setValue(0)
        self.label_init = Qtw.QLabel("Initialisation")
        self.myLayout.addWidget(self.label_init, 1, 1)
        self.check_init = MySwitch()
        self.myLayout.addWidget(self.check_init, 1, 3)

        self.currentPosition = {
            PosEnum.POS_X : 0.0,
            PosEnum.POS_Y : 0.0,
            PosEnum.POS_THETA : 0.0
        }

        self.slider_x.valueChanged.connect(self.sendNewPosition)
        self.slider_y.valueChanged.connect(self.sendNewPosition)
        self.dial_theta.valueChanged.connect(self.sendNewPosition)

        self.check_init.clicked.connect(self.statebutton)

    def statebutton(self):
        if self.check_init.isChecked():
            self.check_init.clicked.connect(lambda : self.parent_window.sendMessage.emit("y"))
        else:
            self.check_init.clicked.connect(lambda : self.parent_window.sendMessage.emit("n"))
    
    def sendNewPosition(self):
        self.currentPosition[PosEnum.POS_X] = self.slider_x.Value()
        self.currentPosition[PosEnum.POS_Y] = self.slider_y.Value()
        self.currentPosition[PosEnum.POS_THETA] = self.dial_theta.Value()

        self.newPositionCalibration.emit(self.currentPosition)


class MySwitch(Qtw.QPushButton):
    def __init__(self, parent = None):
        super().__init__(parent)
        print('init')
        self.setCheckable(True)
        self.setMinimumWidth(66)
        self.setMinimumHeight(22)

    def paintEvent(self, event):
        label = "ON" if self.isChecked() else "OFF"
        bg_color = Qt.Qt.green if self.isChecked() else Qt.Qt.red

        radius = 10
        width = 32
        center = self.rect().center()

        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        painter.translate(center)
        painter.setBrush(QColor(0,0,0))

        pen = QPen(Qt.Qt.black)
        pen.setWidth(2)
        painter.setPen(pen)

        painter.drawRoundedRect(Qt.QRect(-width, -radius, 2*width, 2*radius), radius, radius)
        painter.setBrush(QBrush(bg_color))
        sw_rect = Qt.QRect(-radius, -radius, width + radius, 2*radius)
        if not self.isChecked():
            sw_rect.moveLeft(-width)
        painter.drawRoundedRect(sw_rect, radius, radius)
        painter.drawText(sw_rect, Qt.Qt.AlignCenter, label)

class map_GUI(Qtw.QFrame):
    def __init__(self):
        super().__init__()
        self.posX = 0.0
        self.posY = 0.0
        self.theta = 0.0

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
    
    sendMessage = Qt.pyqtSignal(str)
    def __init__(self):
        ## SETUP ##
        super().__init__()
        self.mainLayout=Qtw.QVBoxLayout(self)
        self.setLayout(self.mainLayout)
        self.posCompution = PositionComputation(self)
        self.parserThread=Parser(self)

        ## WIDGETS ##
        self.commWidget=Comm()
        self.mainLayout.addWidget(self.commWidget)

        self.debugWidget=DebugMessage()
        self.mainLayout.addWidget(self.debugWidget)

        self.settings=initialisation_settings(self)
        self.mainLayout.addWidget(self.settings)

        self.sendButton = Qtw.QPushButton("Click click")
        self.mainLayout.addWidget(self.sendButton)
        self.sendButton.clicked.connect(lambda : self.sendMessage.emit("!"))

        self.mapWidget = map_GUI()
        self.mainLayout.addWidget(self.mapWidget)
        
        
        ## COMMUNICATION ##
        self.sendMessage.connect(self.parserThread.sendMessage) # When MainWindow emit message, the parser catch them and send on serial port
        self.parserThread.newData.connect(self.commWidget.update) # When parser emit newData    ## TMP
        self.parserThread.newData.connect(self.posCompution.dataReception) # When parser emit newData, posComputation catch en translate to newPos

        self.parserThread.newDebug.connect(self.debugWidget.update) # Print to screen when debug message comming in
        self.parserThread.newDebug.connect(print) # Print to console when debug message comming in
        
        self.posCompution.newPosition.connect(self.mapWidget.updatePosition)
        
        self.settings.newPositionCalibration.connect(self.posCompution.posCalibrationReception)
        
        # self.parserThread.start()
