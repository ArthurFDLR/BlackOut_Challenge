from PyQt5 import QtWidgets as Qtw
from PyQt5.QtGui import QColor, QPixmap, QPainter, QPen, QBrush
from PyQt5 import QtCore as Qt
from parseSerial import Parser
from positionComputation import PositionComputation, PosEnum

import Goose_GUI as GUI

class Comm(Qtw.QFrame):
    def __init__(self):
        super().__init__()
        self.setFrameShadow(Qtw.QFrame.Plain)
        self.setFrameShape(Qtw.QFrame.StyledPanel)
        self.myLayout=Qtw.QHBoxLayout(self)

        self.lab=Qtw.QLabel("Last byte received : ")
        self.lab.setAlignment(Qt.Qt.AlignCenter)
        self.myLayout.addWidget(self.lab)

        self.mess=Qtw.QLabel("")
        self.myLayout.addWidget(self.mess)

    def update(self, x : dict):
        for name,value in x.items():
            #print(name + " : " + str(value))
            self.mess.setText(name + " : " + str(value))

class initialisation_settings(Qtw.QFrame):
    newPositionCalibration = Qt.pyqtSignal(dict)
    newCalibrationState = Qt.pyqtSignal(bool)

    def __init__(self, mainWindow):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setFrameShadow(Qtw.QFrame.Plain)
        self.setFrameShape(Qtw.QFrame.StyledPanel)
        self.myLayout = Qtw.QGridLayout(self)

        self.label = Qtw.QLabel("Settings")
        self.label.setAlignment(Qt.Qt.AlignCenter)
        self.myLayout.addWidget(self.label, 2, 1, 1, 2)

        #  Parametre x

        self.slider_x = Qtw.QSlider(Qt.Qt.Horizontal)
        self.myLayout.addWidget(self.slider_x, 3, 2)
        self.slider_x.setMinimum(-1500)
        self.slider_x.setMaximum(1500)
        self.slider_x.setValue(0)
        self.label_x = Qtw.QLabel("X")
        self.label_x.setAlignment(Qt.Qt.AlignCenter)
        self.myLayout.addWidget(self.label_x, 3, 1)

        #  Parametre y

        self.slider_y = Qtw.QSlider(Qt.Qt.Horizontal)
        self.myLayout.addWidget(self.slider_y, 4, 2)
        self.slider_y.setMinimum(-1800)
        self.slider_y.setMaximum(1800)
        self.slider_y.setValue(0)
        self.label_y = Qtw.QLabel("Y")
        self.label_y.setAlignment(Qt.Qt.AlignCenter)
        self.myLayout.addWidget(self.label_y, 4, 1)

        # Parametre theta

        self.dial_theta = Qtw.QDial()
        self.myLayout.addWidget(self.dial_theta, 5, 2)
        self.dial_theta.setMinimum(-180)
        self.dial_theta.setMaximum(180)
        self.dial_theta.setValue(0)
        self.label_theta = Qtw.QLabel("Theta")
        self.label_theta.setAlignment(Qt.Qt.AlignCenter)
        self.myLayout.addWidget(self.label_theta, 5, 1)

        # Check phase d'initialisation

        self.label_init = Qtw.QLabel("Initialisation")
        self.label_init.setAlignment(Qt.Qt.AlignCenter)
        self.myLayout.addWidget(self.label_init, 1, 1)
        self.check_init = MySwitch()
        self.myLayout.addWidget(self.check_init, 1, 2)

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
            self.newCalibrationState.emit(True)
        else:
            self.newCalibrationState.emit(False)
    
    def sendNewPosition(self):
        self.currentPosition[PosEnum.POS_X] = self.slider_x.value()
        self.currentPosition[PosEnum.POS_Y] = self.slider_y.value()
        self.currentPosition[PosEnum.POS_THETA] = self.dial_theta.value()
        self.newPositionCalibration.emit(self.currentPosition)


