#define BLYNK_TEMPLATE_ID "TMPL6obEA1g6L"
#define BLYNK_TEMPLATE_NAME "Sensor dan Sinyal Pertemuan 10"
#define BLYNK_AUTH_TOKEN "qMrxHqtwtYPyz2D5K89uUN4Ntf0ZqFZu"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <HTTPClient.h>
// WiFi
char ssid[] = "Sukamara";
char pass[] = "sukamare2024";
// Telegram
String BOT_TOKEN = "8182004991:AAGqVI5GJEpnllijG0PLkiEGhjPRcfTfdxY";
String CHAT_ID = "1024390727";
// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
// DHT
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Sensor pin
#define FAN_PIN 5
#define LED_PIN 2
#define BUZZER_PIN 18
#define LIGHT_SENSOR_PIN 35      // LDR A0 ke GPIO35 (ADC)
#define MQ135_SENSOR_PIN 34      // MQ135 A0 ke GPIO34 (ADC)
// Threshold
#define MAX_TEMP 34
#define MIN_TEMP 27
#define ALERT_TEMP 35.0
#define BAD_AIR_QUALITY 1000
// Waktu sampling
unsigned long prev = 0;
#define REFRESH_RATE 5000
bool fan_status = false;
bool led_status = false;
bool was_alerted = false;

void fan_on(bool value) {
  digitalWrite(FAN_PIN, value);
}

void alert_on(bool state) {
  digitalWrite(BUZZER_PIN, state);
}

void sendTelegramAlert(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    message.replace(" ", "%20");
    message.replace("\n", "%0A");
    message.replace("°", "%C2%B0");
    message.replace(":", "%3A");
    String url = "https://api.telegram.org/bot" + BOT_TOKEN + "/sendMessage?chat_id=" + CHAT_ID + "&text=" + message;
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      String response = http.getString();
      Serial.println("Telegram terkirim: " + response);
    } else {
      Serial.print("Gagal kirim Telegram. HTTP code: ");
      Serial.println(httpCode);
    }
    http.end();
  } else {
    Serial.println("WiFi tidak terhubung saat mencoba kirim Telegram.");
  }
}

void read_send_Data() {
  float suhu = dht.readTemperature();
  int kelembaban = dht.readHumidity();
  int cahaya = analogRead(LIGHT_SENSOR_PIN);
  int udara = analogRead(MQ135_SENSOR_PIN);
  if (isnan(suhu) || isnan(kelembaban)) {
    Serial.println("Gagal membaca dari DHT11!");
    return;
  }
  // OLED
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setTextSize(1);
  oled.setCursor(0, 0); oled.print("Suhu: "); oled.print(suhu); oled.print("C");
  oled.setCursor(0, 10); oled.print("Lembab: "); oled.print(kelembaban); oled.print("%");
  oled.setCursor(0, 20); oled.print("Cahaya: "); oled.print(cahaya);
  oled.setCursor(0, 30); oled.print("Udara: "); oled.print(udara);
  oled.display();
  // Kipas
  if (suhu >= MAX_TEMP) {
    fan_status = true;
    fan_on(HIGH);
  } else if (suhu < MIN_TEMP) {
    fan_status = false;
    fan_on(LOW);
  }

  // Deteksi kondisi buruk
  bool kondisi_buruk = (suhu > ALERT_TEMP || udara > BAD_AIR_QUALITY);
  // Alert & Telegram
  if (kondisi_buruk && !was_alerted) {
    alert_on(true);
    sendTelegramAlert("⚠️ ALERT!\nSuhu: " + String(suhu) + "°C\nUdara Buruk: " + String(udara));
    was_alerted = true;
  } else if (!kondisi_buruk && was_alerted) {
    alert_on(false);
    sendTelegramAlert("✅ Kondisi kembali normal\nSuhu: " + String(suhu) + "°C\nUdara: " + String(udara));
    was_alerted = false;
  } else {
    alert_on(kondisi_buruk);
  }
  // LED berkedip
  digitalWrite(LED_PIN, led_status);
  led_status = !led_status;
  // Serial log
  Serial.print("Suhu: "); Serial.print(suhu);
  Serial.print(" C, Kelembaban: "); Serial.print(kelembaban);
  Serial.print(" %, Cahaya: "); Serial.print(cahaya);
  Serial.print(", Udara: "); Serial.println(udara);
  Blynk.virtualWrite(V0, suhu);
  Blynk.virtualWrite(V1, kelembaban);
  Blynk.virtualWrite(V2, fan_status);
  Blynk.virtualWrite(V3, cahaya);
  Blynk.virtualWrite(V4, udara);
}


void setup() {
  Serial.begin(115200);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  dht.begin();
  Wire.begin();
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED gagal dimulai"));
    while (1);  }
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.print("Menghubungkan WiFi...");
  oled.display();
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");  }
  Serial.println("Terhubung WiFi");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}
void loop() {
  Blynk.run();
  if (millis() - prev > REFRESH_RATE) {
    read_send_Data();
    prev = millis();
  }
}
