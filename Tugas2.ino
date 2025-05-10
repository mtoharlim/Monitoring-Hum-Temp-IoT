#define BLYNK_TEMPLATE_ID "TMPL6obEA1g6L"
#define BLYNK_TEMPLATE_NAME "Sensor dan Sinyal Pertmuan 10"
#define BLYNK_AUTH_TOKEN "qMrxHqtwtYPyz2D5K89uUN4Ntf0ZqFZu"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
char ssid[] = "Sukamara";
char pass[] = "sukamare2024";
// OLED settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
// DHT settings
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
// Pin definitions
#define fan_pin 5
#define led_pin 2
// Thresholds
#define MaxLevel 34
#define MinLevel 27
// Variables
unsigned long prev = 0;
#define refresh_rate 1000
bool fan_status = false;
bool led_status = false;
void fan_on(bool value) {
  digitalWrite(fan_pin, value);
}

// Main logic
void read_send_Data() {
  float t = dht.readTemperature();
  int h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println("Gagal membaca dari sensor DHT!");
    return;
  }

  // OLED display
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.print("Suhu: ");
  oled.setTextSize(2);
  oled.setCursor(0, 10);
  oled.print(t);
  oled.cp437(true);
  oled.write(167); // derajat
  oled.print("C");
  oled.setTextSize(1);
  oled.setCursor(0, 35);
  oled.print("Kelembaban: ");
  oled.setTextSize(2);
  oled.setCursor(0, 45);
  oled.print(h);
  oled.print(" %");
  oled.display();

  // Fan logic
  if (t >= MaxLevel) {
    fan_status = true;
    fan_on(HIGH);
  } else if (t < MinLevel) {
    fan_status = false;
    fan_on(LOW);
  }

  digitalWrite(led_pin, led_status);
  led_status = !led_status;
  // Debug
  Serial.print("Suhu: ");
  Serial.print(t);
  Serial.print(" C, Kelembaban: ");
  Serial.print(h);
  Serial.println(" %");
  // Blynk Virtual Pins
  Blynk.virtualWrite(V0, t); // suhu
  Blynk.virtualWrite(V1, h); // kelembaban
  Blynk.virtualWrite(V2, fan_status); // status kipas
}

void setup() {
  Serial.begin(115200);
  pinMode(fan_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  dht.begin();
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED gagal dimulai"));
    while (1);
  }
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.print("Menghubungkan...");
  oled.display();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}
void loop() {
  Blynk.run();
  if (millis() - prev > refresh_rate) {
    read_send_Data();
    prev = millis();
  }
}
