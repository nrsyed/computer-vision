#include <iostream>
#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio/videoio.hpp>

//! Defines for class internal use.
#define PIXEL_MIN 0
#define PIXEL_MAX 255
#define BTN_HUE 127

#define CTRL_WIN "Controls"
#define THRESH_WIN "Thresholded"
#define IM_WIN "Original"

#define COLOR_BGR 0
#define COLOR_GRAY 1
#define COLOR_HSV 2
#define COLOR_Lab 3
#define COLOR_Luv 4
#define COLOR_YCrCb 5
#define COLOR_YUV 6
#define NUM_COLOR_SPACES 7

enum Mode {
    IMAGE,
    VIDEO,
    CAM
};

//! Class for segmenting an image or video via color channel thresholding.
class ColorThreshold {
private:
    //! #img : Original image or video frame.
    //! #thresh : Thresholded image or video frame.
    //! #btn : Controls window clickable 'button' (btn) to display and change
    //!    current color space used for thresholding.
    //! #channels : Array of split channels for 3-channel color spaces
    //!    (not used for GRAY).
    cv::Mat img, thresh, btn, channels[3];

public:
    //! Controls window slider value vars, color space var, and mode enum.
    int ch0LowVal, ch0HighVal, ch1LowVal, ch1HighVal, ch2LowVal, ch2HighVal;
    int colorSpace;
    Mode mode;
    std::string source; // image or video source

    //! Update controls window clickable 'button' to display name of
    //! current color space.
    void updateButton() {
        cv::Mat btnWithText = btn.clone();
        std::string colorSpaceText;
        switch (colorSpace) {
            case COLOR_BGR: colorSpaceText = "BGR"; break;
            case COLOR_GRAY: colorSpaceText = "GRAY"; break;
            case COLOR_HSV: colorSpaceText = "HSV"; break;
            case COLOR_Lab: colorSpaceText = "Lab"; break;
            case COLOR_Luv: colorSpaceText = "Luv"; break;
            case COLOR_YCrCb: colorSpaceText = "YCrCb"; break;
            case COLOR_YUV: colorSpaceText = "YUV"; break;
        }
        cv::putText(btnWithText, colorSpaceText, cv::Point(170, 40),
            cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 4,
            cv::LINE_8, false);
        cv::imshow(CTRL_WIN, btnWithText);
    }

    //! Threshold the current image; store the thresholded image in #thresh,
    //! display the thresholded image in window #THRESH_WIN.
    void thresholdImage() {
        //! Convert color space from BGR if necessary.
        if (colorSpace != COLOR_BGR) {
            int cvColorCode;
            switch (colorSpace) {
                case COLOR_GRAY: cvColorCode = cv::COLOR_BGR2GRAY; break;
                case COLOR_HSV: cvColorCode = cv::COLOR_BGR2HSV; break;
                case COLOR_Lab: cvColorCode = cv::COLOR_BGR2Lab; break;
                case COLOR_Luv: cvColorCode = cv::COLOR_BGR2Luv; break;
                case COLOR_YCrCb: cvColorCode = cv::COLOR_BGR2YCrCb; break;
                case COLOR_YUV: cvColorCode = cv::COLOR_BGR2YUV; break;
            }
            cv::cvtColor(img, thresh, cvColorCode, 0);
        } else {
            thresh = img.clone();
        }

        //! Display the original image after converting color space (but
        //! before thresholding color channel(s)).
        cv::imshow(IM_WIN, thresh);

        //! Split channels (if not grayscale); perform inRange operations
        //! to produce thresholded image.
        if (colorSpace == COLOR_GRAY) {
            cv::inRange(thresh, ch0LowVal, ch0HighVal, thresh);
        } else {
            cv::split(thresh, channels);
            cv::inRange(channels[0], ch0LowVal, ch0HighVal, channels[0]);
            cv::inRange(channels[1], ch1LowVal, ch1HighVal, channels[1]);
            cv::inRange(channels[2], ch2LowVal, ch2HighVal, channels[2]);
            cv::bitwise_and(channels[0], channels[1], thresh);
            cv::bitwise_and(thresh, channels[2], thresh);
        }

        cv::imshow(THRESH_WIN, thresh);
    }

    //! OpenCV trackbar callback.
    static void onTrackbar(int val, void* data) {
        //! Threshold image on trackbar change only in IMAGE mode. If VIDEO
        //! or CAM mode, the while loop in start() handles this.
        ColorThreshold* c = (ColorThreshold*)data;
        if (c->mode == IMAGE) {
            c->thresholdImage();
        }
    }

