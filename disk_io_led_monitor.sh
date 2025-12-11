#!/usr/bin/env bash

# Set LED sysfs paths (adjust as needed for your hardware)
GREEN_LED="/sys/class/leds/ACT/brightness" # Red LED path
RED_LED="/sys/class/leds/PWR/brightness"   # Green LED path

# Set SSD device (adjust as needed, e.g., sda, nvme0n1)
SSD_DEV="nvme0n1"

monitor_swap() {
  prev_in=$(awk '/pswpin/ {print $2}' /proc/vmstat)
  prev_out=$(awk '/pswpout/ {print $2}' /proc/vmstat)
  while true; do
    cur_in=$(awk '/pswpin/ {print $2}' /proc/vmstat)
    cur_out=$(awk '/pswpout/ {print $2}' /proc/vmstat)
    if [ "$cur_in" -gt "$prev_in" ] || [ "$cur_out" -gt "$prev_out" ]; then
      echo 1 >"$RED_LED"
      sleep 0.05
      echo 0 >"$RED_LED"
      prev_in=$cur_in
      prev_out=$cur_out
    fi
    sleep 0.05
  done
}

monitor_ssd() {
  prev=$(awk '{print $3+$7}' /sys/block/$SSD_DEV/stat)
  while true; do
    cur=$(awk '{print $3+$7}' /sys/block/$SSD_DEV/stat)
    if [ "$cur" -gt "$prev" ]; then
      echo 0 >"$GREEN_LED"
      sleep 0.05
      echo 1 >"$GREEN_LED"
      prev=$cur
    fi
    sleep 0.05
  done
}

monitor_swap &
monitor_ssd &

wait
