#!/usr/bin/env bash
if [ "$1" != "" ]; then
  ETH_IF="$1"
else
  ETH_IF="end0"
fi

RX_LED_SHOT="/sys/class/leds/ACT/brightness"
TX_TRIGGER="/sys/class/leds/PWR/trigger"
TX_LED_SHOT="/sys/class/leds/PWR/brightness"

while true; do

  if [ -d /sys/class/net/$ETH_IF ]; then

    STATE="active"

    monitor_packets() {
      local type="$1"     # "tx" or "rx"
      local led_shot="$2" # LED sysfs path
      local msg="$3"      # Message to print

      local stat_file="/sys/class/net/$ETH_IF/statistics/${type}_packets"
      local prev=$([ -f "$stat_file" ] && cat "$stat_file" || echo "")
      while true; do
        local cur=$([ -f "$stat_file" ] && cat "$stat_file" || echo "")
        if [ "$cur" == "" ]; then
          exit 0
        fi
        if [ "$cur" -gt "$prev" ]; then
          echo "$msg: $((cur - prev)) packets"
          if [ "$type" = "tx" ]; then
            echo 1 | tee $led_shot >/dev/null && echo 0 | tee $led_shot >/dev/null
          else
            echo 0 | tee $led_shot >/dev/null && echo 1 | tee $led_shot >/dev/null
          fi
          prev=$cur
        fi
        sleep 0.05
      done
    }

    monitor_packets "tx" "$TX_LED_SHOT" "Data transmitting" &
    monitor_packets "rx" "$RX_LED_SHOT" "Data received" &

    wait
  else
    if [ "$STATE" != "waiting" ]; then
      echo "Network interface $ETH_IF does not exist. Please check the interface name."
      echo 1 | tee /sys/class/leds/ACT/brightness >/dev/null
      echo 0 | tee /sys/class/leds/PWR/brightness >/dev/null
      echo timer | tee $TX_TRIGGER >/dev/null
      echo 100 | tee /sys/class/leds/PWR/delay_on >/dev/null
      echo 9000 | tee /sys/class/leds/PWR/delay_off >/dev/null
      STATE="waiting"
    fi
    sleep 2
  fi
done
