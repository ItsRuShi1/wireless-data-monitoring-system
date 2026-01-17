
#include "thingProperties.h"
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define BOT_TOKEN "YOUR BOT TOKEN"
#define CHAT_ID "CHAT-ID"

#define DHTPIN 21
#define DHTTYPE DHT11
#define SOILMOISTURE_PIN 35
#define LDR_PIN 34
#define TFT_CS 25
#define TFT_RST 26
#define TFT_DC 27
#define FAN_PIN 13

#define ORANGE 0xFD20
#define BLUE 0x001F
#define YELLOW 0xFFE0
#define BROWN 0xA145
#define WHITE 0xFFFF
#define BLACK 0x0000

const uint32_t LDR_INTERVAL = 100;
const uint32_t DHT_INTERVAL = 1000;
const uint32_t SOIL_INTERVAL = 2000;
const uint32_t TELEGRAM_INTERVAL = 120000;

DHT dht(DHTPIN, DHTTYPE);
Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);
struct {
  bool fanState = false;
  bool wasConnected = false;
  uint32_t lastLDR = 0, lastDHT = 0, lastSoil = 0, lastTelegram = 0;
} state;

void drawSymbol(int x, int y, uint16_t color, uint8_t type);
void drawBox(int x, int y, const char* name, float value, const char* unit, uint16_t color, uint8_t symbolType);
void sendTelegram(const String& message);
void handleTelegram();

void setup() {
  Serial.begin(9600);
  delay(1500);

  pinMode(SOILMOISTURE_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, HIGH);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 50);
  tft.print("TECH TITANS");
  delay(2000);
  tft.fillScreen(BLACK);

  Serial.printf("Connecting to %s\n", SSID);
  WiFi.begin(SSID, PASS);
  for (uint8_t i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\nWiFi Connected! IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\nWiFi Connection Failed!");
  }

  // Initialize cloud variables
  lDR = 0;
  sOILMOISTURE = 0;
  temperature = 0;
  humidity = 0;
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  if (WiFi.status() == WL_CONNECTED) {
    sendTelegram("System is Online");
    state.wasConnected = true;
  }

  dht.begin();
}

void drawSymbol(int x, int y, uint16_t color, uint8_t type) {
  switch (type) {
    case 0: tft.drawRect(x, y, 4, 12, color); tft.fillCircle(x + 2, y + 14, 4, color); break;
    case 1: tft.fillCircle(x, y + 6, 4, color); tft.fillTriangle(x, y, x - 4, y + 6, x + 4, y + 6, color); break;
    case 2: tft.fillCircle(x, y, 4, color); for (uint8_t i = 0; i < 8; i++) { float angle = i * 45 * PI / 180; tft.drawLine(x, y, x + cos(angle) * 8, y + sin(angle) * 8, color); } break;
    case 3: tft.drawLine(x - 4, y + 2, x + 4, y + 2, color); tft.drawLine(x - 4, y + 4, x + 4, y + 4, color); tft.drawLine(x - 4, y + 6, x + 4, y + 6, color); break;
  }
}

void drawBox(int x, int y, const char* name, float value, const char* unit, uint16_t color, uint8_t symbolType) {
  tft.drawRect(x, y, 80, 64, WHITE);
  tft.setTextColor(color);
  tft.setTextSize(1);
  tft.setCursor(x + 2, y + 2);
  tft.print(name);
  drawSymbol(x + 10, y + 20, color, symbolType);
  tft.setCursor(x + 30, y + 45);
  tft.print(value);
  tft.print(unit);
}

void sendTelegram(const String& message) {
  bot.sendMessage(CHAT_ID, message);
}

void handleTelegram() {
  int numMessages = bot.getUpdates(bot.last_message_received + 1);
  for (int i = 0; i < numMessages; i++) {
    if (bot.messages[i].chat_id != CHAT_ID) continue;
    String text = bot.messages[i].text;
    if (text == "/on") {
      state.fanState = true;
      digitalWrite(FAN_PIN, LOW);
      motorSwitch = true;
      sendTelegram("Motor is ON (Telegram)");
    } else if (text == "/off") {
      state.fanState = false;
      digitalWrite(FAN_PIN, HIGH);
      motorSwitch = false;
      sendTelegram("Motor is OFF (Telegram)");
    } else if (text == "/start") {
      sendTelegram("Commands: /on, /off, /status");
    } else if (text == "/status") {
      sendTelegram(state.fanState ? "Motor is ON" : "Motor is OFF");
    }
  }
}

