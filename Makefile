CXX = g++
CC = gcc
CXXFLAGS = -std=c++17 -O2

TARGET = rxtx_led
SRC = rxtx_led.cpp

all: $(TARGET) disk_io_led_monitor

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) disk_io_led_monitor

disk_io_led_monitor: disk_io_led_monitor.c
	$(CC) -o disk_io_led_monitor disk_io_led_monitor.c -lpthread
