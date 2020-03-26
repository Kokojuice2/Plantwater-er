/*
 * Info can be found on:  
 * http://docs.blynk.cc/
 * 
 * Used material:
 * 
 * - Micro-controller ---> ESP32 
 * 
 * - Lightsensor (LDR) with 100 Ohm resistor is attached to D35
 * - Water Sensor is attached to D32
 * - LED with 100 Ohm resistor attached to D33
 * 
 * - 0,96 inch OLED screen
 *    GND --> GND
 *    VCC --> 3v3
 *    SCL --> D22
 *    SDA --> D21
 * 
 * - Stepper motor for opening and closing water valve
 *     +   --> 3v3
 *     -   --> GND
 *    Int1 --> pin ...
 *    Int2 --> pin ...
 *    Int3 --> pin ...
 *    Int4 --> pin ...
 * 
 * 
 * Created by Joachim Pham,      Belgian
 * Licensed under "GNU General Public License v3.0"
 * 
 */
 
//----------- Sensors ---------------------
int sensor = 0;
const int led = 33;
const int water = 32;
const int ldr = 35;


//----------Stepper motor with SBT0811 ------------
#define IN1  2
#define IN2  4
#define IN3  5
#define IN4  18

int Steps = 0;
boolean Direction = true;// gre
unsigned long last_time;
unsigned long currentMillis ;
int steps_left=4095;
long _time;

 
//-------------OLED screen ----------------------
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);



//--------- Blynk Server -----------------------
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


char auth[] = " ................................";                            // Enter your Auth Code from your project in the app
char ssid[] = ".............";                                                // Enter your wifi name
char pass[] = ".............";                                                // Enter your wifi password


//-------- Blynk App Button Function --------------
BLYNK_WRITE(V3){                                                              // Make a button on the app, choose pin(V3)
  boolean ledstatus = param.asInt();                                          //boolean returns 0 or 1
  Serial.print("V3 led value is: ");
  Serial.println(ledstatus); 


if (ledstatus == 1){
  digitalWrite(led, HIGH);                                                    // if 1, led on
} else {
  digitalWrite(led, LOW);                                                     // if 0, led off
}
}

void setup(){
Serial.begin(115200);                                                         // Communication started with 115200 baud, for motor

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay(); 

  display.setTextSize(1);                                                     // Get your OLED awake @ setup, say something
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("Water sensor active!");
  display.println("Light sensor active!");
  display.display(); 

pinMode(water, INPUT);                                                        // Declare pins in- or output
pinMode(led, OUTPUT);
pinMode(ldr, INPUT);

pinMode(IN1, OUTPUT); 
pinMode(IN2, OUTPUT); 
pinMode(IN3, OUTPUT); 
pinMode(IN4, OUTPUT); 



Blynk.begin(auth, ssid, pass, "server.wyns.it", 8081);                        // Private Blynk Server

// Blynk.begin(auth, ssid, pass);

// Specify server:
// Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
// Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

}


void loop(){

Blynk.run();


int sensor=analogRead(water);                                                 // Get value of water sensor
Serial.print("Water : ");
Serial.println(sensor);   

display.clearDisplay();                                                       // Print value sensor on OLED

display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0, 10);

display.print("Detected ");
display.print(sensor); 
display.print(" water.");
display.display();
delay(1000);



int light = analogRead(ldr);                                                 // Get value of light sensor
Serial.println("Light intensity : ");
Serial.println(light);

Blynk.virtualWrite(V2, light);                                               // Set values in App on V2
Blynk.virtualWrite(V1, sensor);                                              // Set values in App on V1


display.clearDisplay();                                                      // Print value light on OLED

display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0, 10);

display.print("Detected ");
display.print(light);
display.print(" light.");
display.display();
delay(1000);

//----------- Stepper Motor Loop ---------------

while(steps_left>0){                                // Code for Stepper motor, withoout external library
currentMillis = micros();                           // This code in the loop, gets the motor to open en close the valve for 1 second
if(currentMillis-last_time>=1000){
stepper(1);                                         //Couldn't get it working for longer time, based on direct light on the plants, during the day
_time = _time+micros()-last_time;
last_time=micros();
steps_left--;
      }
   }
Serial.println(_time);
Serial.println("Wait....!");
delay(2000);
Direction=!Direction;
steps_left=4095;

}

void stepper(int xw){
      for (int x=0;x<xw;x++){
    switch(Steps){
       case 0:
         digitalWrite(IN1, LOW); 
         digitalWrite(IN2, LOW);
         digitalWrite(IN3, LOW);
         digitalWrite(IN4, HIGH);
       break; 
       case 1:
         digitalWrite(IN1, LOW); 
         digitalWrite(IN2, LOW);
         digitalWrite(IN3, HIGH);
         digitalWrite(IN4, HIGH);
       break; 
       case 2:
         digitalWrite(IN1, LOW); 
         digitalWrite(IN2, LOW);
         digitalWrite(IN3, HIGH);
         digitalWrite(IN4, LOW);
       break; 
       case 3:
         digitalWrite(IN1, LOW); 
         digitalWrite(IN2, HIGH);
         digitalWrite(IN3, HIGH);
         digitalWrite(IN4, LOW);
       break; 
       case 4:
         digitalWrite(IN1, LOW); 
         digitalWrite(IN2, HIGH);
         digitalWrite(IN3, LOW);
         digitalWrite(IN4, LOW);
       break; 
       case 5:
         digitalWrite(IN1, HIGH); 
         digitalWrite(IN2, HIGH);
         digitalWrite(IN3, LOW);
         digitalWrite(IN4, LOW);
       break; 
         case 6:
         digitalWrite(IN1, HIGH); 
         digitalWrite(IN2, LOW);
         digitalWrite(IN3, LOW);
         digitalWrite(IN4, LOW);
       break; 
       case 7:
         digitalWrite(IN1, HIGH); 
         digitalWrite(IN2, LOW);
         digitalWrite(IN3, LOW);
         digitalWrite(IN4, HIGH);
       break; 
       default:
         digitalWrite(IN1, LOW); 
         digitalWrite(IN2, LOW);
         digitalWrite(IN3, LOW);
         digitalWrite(IN4, LOW);
       break; 
    }
    SetDirection();
    }
}


void SetDirection(){
    if(Direction==1){ Steps++;}
    if(Direction==0){ Steps--; }
    if(Steps>7){Steps=0;}
    if(Steps<0){Steps=7; }
}
