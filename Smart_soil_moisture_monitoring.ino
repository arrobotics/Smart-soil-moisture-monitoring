// 🔧 Blynk Configuration
#define BLYNK_TEMPLATE_ID "TMPL3huF9rXic"
#define BLYNK_TEMPLATE_NAME "Water system"
#define BLYNK_AUTH_TOKEN "CeCf5vCe8x-6-q_sjRLNj-7uKPoJFFR3"

// 📦 Libraries
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// 📶 WiFi Credentials
char ssid[] = "AR";
char pass[] = "12345678";

// 📺 OLED settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 🌱 Moisture Sensor
const int moistureSensorPin = A0;
int airValue = 582;
int waterValue = 420;

// ⚡ Relay
const int relayPin = D5;

// 🌡️ DHT11 Sensor
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true); // Stop if OLED fails
  }
  display.display();
  delay(2000);
  display.clearDisplay();

  // Initialize Relay and DHT
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  dht.begin();

  // Connect to WiFi and Blynk
  WiFi.begin(ssid, pass);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();

  // 🌱 Read Soil Moisture
  int sensorValue = analogRead(moistureSensorPin);
  int moisturePercent = map(sensorValue, airValue, waterValue, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  // 🌡️ Read Temperature and Humidity
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // 🛰️ Send to Blynk
  Blynk.virtualWrite(V0, moisturePercent);
  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity);

  // ⚡ Relay Control: ON when 0-15%, OFF at 16% or higher
  if (moisturePercent >= 0 && moisturePercent <= 15) {
    digitalWrite(relayPin, HIGH);
  } else {
    digitalWrite(relayPin, LOW);
  }

  // 📺 Display on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print("Moisture: ");
  display.print(moisturePercent);
  display.println("%");

  display.setCursor(0, 20);
  display.print("Temp: ");
  if (isnan(temperature)) {
    display.print("Error");
  } else {
    display.print(temperature);
    display.print(" C");
  }

  display.setCursor(0, 35);
  display.print("Humidity: ");
  if (isnan(humidity)) {
    display.print("Error");
  } else {
    display.print(humidity);
    display.print(" %");
  }

  display.display();
  delay(1000);
}
