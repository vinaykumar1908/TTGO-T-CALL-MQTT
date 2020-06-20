/* INDIAN RAILWAYS PROJECT
VINAY KUMAR, IRSME-2016
Objective: To transmit data from ESP32-Sim800l (TTGO T-Call), via vodafone GSM 2G Network, to ThingsBoard Server. */


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


void setup() {
  // Set console baud rate
  SerialMon.begin(115200);
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

  if (!modemConnected) {
    SerialMon.print(F("Waiting for network..."));
    if (!modem.waitForNetwork()) {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    Serial.println(" OK");
  
  if (modem.isNetworkConnected()) {
    SerialMon.println("Network connected");
  }

  SerialMon.print(F("Connecting to APN: "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" OK");
  }
  if (!tb.connected()) {
    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
  }
}

  void loop() {

    

  Serial.println("Sending data...");

  // Uploads new telemetry to ThingsBoard using MQTT.
  // See https://thingsboard.io/docs/reference/mqtt-api/#telemetry-upload-api
  // for more details

  tb.sendTelemetryInt("temperature", 10);
  tb.sendTelemetryFloat("humidity", 42.5);
  tb.sendTelemetryInt("Boiling Point", 100);

  tb.loop();

  delay(1000);
}
  
