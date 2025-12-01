#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;
 
#define TFT_DC 2
#define TFT_CS 4
#define TFT_RST 5

#define TFT_SCK 18
#define TFT_MOSI 19
#define TFT_MISO 20
 
// Hardware SPI on Feather or other boards
Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);

float averageRoll = 0;
float averagePitch = 0;

float minRoll = 100;
float maxRoll = -100;

float minPitch = 100;
float maxPitch = -100;

 
void setup() {

  Serial.begin(9600);

  SPI.begin(TFT_SCK, TFT_MISO, TFT_MOSI);

  Wire.begin(1, 3); // Specify SDA (0) and SCL (5) for ESP32-C3

  Serial.println("1");

  // test mpu innitalisation is functioning
  if (!mpu.begin()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }

  Serial.println("2");

  // establish TFT communication
  tft.begin();

  // reset the screen
  tft.fillScreen(GC9A01A_BLACK);

  initialiseSand();

  Serial.println("3");
}
void loop(void) {
  sand();
  // ball();

}

const int sandAmount = 4000;
int sandX[sandAmount];
int sandY[sandAmount];

int sandGrid[240][240];

void initialiseSand(){
  // create sand particles
  for(int i = 0; i < sandAmount; i++){
    bool avaliable = false;

    // ensure that sand isnt placed ontop of eachother
    while(avaliable == false){
      sandX[i] = random(0, 240);
      sandY[i] = random(0, 240);

      if(sandGrid[sandX[i]][sandY[i]] == 0){
        sandGrid[sandX[i]][sandY[i]] = 1;
        avaliable = true;
      }
    }
    // draw particle onto the screen
    tft.drawPixel(sandX[i], sandY[i], GC9A01A_RED);
  }
}

// !!! TODO
// sand collsion is causing the sand to fall slowly (maybe?)
// sand also keeps getting stuck

// change the sensativity to 0 (done)
// create better collision
// remove drifting

void sand(){

  // creates 3 varaibles to hold the acceleration, gyroscope and tempreture data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  // get roll and pitch of the gyroscope
  // EXPLAIN HOW IT WORKS
  float roll = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
  float pitch = atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180 / PI;

  Serial.println("roll: ");
  Serial.print(roll);
  Serial.print(" pitch: ");
  Serial.print(pitch);

  // move all sand particles
  for(int i = 0; i < sandAmount; i++){

    // remove prior sand particle
    tft.drawPixel(sandX[i], sandY[i], GC9A01A_BLACK);
    sandGrid[sandX[i]][sandY[i]] = 0;

    // get new sand particle position
    int newX = sandX[i] + round(roll * 1); // 0.1
    int newY = sandY[i] - round(pitch * 1); // 0.1

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

    // check if sand is colliding
    bool collide = false;
    if(sandGrid[newX][newY] == 0){
      sandX[i] = newX;
      sandY[i] = newY;
    }

    // draw sand particle in new position
    sandGrid[sandX[i]][sandY[i]] = 1;
    tft.drawPixel(sandX[i], sandY[i], GC9A01A_RED);

    
  }

  // delay(10);
}

int ballX = 120;
int ballY = 120;

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
