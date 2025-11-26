#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  Wire.begin(0, 5); // Specify SDA (4) and SCL (5) for ESP32-C3

  if (!mpu.begin()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }
}

void loop() {
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

  delay(100);
}