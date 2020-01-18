import cv2
import numpy as np
import random
import time


def function():
    a = 5
    v = 10
    bc_off = True
    if int(random.uniform(0, 10)) == 5:
        bc_off = False
    return int(random.uniform(-v, a)), int(random.uniform(-v, v)), bc_off


def get_roi(img, x_center, y_center, width_screen, height_screen):
    w_s = width_screen
    h_s = height_screen
    global w_image
    global h_image
    if w_image - w_s > x_center > w_s and h_image - h_s > y_center > h_s:
        return img[x_center - w_s // 2: x_center + w_s // 2, y_center - h_s // 2: y_center + h_s // 2], 1
    else:
        return 0, 0


if __name__ == "__main__":
    X0, Y0 = 1000, 1000
    X, Y = X0, Y0
    while True:
        scale = 0.51
        w_screen = 620
        h_screen = 480
        w_g = 100
        goose_pos = 0.5
        delta_X, delta_Y, beacon_off = function()
        X += int(delta_X * scale)
        Y += int(delta_Y * scale)
        image = cv2.imread("map_chassou_ENSMA.jpg")
        if beacon_off:
            goose = cv2.imread("goose_beacon_off.png")
        else:
            goose = cv2.imread("goose_beacon_on.png")
        goose = cv2.resize(goose, (w_g, w_g))
        h_image, w_image, _ = image.shape
        screen_center = X - int(goose_pos * h_screen)

        roi, success = get_roi(image, screen_center, Y, w_screen, h_screen)

        if success == 0:
            print("Mec t'es fou t'as atteint les limites là !!!")
        else:
            position_goose_X = h_screen // 2 + int(goose_pos * h_screen)
            roi_goose = roi[position_goose_X - w_g // 2:position_goose_X + w_g // 2,
                        h_screen // 2 - w_g // 2:h_screen // 2 + w_g // 2]

            goose_rgb = cv2.cvtColor(goose, cv2.COLOR_BGR2RGB)
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
            roi[position_goose_X - w_g // 2:position_goose_X + w_g // 2,
                        h_screen // 2 - w_g // 2:h_screen // 2 + w_g // 2] = final

            if not beacon_off:
                cv2.circle(roi, (h_screen//2, position_goose_X), 2*w_g//3, (170, 255, 0), 3)
                cv2.circle(roi, (h_screen//2, position_goose_X), 2*w_g//3 + w_g//4, (170, 255, 0), 2)
                cv2.circle(roi, (h_screen//2, position_goose_X), 2*w_g//3 + w_g//2, (170, 255, 0), 1)

            cv2.imshow('res', roi)

        keypress = cv2.waitKey(1) & 0xFF
        if keypress == ord('q'):
            break
