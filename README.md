# Ether.Labs.HA
A hopefully complete plant care System based on Ravensystems Homekit API



Ether.Aqua
Because of Accidents during Soldering this one is Based on an ESP8266 but will be revised as an ESP32 eventually if needed.
Will for now just Water the Plants in certain Timeframes depending on Ether.Soil data.



Ether.Soil
Based on ESP32 since a huge amount of Analog Pins is required. 
Measures Soil Moister to stop overwatering.

Ether.Solar
Based on ESP8266 since only a simple I2C interface is needed. Will also Serve as Power Distribution Unit for the other Devices.
Smart Dimmer for 0-10V Growlights.


Ether.Env
Coming Soon. ESP8266 based Environmewnt Climate Sensor, using a BME688 to ddetermine the general State of the Growing-Area
