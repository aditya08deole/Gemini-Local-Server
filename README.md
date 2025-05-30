# Gemini-Powered Local AI Assistant on ESP32 (EDITH)

## 🔧 Project Overview

**EDITH (ESP32-Driven Intelligent Task Handler)** is a local AI assistant system built using the **ESP32** microcontroller. This project sets up a local web server on the ESP32 that allows users to enter commands via a browser interface. These commands are sent to **Google Gemini AI** using the **Gemini API key**, and the AI's response is:

- Displayed on a connected **OLED screen** for quick view
- Returned to the user's browser through the ESP32 server interface

It is designed and programmed entirely using the **Arduino IDE**, leveraging the `WiFi`, `HTTPClient`, and `U8g2` (or similar) libraries to handle networking and OLED display rendering.

---

## 🛠️ Key Features

- 🌐 **Local Web Server**: Users can send input queries via a simple HTML form hosted on the ESP32.
- 🤖 **Gemini AI Integration**: The input is sent to Google’s Gemini API for intelligent task handling and response generation.
- 🖥️ **OLED Display Output**: Responses from Gemini AI are parsed and rendered on a compact OLED screen.
- ⚡ **Real-Time Interaction**: Facilitates real-time command handling, making it useful as a smart assistant or a base for home automation.

---

## 🧰 Tech Stack

| Component        | Description |
|------------------|-------------|
| **ESP32 Dev Module** | Wi-Fi-enabled microcontroller used to run the local server and communicate with Gemini API |
| **Google Gemini API** | Cloud AI model used to generate intelligent responses |
| **OLED Display (SSD1306)** | 128x64 screen used to display the AI response |
| **Arduino IDE**  | Development platform used to write, compile, and upload code to ESP32 |
| **Libraries Used** | `WiFi.h`, `HTTPClient.h`, `ArduinoJson.h`, `U8g2lib.h` |

---

## 🚀 Getting Started

### 1. **Hardware Requirements**

- ESP32 Devkit V1
- SSD1306 OLED Display (I2C)
- USB cable and power supply
- Wi-Fi network credentials

### 2. **Software Requirements**

- Arduino IDE with:
  - ESP32 board manager installed
  - Required libraries installed:
    - `WiFi.h`
    - `HTTPClient.h`
    - `ArduinoJson.h`
    - `U8g2lib.h` or `Adafruit_SSD1306.h`

### 3. **Setup Instructions**

1. **Clone or Download the Repository**
2. **Open the `.ino` file** in Arduino IDE
3. **Configure the following in the sketch:**
   - Wi-Fi SSID and password
   - Gemini API Key
4. **Connect the OLED display to ESP32** (typically SDA -> GPIO 21, SCL -> GPIO 22)
5. **Upload the code** to ESP32 via USB
6. **Open Serial Monitor** for debugging
7. **Access the local server** (find IP address via Serial Monitor)
8. **Submit a query** on the web form and view the response on the OLED and browser

---

## 💡 How It Works

1. ESP32 connects to the local Wi-Fi network and starts a web server.
2. Users open the ESP32 IP address in a browser and submit a query through a form.
3. ESP32 receives the query and makes an HTTPS POST request to the Gemini API endpoint.
4. The response is parsed and:
   - Printed to Serial Monitor
   - Displayed on OLED
   - Sent back to the browser

---

## 📈 Future Development Ideas

This project serves as a strong foundation for more advanced IoT and AI integrations. Potential future upgrades include:

- 🏠 **Smart Home Integration**: Control lights, appliances, or sensors based on natural language commands.
- 🔊 **Voice Command Module**: Add a microphone module and integrate offline speech-to-text.
- 📷 **Camera Integration**: Use image recognition or computer vision APIs for smart surveillance.
- 📚 **Chat History Log**: Save queries and responses to SPIFFS/SD card for historical analysis.
- 📱 **Mobile App Interface**: Develop a Flutter or Android/iOS app to interact with ESP32.
- 🧠 **Local LLM Execution**: Integrate TinyML models to handle basic logic locally without relying on cloud AI.

---



---

## 🤝 Acknowledgements

- Google Gemini AI
- ESP32 and Arduino IDE communities
- Adafruit and U8g2 Libraries
