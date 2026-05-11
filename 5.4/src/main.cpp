#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_AHTX0.h>

#define SDA_PIN        8
#define SCL_PIN        9
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_AHTX0   aht;

bool liveDot = false;

// ── Barra de progreso redondeada ──────────────────────────
void drawProgressBar(int x, int y, int w, int h, float pct) {
  display.drawRoundRect(x, y, w, h, 2, SSD1306_WHITE);
  int fill = constrain((int)((pct / 100.0f) * (w - 4)), 0, w - 4);
  if (fill > 0)
    display.fillRoundRect(x + 2, y + 2, fill, h - 4, 1, SSD1306_WHITE);
}

// ── Texto centrado en una columna ─────────────────────────
void printCentered(const char* str, int colX, int colW, int y, int textSize) {
  int charW = 6 * textSize;
  int strW  = strlen(str) * charW;
  display.setCursor(colX + (colW - strW) / 2, y);
  display.setTextSize(textSize);
  display.print(str);
}

// ── Pantalla principal ────────────────────────────────────
void drawUI(float temp, float hum) {
  display.clearDisplay();
  display.cp437(true);          // necesario para el símbolo °
  display.setTextColor(SSD1306_WHITE);

  // ── 1. Header ─────────────────────────────────────────
  display.fillRoundRect(0, 0, 128, 13, 3, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setTextSize(1);
  display.setCursor(28, 3);     // "AHT20 Sensor" = 12 chars × 6px = 72px → x=(128-72)/2
  display.print("AHT20 Sensor");
  // Punto "live" parpadeante en la esquina izquierda del header
  if (liveDot) display.fillCircle(6, 6, 3, SSD1306_BLACK);
  else         display.drawCircle(6, 6, 3, SSD1306_BLACK);

  // ── 2. Divisor vertical ───────────────────────────────
  display.setTextColor(SSD1306_WHITE);
  display.drawFastVLine(63, 13, 38, SSD1306_WHITE);

  // ── 3. Etiquetas de columna ───────────────────────────
  display.setTextSize(1);
  printCentered("TEMP", 0,  63, 16, 1);
  printCentered("HUM",  64, 64, 16, 1);

  // ── 4. Valores numéricos (tamaño 2 = 12×16px/char) ───
  char tempStr[8], humStr[8];
  // Siempre 4 caracteres para centrado consistente: " 5.3" o "23.4"
  sprintf(tempStr, "%4.1f", temp);
  sprintf(humStr,  "%4.1f", hum);

  display.setTextSize(2);
  printCentered(tempStr, 0,  63, 26, 2);
  printCentered(humStr,  64, 64, 26, 2);

  // ── 5. Unidades ───────────────────────────────────────
  display.setTextSize(1);
  // "°C" centrado en columna izquierda
  display.setCursor(22, 44);
  display.write(248);           // 248 = ° en CP437
  display.print("C");
  // "%" centrado en columna derecha
  printCentered("%", 64, 64, 44, 1);

  // ── 6. Separador horizontal ───────────────────────────
  display.drawFastHLine(0, 51, 128, SSD1306_WHITE);

  // ── 7. Barra de humedad ───────────────────────────────
  display.setTextSize(1);
  display.setCursor(1, 55);
  display.print("H");
  drawProgressBar(11, 54, 116, 9, hum);

  display.display();
}

// ── Pantalla de error ─────────────────────────────────────
void drawError(const char* msg) {
  display.clearDisplay();
  display.fillRoundRect(0, 0, 128, 13, 3, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setTextSize(1);
  display.setCursor(46, 3);
  display.print("ERROR");
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5, 25);
  display.print(msg);
  display.display();
}

// ── Pantalla de boot ──────────────────────────────────────
void drawBoot() {
  display.clearDisplay();
  display.fillScreen(SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setTextSize(2);
  display.setCursor(14, 16);
  display.print("AHT20");
  display.setTextSize(1);
  display.setCursor(26, 40);
  display.print("Iniciando...");
  display.display();
}

// ─────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  // Inicializar pantalla
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("ERROR: SSD1306 no encontrado en 0x3C");
    while (1);
  }
  drawBoot();

  // Inicializar sensor
  if (!aht.begin(&Wire)) {
    drawError("AHT20 no hallado");
    Serial.println("ERROR: AHT20 no encontrado en 0x38");
    while (1);
  }

  Serial.println("Sistema listo — AHT20 + SSD1306");
  delay(1500);
}

void loop() {
  sensors_event_t evtHum, evtTemp;
  aht.getEvent(&evtHum, &evtTemp);

  float t = evtTemp.temperature;
  float h = evtHum.relative_humidity;

  liveDot = !liveDot;
  drawUI(t, h);

  Serial.printf("Temp: %.1f°C  |  Humedad: %.1f%%\n", t, h);

  delay(2000);
}