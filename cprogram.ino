//include libraries
#include <SoftwareSerial.h>
#include <Stepper.h>

SoftwareSerial esp8266(3, 2); //RX pin = 3, TX pin = 2

//definition of variables
#define DEBUG true //show messages between ESP8266 and Arduino in serial port
const int stepsPerRevolution = 500;
int state = 5;

Stepper rightStepper(stepsPerRevolution, 8,10,9,11);
Stepper leftStepper(stepsPerRevolution, 4,6,5,7);

//*****
//SETUP
//*****
void setup()
{
  //start communication
  Serial.begin(9600);
  esp8266.begin(19200);
  
  sendData("AT+RST\r\n", 2000, DEBUG); //reset module
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG); //set station mode
  sendData("AT+CWJAP=\"Thaiane\",\"1391162683\"\r\n", 2000, DEBUG); //connect wi-fi network
  while(!esp8266.find("OK")) { //wait for connection
  } 
  sendData("AT+CIFSR\r\n", 1000, DEBUG); //show IP address
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG); //allow multiple connections
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG); // start web server on port 80
  
  //Define motor speed
  rightStepper.setSpeed(60);
  leftStepper.setSpeed(60);
  
}

void loop()
{
  
  if (esp8266.available())  //verify incoming data
  {
    if (esp8266.find("+IPD,")) //if there is a message
    {
      String msg;
      esp8266.find("?"); //look for the message
      msg = esp8266.readStringUntil(' '); //read whole message
      String command = msg.substring(0, 3); //first 3 characters = command
      String valueStr = msg.substring(4);   //next 3 characters = value
      int value = valueStr.toInt();
      if (DEBUG) {
        Serial.println(command);
        Serial.println(value);
      }
           
      //move forward
      if(command == "cm1") {
          state = 1;
      }

      //move backward
      if(command == "cm2") {
          state = 2;
      }

      //turn right
      if(command == "cm3") {
          state = 3;
       }
       
       //turn left
       if(command == "cm4") {
          state = 4;
       }

       //do nothing       
       if(command == "cm5") {
          state = 5;
       }

    }
  }
  
  //move forward
  if (state == 1) {
    rightStepper.step(1);
    leftStepper.step(-1);    
  }
  //move backward
  if (state == 2) {
    rightStepper.step(-1);
    leftStepper.step(1);    
  }
  //move right
  if (state == 3) {
    rightStepper.step(1);
    leftStepper.step(1);    
  }
  //move left
  if (state == 4) {
    rightStepper.step(-1);
    leftStepper.step(-1);    
  }
  //do nothing
  if (state == 5) {
  }
  
}

//*******************
//Auxiliary functions
//*******************

String sendData(String command, const int timeout, boolean debug)
{
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      char c = esp8266.read();
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}
