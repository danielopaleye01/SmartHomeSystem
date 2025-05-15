#include <DHT.h>  // DHT11 sensor library

// --- Pin Definitions ---
#define DHTPIN         3
#define DHTTYPE        DHT11
#define FAN_PIN        5       // DC motor/fan via transistor
#define BUZZER_PIN     8       // Active buzzer
#define IR_PROX_PIN    7       // TCRT5000 sensor OUT
#define ALERT_LED      6       // LED that stays on after IR trigger

DHT dht(DHTPIN, DHTTYPE);

// --- State ---
bool ledLockedOn = false;

// Static timestamp for timing
static unsigned long measurement_timestamp = 0;

// Function to measure environment with timing control
static bool measure_environment(float *temperature, float *humidity) {
  if (millis() - measurement_timestamp > 3000ul) { // every 3 seconds
    *temperature = dht.readTemperature();
    *humidity = dht.readHumidity();
    measurement_timestamp = millis();

    if (!isnan(*temperature) && !isnan(*humidity)) {
      return true;
    }
  }
  return false;
}

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(FAN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(IR_PROX_PIN, INPUT);
  pinMode(ALERT_LED, OUTPUT);

  Serial.println("System Ready.");
}

void loop() {
  float temp = 0.0;
  float hum = 0.0;

  // --- Read Temp/Humidity and Control Fan ---
  if (measure_environment(&temp, &hum)) {
    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.print(" °C  |  Hum: ");
    Serial.print(hum);
    Serial.println(" %");

    if (temp >= 34.0) {
      digitalWrite(FAN_PIN, HIGH);
      Serial.println("Fan ON (Temp > 75°F).");
    } else {
      digitalWrite(FAN_PIN, LOW);
      Serial.println("Fan OFF (Temp < 75°F).");
    }
  }

  // --- TCRT5000 IR Sensor Triggers Buzzer & Locks LED ON ---
  if (digitalRead(IR_PROX_PIN) == LOW) {
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.println("Proximity detected - Buzzer ON.");

    if (!ledLockedOn) {
      digitalWrite(ALERT_LED, HIGH);
      ledLockedOn = true;
      Serial.println("LED latched ON.");
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(500);
}

