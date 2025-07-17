🛠️ ESP32 & Rugged Board Ultrasonic Distance Measurement
This project demonstrates how to measure distance using an ultrasonic sensor connected to a rugged board and display the results via a web interface hosted on an ESP32. Communication between the ESP32 and rugged board is handled through UART.

📸 Hardware Setup
Ultrasonic Sensor (e.g., HC-SR04)

Rugged Board with GPIO access via sysfs

ESP32 for Wi-Fi and web server

UART Connection between ESP32 and rugged board

Sample wiring shown below:

Ultrasonic sensor connected to rugged board via GPIO, communicating with ESP32 over UART.

📦 Features
Distance measurement using ultrasonic sensor

UART-based command exchange between ESP32 and rugged board

Web interface to trigger measurement and display results

🔧 Rugged Board Code (C)
Configures GPIO pins for TRIG and ECHO

Measures pulse duration to calculate distance

Listens for "GETDIST" command via UART

Sends back "DIST=<value>" and acknowledgment

🌐 ESP32 Code (Arduino)
Connects to Wi-Fi

Hosts a web server with a button to request distance

Sends "GETDIST" to rugged board via UART

Displays received distance on the webpage

🚀 How to Run
Flash ESP32 with the Arduino sketch.

Compile and run the rugged board C program.

Connect hardware as shown in the image.

Access the ESP32 IP in your browser and click Get Distance.

📁 File Structure

├── usensor.c
├── uwifi.ino
├── README.md
└── output.jpeg

📡 Sample Output

in uart via minicom

✅ UART connected successfully on /dev/ttyS3
📨 Received 'GETDIST' from ESP32 via UART
📡 Measured distance: 12.34 cm — sending back via UART

<img width="1024" height="1536" alt="Copilot_20250717_114451" src="https://github.com/user-attachments/assets/0f90fcd7-0a18-4496-a0d3-42e081448479" />







