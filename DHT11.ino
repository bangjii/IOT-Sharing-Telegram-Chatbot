#include "DHT.h"

#define DHTPIN 14
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)) {
    h = 0;
    t = 0;
  }  
  Serial.print(t);
  Serial.println("°C");
  Serial.print(h);
  Serial.println(%);
  delay(2000);
}
