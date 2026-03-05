#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LM35_PIN 34

// Wi-Fi credentials
const char* ssid = "@armaan_khxn__";
const char* password = "786786786j";

// MQTT server (your PC)
const char* mqtt_server = "192.168.222.100";

WiFiClient espClient;
PubSubClient client(espClient);

// LCD I2C (16x2)
LiquidCrystal_I2C lcd(0x27,16,2);

// ----------------- WIFI -----------------
void setupWiFi() {
  Serial.println("Wi-Fi Connecting...");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Wi-Fi Connecting...");

  WiFi.begin(ssid,password);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Wi-Fi Connected!");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Wi-Fi Connected");
  delay(2000);
}

// ----------------- MQTT -----------------
void reconnectMQTT() {
  lcd.clear();
  lcd.setCursor(0,0);

  while(!client.connected()) {
    Serial.println("MQTT Connecting...");
    lcd.setCursor(0,0);
    lcd.print("MQTT Connecting   "); // spaces to overwrite old text

    if(client.connect("ESP32Client")) {
      Serial.println("MQTT Connected!");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("MQTT Connected");
      delay(2000);
      break;
    }
    delay(1000); // retry every 1 second
  }
}

// ----------------- SETUP -----------------
void setup() {
  Serial.begin(115200);
  delay(4000); // Important for Robot Framework to wait

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("System Starting");
  delay(2000);

  setupWiFi();
  client.setServer(mqtt_server, 1883);
}

// ----------------- MAIN LOOP -----------------
void loop() {
  // Ensure Wi-Fi & MQTT are connected
  if(WiFi.status() != WL_CONNECTED) setupWiFi();
  if(!client.connected()) reconnectMQTT();

  client.loop();

  // Read LM35
  int sensorValue = analogRead(LM35_PIN);
  float voltage = sensorValue * (3.3 / 4095.0);
  float temperature = voltage * 100;

  int wifiRSSI = WiFi.RSSI();

  // Serial output for Robot Framework
  Serial.print("TEMP:");
  Serial.println(temperature);
  Serial.print("RSSI:");
  Serial.println(wifiRSSI);

  // Publish to MQTT
  char tempStr[10];
  dtostrf(temperature,4,2,tempStr);
  char wifiStr[10];
  itoa(wifiRSSI,wifiStr,10);

  client.publish("iot/temperature", tempStr);
  client.publish("iot/wifi", wifiStr);

  // Display on LCD (NO CLEAR, keeps values after tests)
  lcd.setCursor(0,0);
  lcd.print("Temp:");
  lcd.print(temperature);
  lcd.print("C   "); // overwrite old digits

  lcd.setCursor(0,1);
  lcd.print("WiFi:");
  lcd.print(wifiRSSI);
  lcd.print("    ");

  delay(5000); // update every 5 seconds
}