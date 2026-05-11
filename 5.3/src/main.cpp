#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SDA_PIN 8
#define SCL_PIN 9

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64   // Cámbialo a 32 si tu OLED es de 128x32
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Muestra un mensaje centrado verticalmente con texto grande
void mostrarEnDisplay(String linea1, String linea2 = "", String linea3 = "") {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(linea1);

  if (linea2.length() > 0) {
    display.setCursor(0, 20);
    display.println(linea2);
  }

  if (linea3.length() > 0) {
    display.setCursor(0, 40);
    display.println(linea3);
  }

  display.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("ERROR: Fallo al inicializar SSD1306 en 0x3C");
    while (1);
  }

  // Pantalla de bienvenida
  mostrarEnDisplay("Display listo", "Escribe en el", "monitor serie...");
  
  Serial.println("=== Control de OLED por Serial ===");
  Serial.println("Escribe texto y pulsa Enter para mostrarlo.");
  Serial.println("Puedes separar líneas con '|'");
  Serial.println("Ejemplo:  Hola mundo|Linea 2|Linea 3");
  Serial.println("----------------------------------");
}

void loop() {
  if (Serial.available()) {
    String entrada = Serial.readStringUntil('\n');
    entrada.trim();

    if (entrada.length() == 0) return;

    // Separar por '|' para múltiples líneas
    String linea1 = "", linea2 = "", linea3 = "";
    int sep1 = entrada.indexOf('|');

    if (sep1 == -1) {
      linea1 = entrada;
    } else {
      linea1 = entrada.substring(0, sep1);
      String resto = entrada.substring(sep1 + 1);
      int sep2 = resto.indexOf('|');
      if (sep2 == -1) {
        linea2 = resto;
      } else {
        linea2 = resto.substring(0, sep2);
        linea3 = resto.substring(sep2 + 1);
      }
    }

    mostrarEnDisplay(linea1, linea2, linea3);

    Serial.print("✓ Mostrado en OLED: \"");
    Serial.print(entrada);
    Serial.println("\"");
  }
}