    //! OpenCV mouse event callback. 
    static void onMouse(int event, int x, int y, int flags, void* data) {
        ColorThreshold* c = (ColorThreshold*)data;
        if (event == cv::EVENT_LBUTTONDOWN || event == cv::EVENT_RBUTTONDOWN) {
            //! Left click to cycle forward through color spaces, right click
            //! to cycle backward.
            int increment = event == cv::EVENT_LBUTTONDOWN ? 1 : -1;
            c->colorSpace = (c->colorSpace + increment) % NUM_COLOR_SPACES;

            //! Modulo of negative number is implementation-dependent,
            //! so perform manual check and set to last color space index.
            if (c->colorSpace < 0) {
                c->colorSpace = NUM_COLOR_SPACES - 1;
            }

            c->updateButton();

            //! Threshold image on color space change only in IMAGE mode.
            //! If VIDEO or CAM mode, the while loop in start() handles this.
            if (c->mode == IMAGE) {
                c->thresholdImage();
            }
        }
    }

    void start() {
        //! Load image or open VideoCapture and start thresholding routine.
        if (mode == IMAGE) {
            img = cv::imread(source);
            cv::imshow(IM_WIN, img);
            thresholdImage();
            cv::waitKey(0);
        } else {
            cv::VideoCapture cap;
            if (mode == VIDEO) {
                cap.open(source);
            } else {
                // If CAM mode, convert camera index string to int.
                cap.open(std::stoi(source, nullptr, 10));
            }

            if (!cap.isOpened()) {
                throw std::runtime_error("Error opening VideoCapture");
            }

            //! Threshold each frame of video in while loop.
            while (1) {
                if (!cap.read(img)) {
                    break;
                }
                cv::imshow(IM_WIN, img);
                thresholdImage();
                if (cv::waitKey(1) == 'q') {
                    break;
                }
            }
        }
    }

	void getValues(std::vector<int>& channelValues) {
		channelValues.push_back(ch0LowVal);
		channelValues.push_back(ch0HighVal);
		channelValues.push_back(ch1LowVal);
		channelValues.push_back(ch1HighVal);
		channelValues.push_back(ch2LowVal);
		channelValues.push_back(ch2HighVal);
	}

    ColorThreshold(Mode mode=CAM, std::string source="0") {
        ch0LowVal = PIXEL_MIN;
        ch0HighVal = PIXEL_MAX;
        ch1LowVal = PIXEL_MIN;
        ch1HighVal = PIXEL_MAX;
        ch2LowVal = PIXEL_MIN;
        ch2HighVal = PIXEL_MAX;
        colorSpace = 0;
        cv::namedWindow(CTRL_WIN);
        cv::namedWindow(THRESH_WIN);
        btn = cv::Mat(50, 400, CV_8UC3, cv::Scalar(BTN_HUE, BTN_HUE, BTN_HUE));
        this->mode = mode;
        this->source = source;

        //! Create trackbars. Note that we must pass 'this' pointer to
        //! static member functions.
        cv::createTrackbar("Ch0 Low", CTRL_WIN, &ch0LowVal, PIXEL_MAX,
            onTrackbar, this);
        cv::createTrackbar("Ch0 High", CTRL_WIN, &ch0HighVal, PIXEL_MAX,
            onTrackbar, this);
        cv::createTrackbar("Ch1 Low", CTRL_WIN, &ch1LowVal, PIXEL_MAX,
            onTrackbar, this);
        cv::createTrackbar("Ch1 High", CTRL_WIN, &ch1HighVal, PIXEL_MAX,
            onTrackbar, this);
        cv::createTrackbar("Ch2 Low", CTRL_WIN, &ch2LowVal, PIXEL_MAX,
            onTrackbar, this);
        cv::createTrackbar("Ch2 High", CTRL_WIN, &ch2HighVal, PIXEL_MAX,
            onTrackbar, this);

        //! Link color space 'button' to mouse event callback. Call
        //! updateButton() to initialize button with color space text.
        cv::setMouseCallback(CTRL_WIN, onMouse, this);
        updateButton();
    }
};

int main(int argc, char **argv) {
    Mode mode;
    std::string source;
    if (argc == 3 && strcmp(argv[1], "-i") == 0) {
        mode = IMAGE;
        source = argv[2];
    } else if (argc == 3 && strcmp(argv[1], "-v") == 0) {
        mode = VIDEO;
        source = argv[2];
    } else if (argc == 3 && strcmp(argv[1], "-c") == 0) {
        mode = CAM;
        source = argv[2];
    } else {
        mode = CAM;
        source = "0";
    }

    ColorThreshold c(mode, source);
    c.start();
    return 0;
}
