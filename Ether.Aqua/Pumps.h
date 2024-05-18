#define  HEAD_DURATION    20     
#define  HEAD_SPEED       5000

#define PUMP1 12
#define PUMP2 27
#define PUMP3 21
#define PUMP4 32
#define PUMP5 34
#define PUMP6 14
#define PUMP7 26
#define PUMP8 33
#define PUMP9 35

struct Head : Service::Valve {

  SpanCharacteristic *active=new Characteristic::Active(0);
  SpanCharacteristic *inUse=new Characteristic::InUse(0);
  SpanCharacteristic *enabled = new Characteristic::IsConfigured(1,true);
  SpanCharacteristic *setDuration = new Characteristic::SetDuration(HEAD_DURATION);
  SpanCharacteristic *remainingDuration = new Characteristic::RemainingDuration(0);
  SpanCharacteristic *name;

  int relayPin;
  Head(const char *headName, int pin) : Service::Valve(), relayPin(pin) {
    new Characteristic::ValveType(1);                           // Set Valve Type = Irrigation
    name=new Characteristic::ConfiguredName(headName,true);     // This Characteristic was introduced for TV Services, but works well here
    enabled->addPerms(PW);                                      // Adding "PW" to the IsConfigured Characteristic allows for enabling/disabling valves
  }

  boolean update() override {
    
    if(enabled->updated()){
      if(enabled->getNewVal()){
        Serial.printf("%s value ENABLED\n",name->getString());
      } else {
        Serial.printf("%s value DISABLED\n",name->getString());          
        if(active->getVal()){
          active->setVal(0);
          remainingDuration->setVal(0);
          Serial.printf("%s is CLOSING\n",name->getString());          
        }
      }
    }

    if(active->updated()){
      if(active->getNewVal()){
        Serial.printf("%s valve is OPENING\n",name->getString());
        remainingDuration->setVal(setDuration->getVal());
      } else {
        Serial.printf("%s valve is CLOSING\n",name->getString());
        remainingDuration->setVal(0);
      }
    }
    
    return(true);
  }

  void loop() override {
    if(active->getVal()){
      int remainingTime=setDuration->getVal()-active->timeVal()/1000;
         
      if(remainingTime<=0){
        Serial.printf("%s valve is CLOSING (%d-second timer is complete)\n",name->getString(),setDuration->getVal());
        active->setVal(0);
        remainingDuration->setVal(0);
      } else

      if(remainingTime<remainingDuration->getVal()){
        remainingDuration->setVal(remainingTime,false);
      }
    }

    // Below we simulate valves that take some fixed time to open/close so that changes in InUse lags changes in Active.
    // The Home App will accurately reflect this intermediate state by displaying "Waiting..." when a value is initially opened.
    
    if(active->timeVal()>HEAD_SPEED && active->getVal()!=inUse->getVal()){
      inUse->setVal(active->getVal());
      Serial.printf("%s value is %s\n",name->getString(),inUse->getVal()?"OPEN":"CLOSED");
    }
  }

};

////////////////////////////////////////////////////////////////////////

struct Sprinkler : Service::IrrigationSystem {

  void setupRelays() {
  pinMode(PUMP1, OUTPUT);
  pinMode(PUMP2, OUTPUT);
  pinMode(PUMP3, OUTPUT);
  pinMode(PUMP4, OUTPUT);
  pinMode(PUMP5, OUTPUT);
  pinMode(PUMP6, OUTPUT);
  pinMode(PUMP7, OUTPUT);
  pinMode(PUMP8, OUTPUT);
  pinMode(PUMP9, OUTPUT);

  digitalWrite(PUMP1, LOW);
  digitalWrite(PUMP2, LOW);
  digitalWrite(PUMP3, LOW);
  digitalWrite(PUMP4, LOW);
  digitalWrite(PUMP5, LOW);
  digitalWrite(PUMP6, LOW);
  digitalWrite(PUMP7, LOW);
  digitalWrite(PUMP8, LOW);
  digitalWrite(PUMP9, LOW);
}
  // In this configuration we will LINK one or more Heads (i.e. Valve Services) to the Irrigation Service, INSTEAD of
  // having the Irrigation Service be a standalone Service with unlinked Valves.  This means that the Home App will
  // not display a separate "master" control for the Irrigation Service, and will instead automatically determine whether
  // the system is Active according to whether one more values are Active.

