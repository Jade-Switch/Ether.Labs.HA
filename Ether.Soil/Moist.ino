#include "HomeSpan.h"
#include "moist_1.h"
#include "moist_2.h"
#include "moist_3.h"
#include "moist_4.h"
#include "moist_5.h"
#include "moist_6.h"
//#include "hum.h"
//#include "temp.h"

void setup() {
  Serial.begin(115200);
  homeSpan.setPairingCode("40324032");
  homeSpan.setQRID("403-24-032");
  homeSpan.begin(Category::Bridges,"Ether.Soil");

  new SpanAccessory();  
    new Service::AccessoryInformation();
      new Characteristic::Identify();                                    

  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Moisture Sensor 1");  
    new Moist_1();

  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Moisture Sensor 2");  
    new Moist_2(); 
    
  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Moisture Sensor 3");  
    new Moist_3();         

  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Moisture Sensor 4");  
    new Moist_4();     

  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Moisture Sensor 5");  
    new Moist_5();                                            

  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Moisture Sensor 6");  
    new Moist_6(); 
} 

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();
  
} // end of loop()

//////////////////////////////////////
