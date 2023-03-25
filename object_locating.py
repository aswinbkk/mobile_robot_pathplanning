import cv2
import numpy as np

# get user input for number of rows and columns in the grid
num_rows = 6
num_cols = 6

# initialize video capture object
cap = cv2.VideoCapture(0)

# loop through video frames
while True:
    # read frame from video capture object
    ret, frame = cap.read()

    # check if frame was successfully read
    if not ret:
        break

    # convert frame to HSV color space
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # define range of red color in HSV
    lower_red = np.array([0, 100, 100])
    upper_red = np.array([10, 255, 255])
    mask1 = cv2.inRange(hsv, lower_red, upper_red)

    lower_red = np.array([160, 100, 100])
    upper_red = np.array([179, 255, 255])
    mask2 = cv2.inRange(hsv, lower_red, upper_red)

    # combine masks
    mask = mask1 + mask2

    # apply Gaussian blur to mask
    mask = cv2.GaussianBlur(mask, (5, 5), 0)

    # find contours in mask
    contours, hierarchy = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # loop through contours
    for cnt in contours:
        # find the center of the contour
        M = cv2.moments(cnt)
        if M["m00"] != 0:
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])

            # find the row and column of the center of the contour
            row = int(cY / (frame.shape[0] / num_rows))+1
            col = int(cX / (frame.shape[1] / num_cols))+1

            # draw the contour and label on the frame
            cv2.drawContours(frame, [cnt], -1, (0, 255, 0), 2)
            cv2.putText(frame, "Red object", (cX, cY), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)

            # print the row and column of the center of the contour
            print(f"Red object found at row {row}, column {col}")
            # cap.release()
            # cv2.destroyAllWindows()

    # calculate width and height of grid cells
    cell_width = frame.shape[1] // num_cols
    cell_height = frame.shape[0] // num_rows

    # draw grid on frame
    for row in range(num_rows):
        for col in range(num_cols):
            x1 = col * cell_width
            y1 = row * cell_height
            x2 = (col + 1) * cell_width
            y2 = (row + 1) * cell_height
            cv2.rectangle(frame, (x1, y1), (x2, y2), (255, 0, 0), 1)

    # display frame
    cv2.imshow("frame", frame)

    # wait for user input to exit
    if cv2.waitKey(1) == 13:
        break

# release video capture object and close all windows
cap.release()
cv2.destroyAllWindows()
