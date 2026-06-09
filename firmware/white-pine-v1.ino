#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pins
const uint8_t OLED_SCL_PIN = D1;
const uint8_t OLED_SDA_PIN = D2;
const uint8_t BUTTON_PIN   = D5;

// Timing
const unsigned long DEBOUNCE_MS = 35;
const unsigned long LONG_PRESS_MS = 800;
const unsigned long STARTUP_COUNT_SCREEN_MS = 3000;
const unsigned long LONG_PRESS_SCREEN_MS = 5000;
const unsigned long SPLASH_TIME_MS = 2200;

// EEPROM
const int EEPROM_SIZE = 32;
const int MAGIC_ADDR = 0;
const int LIFE_ADDR  = 4;
const uint32_t EEPROM_MAGIC = 0x50535442; // "PSTB"

// Counts
uint32_t currentCount = 0;
uint32_t lifetimeCount = 0;

// Button state
bool buttonWasDown = false;
unsigned long buttonDownTime = 0;
bool longPressTriggered = false;

void oledOn() {
  display.ssd1306_command(SSD1306_DISPLAYON);
}

void oledOff() {
  display.ssd1306_command(SSD1306_DISPLAYOFF);
}

void saveLifetime() {
  EEPROM.put(LIFE_ADDR, lifetimeCount);
  EEPROM.commit();
}

void loadLifetime() {
  uint32_t magic = 0;
  EEPROM.get(MAGIC_ADDR, magic);

  if (magic != EEPROM_MAGIC) {
    lifetimeCount = 0;
    EEPROM.put(MAGIC_ADDR, EEPROM_MAGIC);
    EEPROM.put(LIFE_ADDR, lifetimeCount);
    EEPROM.commit();
    return;
  }

  EEPROM.get(LIFE_ADDR, lifetimeCount);

  if (lifetimeCount > 1000000UL) {
    lifetimeCount = 0;
    saveLifetime();
  }
}

void drawFiveNeedleFascicle(int cx, int cy) {
  int bx = cx;
  int by = cy + 14;

  // EXACTLY FIVE NEEDLES
  display.drawLine(bx, by, cx - 30, cy - 3, SSD1306_WHITE);
  display.drawLine(bx, by, cx - 15, cy - 16, SSD1306_WHITE);
  display.drawLine(bx, by, cx,      cy - 22, SSD1306_WHITE);
  display.drawLine(bx, by, cx + 15, cy - 16, SSD1306_WHITE);
  display.drawLine(bx, by, cx + 30, cy - 3, SSD1306_WHITE);

  display.fillCircle(bx, by, 2, SSD1306_WHITE);
}

void showCounts(unsigned long durationMs) {
  oledOn();
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(25, 2);
  display.print("PINUS STROBUS");

  display.drawLine(0, 15, 127, 15, SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(4, 25);
  display.print("CURRENT");

  display.setCursor(92, 25);
  display.print(currentCount);

  display.setCursor(4, 45);
  display.print("LIFETIME");

  display.setCursor(92, 45);
  display.print(lifetimeCount);

  display.display();
  delay(durationMs);
  oledOff();
}

void showSplash() {
  oledOn();
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(25, 2);
  display.print("PINUS STROBUS");

  drawFiveNeedleFascicle(64, 35);

  display.display();
  delay(SPLASH_TIME_MS);
}

void recordAttention() {
  currentCount++;
  lifetimeCount++;
  saveLifetime();
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(1);

  EEPROM.begin(EEPROM_SIZE);
  loadLifetime();

  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true) {
      delay(100);
    }
  }

  display.clearDisplay();
  display.display();

  showSplash();
  showCounts(STARTUP_COUNT_SCREEN_MS);
}

void loop() {
  bool buttonDown = (digitalRead(BUTTON_PIN) == LOW);

  if (buttonDown && !buttonWasDown) {
    delay(DEBOUNCE_MS);
    if (digitalRead(BUTTON_PIN) == LOW) {
      buttonWasDown = true;
      buttonDownTime = millis();
      longPressTriggered = false;
    }
  }

  if (buttonDown && buttonWasDown && !longPressTriggered) {
    if (millis() - buttonDownTime >= LONG_PRESS_MS) {
      longPressTriggered = true;
      showCounts(LONG_PRESS_SCREEN_MS);
    }
  }

  if (!buttonDown && buttonWasDown) {
    delay(DEBOUNCE_MS);
    if (digitalRead(BUTTON_PIN) == HIGH) {
      if (!longPressTriggered) {
        recordAttention();
      }

      buttonWasDown = false;
    }
  }

  delay(5);
}
