/*
BME280 connected to NodeMCU
SDA - Nodemcu GPIO4 pin D2
SCL - Nodemcu GPIO5 pin D1
this appears to be the default for I2C

Geiger at Nodemcu GPIO14 pin D5
*/

#include <Arduino.h>
#include <Adafruit_BMP280.h>

#define ONE_MINUTE 60000
#define GPIO_LED 2  //pin D4
#define GPIO_GEIGER 14  //pin D5

Adafruit_BMP280 bmp;
volatile unsigned geigerCounter = 0;
unsigned long lastGeiger = 0;
unsigned long lastElapsedMillis = 0;
unsigned long currentMillis;

IRAM_ATTR void updateGeigerCounter(){
  geigerCounter++;
}

bool SendPayload(unsigned long currentMillis) 
{
  digitalWrite(GPIO_LED, LOW);
  float pressure = bmp.readPressure();
  if (pressure < 90000){
    //iets
  }
  //String payload = "{\"seconds\": {seconds},\"temperature\": {temperature},\"pressure\": {pressure},\"altitude\": {altitude},\"geigercounter\": {geigercounter},\"geigerdiff\": {geigerdiff}}";
  String payload = "{seconds},{temperature},{pressure},{geigercounter},{geigerdiff}\n";
  unsigned long counterToSend = geigerCounter;
  unsigned long geigerDiff = counterToSend - lastGeiger;
  lastGeiger = counterToSend;

  payload.replace("{seconds}", String(currentMillis/1000));
  payload.replace("{temperature}", String(bmp.readTemperature(),2));
  payload.replace("{pressure}", String(pressure/100,2));
  //payload.replace("{altitude}", String(bmp.readAltitude(1019.66),2));
  payload.replace("{geigercounter}", String(counterToSend));
  payload.replace("{geigerdiff}", String(geigerDiff));
  Serial.print(payload);
  digitalWrite(GPIO_LED, HIGH);
  return true;
}

void setup() {
  Serial.begin(9600);
  pinMode(GPIO_LED, OUTPUT);
  digitalWrite(GPIO_LED, HIGH);

  pinMode(GPIO_GEIGER, INPUT);

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
  
    attachInterrupt(digitalPinToInterrupt(GPIO_GEIGER), updateGeigerCounter, RISING);
}

void loop() {
  currentMillis = millis();
  if (lastElapsedMillis > currentMillis){
    lastElapsedMillis = 0;
  }
  if (currentMillis - lastElapsedMillis >= ONE_MINUTE){
    lastElapsedMillis = currentMillis;
    SendPayload(currentMillis);
  }
}
