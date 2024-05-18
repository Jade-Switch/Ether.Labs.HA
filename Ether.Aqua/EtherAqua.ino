#include "HomeSpan.h"
#include "Pumps.h"
#include "Thermo.h"
#include "WaterLevel.h"




void setup() {
  Serial.begin(115200);
  homeSpan.setPairingCode("40324032");
  homeSpan.setQRID("403-24-032");
  homeSpan.begin(Category::Bridges,"Ether.Aqua");

  new SpanAccessory();  
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 

  new SpanAccessory();                                  
    new Service::AccessoryInformation();  
      new Characteristic::Identify();                           
    new Sprinkler(9);
      
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Temp Sensor");
    new TempSensor();                                                               

  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("WaterLevel");  
    new UtraSonicSensor();                                                          

} 

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();
  
} // end of loop()

//////////////////////////////////////
