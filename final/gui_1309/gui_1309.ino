#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>

#define CS 5
#define DC 16
#define RST 17

# VCC -- 3.3V, SCLK -- 18, MOSI/DIN -- 23

U8G2_SSD1309_128X64_NONAME2_F_4W_HW_SPI oled(U8G2_R0, CS, DC, RST);

float bgl[25] = {
  8.8, 8.4, 7.9, 7.2, 6.4,
  6.8, 7.5, 8.1, 8.7, 9.2,
  10.2, 9.8, 9.1, 8.5, 7.8,
  7.2, 6.9, 7.4, 8.0, 8.8,
  9.5, 10.1, 9.6, 8.9, 9.2
};

unsigned long lastUpdate = 0;
int currentPoint = 24;

void drawGraph() {
  int gx = 0, gy = 12, gw = 84, gh = 40;
  oled.drawFrame(gx, gy, gw, gh);

  for (int i = 0; i < 24; i++) {
    int x1 = gx + 2 + i * 3.3;
    int x2 = gx + 2 + (i + 1) * 3.3;
    int y1 = gy + gh - 2 - (bgl[i] - 4.0) * 5.0;
    int y2 = gy + gh - 2 - (bgl[i + 1] - 4.0) * 5.0;
    y1 = constrain(y1, gy + 2, gy + gh - 2);
    y2 = constrain(y2, gy + 2, gy + gh - 2);
    oled.drawLine(x1, y1, x2, y2);
  }

  oled.setFont(u8g2_font_5x7_tr);
  oled.drawStr(0, 62, "0");
  oled.drawStr(20, 62, "30");
  oled.drawStr(40, 62, "60");
  oled.drawStr(60, 62, "90");
  oled.drawStr(75, 62, "120");
}

void drawGUI() {
  oled.clearBuffer();

  oled.setFont(u8g2_font_6x10_tr);
  oled.drawStr(2, 9, "BGL");

  drawGraph();

  oled.drawVLine(86, 0, 64);

  char value[8];
  dtostrf(bgl[currentPoint], 3, 1, value);

  oled.setFont(u8g2_font_logisoso24_tr);
  oled.drawStr(88, 29, value);

  oled.setFont(u8g2_font_6x10_tr);
  oled.drawStr(94, 41, "mmol/L");

  oled.drawCircle(92, 53, 3, U8G2_DRAW_ALL);
  oled.drawStr(100, 56, "OK");

  oled.sendBuffer();
}

void addNewBGL() {
  for (int i = 0; i < 24; i++) {
    bgl[i] = bgl[i + 1];
  }

  float change = random(-12, 13) / 10.0;
  bgl[24] += change;

  if (bgl[24] < 4.5) bgl[24] = 4.5;
  if (bgl[24] > 12.0) bgl[24] = 12.0;
}

void setup() {
  randomSeed(analogRead(34));
  oled.begin();
  drawGUI();
}

void loop() {
  if (millis() - lastUpdate >= 5000) {
    lastUpdate = millis();
    addNewBGL();
    drawGUI();
  }
}