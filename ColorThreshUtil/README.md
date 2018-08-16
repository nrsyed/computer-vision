# Color Thresholding Utility

Threshold an image or video, in real time, based on its color channels. The following color spaces are currently supported: BGR, grayscale, HSV, Lab, Luv,YCrCb, YUV.

![Color thresholding utility demo](images/colorthresh_screenshot.png)

[Click here to watch a demo of the tool on YouTube](https://youtu.be/YGzXznbvyNU)

Running the program produces three windows:

### Controls Window
This window displays three sets of color channel sliders and a "button" that displays the name of the current color space. Use the six sliders set the lower and upper bounds for channel 0, channel 1, and channel 2. For example, in the BGR color space, channel 0 = B (blue), channel 1 = G (green), and channel 2 = R (red). In the grayscale color space, only the channel 0 sliders are utilized. Slider values range from 0 to 255, corresponding to an 8-bit unsigned integer representation for each channel. There's one notable exception with OpenCV: the hue channel of the HSV color space is represented by a value from 0-180.

To cycle through color spaces, click the color space button. Left-click to cycle forward, right-click to cycle in reverse.

### Image Window
This window displays the original image or video after color space conversion (but before thresholding). Note that OpenCV treats images as BGR (for 3-channel images) or grayscale (for 1-channel images) and displays them accordingly, regardless of what color space they're actually in. This means that images in other color spaces, though they're still displayed (and tend to look pretty interesting), do not necessarily have any visual or physical meaning.

### Thresholded Window
This window displays the thresholded image in black and white. The pixels in each channel of the original image are thresholded based on their respective slider values. Pixels with a value greater than the minimum ("ChX Low") and less than the maximum ("ChX High") are displayed in white. Pixels that fall outside the range are displayed in black. The intersection of all three channels is computed via bitwise AND, i.e., a pixel will only be displayed in white if all three of its color channel values fall within their respective ranges (which you set using the sliders). In the case of the grayscale color space, the channel 1 and channel 2 sliders are ignored.

## Usage

If using  C++ on Linux, you can use the included Makefile to first compile the program using `make`.

Run the program without arguments to start in camera mode with default source "0" (usually the first connected webcam):

**Python**
```python
python colorthresh.py
```

**C++**
```cpp
./colorthresh
```

Run the program with the `-c` flag followed by an integer representing a camera index to start in camera mode (the first connected webcam is usually index "0"):

**Python**
```python
python colorthresh.py -c 1
```

**C++**
```cpp
./colorthresh -c 1
```

Run the program with the `-v` flag followed by a filename to start in video mode:

**Python**
```python
python colorthresh.py -v myVideoFile.mp4
```

**C++**
```cpp
./colorthresh -v myVideoFile.mp4
```

Run the program with the `-i` flag followed by a filename to start in image mode:

**Python**
```python
python colorthresh.py -i myImageFile.png
```

**C++**
```cpp
./colorthresh -i myImageFile.png
```

After the program has started, press the "q" key at any time to quit.
