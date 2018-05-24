CC = g++
TARGET = ColorThreshUtil
LIBS = -lopencv_core -lopencv_highgui -lopencv_videoio -lopencv_imgproc -lopencv_imgcodecs
RM = rm

all: $(TARGET)

$(TARGET): $(TARGET).o
	$(CC) $< $(LIBS) -o $@

%.o: %.cpp
	$(CC) -c $<

clean:
	$(RM) $(TARGET).o $(TARGET)
