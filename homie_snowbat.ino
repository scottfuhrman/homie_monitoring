//
// 3 Sensor Snow Depth Sensor Array
// Copyright(c) Scott Fuhrman, 2016
//
// S1, S2, and S3 are HC - SR04 sensors
//                       Top View:
//
//                           S3
//                           ||
//                           ||
//                           ||
//                           ||
//                           ||
//                           ||
// S2 == == == == == == == = || == == == == == == == == == S1

#include <Homie.h>
#include <QuickStats.h>

#define sen1EchoPin D0 // Echo Pin
#define sen1TrigPin D1 // Trigger Pin
#define sen2EchoPin D5 // Echo Pin
#define sen2TrigPin D2 // Trigger Pin
#define sen3EchoPin D6 // Echo Pin
#define sen3TrigPin D4 // Trigger Pin

const int UPDATE_INTERVAL = 60;
unsigned long lastSnowDataSent = 0;
long sen1Duration, sen2Duration, sen3Duration;
float sen1Distance, sen2Distance, sen3Distance; // Duration used to calculate distance
float distance[3]; // array to hold values
float sensorHeightCm = 125; // sensor height above ground in cm
float snowDepthCm;
float snowDepthInches;

QuickStats stats;

HomieNode snowNode("snow", "snow");
HomieNode locationNode("location", "location");
HomieSetting<const char*> locStateSetting("locState", "Location: State");
HomieSetting<const char*> locCitySetting("locCity", "Location: State");
HomieSetting<const char*> locDomainSetting("locDomain", "Location: State");
HomieSetting<const char*> locAreaSetting("locArea", "Location: State");


void setupHandler() {
  //temperatureNode.setProperty("unit").send("f");
  locationNode.setProperty("state").send(locStateSetting.get());
  locationNode.setProperty("city").send(locCitySetting.get());
  locationNode.setProperty("domain").send(locDomainSetting.get());
  locationNode.setProperty("area").send(locAreaSetting.get());


}
void checkSensors() {
  digitalWrite(sen1TrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(sen1TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sen1TrigPin, LOW);
  sen1Duration = pulseIn(sen1EchoPin, HIGH);
  //Calculate the distance (in cm) based on the speed of sound.
  sen1Distance = sen1Duration / 58.2;
  distance[0] = sen1Distance;

  digitalWrite(sen2TrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(sen2TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sen2TrigPin, LOW);
  sen2Duration = pulseIn(sen2EchoPin, HIGH);
  //Calculate the distance (in cm) based on the speed of sound.
  sen2Distance = sen2Duration / 58.2;
  distance[1] = sen2Distance;

  digitalWrite(sen3TrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(sen3TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sen3TrigPin, LOW);
  sen3Duration = pulseIn(sen3EchoPin, HIGH);
  //Calculate the distance (in cm) based on the speed of sound.
  sen3Distance = sen3Duration / 58.2;
  distance[2] = sen3Distance;

  snowDepthCm = (sensorHeightCm - stats.median(distance, 3));
  snowDepthInches = snowDepthCm / 2.54;
}

void loopHandler() {
  if (millis() - lastSnowDataSent >= UPDATE_INTERVAL * 1000UL || lastSnowDataSent == 0) {
    checkSensors();

    Homie.getLogger() << "sen1DistanceCm: " << sen1Distance << " cm" << endl;
    snowNode.setProperty("sen1DistanceCm").send(String(sen1Distance));
    Homie.getLogger() << "sen2DistanceCm: " << sen2Distance << " cm" << endl;
    snowNode.setProperty("sen2DistanceCm").send(String(sen2Distance));
    Homie.getLogger() << "sen3DistanceCm: " << sen3Distance << " cm" << endl;
    snowNode.setProperty("sen3DistanceCm").send(String(sen3Distance));
    Homie.getLogger() << "avgDistanceCm: " << stats.average(distance, 3) << " cm" << endl;
    snowNode.setProperty("avgDistanceCm").send(String(stats.average(distance, 3)));
    Homie.getLogger() << "medDistanceCm: " << stats.median(distance, 3) << " cm" << endl;
    snowNode.setProperty("medDistanceCm").send(String(stats.median(distance, 3)));
    Homie.getLogger() << "snowDepthInches: " << snowDepthInches << " inches" << endl;
    snowNode.setProperty("snowDepthInches").send(String(snowDepthInches)); 
    Homie.getLogger() << "sensorHeightCm: " << sensorHeightCm << " cm" << endl;
    snowNode.setProperty("sensorHeightCm").send(String(sensorHeightCm)); 
    Homie.getLogger() << "sensorStdDev: " << stats.stdev(distance, 3) << endl;
    snowNode.setProperty("sensorStdDev").send(String(stats.stdev(distance, 3)));     
    lastSnowDataSent = millis();

  }
}

void setup() {
  Serial.begin(115200);
  Serial << endl << endl;
  pinMode(sen1TrigPin, OUTPUT);
  pinMode(sen1EchoPin, INPUT);
  pinMode(sen2TrigPin, OUTPUT);
  pinMode(sen2EchoPin, INPUT);
  pinMode(sen3TrigPin, OUTPUT);
  pinMode(sen3EchoPin, INPUT);
  //Homie.setBrand("fuhrmlabs"); // before Homie.setup()
  locStateSetting.setDefaultValue("unknown");
  locCitySetting.setDefaultValue("unknown");
  locDomainSetting.setDefaultValue("unknown");
  locAreaSetting.setDefaultValue("unknown");

  Homie_setFirmware("snowbat", "1.0.0");
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);

  //temperatureNode.advertise("unit");
  //temperatureNode.advertise("degrees");
  locationNode.advertise("city");
  locationNode.advertise("state");
  locationNode.advertise("domain");
  locationNode.advertise("area");

  Homie.setup();
}

void loop() {
  Homie.loop();
}
