CC = g++
TARGET = colorthresh
LIBS = -lopencv_core -lopencv_highgui -lopencv_videoio -lopencv_imgproc -lopencv_imgcodecs
FLAGS = -std=c++11
RM = rm

all: $(TARGET)

$(TARGET): $(TARGET).o
	$(CC) $< $(LIBS) -o $@

%.o: %.cpp
	$(CC) -c $< $(FLAGS)

clean:
	$(RM) $(TARGET).o $(TARGET)
