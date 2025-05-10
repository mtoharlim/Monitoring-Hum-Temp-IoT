#define BLYNK_TEMPLATE_ID "TMPL6obEA1g6L"
#define BLYNK_TEMPLATE_NAME "Sensor dan Sinyal Pertmuan 10"
#define BLYNK_AUTH_TOKEN "qMrxHqtwtYPyz2D5K89uUN4Ntf0ZqFZu"

#include <WiFi.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <BlynkSimpleEsp32.h>  // Ganti ini untuk ESP32

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

char ssid[] = "Sukamara";
char pass[] = "sukamare2024";

BlynkTimer timer;

void sendSensor() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(h) || isnan(t)) {
    Serial.println("Gagal membaca sensor DHT!");
    return;
  }
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
}

void setup() {
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht.begin();
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Gagal inisialisasi OLED"));
    while (1);
  }
  oled.clearDisplay();
  oled.setTextColor(WHITE);

  timer.setInterval(2000L, sendSensor);
}

void loop() {
  Blynk.run();
  timer.run();
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(h) || isnan(t)) {
    return;
  }
  oled.clearDisplay();
  // Suhu
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.print("Suhu: ");
  oled.setTextSize(2);
  oled.setCursor(0, 10);
  oled.print(t);
  oled.print(" ");
  oled.setTextSize(1);
  oled.cp437(true);
  oled.write(167);  // simbol derajat
  oled.setTextSize(2);
  oled.print("C");
  // Kelembaban
  oled.setTextSize(1);
  oled.setCursor(0, 35);
  oled.print("Kelembaban: ");
  oled.setTextSize(2);
  oled.setCursor(0, 45);
  oled.print(h);
  oled.print(" %");
  oled.display();
}
