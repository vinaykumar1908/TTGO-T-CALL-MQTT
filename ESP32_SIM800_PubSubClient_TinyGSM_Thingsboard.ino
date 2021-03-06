/*------------------------Start Configurations---------------------*/
#include <SoftwareSerial.h>

/* INDIAN RAILWAYS PROJECT
VINAY KUMAR, IRSME-2016
Objective: To transmit data from ESP32-Sim800l (TTGO T-Call), via vodafone GSM 2G Network, to ThingsBoard Server. */

//setup for reading wagon number
const int buttonPin = 35;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status
int buttonstatus = 0;
char c[6] ;
int WagonNumber;
bool tbStatus = false;
//setup end for reading wagon number

// Initial setup for flow meter
int flowPin = 34;    //This is the input pin on the Arduino
float flowRate;    //This is the value we intend to calculate. 
volatile int count; //This integer needs to be set as volatile to ensure it updates correctly during the interrupt process.  
// Initial setup close for flow meter 

// Your GPRS credentials (leave empty, if missing)
const char apn[]      = "www"; // Your APN
const char gprsUser[] = ""; // User
const char gprsPass[] = ""; // Password
const char simPIN[]   = ""; // SIM card PIN code, if any

// TTGO T-Call pin definitions
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define I2C_SDA              21
#define I2C_SCL              22

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to the module)
#define SerialAT  Serial1
// Software Serial for Communication to Arduino Nano

SoftwareSerial Master(32,33);

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800      // Modem is SIM800
#define TINY_GSM_RX_BUFFER   1024  // Set RX buffer to 1Kb

#include "ThingsBoard.h"

// See https://thingsboard.io/docs/getting-started-guides/helloworld/
// to understand how to obtain an access token
#define TOKEN               "2lCpUoT0DOdDkAAMXFov"
#define THINGSBOARD_SERVER  "167.71.235.149"


// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon
#define DUMP_AT_COMMANDS

#include <Wire.h>
#include <TinyGsmClient.h>
#include "utilities.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
// Initialize GSM modem
  TinyGsm modem(SerialAT);
#endif

// Initialize GSM client
TinyGsmClient client(modem);


// Initialize ThingsBoard instance
ThingsBoard tb(client);

// Set to true, if modem is connected
bool modemConnected = false;

/*-----------------------End Configurations-------------------------*/

/*-----------------------Start ThingsBoard Function------------------*/
void connectToTB()
{
  if (modem.isNetworkConnected()){
  if (!tb.connected())
  {
    // Connect to the ThingsBoard
    SerialMon.print("Connecting to: ");
    SerialMon.print(THINGSBOARD_SERVER);
    SerialMon.print(" with token ");
    SerialMon.println(TOKEN);
    //oled
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Connecting to ThingsBoard !");
    display.display();
    delay(2000);
    //end oled
    while(!tb.connected())
    {
      tb.connect(THINGSBOARD_SERVER, TOKEN);
      Serial.println("Current Status Not Connected, Trying till TB.Connected !");
      }
    if (tb.connect(THINGSBOARD_SERVER, TOKEN))
    {
      SerialMon.println("ThingsBoard Connected !");
      //oled
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("ThingsBoard Server Connected !! ");
      display.display();
      delay(2000);
      loop();
      //end oled
      //return;
    }else if(!tb.connect(THINGSBOARD_SERVER, TOKEN))
    {
      SerialMon.println("Failed to connect");
      //oled
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("Could Not Connect To Thingsboard :( ");
      display.display();
      delay(2000);
      //end oled
      //return;
      }
  }
}else
{
  SerialMon.println("In thingsboard function, Network not Connected");
  display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("In thingsboard function, Network not Connected");
      display.display();
      delay(2000);
  connectToServer();
  }
}
/*---------------------End ThingsBoard Function-------------------------*/

/*---------------------Start Wagon Number Function----------------------*/
void readWagonNumber( )
{
  
  SerialMon.println("Enter Read Wagon Loop");
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Enter Wagon Number... ");
  display.display();
  delay(3000);
  while (buttonstatus == 0)
  {
    buttonState = digitalRead(buttonPin); // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
    if (buttonState == HIGH)
    {
      delay(1000);
      SerialMon.println("Reading Slave Data");
      for (int i = 0; i < 8; i++)
      {
        if (Master.available() > 0)
        {
          c[i] = Master.read();
        }
      }

      buttonstatus = 1;
    }
    else
    {
      buttonstatus = 0;
    }
  }
  SerialMon.print("Master Reads: ");
  SerialMon.println(c);
  WagonNumber = atol(c);
  SerialMon.println(WagonNumber);
  SerialMon.println(buttonstatus);
  Master.flush();
  Master.end();

  delay(50);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 4);
  display.print("Wagon Number : ");
  display.display();
  delay(500);
  display.setTextSize(3);
  display.setCursor(0, 14);
  display.println(WagonNumber);
  display.display();
  delay(4000);
  SerialMon.println("Exit Read Wagon Loop");
  connectToServer();
  
}
/*--------------------------End Wagon Number Function--------------------------*/
/*--------------------------Start Connect Network Function-----------------------*/
void connectToServer()
{
  Serial.println("Starting connectToServer Function");

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Wagon Number Saved!!  ... Network loop started");
  display.display();
  delay(2000);

  if (!modemConnected)
  {
    SerialMon.print(F("Waiting for network..."));
    //oled
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Waiting for Network ...");
    display.display();
    delay(2000);
    //end oled
    if (!modem.waitForNetwork())
    {
      SerialMon.println(" fail");
      //oled
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("Network Not Found !");
      display.display();
      //end oled
      delay(2000);
      //return;
    }
    SerialMon.println(" OK");
  }
    if (modem.isNetworkConnected())
    {
      SerialMon.println("Network connected");
      //oled
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("Network Connected !");
      display.display();
      delay(2000);
      //end oled
    }

    SerialMon.print(F("Connecting to APN: "));
    //oled
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println(" Connecting to APN ...");
    display.display();
    delay(2000);
    //end oled

    SerialMon.print(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass))
    {
      SerialMon.println(" fail");
      //oled
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("Cannot Connect To APN !");
      display.display();
      //end oled
      delay(2000);
      //return;
    }
    SerialMon.println(" OK");
    
  
  if (modem.gprsConnect(apn, gprsUser, gprsPass))
  {
    SerialMon.println("APN connected");
    delay(5000);
    //oled
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("APN Connected !");
    display.display();
    delay(2000);
    //end oled
    SerialMon.println("Connecting to ThingsBoard");
    //oled
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Connecting to ThingsBoard...");
    display.display();
    delay(2000);
    //end oled
    connectToTB();
  }else
  {
    connectToServer();
    }
}