class MySwitch(Qtw.QPushButton):
    def __init__(self, parent = None):
        super().__init__(parent)
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
        painter.setBrush(QColor(170,170,170))

        pen = QPen(Qt.Qt.white)
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

        self.beaconCounter = 0
        self.beacon = False
        self.initUI()
    
    def updateBaliseMode(self):
        self.beacon = True
        self.beaconCounter = 0

    def updatePosition(self, pos : dict):
        self.posX = pos[PosEnum.POS_X]
        self.posY = pos[PosEnum.POS_Y]
        self.theta = pos[PosEnum.POS_THETA]

        if ((self.beaconCounter < 5) and self.beacon):
            self.beaconCounter += 1
        elif (self.beacon) :
            self.beacon = False

        self.debugLabel.setText( "X : " + str(self.posX) + " ; " + "Y : " + str(self.posY) + " ; " + "Theta : " + str(self.theta))

        GUI.create_frame_map(self.posX, self.posY, self.theta, self.beacon)
        pixmap = QPixmap("map_created.jpg")
        self.label_image.setPixmap(pixmap)

    def initUI(self):
        self.setFrameShadow(Qtw.QFrame.Plain)
        self.setFrameShape(Qtw.QFrame.StyledPanel)
        self.myLayout = Qtw.QGridLayout(self)
        
        self.debugLabel=Qtw.QLabel("")
        self.debugLabel.setAlignment(Qt.Qt.AlignCenter)
        self.myLayout.addWidget(self.debugLabel, 1, 1)

        self.label_image = Qtw.QLabel()
        self.myLayout.addWidget(self.label_image, 2, 1)
        self.beacon = GUI.beacon()

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

        self.setStyleSheet(open("./design_GUI.css").read())

        self.mainLayout=Qtw.QGridLayout(self)
        self.setLayout(self.mainLayout)

        self.mainLayout.setSpacing(0)
        self.mainLayout.setContentsMargins(0, 0, 0, 0)

        self.posCompution = PositionComputation(self)

        ## WIDGETS ##
        self.lab_im = Qtw.QLabel()
        pixmp = QPixmap("Logo_Goose_Nav.png")
        self.pixmp = pixmp.scaled(self.width()//2,self.height()//2,Qt.Qt.KeepAspectRatio)
        self.lab_im.setPixmap(self.pixmp)
        self.lab_im.resize(self.width()//2,self.height()//2)
        self.lab_im.setAlignment(Qt.Qt.AlignCenter)
        self.mainLayout.addWidget(self.lab_im, 1, 1)

        self.mapWidget = map_GUI()
        self.mainLayout.addWidget(self.mapWidget, 1, 2, 5, 1)

        self.commWidget=Comm()
        self.mainLayout.addWidget(self.commWidget, 2, 1)

        self.debugWidget=DebugMessage()
        self.mainLayout.addWidget(self.debugWidget, 3, 1)

        self.settings=initialisation_settings(self)
        self.mainLayout.addWidget(self.settings, 4, 1)

        self.sendButton = Qtw.QPushButton("Test connection")
        self.mainLayout.addWidget(self.sendButton, 5, 1)
        self.sendButton.clicked.connect(lambda : self.sendMessage.emit("!"))

        self.baliseButton = Qtw.QPushButton("Balise")
        self.mainLayout.addWidget(self.baliseButton, 0, 3)
        self.baliseButton.clicked.connect(self.mapWidget.updateBaliseMode)
        self.baliseButton.clicked.connect(self.posCompution.posBaliseReception)


        ## COMMUNICATION ##

        self.posCompution.newPosition.connect(self.mapWidget.updatePosition)

        self.settings.newPositionCalibration.connect(self.posCompution.posCalibrationReception)
        self.settings.newCalibrationState.connect(lambda x: self.sendMessage.emit("y") if (x) else self.sendMessage.emit("n"))  # Send 'y' if check box true, else send 'n'

        if (True): #True if Com port connexion established
            self.parserThread=Parser(self)

            self.sendMessage.connect(self.parserThread.sendMessage) # When MainWindow emit message, the parser catch them and send on serial port
            self.parserThread.newData.connect(self.commWidget.update) # When parser emit newData    ## TMP
            self.parserThread.newData.connect(self.posCompution.dataReception) # When parser emit newData, posComputation catch en translate to newPos

            self.parserThread.newDebug.connect(self.debugWidget.update) # Print to screen when debug message comming in
            self.parserThread.newDebug.connect(print) # Print to console when debug message comming in

            self.parserThread.start()
