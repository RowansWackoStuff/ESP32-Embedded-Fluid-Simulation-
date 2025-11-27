#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;
 
#define TFT_DC 2
#define TFT_CS 7
 
// Hardware SPI on Feather or other boards
Adafruit_GC9A01A tft(TFT_CS, TFT_DC);

float averageRoll = 0;
float averagePitch = 0;

float minRoll = 100;
float maxRoll = -100;

float minPitch = 100;
float maxPitch = -100;

 
void setup() {

  Serial.begin(9600);

  Wire.begin(1, 3); // Specify SDA (0) and SCL (5) for ESP32-C3

  if (!mpu.begin()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }

  
  Serial.println("GC9A01A Test!");
 
  tft.begin();

  sensors_event_t a, g, temp;
  
  for(int i = 0; i < 25; i++){
    mpu.getEvent(&a, &g, &temp);
    float roll = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
    float pitch = atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180 / PI;

    averageRoll += roll;
    averagePitch += pitch;

    if(pitch < minPitch){
      minPitch = pitch;
    }

    if(pitch > maxPitch){
      maxPitch = pitch;
    }

    if(roll < minRoll){
      minRoll = roll;
    }

    if(roll > maxRoll){
      maxRoll = roll;
    }
  }

  averageRoll /= 25;
  averagePitch /= 25;

  Serial.println(averageRoll);
  Serial.println(averagePitch);

  tft.fillScreen(GC9A01A_BLACK);
 /*
  Serial.println(F("Benchmark                Time (microseconds)"));
  delay(10);
  Serial.print(F("Screen fill              "));
  Serial.println(testFillScreen());
  delay(500);
 
  Serial.print(F("Text                     "));
  Serial.println(testText());
  delay(3000);
 
  Serial.print(F("Lines                    "));
  Serial.println(testLines(GC9A01A_CYAN));
  delay(500);
 
  Serial.print(F("Horiz/Vert Lines         "));
  Serial.println(testFastLines(GC9A01A_RED, GC9A01A_BLUE));
  delay(500);
 
  Serial.print(F("Rectangles (outline)     "));
  Serial.println(testRects(GC9A01A_GREEN));
  delay(500);
 
  Serial.print(F("Rectangles (filled)      "));
  Serial.println(testFilledRects(GC9A01A_YELLOW, GC9A01A_MAGENTA));
  delay(500);
 
  Serial.print(F("Circles (filled)         "));
  Serial.println(testFilledCircles(10, GC9A01A_MAGENTA));
 
  Serial.print(F("Circles (outline)        "));
  Serial.println(testCircles(10, GC9A01A_WHITE));
  delay(500);
 
  Serial.print(F("Triangles (outline)      "));
  Serial.println(testTriangles());
  delay(500);
 
  Serial.print(F("Triangles (filled)       "));
  Serial.println(testFilledTriangles());
  delay(500);
 
  Serial.print(F("Rounded rects (outline)  "));
  Serial.println(testRoundRects());
  delay(500);
 
  Serial.print(F("Rounded rects (filled)   "));
  Serial.println(testFilledRoundRects());
  delay(500);
 
  Serial.println(F("Done!"));
  */
}

int ballY = 120;
int ballX = 120;

// can do distance from the center to stop from going outta bounds
 
void loop(void) {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // --- 1. Basic Accelerometer-based Roll/Pitch ---
  // (In a real application, you would use a Sensor Fusion algorithm here)
  float roll = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
  float pitch = atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180 / PI;

  Serial.print("Roll: ");
  Serial.print(roll);
  Serial.print(" deg | Pitch: ");
  Serial.print(pitch);
  Serial.println(" deg");
  

  delay(50);

  
  // dont want it in a loop as it will keep reseting
  // tft.fillScreen(GC9A01A_BLACK);
  // tft.fillCircle(ballX, ballY, 20, GC9A01A_RED);

  double distance = round(sqrt(pow(ballX - 120, 2) + pow(ballY - 120,2)));
  Serial.println("ballX: ");
  Serial.print(ballX);
  Serial.println("ballY: ");
  Serial.print(ballY);

  tft.fillCircle(ballX, ballY, 20, GC9A01A_BLACK);

  // attempt at solving drifting
  // if(roll > maxRoll*2 || roll < minRoll*0.5){
  //   ballX += round(1 * roll);
  // }

  // if(pitch > maxPitch*2 || pitch < minPitch*0.5){
  //   ballY -= round(1 * pitch);
  // }


  // update position
  ballX += round(1 * roll);
  ballY -= round(1 * pitch);
  
  // ---- CLAMP INSIDE ROUND SCREEN ----
  int centerX = 120;
  int centerY = 120;
  int ballRadius = 20;
  int maxRadius = 120 - ballRadius;

  float dx = ballX - centerX;
  float dy = ballY - centerY;
  float dist = sqrt(dx*dx + dy*dy);

  if (dist > maxRadius) {
      float scale = maxRadius / dist;
      ballX = centerX + dx * scale;
      ballY = centerY + dy * scale;
  }
  // -----------------------------------

  // draw new ball
  tft.fillCircle(ballX, ballY, ballRadius, GC9A01A_RED);

  // position x positive is to the left
  // position y positive down 

  // roll positive as its goes down
  // roll negative as it goes up
  // pitch positive as it goes right
  // pitch negative as it goes left
  
}


 
unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(GC9A01A_BLACK);
  yield();
  tft.fillScreen(GC9A01A_RED);
  yield();
  tft.fillScreen(GC9A01A_GREEN);
  yield();
  tft.fillScreen(GC9A01A_BLUE);
  yield();
  tft.fillScreen(GC9A01A_BLACK);
  yield();
  return micros() - start;
}
 