/*------------------------End Connect Network Function----------------------*/

/*------------------------Start Air Flow Meter Function---------------------*/
void calculateFlowRate()
{

  count = 0;      // Reset the counter so we start counting from 0 again
  interrupts();   //Enables interrupts on the Arduino
  delay(1000);    //Wait 1 second
  noInterrupts(); //Disable the interrupts on the Arduino

  //Start the math
  flowRate = (count * 2.25);  //Take counted pulses in the last second and multiply by 2.25mL
  flowRate = flowRate * 60;   //Convert seconds to minutes, giving you mL / Minute
  flowRate = flowRate / 1000; //Convert mL to Liters, giving you Liters / Minute

  SerialMon.println(flowRate); //Print the variable flowRate to Serial

  SerialMon.println("Sending data...");
  //oled
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("FlowRate:");
  display.println(flowRate);
  display.display();
  //end oled
}
/*------------------------End Air Flow Meter Function---------------------*/

/*------------------------Start Flow Function-----------------------------*/
void Flow()
{
  count++; //Every time this function is called, increment "count" by 1
}

/*------------------------End Flow Function-------------------------------*/

/*------------------------Start Setup Function--------------------------------*/
void setup() {
  SerialMon.begin(115200);
  Master.begin(9600);
//code for reading wagon number
pinMode(buttonPin, INPUT);
  //code for reading wagon number
  Wire.begin();
  
  
  //code for oled
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    SerialMon.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds
  //code end for oled
  
  
      

  // put your setup code here, to run once:
  pinMode(flowPin, INPUT);           //Sets the pin as an input
  attachInterrupt(flowPin, Flow, RISING);  //Configures interrupt 0 (pin 2 on the Arduino Uno) to run the function "Flow"  
  //Serial.begin(9600);  //Start Serial
  // Set console baud rate
  
  delay(10);

  // Keep power when running from battery
  Wire.begin(I2C_SDA, I2C_SCL);
  bool   isOk = setPowerBoostKeepOn(1);
  SerialMon.println(String("IP5306 KeepOn ") + (isOk ? "OK" : "FAIL"));

  // Set-up modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);

  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("Initializing modem...");
  modem.restart();
  // Or, use modem.init() if you don't need the complete restart
  
String modemInfo = modem.getModemInfo();
  SerialMon.print("Modem: ");
  SerialMon.println(modemInfo);

  // Unlock your SIM card with a PIN if needed
  if (strlen(simPIN) && modem.getSimStatus() != 3 ) {
    modem.simUnlock(simPIN);
  }
  delay(1000);

  readWagonNumber();
}

/*------------------------End Setup Function--------------------------------*/

/*------------------------Start Loop Function-------------------------------*/
void loop()
{
  if(tb.connected()) {
    //oled 
    
    //end oled
    
    
    calculateFlowRate();
  // put your main code here, to run repeatedly:  
  

  // Uploads new telemetry to ThingsBoard using MQTT.
  // See https://thingsboard.io/docs/reference/mqtt-api/#telemetry-upload-api
  // for more details

  tb.sendTelemetryInt("flowRate", flowRate);
  tb.sendTelemetryInt("Wagon Number", WagonNumber);
  tb.sendTelemetryInt("Boiling Point", 100);

  tb.loop();

  delay(1000);
}else{
  if ( !modem.isNetworkConnected() || !modem.gprsConnect(apn, gprsUser, gprsPass) )
  {
    SerialMon.println("In Loop(), Either Network or GPRS not connected.. Sending to ConnectToServer Function ");
    //oled
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 4);
  display.println("In Loop(), Either Network or GPRS not connected.. Sending to ConnectToServer Function ");
  display.display();
  //end oled
  connectToServer();
    }else if (!tb.connected()) 
    {
      SerialMon.println("In Loop(), ThingsBoard not connected.. Sending to ThingsBoard Function ");
    //oled
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 4);
  display.println("In Loop(), ThingsBoard not connected.. Sending to ThingsBoard Function ");
  display.display();
  //end oled
  connectToTB();
      }
  }
}

/*------------------------End Loop Function-------------------------------*/
