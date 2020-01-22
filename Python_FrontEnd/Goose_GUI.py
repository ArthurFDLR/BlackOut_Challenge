import cv2
import numpy as np
import random

#  Déclencher l'apparition d'un beacon avec 1 chance sur 10
def beacon():
    if int(random.uniform(0, 10)) == 5:
        return True

#  Renvoie la ROI si celle-ci existe (Limite de l'image principale non atteinte)
def get_roi(img, x_center, y_center, width_screen, height_screen):
    w_s = width_screen
    h_s = height_screen
    global w_image
    global h_image
    if w_image - w_s//2 > x_center > w_s//2 and h_image - h_s//2 > y_center > h_s//2:
        return img[y_center - h_s // 2: y_center + h_s // 2, x_center - w_s // 2: x_center + w_s // 2], 1
    else:
        return 0, 0

########################################################
################## Create_frame_map ####################
########################################################
# E : X : float (coordonnée selon l'axe x de la voiture)
#     Y : float (coordonnée selon l'axe x de la voiture)
#     beacon : boolean (True : balise reconnue, False : pas de balise)
# S : map : array (image finale à projeter)
# Necessite :
########################################################

def create_frame_map(delta_X, delta_Y, theta, beacon):

    # Facteur de mise à l'échelle (px/m)
    scale = 0.51
    #  Mise à l'échelle ( X_scaled en px et X en m)
    delta_X_scaled = int(delta_X * scale)
    delta_Y_scaled = int(delta_Y * scale)
    #  Taille de l'image dans l'écran du GPS
    w_screen = 360
    h_screen = 240
    #  Taille de l'image (carré) de la goose
    w_g = 50
    #  Position par rapport à l'axe Y de la goose
    goose_pos = 0.3
    #  Recuperation de la carte translatee precedente
    image = cv2.imread("map.jpg")
    #  Recuperation des tailles (resolution) de l'image
    global h_image
    global w_image
    h_image, w_image, _ = image.shape

    #  Translation
    mat_trans = np.float32([[1,0,delta_X_scaled],[0,1,delta_Y_scaled]])
    image_trans = cv2.warpAffine(image,mat_trans,(h_image,w_image))

    #  Rotation
    mat_rot = cv2.getRotationMatrix2D((h_image//2,w_image//2),theta,1) #(h_image,w_image)
    image_rot = cv2.warpAffine(image_trans,mat_rot, (h_image,w_image))

    #  Selection de la ROI
    image_zoom = image_rot[(h_image-h_screen)//2:(h_image+h_screen)//2,(w_image-w_screen)//2:(w_image+w_screen)//2]

    #  Test du mode (Mode balise reconnue ou Mode Sans balise)
    if beacon:
        goose = cv2.imread("goose_beacon_on.png")  #  Balise reconnue
    else:
        goose = cv2.imread("goose_beacon_off.png") #  Pas de balise
    #  Redimensionner la goose à la taille voulue
    goose = cv2.resize(goose, (w_g, w_g))

    #  Redefinition de la position de la goose selon l'axe y sur l'écran
    pos_goose_Y = h_screen // 2 + int(goose_pos * h_screen)
    #  Recuperation de la ROI pour placer la goose
    roi_goose = image_zoom[pos_goose_Y - w_g // 2:pos_goose_Y + w_g // 2,
                    w_screen // 2 - w_g // 2:w_screen // 2 + w_g // 2]

    #  Traitement d'image (But : Intégrer logo à fond transparent sur la carte)
    # Passage en RGB
    goose_rgb = cv2.cvtColor(goose, cv2.COLOR_BGR2RGB)
    # Définition limite couleur image
    lower_green = np.array([49, 13, 245], dtype=np.uint8)
    upper_green = np.array([49, 13, 255], dtype=np.uint8)
    mask = cv2.inRange(goose_rgb, lower_green, upper_green)
    mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3, 3)))
    mask = cv2.bitwise_not(mask)
    fg_masked = cv2.bitwise_and(goose_rgb, goose_rgb, mask=mask)
    mask = cv2.bitwise_not(mask)
    bk_masked = cv2.bitwise_and(roi_goose, roi_goose, mask=mask)
    final = cv2.bitwise_or(fg_masked, bk_masked)
    image_zoom[pos_goose_Y - w_g // 2:pos_goose_Y + w_g // 2,
                w_screen // 2 - w_g // 2:w_screen // 2 + w_g // 2] = final

    #  Rond vert pour insister sur la presence d'une balise
    if not beacon:
        cv2.circle(image_zoom, (pos_goose_Y, w_screen//2), 2*w_g//3, (170, 255, 0), 3)
        cv2.circle(image_zoom, (pos_goose_Y, w_screen//2), 2*w_g//3 + w_g//6, (170, 255, 0), 2)
        cv2.circle(image_zoom, (pos_goose_Y, w_screen//2), 2*w_g//3 + w_g//4, (170, 255, 0), 1)

    #  Agrandissement de la carte
    image_res = cv2.resize(image_zoom, None, None, 1.5, 1.5)

    #  Ecriture de la carte translatee pour recuperation future
    cv2.imwrite('map.jpg', image_trans)

    #  Ecriture de la carte traitee a recuperer
    cv2.imwrite('map_created.jpg', image_res)


#################################
#  Fonction de test du module
#################################

'''
    cv2.imshow('res', image_res)

delta_X = 3
delta_Y = 2
theta = 0.9
image = cv2.imread("map_chassou_ENSMA.jpg")
cv2.imwrite('map.jpg', image)
while True :
    create_frame_map(delta_X,delta_Y,theta,beacon)

    theta += 1

    keypress = cv2.waitKey(1) & 0xFF
    if keypress == ord('q'):
        break
'''