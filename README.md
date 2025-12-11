# Raspberry Pi 5 LED I/O Monitors

This project provides utilities to monitor system I/O activity on a Raspberry Pi (or similar Linux device) and indicate activity using onboard LEDs.

## Features

### rxtx_led.sh

- Bash script to monitor network interface packet activity.
- Flashes LEDs to indicate RX (receive) and TX (transmit) packets.
- Interface name can be specified as an argument (default: `end0`).
- Uses sysfs to control LEDs (`/sys/class/leds/ACT/brightness` and `/sys/class/leds/PWR/brightness`).
- Provides visual feedback for network activity and interface status.

### disk_io_led_monitor

- C program (`disk_io_led_monitor.c`) and Bash script (`disk_io_led_monitor.sh`) to monitor disk and swap I/O.
- Flashes the green LED for SSD I/O and the red LED for swap I/O.
- SSD device can be specified as a command-line argument (e.g., `sda`, `nvme0n1`).
- Uses threads with named identifiers for easier debugging (C version).
- Requires root privileges to access sysfs LED controls.

## Building

To build the C program, run:

```sh
make disk_io_led_monitor
```

## Usage

### Network RX/TX LED Monitor

```sh
sudo ./rxtx_led.sh [interface]
```
- `[interface]` is optional (default: `end0`).

### Disk and Swap I/O LED Monitor

```sh
sudo ./disk_io_led_monitor [device]
```
- `[device]` is optional (default: `sda`). Example: `sudo ./disk_io_led_monitor nvme0n1`

## Cleaning

To remove built binaries:

```sh
make clean
