#include "DS18B20.h"
#include "OneWire.h"
DS18B20 ds(13);

struct TempSensor : Service::TemperatureSensor {     // A standalone Temperature sensor

  SpanCharacteristic *temp;                         // reference to the Current Temperature Characteristic
  
 TempSensor() : Service::TemperatureSensor(){       // constructor() method

    temp=new Characteristic::CurrentTemperature(-10.0);        // instantiate the Current Temperature Characteristic
    temp->setRange(-50,100);                                  // expand the range from the HAP default of 0-100 to -50 to 100 to allow for negative temperatures
        
    Serial.print("Configuring Temperature Sensor");           // initialization message
    Serial.print("\n");

  } // end constructor

  void loop(){

    if(temp->timeVal()>5000){   
      float temperature=ds.getTempC();              // "simulate" a half-degree temperature change...
      temp->setVal(temperature);                            // set the new temperature; this generates an Event Notification and also resets the elapsed time
      LOG1("Temperature Update: ");
      LOG1(temperature);
      LOG1("\n");            
    }
    
  } // loop
  
};