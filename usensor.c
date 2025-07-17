#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <errno.h>

#define UART_PORT "/dev/ttyS3"
#define BAUD_RATE B115200

#define TRIG_GPIO "115"
#define ECHO_GPIO "116"

// 🛠 Setup GPIO via sysfs
void setup_gpio(const char* gpio_num, const char* direction) {
    char path[64];
    int fd;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd >= 0) {
        write(fd, gpio_num, strlen(gpio_num));
        close(fd);
        usleep(100000);
    }

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/direction", gpio_num);
    fd = open(path, O_WRONLY);
    if (fd >= 0) {
        write(fd, direction, strlen(direction));
        close(fd);
    }
}

// ✍️ Write to GPIO
void write_gpio(const char* gpio_num, int value) {
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/value", gpio_num);
    int fd = open(path, O_WRONLY);
    if (fd >= 0) {
        write(fd, value ? "1" : "0", 1);
        close(fd);
    }
}

// 👀 Read from GPIO
int read_gpio(const char* gpio_num) {
    char path[64], val;
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/value", gpio_num);
    int fd = open(path, O_RDONLY);
    if (fd >= 0) {
        read(fd, &val, 1);
        close(fd);
        return val == '1';
    }
    return -1;
}

// 🔌 Setup UART
int setup_uart() {
    int fd = open(UART_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror("Unable to open UART");
        return -1;
    }

    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, BAUD_RATE);
    cfsetospeed(&options, BAUD_RATE);

    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;

    tcsetattr(fd, TCSANOW, &options);
    return fd;
}

// ✉️ Send message over UART
void send_uart(int fd, const char* msg) {
    write(fd, msg, strlen(msg));
}

// 📏 Measure distance using ultrasonic sensor
float measure_distance() {
    struct timespec start, end;

    write_gpio(TRIG_GPIO, 0);
    usleep(2);
    write_gpio(TRIG_GPIO, 1);
    usleep(10);
    write_gpio(TRIG_GPIO, 0);

    while (!read_gpio(ECHO_GPIO));
    clock_gettime(CLOCK_MONOTONIC, &start);

    while (read_gpio(ECHO_GPIO));
    clock_gettime(CLOCK_MONOTONIC, &end);

    long duration_us = (end.tv_sec - start.tv_sec) * 1000000 +
                       (end.tv_nsec - start.tv_nsec) / 1000;

    return duration_us / 58.0;  // cm
}

// 🚀 Main loop
int main() {
    int uart_fd = setup_uart();
    if (uart_fd == -1) return EXIT_FAILURE;
    printf("✅ UART connected successfully on %s\n", UART_PORT);

    setup_gpio(TRIG_GPIO, "out");
    setup_gpio(ECHO_GPIO, "in");

    char buffer[100];
    while (1) {
        int bytes = read(uart_fd, buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';

            if (strstr(buffer, "GETDIST")) {
                printf("📨 Received 'GETDIST' from ESP32 via UART\n");

                float dist = measure_distance();
                printf("📡 Measured distance: %.2f cm — sending back via UART\n", dist);

                snprintf(buffer, sizeof(buffer), "DIST=%.2f\n", dist);
                send_uart(uart_fd, buffer);
                send_uart(uart_fd, "ACK: Distance measured\n");
            }
        }
        usleep(100000);
    }

    close(uart_fd);
    return EXIT_SUCCESS;
}

