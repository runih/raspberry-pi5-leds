# Raspberry Pi 5 Network LED Monitor

This project provides a Bash script to visually indicate network activity on a Raspberry Pi 5 using its onboard LEDs. The script monitors packet transmission and reception on a specified network interface and blinks the ACT and PWR LEDs to signal activity.

## Features

- Monitors both transmitted (TX) and received (RX) packets.
- Blinks LEDs for each packet event.
- Provides visual feedback when the network interface is unavailable.
- Customizable network interface selection.

## Requirements

- Raspberry Pi 5 (or compatible hardware with `/sys/class/leds/ACT` and `/sys/class/leds/PWR`)
- Linux with Bash
- Sufficient permissions to write to `/sys/class/leds/*` and `/sys/class/net/*`

## Installation

1. Clone this repository:
   ```sh
   git clone https://github.com/runih/raspberry-pi5-leds.git
   cd raspberry-pi5-leds
   ```
2. Make the script executable:
   ```sh
   chmod +x rxtx_led.sh
   ```

## Usage

Run the script with the desired network interface (e.g., `eth0`). If omitted, it defaults to `end0`:
```sh
sudo ./rxtx_led.sh eth0
```

## Notes

- Root privileges (`sudo`) are required to access sysfs LED controls.
- The script is designed for Raspberry Pi 5; LED paths may differ on other models.
