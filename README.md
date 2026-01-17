# Wireless Environmental Monitoring & Control System 🌿

A multi-platform IoT solution for smart agriculture using the **ESP32**. This system monitors environmental conditions in real-time and provides remote control capabilities via **Arduino IoT Cloud** and **Telegram**.

---

## 🚀 Key Features
* **Local Dashboard:** 1.8" ST7735 TFT display showing Temperature, Humidity, Light, and Soil Moisture with custom icons.
* **Cloud Integration:** Real-time data logging and motor control via the Arduino IoT Cloud dashboard.
* **Telegram Bot:** * Receive automated sensor reports every 2 minutes.
    * Control the motor remotely using `/on` and `/off` commands.
    * Check system status anytime with `/status`.
* **Offline Resilience:** The local display and sensors continue to work even if Wi-Fi is lost.

---

## 🛠️ Hardware Components
| Component | Purpose | Pin (ESP32) |
| :--- | :--- | :--- |
| **ESP32** | Brain of the project | - |
| **DHT11** | Temp & Humidity | GPIO 21 |
| **Soil Moisture Sensor** | Moisture detection | GPIO 35 |
| **LDR (Photoresistor)** | Light intensity | GPIO 34 |
| **ST7735 TFT 1.8"** | Local visualization | CS:25, RST:26, DC:27 |
| **Relay & Fan/Motor** | Cooling/Irrigation | GPIO 13 |



---

## 💻 Software & Libraries
The following libraries are required to compile this project:
* `DHT sensor library` (Adafruit)
* `Adafruit GFX` & `ST7735`
* `UniversalTelegramBot`
* `ArduinoIoTCloud`
* `WiFiClientSecure`

---

## ⚙️ Setup Instructions

### 1. Telegram Bot Setup
1. Message [@BotFather](https://t.me/botfather) on Telegram to create a bot.
2. Copy the **API Token** and paste it into the `BOT_TOKEN` variable in the code.
3. Use [@IDBot](https://t.me/myidbot) to get your **Chat ID** and paste it into `CHAT_ID`.

### 2. Arduino IoT Cloud
1. Create a new **Thing**.
2. Add the following variables:
   * `lDR` (Integer)
   * `sOILMOISTURE` (Integer)
   * `temperature` (Float)
   * `humidity` (Float)
   * `motorSwitch` (Boolean)
3. Link your ESP32 board and configure your Network credentials.

### 3. Deployment
1. Open the `.ino` file in Arduino IDE.
2. Ensure `thingProperties.h` is in the same folder.
3. Select **DOIT ESP32 DEVKIT V1** and click **Upload**.

---

## 🎮 Telegram Commands
* `/start` - Displays the welcome message and available commands.
* `/on` - Activates the fan/motor.
* `/off` - Deactivates the fan/motor.
* `/status` - Reports if the motor is currently running.

---

## 👤 Author
* **Rushikesh Gadade** [@ItsRuShi1] (https://github.com/ItsRuShi1) 
* Project: Wireless Data Monitoring System

## Contributions
* **Chaitanya Bolake** [@ChaitanyaBolake] (https://github.com/ChaitanyaBolake)
*  API Integration and Telegram Bot Implementation


