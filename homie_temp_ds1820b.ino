#include <Homie.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const int TEMPERATURE_INTERVAL = 60;
unsigned long lastTemperatureSent = 0;

HomieNode temperatureNode("temperature", "temperature");
HomieNode locationNode("location", "location");
HomieSetting<const char*> locStateSetting("locState", "Location: State");
HomieSetting<const char*> locCitySetting("locCity", "Location: State");
HomieSetting<const char*> locDomainSetting("locDomain", "Location: State");
HomieSetting<const char*> locAreaSetting("locArea", "Location: State");


void setupHandler() {
  temperatureNode.setProperty("unit").send("f");
  locationNode.setProperty("state").send(locStateSetting.get());
  locationNode.setProperty("city").send(locCitySetting.get());
  locationNode.setProperty("domain").send(locDomainSetting.get()); 
  locationNode.setProperty("area").send(locAreaSetting.get());  
 

}

void loopHandler() {
  if (millis() - lastTemperatureSent >= TEMPERATURE_INTERVAL * 1000UL || lastTemperatureSent == 0) {
    sensors.requestTemperatures();
    float t = sensors.getTempCByIndex(0);
    if (t == -127.00) {
      Serial.println("Error getting temperature");
    } else {
      float tempF = (DallasTemperature::toFahrenheit(t));
      Homie.getLogger() << "Temperature: " << tempF << " °F" << endl;
      temperatureNode.setProperty("degrees").send(String(tempF));
      lastTemperatureSent = millis();
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial << endl << endl;
  pinMode(ONE_WIRE_BUS, INPUT_PULLUP);
  sensors.begin();
  sensors.setResolution(12);

  //Homie.setBrand("fuhrmlabs"); // before Homie.setup()
  locStateSetting.setDefaultValue("unknown");
  locCitySetting.setDefaultValue("unknown");
  locDomainSetting.setDefaultValue("unknown");
  locAreaSetting.setDefaultValue("unknown");

  Homie_setFirmware("temperature", "1.1.0");
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);

  temperatureNode.advertise("unit");
  temperatureNode.advertise("degrees");
  locationNode.advertise("city");
  locationNode.advertise("state");
  locationNode.advertise("domain");
  locationNode.advertise("area");

  Homie.setup();
}

void loop() {
  Homie.loop();
}
