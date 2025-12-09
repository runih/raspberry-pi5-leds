#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <filesystem>
#include <atomic>

using namespace std;
namespace fs = std::filesystem;

const string RX_LED_SHOT = "/sys/class/leds/ACT/brightness";
const string TX_TRIGGER = "/sys/class/leds/PWR/trigger";
const string TX_LED_SHOT = "/sys/class/leds/PWR/brightness";
const string PWR_DELAY_ON = "/sys/class/leds/PWR/delay_on";
const string PWR_DELAY_OFF = "/sys/class/leds/PWR/delay_off";

atomic<bool> running{true};

void write_sysfs(const string& path, const string& value) {
    ofstream ofs(path);
    if (ofs) ofs << value;
}

string read_sysfs(const string& path) {
    ifstream ifs(path);
    string val;
    if (ifs) ifs >> val;
    return val;
}

void monitor_packets(const string& eth_if, const string& type, const string& led_shot, const string& msg) {
    string stat_file = "/sys/class/net/" + eth_if + "/statistics/" + type + "_packets";
    string prev_str = read_sysfs(stat_file);
    unsigned long prev = prev_str.empty() ? 0 : stoul(prev_str);

    while (running) {
        string cur_str = read_sysfs(stat_file);
        if (cur_str.empty()) break;
        unsigned long cur = stoul(cur_str);

        if (cur > prev) {
            cout << msg << ": " << (cur - prev) << " packets" << endl;
            if (type == "tx") {
              write_sysfs(led_shot, "1");
              this_thread::sleep_for(chrono::milliseconds(50));
              write_sysfs(led_shot, "0");
            } else {
              write_sysfs(led_shot, "0");
              this_thread::sleep_for(chrono::milliseconds(50));
              write_sysfs(led_shot, "1");
            }
            prev = cur;
        }
        this_thread::sleep_for(chrono::milliseconds(50));
    }
}

int main(int argc, char* argv[]) {
    string eth_if = (argc > 1) ? argv[1] : "end0";
    string state = "";

    while (true) {
        if (fs::is_directory("/sys/class/net/" + eth_if)) {
            state = "active";
            running = true;
            thread tx_thread(monitor_packets, eth_if, "tx", TX_LED_SHOT, "Data transmitted");
            thread rx_thread(monitor_packets, eth_if, "rx", RX_LED_SHOT, "Data received");
            tx_thread.join();
            rx_thread.join();
            running = false;
        } else {
            if (state != "waiting") {
                cerr << "Network interface " << eth_if << " does not exist. Please check the interface name." << endl;
                write_sysfs(RX_LED_SHOT, "1");
                write_sysfs(TX_LED_SHOT, "0");
                write_sysfs(TX_TRIGGER, "timer");
                write_sysfs(PWR_DELAY_ON, "100");
                write_sysfs(PWR_DELAY_OFF, "9000");
                state = "waiting";
            }
            this_thread::sleep_for(chrono::seconds(2));
        }
    }
    return 0;
}
