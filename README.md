:Author: Rushikesh Gadade
:Date: 16/02/2025
:Revision: 1.0
:License: Public Domain

= Project: Tech Titans Smart Agriculture System

This project is an IoT-based environmental monitoring station. It uses an ESP32 to track temperature, humidity, light intensity, and soil moisture, displaying the data on a local TFT screen while providing remote monitoring via Arduino IoT Cloud and a Telegram Bot.

== Step 1: Installation
1. Install the Arduino IDE.
2. Install the following libraries:
   * DHT sensor library
   * Adafruit GFX & ST7735
   * UniversalTelegramBot
   * ArduinoIoTCloud
3. Set up a new "Thing" in Arduino IoT Cloud and link the variables: `lDR`, `sOILMOISTURE`, `temperature`, `humidity`, and `motorSwitch`.

== Step 2: Assemble the circuit
Assemble the circuit using the following pinout:
* **DHT11:** GPIO 21
* **LDR:** GPIO 34
* **Soil Sensor:** GPIO 35
* **TFT Display:** CS:25, RST:26, DC:27
* **Relay/Fan:** GPIO 13



== Step 3: Load the code
1. Open `wrdm_code.ino` in Arduino IDE.
2. Fill in your WiFi credentials and Telegram Token in the `thingProperties.h` tab.
3. Upload the code to your ESP32 board.

=== Folder structure
....
 Wireless Data Monitoring System        => Main project folder
  ├── wrdm_code.ino       => Main Arduino sketch
  ├── thingProperties.h    => Cloud configuration & credentials
  ├── layout.png           => Circuit diagram image
  └── ReadMe.adoc          => This documentation file
....

=== BOM (Bill of Materials)

|===
| ID | Part name       | Quantity
| U1 | ESP32 Dev Kit   | 1        
| S1 | DHT11 Sensor    | 1        
| S2 | Soil Moisture   | 1        
| S3 | LDR (Photoresistor)| 1      
| D1 | ST7735 TFT 1.8" | 1        
| K1 | 5V Relay Module | 1        
|===

=== Help
For Telegram Bot setup, visit the @BotFather on Telegram. For Arduino Cloud issues, check the official Arduino IoT Cloud documentation.