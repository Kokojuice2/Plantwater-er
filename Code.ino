/*
 * Info can be found on:  
 * http://docs.blynk.cc/
 * 
 * 
 * Used material:
 * 
 * - Micro-controller ---> ESP32 
 * 
 * - Lightsensor (LDR) with 100 Ohm resistor is attached to D35
 * - Water Sensor is attached to D32
 * - LED with 100 Ohm resistor attached to D33
 * 
 * Get I2C code with Scanner: 
 *    https://github.com/RobTillaart/Arduino/blob/master/sketches/MultiSpeedI2CScanner/MultiSpeedI2CScanner.ino
 *    Handy for checkup @ setup
 *    
 *    if I2C code 0x... not working --> error
 *
 *
 * 
 * - BMP280 Temperature and Pressure Sensor    on I2C
 *    GND --> GND
 *    VCC --> 3v3
 *    SCL --> D22
 *    SDA --> D21
 * 
 * 
 * - 0,96 inch OLED screen    on I2C
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
 * 
 * 
 */
 
//----------- Sensors ---------------------
int sensor = 0;
const int led = 33;
const int water = 32;
const int ldr = 35;


//----------- BMP280 Temp and Pressure Sensor ----------------

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bme;        // I2C


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


char auth[] = " ........................................";                               // Authentication Code from Blynk app project
char ssid[] = "................";                                                        // Wifi Name 
char pass[] = "................";                                                        // Wifi Password


//-------- Blynk App Button Function --------------
BLYNK_WRITE(V5){                                // Make a button on the app, choose pin(V3)
  boolean ledstatus = param.asInt();            //boolean returns 0 or 1
  Serial.print("V5 led value is: ");
  Serial.println(ledstatus); 


  if (ledstatus == 1){
    digitalWrite(led, HIGH);                      // if 1, led on
  }else {
    digitalWrite(led, LOW);                       // if 0, led off
  }
}


BLYNK_WRITE(V6){                                              // Manually control the motor for opening and closing the watering valve
  boolean motor = param.asInt();
  Serial.println("V6 motor is: ");
  Serial.println(motor);

  if (motor == 1){
    valve();
  }else{
    valve();
  }
}

void setup(){
Serial.begin(115200);     // Communication started with 115200 baud  (for motor)

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

Serial.println(F("BMP280 test"));
 
  if (!bme.begin(0x76)) {                       // Zoek de  I2C code met scanner
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }


  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("Water sensor active!");
  display.println("LDR sensor active!");
  display.println("BMP280 sensor active!");
  display.display(); 

pinMode(water, INPUT);
pinMode(led, OUTPUT);
pinMode(ldr, INPUT);

pinMode(IN1, OUTPUT); 
pinMode(IN2, OUTPUT); 
pinMode(IN3, OUTPUT); 
pinMode(IN4, OUTPUT); 



Blynk.begin(auth, ssid, pass, "server.wyns.it", 8081);

// Blynk.begin(auth, ssid, pass);

// Specify server:
// Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
// Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);




}


void loop(){

Blynk.run();

//--------------- Temperature Read ---------------

int celcius=bme.readTemperature();                                     // Reading BMP280 temperature
Serial.print("Temperature : ");
Serial.println(celcius);
Serial.println(" *C");

Blynk.virtualWrite(V4, celcius);                              // Set value celcius to Blynk app V4

display.clearDisplay();

display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0, 10);

display.print("Detected ");
display.print(celcius);
display.print(" Degrees");
display.display();
delay(1000);

//----------- Pressure Read -------------------

int pressure=(bme.readPressure()/100);                                     // Reading BMP280 temperature
Serial.print("Local air pressure : ");
Serial.println(pressure);
Serial.println(" hPa");

Blynk.virtualWrite(V3, pressure);                                          // Set Temperature value on Blynk app V3

display.clearDisplay();

display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0, 10);

display.print("Detected ");
display.print(pressure);
display.print(" hPa");
display.display();
delay(1000);


//--------------- Water sensor Read -----------------

int sensor=analogRead(water);                                     // Reading water sensor
Serial.print("Water : ");
Serial.println(sensor);   //Wrote serial port

Blynk.virtualWrite(V1, sensor);                                    // Set values in Blynk App on V1

display.clearDisplay();

display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0, 10);

display.print("Detected ");
display.print(sensor);
display.print(" water.");
display.display();
delay(1000);

//---------------- Light Sensor Read --------------------------

int light = analogRead(ldr);                                        // Reading Light intensity
Serial.println("Light intensity : ");
Serial.println(light);

Blynk.virtualWrite(V2, light);                                      // Set values in Blynk App on V2            URL: docs.blynk.cc


display.clearDisplay();

display.setTextSize(1);
display.setTextColor(WHITE);
display.setCursor(0, 10);

display.print("Detected ");
display.print(light);
display.print(" light.");
display.display();
delay(1000);


if((light == 0) && (sensor == 0)){                                  // If there's no light nor water, turn on valve ones and turn led on
  digitalWrite(led, HIGH);
  boolean needwater = true;
} else {
  digitalWrite(led, LOW);
  boolean needwater = false;
}


}


//----------- Stepper Motor Functions ---------------

void valve(){                                                                           // URL:  https://www.instructables.com/id/BYJ48-Stepper-Motor/
    while(steps_left>0){
    currentMillis = micros();
    if(currentMillis-last_time>=1000){
    stepper(1);
    _time = _time+micros()-last_time;
    last_time=micros();
    steps_left--;
      }
    }
    Serial.println(_time);
    Serial.println("Wait....!");
    delay(1000);
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
