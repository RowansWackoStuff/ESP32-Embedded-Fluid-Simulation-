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

  initialiseSand();
}

int ballY = 120;
int ballX = 120;

// can do distance from the center to stop from going outta bounds
 
void loop(void) {
  // ball();
  sand();

}

const int sandAmount = 900;
int sandX[sandAmount];
int sandY[sandAmount];

int sandGrid[240][240];

void initialiseSand(){
  for(int i = 0; i < sandAmount; i++){
    sandX[i] = random(0, 240);
    sandY[i] = random(0, 240);

    tft.drawPixel(sandX[i], sandY[i], GC9A01A_RED);

    sandGrid[sandX[i]][sandY[i]] = 1;
  }


}

void sand(){
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  // --- 1. Basic Accelerometer-based Roll/Pitch ---
  // (In a real application, you would use a Sensor Fusion algorithm here)
  float roll = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
  float pitch = atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180 / PI;

  for(int i = 0; i < sandAmount; i++){

    tft.drawPixel(sandX[i], sandY[i], GC9A01A_BLACK);
    sandGrid[sandX[i]][sandY[i]] = 0;

    int newX = sandX[i] + round(roll * 0.1);
    int newY = sandY[i] - round(pitch * 0.1);

    // sandX[i] += round(roll * 0.1);
    // sandY[i] -= round(pitch * 0.1);

    // keep sand in screen
    int centerX = 120;
    int centerY = 120;
    int maxRadius = 120;

    float dx = newX - centerX;
    float dy = newY - centerY;
    float dist = sqrt(dx*dx + dy*dy);

    if (dist > maxRadius) {
        float scale = maxRadius / dist;
        newX = centerX + dx * scale;
        newY = centerY + dy * scale;
    }

    bool collide = false;

    // only check if the spots around it are full?

    // for(int j = 0; j < sandAmount; j++){
    //   if(newX == sandX[j] && newY == sandY[j]){
    //     collide = true;
    //     break;
    //   }
    // }

    if(sandGrid[newX][newY] == 1){
      collide = true;
    }

    if(collide == false){
      sandX[i] = newX;
      sandY[i] = newY;
    }
    sandGrid[sandX[i]][sandY[i]] = 1;

    tft.drawPixel(sandX[i], sandY[i], GC9A01A_RED);

    
  }

  // delay(10);

  // update position
  // ballX += round(roll * 0.1);
  // ballY -= round(pitch * 0.1);
}

void ball(){
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
  


  
  // dont want it in a loop as it will keep reseting
  // tft.fillScreen(GC9A01A_BLACK);
  // tft.fillCircle(ballX, ballY, 20, GC9A01A_RED);

  double distance = round(sqrt(pow(ballX - 120, 2) + pow(ballY - 120,2)));
  Serial.println("ballX: ");
  Serial.print(ballX);
  Serial.println("ballY: ");
  Serial.print(ballY);

  tft.fillCircle(ballX, ballY, 20, GC9A01A_BLACK);

  // update position
  ballX += round(roll * 0.1);
  ballY -= round(pitch * 0.1);
  
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


  delay(10);
}
