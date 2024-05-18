const int trigPin = 23;
const int echoPin = 22;

struct UtraSonicSensor : Service::HumidifierDehumidifier { 

  SpanCharacteristic *WaterLevel;                 
               
  UtraSonicSensor() : Service::HumidifierDehumidifier(){       // constructor() method
    
    WaterLevel = new Characteristic::WaterLevel(1);                         // instantiate the Air Quality Characteristic and set initial value to 1
    
    Serial.print("Configuring Ultrasonic Sensor");   // initialization message
    Serial.print("\n");

  } // end constructor

  void loop(){

    if(WaterLevel->timeVal()>10000) {
      digitalWrite(trigPin, LOW);
      delayMicroseconds(5);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      long duration = pulseIn(echoPin, HIGH);
      long distance = duration * 0.0343 / 2;
      WaterLevel->setVal(distance);
      }


  } // loop

};