#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "spo2_algorithm.h"

MAX30105 particleSensor;

#define SDA_PIN 4
#define SCL_PIN 5

#define BUFFER_SIZE 100
uint32_t irBuffer[BUFFER_SIZE];
uint32_t redBuffer[BUFFER_SIZE];

int32_t spo2;
int8_t validSpO2;
int32_t heartRate;
int8_t validHeartRate;

void setup() {
  Serial.begin(115200);
  delay (200);
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("Không tìm thấy MAX30102");
    while (1);
  }

  Serial.println("Cảm biến sẵn sàng");

  particleSensor.setup(
    0x1F,  // LED brightness
    4,     // sample average
    2,     // mode: Red + IR
    100,   // sample rate
    411,   // pulse width
    4096   // ADC range
  );
}

void loop() {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    while (!particleSensor.available()) {
      particleSensor.check();
    }
    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();
  }

  
  maxim_heart_rate_and_oxygen_saturation(
    irBuffer,
    BUFFER_SIZE,
    redBuffer,
    &spo2,
    &validSpO2,
    &heartRate,
    &validHeartRate
  );

  Serial.print("Nhịp tim: ");
  if (validHeartRate)
    Serial.print(heartRate);
  else
    Serial.print("--");
  Serial.println(" BPM");

  Serial.print("SpO2: ");
  if (validSpO2)
    Serial.print(spo2);
  else
    Serial.print("--");
  Serial.println(" %");

  Serial.println("---------------------");
  delay(1000);
}