  // Also, because this configuration is based on LINKED Valves, DO NOT include Characteristic::InUse for the Irrigation Service

  SpanCharacteristic *programMode=new Characteristic::ProgramMode(0);   // HomeKit requires this Characteristic, but it is only for display purposes in the Home App
  SpanCharacteristic *active=new Characteristic::Active(0);             // though in this configuration the Home App will not display a "master" control, the Active Characteristic is still required
  
  vector<Head *> heads;                                                 // OPTIONAL: vector to store list of linked Heads that will be used for running a scheduled program
  vector<Head *>::iterator currentHead;                                 // OPTIONAL: pointer to the current head in a scheduled program

  Sprinkler(uint8_t numHeads) : Service::IrrigationSystem() {
  for (int i = 1; i <= numHeads; i++) {              // create Heads (Valves) and link each to the Sprinkler object
      char name[16];
      sprintf(name,"Head-%d",i);               // unique name for each Head that can be changed in the Home App
      addLink(new Head(name, PUMP1 + i - 1)); // PUMP1 + i - 1 gives the correct pin number for each head
    }
    
    for(auto s : getLinks())                  // OPTIONAL: add each linked Head into storage vector for easy access below
      heads.push_back((Head *)s);
      
    new SpanUserCommand('p', "- starts/stops scheduled program",startProgram,this);     // OPTIONAL: allows user to start a schedule program to sequentially turn on each enabled Head
  }

  static void startProgram(const char *buf, void *arg){     // OPTIONAL: start scheduled program
    
    Sprinkler *sprinkler=(Sprinkler *)arg;                      // recast the second arguments into a Sprinkler
        
    for(auto s : sprinkler->getLinks()) {                       // loop over all linked Services
      Head *head = (Head *)s;                                   // recast linked Service to a Head
      if(head->enabled->getVal() && head->active->getVal())     // if Head is both enabled and active
        head->active->setVal(0);                                // turn off Heads
        
    }

    sprinkler->currentHead=sprinkler->heads.begin();                       // reset current head in scheduled program
    sprinkler->active->setVal(0);                                          // set sprinkler Active to false
    //digitalWrite(head, LOW);
    sprinkler->programMode->setVal(!sprinkler->programMode->getVal());     // toggle Program Mode

    Serial.printf("Scheduled program: %s\n",sprinkler->programMode->getVal()?"STARTED":"STOPPED");
  }


  void loop(){                                              // OPTIONAL: only needed to support the running of scheduled programs
     
    if(!programMode->getVal())      // program mode not started
      return;

    if(currentHead==heads.end()){
      Serial.printf("Scheduled program: COMPLETED\n");
      programMode->setVal(0);
      active->setVal(0);
      return;
    }

    if(!(*currentHead)->enabled->getVal()){      // current Head is not enabled
      currentHead++;                             // advance to next Head
      return;
    }

    if((*currentHead)->active->getVal()){        // current Head is Active
      if(!active->getVal()){                     // if sprinkler Active is still false (because user manually turned on Head)...
        active->setVal(1);                       // ...set sprinkler Active to true
        Serial.printf("Scheduled program: %s is ALREADY OPEN\n",(*currentHead)->name->getString());
        
      }
      return;
      
    } else if((*currentHead)->inUse->getVal()){  // current Head is not Active but still InUse
      return;
    }
    
    if(!active->getVal()){                       // current Head is not Active nor InUse and sprinkler Active is false...
      active->setVal(1);                         // ...set sprinkler Active to true and turn on Head
      (*currentHead)->active->setVal(1);
      (*currentHead)->remainingDuration->setVal((*currentHead)->setDuration->getVal());
      Serial.printf("Scheduled program: %s is OPENING\n",(*currentHead)->name->getString());
      
    } else if(!(*currentHead)->inUse->getVal()){  // wait for water to stop flowing in current Head before moving to next Head
      active->setVal(0,false);
      currentHead++;      
    }

     if ((*currentHead)->active->getVal()) {
        if (!active->getVal()) {
          active->setVal(1); // Set sprinkler Active to true
          // Turn on the corresponding relay for the current active head
          digitalWrite((*currentHead)->relayPin, HIGH);
          Serial.printf("Scheduled program: %s is ALREADY OPEN\n", (*currentHead)->name->getString());
        }
        return;
      }
   

  } // loop()

};



