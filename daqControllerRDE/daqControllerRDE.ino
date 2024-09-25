// RDE Firing

#include <SD.h>
#include <SPI.h>

const int triggerPin = 30;
const int fuelRunPin = 8;
const int oxidizerRunPin = 28;
const int igniterPin = 2;
const int oscilloscopePin = 6;
const int cameraPin = 9;
const int N2Valve1Pin = 1;
const int N2Valve2Pin = 7;
const int CO2ValvePin = 0;
const int analogPins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14};
const int numAnalogPins = sizeof(analogPins) / sizeof(analogPins[0]);

const int ignitionDelay = 100;   // Delay in milliseconds for ignition
const int pulseDelay = 20;           // Delay in milliseconds for pulse signals
const int experimentDuration = 1300; // Experiment duration in milliseconds
const int N2ValveDelay = 2000;       // Delay in milliseconds for opening N2 valves
const int CO2ValveDelay = 10000;     // Delay in milliseconds for opening CO2 valve
const char* filename = "datalog.csv"; // Name of the file to save data

void setup() {
  pinMode(fuelRunPin, OUTPUT);
  pinMode(oxidizerRunPin, OUTPUT);
  pinMode(igniterPin, OUTPUT);
  pinMode(oscilloscopePin, OUTPUT);
  pinMode(cameraPin, OUTPUT);
  pinMode(N2Valve1Pin, OUTPUT);
  pinMode(N2Valve2Pin, OUTPUT);
  pinMode(CO2ValvePin, OUTPUT);
  pinMode(30, INPUT_PULLUP); // Set pin 30 as input with internal pull-up resistor
  
  digitalWrite(fuelRunPin, LOW);
  digitalWrite(oxidizerRunPin, LOW);
  digitalWrite(igniterPin, LOW);
  digitalWrite(oscilloscopePin, LOW);
  digitalWrite(cameraPin, LOW);
  digitalWrite(N2Valve1Pin, LOW);
  digitalWrite(N2Valve2Pin, LOW);
  digitalWrite(CO2ValvePin, LOW);

  Serial.begin(9600);
  
  if (!SD.begin(BUILTIN_SDCARD)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  
  Serial.println("RDE Firing Code started.");
}

void loop() {
  // Wait for system triggerto go high
  if (digitalRead(triggerPin) == HIGH) {
  
  // Actuate fuel and oxidizer run lines
  digitalWrite(fuelRunPin, HIGH);
  digitalWrite(oxidizerRunPin, HIGH);
  
  delay(ignitionDelay);
  
  // Trigger igniter, oscilloscope, and camera
  digitalWrite(oscilloscopePin, HIGH);
  digitalWrite(cameraPin, HIGH);
  digitalWrite(igniterPin, HIGH);
  
  //delay(pulseDelay);  // Need to test if the system works leaving the triggers high for the experiment duration, so that the DAQ is not delayed.
  
  //digitalWrite(igniterPin, LOW);
  //digitalWrite(oscilloscopePin, LOW);
  //digitalWrite(cameraPin, LOW);
  
  // Record and print data from analog pins during experiment duration
  unsigned long startTime = millis();
  while (millis() - startTime < experimentDuration) {
    for (int i = 0; i < numAnalogPins; i++) {
      int value = analogRead(analogPins[i]);
      Serial.print("Analog Pin ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(value);
    }
  }
  
  // Close fuel and oxidizer run lines
  digitalWrite(fuelRunPin, LOW);
  digitalWrite(oxidizerRunPin, LOW);

  // Open N2 valves
  digitalWrite(N2Valve1Pin, HIGH);
  digitalWrite(N2Valve2Pin, HIGH);
  
  delay(N2ValveDelay);
  
  // Close N2 valves
  digitalWrite(N2Valve1Pin, LOW);
  digitalWrite(N2Valve2Pin, LOW);
  
  // Open CO2 valve
  digitalWrite(CO2ValvePin, HIGH);
  
  delay(CO2ValveDelay);
  
  // Close CO2 valve
  digitalWrite(CO2ValvePin, LOW);

  // Save the data to SD card
  File dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    while (Serial.available()) {
      char c = Serial.read();
      dataFile.write(c);
    }
    dataFile.close();
  }
  
  delay(10000); // Delay at the end
}
}




