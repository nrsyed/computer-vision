/*
 * Name: get_video_pixel
 * Description: Take a snapshot of a video and get the RGB value
 *	of any pixel in the snapshot.
 * Author: Najam Syed (github.com/nrsyed)
 * Created: 2018-Feb-12
 */

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#define COLOR_ROWS 80
#define COLOR_COLS 250

void on_mouse_click(int event, int x, int y, int flags, void* ptr) {
	if (event == cv::EVENT_LBUTTONDOWN) {
		cv::Mat* snapshot = (cv::Mat*)ptr;
		cv::Vec3b pixel = snapshot->at<cv::Vec3b>(cv::Point (x, y));
		int b, g, r;
		b = pixel[0];
		g = pixel[1];
		r = pixel[2];
		std::string rgbText = "[" + std::to_string(r) + ", " + std::to_string(g)
			+ ", " + std::to_string(b) + "]";

		// From stackoverflow.com/questions/1855884/determine-font-color-based-on-background-color
		float luminance = 1 - (0.299*r + 0.587*g + 0.114*b) / 255;
		cv::Scalar textColor;
		if (luminance < 0.5) {
			textColor = cv::Scalar(0,0,0);
		} else {
			textColor = cv::Scalar(255,255,255);
		}

		cv::Mat colorArray;
		colorArray = cv::Mat(COLOR_ROWS, COLOR_COLS, CV_8UC3, cv::Scalar(b,g,r));
		cv::putText(colorArray, rgbText, cv::Point2d(20, COLOR_ROWS - 20),
			cv::FONT_HERSHEY_SIMPLEX, 0.8, textColor);
		cv::imshow("Color", colorArray);
	}
}

int main(int argc, char** argv) {
	cv::VideoCapture capture(0);

	if (!capture.isOpened()) {
		std::cout << "Error opening VideoCapture." << std::endl;
		return -1;
	}

	cv::Mat frame, snapshot, colorArray;
	capture.read(frame);

	snapshot = cv::Mat(frame.size(), CV_8UC3, cv::Scalar(0,0,0));
	cv::imshow("Snapshot", snapshot);

	colorArray = cv::Mat(COLOR_ROWS, COLOR_COLS, CV_8UC3, cv::Scalar(0,0,0));
	cv::imshow("Color", colorArray);
	cv::setMouseCallback("Snapshot", on_mouse_click, &snapshot);

	int keyVal;
	while (1) {
		if (!capture.read(frame)) {
			break;
		}
		cv::imshow("Video", frame);

		keyVal = cv::waitKey(1) & 0xFF;
		if (keyVal == 113) {
			break;
		} else if (keyVal == 116) {
			snapshot = frame.clone();
			cv::imshow("Snapshot", snapshot);
		}
	}
	return 0;
}
