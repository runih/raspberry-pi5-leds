#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define GREEN_LED "/sys/class/leds/ACT/brightness"
#define RED_LED   "/sys/class/leds/PWR/brightness"

void flash_led(const char *led_path, const char *signal) {
    FILE *f = fopen(led_path, "w");
    if (f) {
        fputc(signal[0], f);
        fclose(f);
        usleep(50000); // 50 ms
        f = fopen(led_path, "w");
        if (f) {
            fputc(signal[1], f);
            fclose(f);
        }
    }
}

void *monitor_swap(void *arg) {
    pthread_setname_np(pthread_self(), "swapmon");
    unsigned long prev_in = 0, prev_out = 0;
    char line[128];
    while (1) {
        FILE *vmstat = fopen("/proc/vmstat", "r");
        if (!vmstat) {
            perror("fopen /proc/vmstat");
            sleep(1);
            continue;
        }
        unsigned long cur_in = 0, cur_out = 0;
        while (fgets(line, sizeof(line), vmstat)) {
            if (sscanf(line, "pswpin %lu", &cur_in) == 1) continue;
            if (sscanf(line, "pswpout %lu", &cur_out) == 1) continue;
        }
        fclose(vmstat);

        if ((cur_in > prev_in) || (cur_out > prev_out)) {
            flash_led(RED_LED, "10");
            prev_in = cur_in;
            prev_out = cur_out;
        }
        usleep(50000); // 50 ms
    }
    return NULL;
}

void *monitor_ssd(void *arg) {
    pthread_setname_np(pthread_self(), "ssdmon");
    const char *ssd_stat = (const char *)arg;
    unsigned long prev = 0;
    while (1) {
        FILE *stat = fopen(ssd_stat, "r");
        if (!stat) {
            perror("fopen SSD_STAT");
            sleep(1);
            continue;
        }
        unsigned long vals[11] = {0};
        for (int i = 0; i < 11; ++i)
            fscanf(stat, "%lu", &vals[i]);
        fclose(stat);

        unsigned long cur = vals[2] + vals[6]; // sectors read + written
        if (cur > prev) {
            flash_led(GREEN_LED, "01");
            prev = cur;
        }
        usleep(50000); // 50 ms
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    char ssd_stat_path[128];
    const char *device = "sda"; // default device
    if (argc > 1) {
        device = argv[1];
    }
    snprintf(ssd_stat_path, sizeof(ssd_stat_path), "/sys/block/%s/stat", device);

    pthread_t swap_thread, ssd_thread;
    if (pthread_create(&swap_thread, NULL, monitor_swap, NULL) != 0) {
        perror("pthread_create swap");
        return 1;
    }
    if (pthread_create(&ssd_thread, NULL, monitor_ssd, ssd_stat_path) != 0) {
        perror("pthread_create ssd");
        return 1;
    }
    pthread_join(swap_thread, NULL);
    pthread_join(ssd_thread, NULL);
    return 0;
}