void updateSensorsAndDisplay(uint32_t currentMillis) {
  if (currentMillis - state.lastLDR >= LDR_INTERVAL) {
    state.lastLDR = currentMillis;
    int ldrRaw = analogRead(LDR_PIN);
    int ldrMapped = map(ldrRaw, 0, 4095, 100, 0);
    lDR = constrain(ldrMapped, 0, 100);
    Serial.printf("LDR Raw: %d, Light: %d%%\n", ldrRaw, lDR);
  }

  if (currentMillis - state.lastDHT >= DHT_INTERVAL) {
    state.lastDHT = currentMillis;
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (!isnan(h) && !isnan(t)) {
      humidity = h;
      temperature = t;
      Serial.printf("Temp: %.1f°C, Humidity: %.1f%%\n", t, h);
    } else {
      Serial.println("DHT Error");
    }
  }

  if (currentMillis - state.lastSoil >= SOIL_INTERVAL) {
    state.lastSoil = currentMillis;
    int soilRaw = analogRead(SOILMOISTURE_PIN);
    int soilMapped = map(soilRaw, 4095, 0, 0, 100);
    sOILMOISTURE = constrain(soilMapped, 0, 100);
    Serial.printf("Soil Raw: %d, Soil Moisture: %d%%\n", soilRaw, sOILMOISTURE);

    tft.fillScreen(BLACK);
    drawBox(0, 0, "Temperature", temperature, " C", ORANGE, 0);
    drawBox(80, 0, "Humidity", humidity, " %", BLUE, 1);
    drawBox(0, 64, "Light", lDR, " %", YELLOW, 2);
    drawBox(80, 64, "Soil Moisture", sOILMOISTURE, " %", BROWN, 3);
  }
}

void loop() {
  uint32_t currentMillis = millis();
  
  // Always update sensors and display, regardless of Wi-Fi status
  updateSensorsAndDisplay(currentMillis);

  // Wi-Fi-dependent operations
  if (WiFi.status() == WL_CONNECTED) {
    ArduinoCloud.update();
    bool isConnected = ArduinoCloud.connected();
    if (isConnected != state.wasConnected) {
      sendTelegram(isConnected ? "System is Online" : "System is Offline");
      state.wasConnected = isConnected;
    }

    if (motorSwitch != state.fanState) {
      state.fanState = motorSwitch;
      digitalWrite(FAN_PIN, state.fanState ? LOW : HIGH);
      sendTelegram(state.fanState ? "Motor is ON (Dashboard)" : "Motor is OFF (Dashboard)");
      Serial.println(state.fanState ? "Motor ON (Dashboard)" : "Motor OFF (Dashboard)");
    }

    if (currentMillis - state.lastTelegram >= TELEGRAM_INTERVAL) {
      state.lastTelegram = currentMillis;
      String message = "Sensor Data:\n"
                       "Temperature: " + String(temperature) + " °C\n"
                       "Humidity: " + String(humidity) + " %\n"
                       "Light: " + String(lDR) + " %\n"
                       "Soil Moisture: " + String(sOILMOISTURE) + " %\n"
                       "Motor: " + String(state.fanState ? "ON" : "OFF");
      sendTelegram(message);
    }

    handleTelegram();
  } else {
    Serial.println("WiFi Disconnected!");
  }
}

void onMotorSwitchChange() {
  state.fanState = motorSwitch;
  digitalWrite(FAN_PIN, state.fanState ? LOW : HIGH);
  sendTelegram(state.fanState ? "Motor is ON (Dashboard)" : "Motor is OFF (Dashboard)");
  Serial.println(state.fanState ? "Motor ON (Dashboard)" : "Motor OFF (Dashboard)");
}

#define TELEGRAM_CERTIFICATE_ROOT R"(-----BEGIN CERTIFICATE-----\nMIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkGA1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jvb3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAwMDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9iYWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxTaWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZjc6j40+Kfvvxi4Mla+pIH/EqsJuoDhtXA6RmzJBPnoQ1X1XzB8+7E6i/J6mKGPdh5qW/+gD7I0pqpT8KMcMHOI6e8T/Jw7q+EZte9ZPbQ2bXz1Q8g9HxmXQCXw6aaoN47a/hk4mRam/Frf0kRz/hT6G7VRJ+AuCRWqO5I5lD+Kt281kmr3+h6FjX1RtnkI6kX5oXvFOpXbsXJ0dJjJEYgsHmLgb+8rCh8qre9lp9j9fkwRpv7x8i5sS3yW+0a6oaN2Lmh+k+IVbA+Uts0ZtAKM8hW5l+9uYxS+AuKAx4+OBXw4f6d6zRTr+TLX+NRH/FCq6XRNzM5oT6v/TWPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0BAQUFAAOCAQEAQkzEYaojRG3H+PRN2H2BwnN+TsH1e+jsYdwC7MlA/AjXllrZELqR6c+0fDeaL0/MSalKwxvH8gix+N0nXzXSIKXBuCE3uJ6nQ/tgI2ZA2mS9uWPKhZfTLblnkGimVWK/be+2Xh5+UCIahjFKB63+dvWZA9q1e3J8X3j+3tKxNhg49CofWxgAXvK9x9i8uI0Xw7l8oOCc5UDXi71Q3YQeZxpg0f4gT6M4eRp2GD/LOfL61d+hn7P+XlL2V8vC+PZVY6jaE4rNdP7a1lHs5R2mzeolMKaTRLTu7ZjyZ4xkyb6R8sB5G2h+lADdQnpXgiMGUW+0QsO+TTX+4ewK4MCn7Q==\n-----END CERTIFICATE-----)"