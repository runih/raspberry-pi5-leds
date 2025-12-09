CXX = g++
CXXFLAGS = -std=c++17 -O2

TARGET = rxtx_led
SRC = rxtx_led.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
