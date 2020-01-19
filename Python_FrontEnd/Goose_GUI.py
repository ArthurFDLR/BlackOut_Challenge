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
    if w_image - w_s > x_center > w_s and h_image - h_s > y_center > h_s:
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

def create_frame_map(X, Y, theta, beacon):

    # Facteur de mise à l'échelle (px/m)
    scale = 0.51
    #  Mise à l'échelle ( X_scaled en px et X en m)
    X_scaled = X * scale
    Y_scaled = Y * scale
    #  Taille de l'image dans l'écran du GPS
    w_screen = 640
    h_screen = 480
    #  Taille de l'image (carré) de la goose
    w_g = 100
    #  Position par rapport à l'axe Y de la goose
    goose_pos = 0.5
    #  Lecture de la carte d'arrière-plan (Ici carte allant du milieu de Jaunay-Clan
    #  au milieu de la zone commerciale et de l'autoroute à chasseneuil)
    image = cv2.imread("map_chassou_ENSMA.jpg")
    #  Récupération des tailles (résolution) de l'image
    h_image, w_image, _ = image.shape
    #  Test du mode (Mode balise reconnue ou Mode Sans balise)
    mat_rot = cv2.getRotationMatrix2D((h_image//2,w_image//2),theta,(h_image,w_image))
    image_rot = cv2.warpAffine(image,mat_rot)

    if beacon:
        goose = cv2.imread("goose_beacon_on.png")  #  Balise reconnue
    else:
        goose = cv2.imread("goose_beacon_off.png") #  Pas de balise
    #  Redimensionner la goose à la taille voulue
    goose = cv2.resize(goose, (w_g, w_g))
    #  Centre de l'écran selon l'axe y (différent de la position (X,Y)
    screen_center = Y_scaled - int(goose_pos * h_screen)
    #  Récupération de l'image de l'écran (Region Of Interest)
    roi, success = get_roi(image_rot, X_scaled, screen_center, w_screen, h_screen)

    # Vérification qu'on ne soit pas au bord de la carte (Goose ne peut pas aller au-delà du monde connu)
    if success == 0:
        print("Limite de la carte atteinte")  #  Limite atteinte
    else:  #  Limite non atteinte
        #  Redéfinition de la position de la goose selon l'axe y sur l'écran
        pos_goose_Y = h_screen // 2 + int(goose_pos * h_screen)
        #  Récupération de la ROI pour placer la goose
        roi_goose = roi[pos_goose_Y - w_g // 2:pos_goose_Y + w_g // 2,
                    h_screen // 2 - w_g // 2:h_screen // 2 + w_g // 2]

        #  Traitement d'image (But : Intégrer logo à fond transparent sur la carte)
        # Passage en RGB
        goose_rgb = cv2.cvtColor(goose, cv2.COLOR_BGR2RGB)
        # Définition limite couleur image
        lower_green = np.array([49, 13, 245], dtype=np.uint8)
        upper_green = np.array([49, 13, 255], dtype=np.uint8)
        mask = cv2.inRange(goose_rgb, lower_green, upper_green)
        cv2.imshow('res1', mask)
        mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3, 3)))
        mask = cv2.bitwise_not(mask)
        fg_masked = cv2.bitwise_and(goose_rgb, goose_rgb, mask=mask)
        mask = cv2.bitwise_not(mask)
        bk_masked = cv2.bitwise_and(roi_goose, roi_goose, mask=mask)
        final = cv2.bitwise_or(fg_masked, bk_masked)
        roi[pos_goose_Y - w_g // 2:pos_goose_Y + w_g // 2,
                    h_screen // 2 - w_g // 2:h_screen // 2 + w_g // 2] = final

        # Rond vert pour insister sur la présence d'une balise
        if not beacon:
            cv2.circle(roi, (h_screen//2, pos_goose_Y), 2*w_g//3, (170, 255, 0), 3)
            cv2.circle(roi, (h_screen//2, pos_goose_Y), 2*w_g//3 + w_g//4, (170, 255, 0), 2)
            cv2.circle(roi, (h_screen//2, pos_goose_Y), 2*w_g//3 + w_g//2, (170, 255, 0), 1)

        cv2.imwrite('map.jpg', roi)