unsigned long testText() {
  tft.fillScreen(GC9A01A_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(GC9A01A_WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(GC9A01A_YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(GC9A01A_RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(GC9A01A_GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
  return micros() - start;
}
 
unsigned long testLines(uint16_t color) {
  unsigned long start, t;
  int           x1, y1, x2, y2,
                w = tft.width(),
                h = tft.height();
 
  tft.fillScreen(GC9A01A_BLACK);
  yield();
 
  x1 = y1 = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t     = micros() - start; // fillScreen doesn't count against timing
 
  yield();
  tft.fillScreen(GC9A01A_BLACK);
  yield();
 
  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;
 
  yield();
  tft.fillScreen(GC9A01A_BLACK);
  yield();
 
  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;
 
  yield();
  tft.fillScreen(GC9A01A_BLACK);
  yield();
 
  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
 
  yield();
  return micros() - start;
}
 
unsigned long testFastLines(uint16_t color1, uint16_t color2) {
  unsigned long start;
  int           x, y, w = tft.width(), h = tft.height();
 
  tft.fillScreen(GC9A01A_BLACK);
  start = micros();
  for(y=0; y<h; y+=5) tft.drawFastHLine(0, y, w, color1);
  for(x=0; x<w; x+=5) tft.drawFastVLine(x, 0, h, color2);
 
  return micros() - start;
}
 
unsigned long testRects(uint16_t color) {
  unsigned long start;
  int           n, i, i2,
                cx = tft.width()  / 2,
                cy = tft.height() / 2;
 
  tft.fillScreen(GC9A01A_BLACK);
  n     = min(tft.width(), tft.height());
  start = micros();
  for(i=2; i<n; i+=6) {
    i2 = i / 2;
    tft.drawRect(cx-i2, cy-i2, i, i, color);
  }
 
  return micros() - start;
}
 
unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long start, t = 0;
  int           n, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;
 
  tft.fillScreen(GC9A01A_BLACK);
  n = min(tft.width(), tft.height());
  for(i=n; i>0; i-=6) {
    i2    = i / 2;
    start = micros();
    tft.fillRect(cx-i2, cy-i2, i, i, color1);
    t    += micros() - start;
    // Outlines are not included in timing results
    tft.drawRect(cx-i2, cy-i2, i, i, color2);
    yield();
  }
 
  return t;
}
 
unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;
 
  tft.fillScreen(GC9A01A_BLACK);
  start = micros();
  for(x=radius; x<w; x+=r2) {
    for(y=radius; y<h; y+=r2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
 
  return micros() - start;
}
 
unsigned long testCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int           x, y, r2 = radius * 2,
                w = tft.width()  + radius,
                h = tft.height() + radius;
 
  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros();
  for(x=0; x<w; x+=r2) {
    for(y=0; y<h; y+=r2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
 
  return micros() - start;
}
 
unsigned long testTriangles() {
  unsigned long start;
  int           n, i, cx = tft.width()  / 2 - 1,
                      cy = tft.height() / 2 - 1;
 
  tft.fillScreen(GC9A01A_BLACK);
  n     = min(cx, cy);
  start = micros();
  for(i=0; i<n; i+=5) {
    tft.drawTriangle(
      cx    , cy - i, // peak
      cx - i, cy + i, // bottom left
      cx + i, cy + i, // bottom right
      tft.color565(i, i, i));
  }
 
  return micros() - start;
}
 
unsigned long testFilledTriangles() {
  unsigned long start, t = 0;
  int           i, cx = tft.width()  / 2 - 1,
                   cy = tft.height() / 2 - 1;
 
  tft.fillScreen(GC9A01A_BLACK);
  start = micros();
  for(i=min(cx,cy); i>10; i-=5) {
    start = micros();
    tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      tft.color565(0, i*10, i*10));
    t += micros() - start;
    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      tft.color565(i*10, i*10, 0));
    yield();
  }
 
  return t;
}
 
unsigned long testRoundRects() {
  unsigned long start;
  int           w, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;
 
  tft.fillScreen(GC9A01A_BLACK);
  w     = min(tft.width(), tft.height());
  start = micros();
  for(i=0; i<w; i+=6) {
    i2 = i / 2;
    tft.drawRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(i, 0, 0));
  }
 
  return micros() - start;
}
 
unsigned long testFilledRoundRects() {
  unsigned long start;
  int           i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;
 
  tft.fillScreen(GC9A01A_BLACK);
  start = micros();
  for(i=min(tft.width(), tft.height()); i>20; i-=6) {
    i2 = i / 2;
    tft.fillRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(0, i, 0));
    yield();
  }
 
  return micros() - start;
}