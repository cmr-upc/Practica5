#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>

// Pines I2C para ESP32-S3 (ajusta si tu placa es diferente)
#define SDA_PIN 8
#define SCL_PIN 9

Adafruit_AHTX0 aht;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println("=== Lector AHT20 (0x38) ===");

  if (!aht.begin(&Wire)) {
    Serial.println("ERROR: No se encontró el sensor AHT20 en 0x38");
    Serial.println("Verifica el cableado y los pines SDA/SCL");
    while (1) delay(500);
  }

  Serial.println("Sensor AHT20 encontrado correctamente.");
  Serial.println("Formato: Temperatura (°C) | Humedad (%)");
  Serial.println("-------------------------------------------");
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  Serial.print("Temperatura: ");
  Serial.print(temp.temperature, 1);
  Serial.print(" °C  |  Humedad: ");
  Serial.print(humidity.relative_humidity, 1);
  Serial.println(" %");

  delay(2000); // Lectura cada 2 segundos
}