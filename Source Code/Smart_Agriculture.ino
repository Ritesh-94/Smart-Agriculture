#define BLYNK_TEMPLATE_ID "TMPL3MOZSCrz8"
#define BLYNK_TEMPLATE_NAME "Test1"
#define BLYNK_AUTH_TOKEN "huY6L1YB_ONem852APCy2pAKdbUk6r-2"

//Include the library files
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include "NewPing.h"

char auth[] = BLYNK_AUTH_TOKEN;  //Enter your Blynk Auth token
char ssid[] = "Galaxy M33 5G";  //Enter your WIFI SSID
char pass[] = "motorola";      //Enter your WIFI Password

DHT dht(15, DHT11);//(DHT sensor pin,sensor type)  D4 DHT11 Temperature Sensor
BlynkTimer timer;

//Define component pins
#define soil A0     //A0 Soil Moisture Sensor

//void checkPhysicalButton();
int relay1State = LOW;
int automationState = LOW;
//long duration;
long distance;
#define RELAY_PIN_1       0   //D3 Relay
#define VPIN_BUTTON_1     V3
#define VPIN_AUTOMATION   V4
#define trigPin 14
#define echoPin 12
#define Max_dist 100
NewPing sonar(trigPin, echoPin, Max_dist);


void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(115200);
  pinMode(RELAY_PIN_1, OUTPUT);
  digitalWrite(RELAY_PIN_1, LOW);
  digitalWrite(RELAY_PIN_1, relay1State);

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  dht.begin();

  timer.setInterval(100L, soilMoistureSensor);
  timer.setInterval(100L, DHT11sensor);
  timer.setInterval(100L, waterLevel);
}

void waterLevel() {
  Serial.print("Distance = ");
  distance = sonar.ping_cm();
  Serial.print(sonar.ping_cm());
  Serial.println(" cm");
  Blynk.virtualWrite(V5, distance);
  delay(2000);
}

//Get the DHT11 sensor values
void DHT11sensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, h);

}

int value=0, temp=0;
//Get the soil moisture values
void soilMoistureSensor() {
  value = analogRead(soil);
  value = map(value, 0, 1024, 0, 100);
  value = (value - 100) * -1;
  Blynk.virtualWrite(V0, value);
  if(temp != value){
    Serial.print("Moisture:");
    Serial.print(value);
    Serial.print("%");
    Serial.println();
    temp = value;
  }
    if(automationState == HIGH){
      if(value > 60) {
        Blynk.virtualWrite(VPIN_BUTTON_1, 0);
      } else {
        Blynk.virtualWrite(VPIN_BUTTON_1, 1);
      }
    }
}

BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(VPIN_BUTTON_1);
  Blynk.syncVirtual(VPIN_AUTOMATION);
}

BLYNK_WRITE(VPIN_BUTTON_1) {
  relay1State = param.asInt();
  digitalWrite(RELAY_PIN_1, relay1State);
  Serial.println(relay1State);
}
BLYNK_WRITE(VPIN_AUTOMATION) {
  automationState = param.asInt();
  Serial.println(automationState);
}

void loop() {
  Blynk.run();//Run the Blynk library
  timer.run();//Run the Blynk timer
}

