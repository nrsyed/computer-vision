import argparse
from collections import OrderedDict
import numpy as np
import cv2

class ColorThreshold:
    CV_COLOR_CODES = OrderedDict((
        ("BGR", None),
        ("GRAY", cv2.COLOR_BGR2GRAY),
        ("HSV", cv2.COLOR_BGR2HSV),
        ("Lab", cv2.COLOR_BGR2Lab),
        ("Luv", cv2.COLOR_BGR2Luv),
        ("YCrCb", cv2.COLOR_BGR2YCrCb),
        ("YUV", cv2.COLOR_BGR2YUV)
        ))

    def __init__(self, mode="cam", source=0):
        self.PIXEL_MIN = 0
        self.PIXEL_MAX = 255
        self.BTN_HUE = 127

        self.CTRL_WIN = "Controls"
        self.THRESH_WIN = "Thresholded"
        self.IM_WIN = "Original"


        self.COLOR_SPACES = [key for key, code in
            ColorThreshold.CV_COLOR_CODES.items()]

        self.colorSpaceIdx = 0

        self.ch0LowVal = self.PIXEL_MIN
        self.ch0HighVal = self.PIXEL_MAX
        self.ch1LowVal = self.PIXEL_MIN
        self.ch1HighVal = self.PIXEL_MAX
        self.ch2LowVal = self.PIXEL_MIN
        self.ch2HighVal = self.PIXEL_MAX

        cv2.namedWindow(self.CTRL_WIN)
        cv2.namedWindow(self.THRESH_WIN)
        self.btn = self.BTN_HUE * np.ones((50, 400, 3), dtype=np.uint8)
        self.mode = mode
        self.source = source

        # Create trackbars.
        cv2.createTrackbar("Ch0 Low", self.CTRL_WIN, self.ch0LowVal,
            self.PIXEL_MAX, self.onTrackbar)
        cv2.createTrackbar("Ch0 High", self.CTRL_WIN, self.ch0HighVal,
            self.PIXEL_MAX, self.onTrackbar)
        cv2.createTrackbar("Ch1 Low", self.CTRL_WIN, self.ch1LowVal,
            self.PIXEL_MAX, self.onTrackbar)
        cv2.createTrackbar("Ch1 High", self.CTRL_WIN, self.ch1HighVal,
            self.PIXEL_MAX, self.onTrackbar)
        cv2.createTrackbar("Ch2 Low", self.CTRL_WIN, self.ch2LowVal,
            self.PIXEL_MAX, self.onTrackbar)
        cv2.createTrackbar("Ch2 High", self.CTRL_WIN, self.ch2HighVal,
            self.PIXEL_MAX, self.onTrackbar)

        # Link color space button to mouse event callback. Call
        # updateButton() to initialize button with color space text.
        cv2.setMouseCallback(self.CTRL_WIN, self.onMouse)
        self.updateButton()
            
    def updateButton(self):
        """
        Update controls window clickable "button" to display name of
        current color space.
        """

        btnWithText = self.btn.copy()
        colorSpaceIdxText = self.COLOR_SPACES[self.colorSpaceIdx]
        cv2.putText(btnWithText, colorSpaceIdxText, (170, 40),
            cv2.FONT_HERSHEY_SIMPLEX, 1.0, (0, 0, 0), 4)
        cv2.imshow(self.CTRL_WIN, btnWithText)

    def onTrackbar(self, val):
        # OpenCV-Python seems to have issues with trackbars. Each trackbar's
        # linked variable is not updated when the trackbar position changes.
        # Creating a separate callback function for each trackbar (since a
        # reference to the trackbar is not passed to the callback function)
        # doesn't help. Seems when multiple trackbars are defined in the same
        # window, all trackbars call back to the callback function for the 
        # last defined trackbar (in this case, the one corresponding to
        # ch2HighVal), regardless of which callback function was passed to them.
        #
        # Hence, I've opted to explicitly query each trackbar by name on any
        # trackbar change, regardless of which one was changed.

        self.ch0LowVal = cv2.getTrackbarPos("Ch0 Low", self.CTRL_WIN)
        self.ch0HighVal = cv2.getTrackbarPos("Ch0 High", self.CTRL_WIN)
        self.ch1LowVal = cv2.getTrackbarPos("Ch1 Low", self.CTRL_WIN)
        self.ch1HighVal = cv2.getTrackbarPos("Ch1 High", self.CTRL_WIN)
        self.ch2LowVal = cv2.getTrackbarPos("Ch2 Low", self.CTRL_WIN)
        self.ch2HighVal = cv2.getTrackbarPos("Ch2 High", self.CTRL_WIN)

        if self.mode == "image":
            self.thresholdImage()

    def onMouse(self, event, x, y, flags, data):
        if event == cv2.EVENT_LBUTTONDOWN or event == cv2.EVENT_RBUTTONDOWN:
            if event == cv2.EVENT_LBUTTONDOWN:
                increment = 1
            else:
                increment = -1

            self.colorSpaceIdx = (
                (self.colorSpaceIdx + increment) % len(self.COLOR_SPACES))
            self.updateButton()
            
            # Threshold image on color space change only in image
            # mode. If video or cam mode, the while loop in start()
            # handles this.
            if self.mode == "image":
                self.thresholdImage()

    def thresholdImage(self):
        """Threshold the current image and store in self.thresh."""

        # Convert color space from BGR if necessary.
        colorSpaceName = self.COLOR_SPACES[self.colorSpaceIdx]
        if colorSpaceName != "BGR":
            cvColorCode = self.CV_COLOR_CODES[colorSpaceName]
            thresh = cv2.cvtColor(self.img, cvColorCode)
        else:
            thresh = self.img.copy()

        # Display original image after converting color space but
        # before thresholding color channel(s).
        cv2.imshow(self.IM_WIN, thresh)

        # Split channels (if not grayscale). Perform inRange
        # operations to produce thresholded image.
        if colorSpaceName == "GRAY":
            thresh = cv2.inRange(thresh, self.ch0LowVal,
                self.ch0HighVal)
        else:
            channels = cv2.split(thresh)
            channels[0] = cv2.inRange(channels[0], self.ch0LowVal,
                self.ch0HighVal)
            channels[1] = cv2.inRange(channels[1], self.ch1LowVal,
                self.ch1HighVal)
            channels[2] = cv2.inRange(channels[2], self.ch2LowVal,
                self.ch2HighVal)
            thresh = cv2.bitwise_and(channels[0], channels[1])
            thresh = cv2.bitwise_and(thresh, channels[2])

        cv2.imshow(self.THRESH_WIN, thresh)

    def start(self):
        if self.mode == "image":
            self.img = cv2.imread(self.source)
            cv2.imshow(self.IM_WIN, self.img)
            self.thresholdImage()
            cv2.waitKey(0)
        else:
            cap = cv2.VideoCapture(self.source)

            if not cap.isOpened():
                raise RuntimeError("Error opening VideoCapture")

            # Threshold each frame of video in while loop.
            while True:
                grabbed, self.img = cap.read()
                if not grabbed or cv2.waitKey(1) == ord("q"):
                    break
                
                cv2.imshow(self.IM_WIN, self.img)
                self.thresholdImage()
        cv2.destroyAllWindows()

    def getValues(self):
        colorSpaceName = self.COLOR_SPACES[self.colorSpaceIdx]
        channelValues = {
            "colorSpaceName": colorSpaceName,
            "ch0Low": self.ch0LowVal,
            "ch0High": self.ch0HighVal,
            "ch1Low": self.ch1LowVal,
            "ch1High": self.ch1HighVal,
            "ch2Low": self.ch2LowVal,
            "ch2High": self.ch2HighVal
            }
        return channelValues
        
if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--image", "-i", type=str, default=None,
        help="Path to image file (if source is an image)")
    ap.add_argument("--video", "-v", type=str, default=None,
        help="Path to video file (if source is a video)")
    ap.add_argument("--cam", "-c", type=int, default=0,
        help="Camera index (if source is camera); default 0")
    args = vars(ap.parse_args())

    if args["image"] is not None:
        mode = "image"
        source = args["image"]
    elif args["video"] is not None:
        mode = "video"
        source = args["video"]
    else:
        mode = "cam"
        source = args["cam"]

    c = ColorThreshold(mode=mode, source=source)
    c.start()
