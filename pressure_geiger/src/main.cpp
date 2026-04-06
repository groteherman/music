/*
BME280 connected to Pico I2C0
SDA - Pico GP4 pin 6
SCL - Pico GP5 pin 7
*/

#include <Arduino.h>
#include <Adafruit_BMP280.h>

#define ONE_MINUTE 60000

Adafruit_BMP280 bmp;
volatile unsigned counter;
pin_size_t gpioLed = 25;
unsigned long lastElapsedMillis = 0;

bool SendPayload() 
{
  digitalWrite(gpioLed, HIGH);
  float pressure = bmp.readPressure();
  if (pressure < 90000){
    //iets
  }
  String toSend = "{\n \"temperature\": {temperature},\n \"pressure\": {pressure},\n \"altitude\": {altitude}\n \"counter\": {counter}\n}";
  unsigned counterToSend = counter;
  counter = 0;
  
  toSend.replace("{temperature}", String(bmp.readTemperature(),2));
  toSend.replace("{pressure}", String(pressure/100,2));
  toSend.replace("{altitude}", String(bmp.readAltitude(1019.66),2));
  toSend.replace("{counter}", String(counterToSend));
  Serial.print(toSend);
  digitalWrite(gpioLed, LOW);
  return true;
}

void setup() {
  Serial.begin(9600);
  pinMode(gpioLed, OUTPUT);

  unsigned status;
  status = bmp.begin(0x76);
  while (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    status = bmp.begin(0x76);
    delay(500);
  }

  bmp.setSampling(
    Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void loop() {
  if (millis() < lastElapsedMillis){
    lastElapsedMillis = millis();
  }
  if (millis() - lastElapsedMillis > ONE_MINUTE) {
    lastElapsedMillis = millis();
    SendPayload();
  }
}
