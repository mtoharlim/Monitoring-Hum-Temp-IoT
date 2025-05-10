# Monitoring-Hum-Temp-IoT

Tugas 1 - Monitoring sensor DHT11 (Temp & Humidity)
Tugas 2 - Add Fan on & off for condition
Tugas 3 - Add sensor cahaya & udara

# Arduino Project: Environmental Monitoring System

This project monitors temperature, humidity, light levels, and air quality, controlling a fan based on temperature.

## Task 1: Monitoring DHT11 Sensor (Temperature & Humidity)

**Objective:** Read temperature and humidity data from a DHT11 sensor and display it.

**Code Points:**

1.  **Include the DHT library:** You'll need a library to easily interface with the DHT11 sensor. The most common one is `DHT.h`.
    ```arduino
    #include "DHT.h"
    ```

2.  **Define DHT pin and sensor type:** Specify the Arduino pin connected to the DHT11 data pin and define the sensor type.
    ```arduino
    #define DHTPIN 2     // Digital pin connected to the DHT data pin
    #define DHTTYPE DHT11   // DHT 11
    ```

3.  **Create DHT object:** Instantiate a DHT object.
    ```arduino
    DHT dht(DHTPIN, DHTTYPE);
    ```

4.  **Initialize DHT sensor in `setup()`:** Start the DHT sensor.
    ```arduino
    void setup() {
      Serial.begin(9600);
      Serial.println(F("DHT11 Sensor Reading"));
      dht.begin();
    }
    ```

5.  **Read temperature and humidity in `loop()`:** Read the data at regular intervals.
    ```arduino
    void loop() {
      delay(2000); // Wait a few seconds between readings

      float h = dht.readHumidity();
      float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

      // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
      }

      Serial.print(F("Humidity: "));
      Serial.print(h);
      Serial.print(F("%  Temperature: "));
      Serial.print(t);
      Serial.println(F("°C")); // Or "°F" if reading Fahrenheit
    }
    ```

## Task 2: Add Fan Control Based on Temperature

**Objective:** Control a fan to turn on if the temperature exceeds a certain threshold and turn off when it drops.

**Code Points (Adding to Task 1):**

1.  **Define fan pin and temperature threshold:** Specify the Arduino pin connected to the fan (via a transistor and relay if needed) and set the temperature threshold.
    ```arduino
    #define FANPIN 3      // Digital pin connected to the fan control
    #define TEMP_THRESHOLD 30.0 // Temperature in Celsius to turn on the fan
    ```

2.  **Set fan pin as output in `setup()`:** Configure the fan control pin as an output.
    ```arduino
    void setup() {
      // ... (previous setup code) ...
      pinMode(FANPIN, OUTPUT);
      digitalWrite(FANPIN, LOW); // Initially turn the fan off
    }
    ```

3.  **Control fan based on temperature in `loop()`:** Check the temperature and control the fan.
    ```arduino
    void loop() {
      // ... (previous DHT reading code) ...

      if (t > TEMP_THRESHOLD) {
        Serial.println(F("Temperature above threshold! Turning fan ON."));
        digitalWrite(FANPIN, HIGH); // Turn the fan ON (adjust HIGH/LOW based on your circuit)
      } else {
        Serial.println(F("Temperature below threshold. Turning fan OFF."));
        digitalWrite(FANPIN, LOW);  // Turn the fan OFF
      }
    }
    ```

## Task 3: Add Light and Air Quality Sensors

**Objective:** Integrate readings from a light sensor (e.g., LDR) and an air quality sensor (e.g., MQ-135).

**Code Points (Adding to Task 2):**

1.  **Define sensor pins:** Specify the analog pins connected to the light and air quality sensors.
    ```arduino
    #define LIGHT_SENSOR_PIN A0 // Analog pin connected to the light sensor
    #define AIR_SENSOR_PIN A1   // Analog pin connected to the air quality sensor
    ```

2.  **Read light sensor value in `loop()`:** Read the analog value from the light sensor.
    ```arduino
    void loop() {
      // ... (previous DHT reading and fan control code) ...

      int lightValue = analogRead(LIGHT_SENSOR_PIN);
      Serial.print(F("Light Level: "));
      Serial.println(lightValue);
    }
    ```

3.  **Read air quality sensor value in `loop()`:** Read the analog value from the air quality sensor. Note that MQ-series sensors often require calibration and specific formulas to get meaningful gas concentration readings. This example just shows the raw analog reading.
    ```arduino
    void loop() {
      // ... (previous DHT reading, fan control, and light sensor code) ...

      int airQualityValue = analogRead(AIR_SENSOR_PIN);
      Serial.print(F("Air Quality: "));
      Serial.println(airQualityValue);
    }
    ```

**Further Considerations:**

* **Fan Circuit:** Remember that controlling a motor like a fan usually requires a transistor and potentially a relay to handle the current and voltage. Connect the Arduino control pin to the base of the transistor.
* **MQ-135 Calibration:** The MQ-135 sensor requires a warm-up period and calibration to provide accurate readings of specific gases. You'll likely need to research calibration procedures and potentially implement a calibration routine in your code.
* **Light Sensor Interpretation:** The analog value from an LDR will vary depending on the light intensity. You might need to experiment with different lighting conditions to understand the range of values and potentially map them to a more understandable scale.
* **Libraries:** Ensure you have the `DHT.h` library installed in your Arduino IDE (Sketch > Include Library > Manage Libraries...).
* **Error Handling:** The DHT sensor reading includes basic error checking (`isnan()`). You might want to add more robust error handling for other sensors as well.
* **Data Logging/Display:** Consider how you want to visualize or log the sensor data (e.g., using the Serial Monitor, an LCD display, or sending data to a computer or online platform).

This outline should provide a good starting point for your Arduino project. Let me know if you have any specific questions about any of these steps!
