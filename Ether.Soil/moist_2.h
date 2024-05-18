#define moist_2 14
#define wet_2 1049
#define dry_2 2559

/////////////////////////////////MOIST2/////////////////////////////////
struct Moist_2 : Service::HumiditySensor {     

  // reference to the Current Humidity Characteristic
  SpanCharacteristic *hum;                                  

  // constructor() method
  Moist_2() : Service::HumiditySensor() {      
                                   
    // instantiate the Current Temperature Characteristic
    hum = new Characteristic::CurrentRelativeHumidity(0);
    // expand the range to 30%-100% 
    hum->setRange(0, 100);                                

    // initialization message
    Serial.print("Configuring Humidity Sensor");           
    Serial.print("\n");

  } // end constructor

  void loop() {

    // the humidity refreshes every 10 seconds by the elapsed time
    if (hum->timeVal() > 10000) {
      // read humidity from sensor dht22
      float humidity = map(constrain(analogRead(moist_2),wet_2, dry_2), wet_2, dry_2, 100, 0);  
      // set the new humidity; this generates an Event Notification and also resets the elapsed time        
      hum->setVal(humidity);                            

      LOG1("Humidity Update: ");
      LOG1(humidity);
      LOG1(" ; ");
    }
  } // loop
};
