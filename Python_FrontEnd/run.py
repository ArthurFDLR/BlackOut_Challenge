from mainWindow import MainWindow
from PyQt5 import QtCore,QtWidgets
import sys
import cv2

image = cv2.imread("map_chassou_ENSMA.jpg")
cv2.imwrite('map.jpg', image)
app = QtWidgets.QApplication(sys.argv)
mainWin = QtWidgets.QMainWindow()
mainWid = MainWindow()
mainWin.setCentralWidget(mainWid)
mainWin.show()
sys.exit(app.exec_())