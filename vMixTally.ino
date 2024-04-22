/*
  vMix Tally Lights
  Modified version from Thomas Mout Sketches: https://github.com/ThomasMout/Arduino-vMix-tally
*/

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include "FS.h"

 
//------------------------------------------------------------Change to your desire settings-----------------------------------------------
//Everything you edit here is for Setting Pin for led control

//ESP 01
//const int ledSignal = 2;//-------------------------------------------
//const int ledRed = 0;//-------------------------------------------
//const int greenRed = 1;//-------------------------------------------

//ESP 12/nodemcu/
const int ledSignal = 15;//-------------------------------------------
const int ledRed = 12;//-------------------------------------------
const int greenRed = 14;//-------------------------------------------

//----------------Web Menu Setting-----------------------------------
//Everything you edit here is for web menu style, name, color

const char* tallyName = "vMix Tally Lights";

const char* backColor = "red";//Background Tally Menu

const char* tallyCompany = "Smart Media";

const char* tallyDescriptions = "Vicks MediaTech 2018-2024";
//--------------------------Tally AP Settings--------------------------
//Everything you edit here is for your Tally Setting IP Address, SSID & Password

IPAddress tallyIp(192, 168, 48, 15); // change to your desire IP Address for web menu settings, for examlpe : 192, 168, 48, 15, then use this IP to access your tally web menu http://192.168.48.15

  const char* ssidTally = "vMix_Tally_Lights_%d";//This is Your Tally SSID. Change "Smart_Tally_Lights..." to your tally name, but dont change/delete the "_%d". 
 
  const char* passwordTally = "vicksmedia";//This is Your Tally Password. Change "vicksmedia" to your desired tally password
//---------------------------------------------------DONT CHANGE ANYTHING BELOW THIS LINE------------------------------------------------------------------------------


ESP8266WebServer httpServer(80);
char deviceName[32];
int status = WL_IDLE_STATUS;
bool apEnabled = false;
char apPass[64];

int port = 8099;//port vmix tally

char currentState = -1;
const char statusOffTally = 0;
const char ActiveTally = 1;
const char previewTally = 2;

const int sizeOfSsid = 32;
const int sizeOfPass = 32;
const int sizeOfHostName = 24;
const int TallyNumberMaxValue = 12;



// Settings object

struct Settings
{
  char ssid[sizeOfSsid];
  char pass[sizeOfPass];
  char hostName[sizeOfHostName];
  int tallyNumber;
};
Settings settings;
Settings defaultSettings = {
  "Your Router SSID",
  "Your Router Password",
  "Your Router IP Address",
  1
  };

// The WiFi client
WiFiClient client;
int timeout = 10;
int delayTime = 10000;

// Time measure
int interval = 5000;
unsigned long lastCheck = 0;

// Load settings from EEPROM
void loadSettings()
{
  Serial.println("------------");
  Serial.println("Loading settings");

  long ptr = 0;

  for (int i = 0; i < sizeOfSsid; i++)
  {
    settings.ssid[i] = EEPROM.read(ptr);
    ptr++;
  }

  for (int i = 0; i < sizeOfPass; i++)
  {
    settings.pass[i] = EEPROM.read(ptr);
    ptr++;
  }

  for (int i = 0; i < sizeOfHostName; i++)
  {
    settings.hostName[i] = EEPROM.read(ptr);
    ptr++;
  }

  settings.tallyNumber = EEPROM.read(ptr);

  if (strlen(settings.ssid) == 0 || strlen(settings.pass) == 0 || strlen(settings.hostName) == 0 || settings.tallyNumber == 0)
  {
    Serial.println("No settings found");
    Serial.println("Loading default settings");
    settings = defaultSettings;
    saveSettings();
    restart();
  }
  else
  {
    Serial.println("Settings loaded");
    printSettings();
    Serial.println("------------");
  }
}

// Save settings to EEPROM
void saveSettings()
{
  Serial.println("------------");
  Serial.println("Saving settings");

  long ptr = 0;

  for (int i = 0; i < 512; i++)
  {
    EEPROM.write(i, 0);
  }

  for (int i = 0; i < sizeOfSsid; i++)
  {
    EEPROM.write(ptr, settings.ssid[i]);
    ptr++;
  }

  for (int i = 0; i < sizeOfPass; i++)
  {
    EEPROM.write(ptr, settings.pass[i]);
    ptr++;
  }

  for (int i = 0; i < sizeOfHostName; i++)
  {
    EEPROM.write(ptr, settings.hostName[i]);
    ptr++;
  }

  EEPROM.write(ptr, settings.tallyNumber);

  EEPROM.commit();

  Serial.println("Settings saved");
  printSettings();
  Serial.println("------------");
}

// Print settings
void printSettings()
{
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(settings.ssid);
  Serial.print("SSID Password: ");
  Serial.println(settings.pass);
  Serial.print("IP vMix: ");
  Serial.println(settings.hostName);
  Serial.print("Tally WiFi Nomor: ");
  Serial.println(settings.tallyNumber);
}

// Set led intensity from 0 to 7
void ledSetIntensity(int intensity)
{
  //matrix.intensity = intensity;
}

// Set LED's off
void ledSetOff()
{
  digitalWrite(ledSignal, LOW);
   digitalWrite(ledRed, LOW);
  digitalWrite(greenRed, LOW);
  digitalWrite(ledSignal, HIGH);
}

// Draw L(ive) with LED's
void ledTallyActive()
{
  digitalWrite(ledSignal, LOW);
  digitalWrite(ledRed, LOW);
  digitalWrite(greenRed, LOW);
  digitalWrite(ledRed, HIGH);
  digitalWrite(ledSignal, HIGH);
  
}

// Draw P(review) with LED's
void ledSetPreview()
{
   digitalWrite(ledSignal, LOW);
  digitalWrite(ledRed, LOW);
  digitalWrite(greenRed, LOW);
  digitalWrite(greenRed, HIGH);
  digitalWrite(ledSignal, HIGH);
 
}

// Draw C(onnecting) with LED's
void ledSetConnecting()
{
  digitalWrite(ledSignal, LOW);
  digitalWrite(greenRed, LOW);
   digitalWrite(ledRed, LOW);
  digitalWrite(ledSignal, HIGH);
 delay(200);
 digitalWrite(ledSignal, LOW);
 delay(200);
   digitalWrite(ledSignal, HIGH);
 delay(200);
 digitalWrite(ledSignal, LOW);
 delay(200);
   digitalWrite(ledSignal, HIGH);
 delay(200);
 digitalWrite(ledSignal, LOW);
 delay(200);
   digitalWrite(ledSignal, HIGH);
 delay(200);
 digitalWrite(ledSignal, LOW);
 delay(200);
   digitalWrite(ledSignal, HIGH);
 delay(200);
 digitalWrite(ledSignal, LOW);
 delay(200);
    digitalWrite(ledSignal, HIGH);
 delay(200);
 digitalWrite(ledSignal, LOW);
 delay(200);
    digitalWrite(ledSignal, HIGH);
 delay(200);
 digitalWrite(ledSignal, LOW);
 delay(200);
    digitalWrite(ledSignal, HIGH);
 delay(200);
 digitalWrite(ledSignal, LOW);
 delay(200);
  digitalWrite(ledSignal, HIGH);
 delay(200);
 digitalWrite(ledSignal, LOW);
 delay(200);
   digitalWrite(ledSignal, HIGH);
 delay(200);
 digitalWrite(ledSignal, LOW);
 delay(200);
   digitalWrite(ledSignal, HIGH);
 delay(200);
 digitalWrite(ledSignal, LOW);
 delay(200);
   digitalWrite(ledSignal, HIGH);
 delay(200);
 digitalWrite(ledSignal, LOW);
 delay(200);
   digitalWrite(ledSignal, HIGH);
 delay(200);
 digitalWrite(ledSignal, LOW);
 delay(200);
    digitalWrite(ledSignal, HIGH);//------------------------------------------------
 delay(200);
 digitalWrite(ledSignal, LOW);
 delay(200);
    digitalWrite(ledSignal, HIGH);//------------------------------------------------
 delay(200);
 digitalWrite(ledSignal, LOW);
 delay(200);
    digitalWrite(ledSignal, HIGH);//------------------------------------------------
 delay(200);
 digitalWrite(ledSignal, LOW);
 delay(200);

}

// Draw S(ettings) with LED's
void ledSetSettings()
{
   digitalWrite(ledSignal, LOW);//---------------------------------------------
  digitalWrite(greenRed, LOW);//------------------------------------------------
  digitalWrite(ledRed, HIGH);//---------------------------------------------
  delay(700);
  digitalWrite(ledRed, HIGH);
  delay(400);
}

// Set tally to off
void ledTallyOff()
{
  Serial.println("Tally off");

  ledSetOff();
}

// Set tally to program
void SetledTallyActive()
{
  Serial.println("Tally program");

  ledSetOff();
  ledTallyActive();
}

// Set tally to preview
void SetledTallyPreview()
{
  Serial.println("Tally preview");

  ledSetOff();
  ledSetPreview();
}

// Set tally to connecting
void tallyConnectAtur()
{
  ledSetOff();
  ledSetConnecting();
}

// Handle incoming data
void handleData(String data)
{
  // Check if server data is tally data
  if (data.indexOf("TALLY") == 0)
  {
    char newState = data.charAt(settings.tallyNumber + 8);

    // Check if tally state has changed
    if (currentState != newState)
    {
      currentState = newState;

      switch (currentState)
      {
        case '0':
          ledTallyOff();
          break;
        case '1':
          SetledTallyActive();
          break;
        case '2':
          SetledTallyPreview();
          break;
        default:
          ledTallyOff();
      }
    }
  }
  else
  {
    Serial.print("Response from vMix: ");
    Serial.println(data);
  }
}

// Start access point
void apStart()
{
  ledSetSettings();
  Serial.println("AP Start");
  Serial.print("AP SSID: ");
  Serial.println(deviceName);
  Serial.print("AP password: ");
  Serial.println(apPass);


IPAddress Ip(tallyIp);
  IPAddress NMask(255, 255, 255, 0);

  WiFi.softAPConfig(Ip, Ip, NMask);
  WiFi.mode(WIFI_AP);
  WiFi.hostname(deviceName);
  WiFi.softAP(deviceName, apPass);
  delay(100);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("IP address: ");
  Serial.println(myIP);

  apEnabled = true;
}

// Hanle http server root request
void rootPageHandler()
{
  
   String web_menu_desc = "<!DOCTYPE html>";
   web_menu_desc += "<center>";
  web_menu_desc += "<html lang='en'>";
  web_menu_desc += "<head>";
  web_menu_desc += "<title>" + String(deviceName) + "</title>";
  web_menu_desc += "<meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'>";
  web_menu_desc += "<meta charset='utf-8'>";
  web_menu_desc += "<link rel='icon' type='image/x-icon' href='favicon.ico'>";
  web_menu_desc += "<link rel='stylesheet' href='styles.css'>";
  web_menu_desc += "<style>body {width: 100%;height: 100%;padding: 25px; color: white;} .title1 {display: flex; flex-direction: column; background: white; color: #009eff;} .wtitle1 {display: flex; flex-direction: column; background: white; color: black;} .title2 {display: flex; flex-direction: column; background: black; color: yellow;} .title3 {color: white;} .title4 {background: white; color: purple;}</style>";
  web_menu_desc += "</head>";

  web_menu_desc += "<h1 class='title1'>"+String(tallyCompany)+"</h1>";
  web_menu_desc += "<h9 class='wtitle1'>"+String(tallyName)+"</h9>";

  web_menu_desc += "<body class='bg-light'>";
  web_menu_desc += "<style>body {background-color: "+String(backColor)+"}</style>";//warna hijau
  
  web_menu_desc += "<h1 class='title2'>Tally Camera " + String(settings.tallyNumber) + "</h1>";
  web_menu_desc += "<div data-role='content' class='row'>";

  web_menu_desc += "<div class='col-md-6'>";
  web_menu_desc += "<h2 class='title3'>SETTINGS</h2>";
  web_menu_desc += "<form action='/save' method='post' enctype='multipart/form-data' data-ajax='false'>";

  web_menu_desc += "<div class='form-group row'>";
  web_menu_desc += "<label for='ssid' class='col-sm-4 col-form-label'>SSID/Router</label>";
  web_menu_desc += "<div class='col-sm-8'>";
  web_menu_desc += "<input id='ssid' class='form-control' type='text' size='20' maxlength='20' name='ssid' value='" + String(settings.ssid) + "'>";
  web_menu_desc += "</div></div>";

  web_menu_desc += "<div class='form-group row'>";
  web_menu_desc += "<label for='ssidpass' class='col-sm-4 col-form-label'>Password SSID/Router</label>";
  web_menu_desc += "<div class='col-sm-8'>";
  web_menu_desc += "<input id='ssidpass' class='form-control' type='text' size='20' maxlength='20' name='ssidpass' value='" + String(settings.pass) + "'>";
  web_menu_desc += "</div></div>";

  web_menu_desc += "<div class='form-group row'>";
  web_menu_desc += "<label for='hostname' class='col-sm-4 col-form-label'>vMix IP</label>";
  web_menu_desc += "<div class='col-sm-8'>";
  web_menu_desc += "<input id='hostname' class='form-control' type='text' size='20' maxlength='20' name='hostname' value='" + String(settings.hostName) + "'>";
  web_menu_desc += "</div></div>";

  web_menu_desc += "<div class='form-group row'>";
  web_menu_desc += "<label for='inputnumber' class='col-sm-4 col-form-label'>Nomor (Tally number...) (1-200)</label>";
  web_menu_desc += "<div class='col-sm-8'>";
  web_menu_desc += "<input id='inputnumber' class='form-control' type='number' size='64' min='0' max='1000' name='inputnumber' value='" + String(settings.tallyNumber) + "'>";
  web_menu_desc += "</div></div>";

  web_menu_desc += "<input type='submit' value='SAVE' class='btn btn-primary'></form>";
  web_menu_desc += "</div>";
//int batt = analogRead(A0);
//int level = map(batt, 195, 248, 0, 100);
  web_menu_desc += "<div class='col-md-6'>";
  web_menu_desc += "<h2 class='title3'>TALLY Info</h2>";
  web_menu_desc += "<table class='table'><tbody>";
  //web_menu_desc += "<h4 class='title3'>Battery Status</h4>";
  //web_menu_desc += "<h3 class='title2'>" + String(level) +"%</h3>";
  char ip[13];
  sprintf(ip, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
  web_menu_desc += "<tr><th>IP</th><td>" + String(ip) + "</td></tr>";

  web_menu_desc += "<tr><th>MAC</th><td>" + String(WiFi.macAddress()) + "</td></tr>";
  web_menu_desc += "<tr><th>Sinyal</th><td>" + String(WiFi.RSSI()) + " dBm</td></tr>";
  web_menu_desc += "<tr><th>Tally Number</th><td>" + String(deviceName) + "</td></tr>";

  if (WiFi.status() == WL_CONNECTED)
  {
    web_menu_desc += "<tr><th>Status</th><td>Terhubung</td></tr>";
  }
  else
  {
    web_menu_desc += "<tr><th>Status</th><td>Tidak Terhubung</td></tr>";
  }

  web_menu_desc += "</tbody></table>";
  web_menu_desc += "</div>";
  web_menu_desc += "</div>";
web_menu_desc += "<h4 class='title4'> "+String(tallyDescriptions)+"</h4>";
  web_menu_desc += "</body>";
  web_menu_desc += "</html>";
  web_menu_desc += "</center>";

  httpServer.sendHeader("Connection", "close");
  httpServer.send(200, "text/html", String(web_menu_desc));
}

// Settings POST handler
void handleSave()
{
  bool doRestart = false;

  httpServer.sendHeader("Location", String("/"), true);
  httpServer.send(302, "text/plain", "Redirected to: /");

  if (httpServer.hasArg("ssid"))
  {
    if (httpServer.arg("ssid").length() <= sizeOfSsid)
    {
      httpServer.arg("ssid").toCharArray(settings.ssid, sizeOfSsid);
      doRestart = true;
    }
  }

  if (httpServer.hasArg("ssidpass"))
  {
    if (httpServer.arg("ssidpass").length() <= sizeOfPass)
    {
      httpServer.arg("ssidpass").toCharArray(settings.pass, sizeOfPass);
      doRestart = true;
    }
  }

  if (httpServer.hasArg("hostname"))
  {
    if (httpServer.arg("hostname").length() <= sizeOfHostName)
    {
      httpServer.arg("hostname").toCharArray(settings.hostName, sizeOfHostName);
      doRestart = true;
    }
  }

  if (httpServer.hasArg("inputnumber"))
  {
    if (httpServer.arg("inputnumber").toInt() > 0 and httpServer.arg("inputnumber").toInt() <= TallyNumberMaxValue)
    {
      settings.tallyNumber = httpServer.arg("inputnumber").toInt();
      doRestart = true;
    }
  }

  if (doRestart == true)
  {
    restart();
  }
}

// Connect to WiFi
void connectToWifi()
{
  Serial.println();
  Serial.println("------------");
  Serial.println("Connecting to WiFi");
  Serial.print("SSID: ");
  Serial.println(settings.ssid);
  Serial.print("Passphrase: ");
  Serial.println(settings.pass);

  int timeout = 15;

  //WiFi.mode(WIFI_STA);// -------Disable AP_STA Line below if you in Station mode only
  WiFi.mode(WIFI_AP_STA);
  WiFi.hostname(deviceName);
  WiFi.begin(settings.ssid, settings.pass);

//-------Disable this if you on Station mode only
  IPAddress Ip(tallyIp);
  IPAddress NMask(255, 255, 255, 0);

  WiFi.softAPConfig(Ip, Ip, NMask);
  WiFi.softAP(deviceName, apPass);
  //-------Disable until here if you on Station mode only

  Serial.print("Waiting for connection.");
  while (WiFi.status() != WL_CONNECTED and timeout > 0)
  {
    delay(1000);
    timeout--;
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Success!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Device name: ");
    Serial.println(deviceName);
    Serial.println("------------");
  }
  else
  {
    if (WiFi.status() == WL_IDLE_STATUS)
      Serial.println("Idle");
    else if (WiFi.status() == WL_NO_SSID_AVAIL)
      Serial.println("No SSID Available");
    else if (WiFi.status() == WL_SCAN_COMPLETED)
      Serial.println("Scan Completed");
    else if (WiFi.status() == WL_CONNECT_FAILED)
      Serial.println("Connection Failed");
    else if (WiFi.status() == WL_CONNECTION_LOST)
      Serial.println("Connection Lost");
    else if (WiFi.status() == WL_DISCONNECTED)
     Serial.println("Disconnected");
    else
      Serial.println("Unknown Failure");

    Serial.println("------------");
    apStart();
  }
}
void battStatus (){
int batt = analogRead(A0);
int level = map(batt, 195, 248, 0, 100);

  if (level<80) {digitalWrite(16, LOW);}
  else if (level<60) {digitalWrite(5, LOW);}
  else if (level<40) {digitalWrite(4, LOW);}
  else if (level<20) {digitalWrite(14, LOW);}
  else {
    digitalWrite(16, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(14, HIGH);
  }
}
// Connect to vMix instance
void vMixConnectt()
{
  Serial.print("Connecting to vMix on ");
  Serial.print(settings.hostName);
  Serial.print("...");

  if (client.connect(settings.hostName, port))
  {
    Serial.println(" Connected!");
    Serial.println("------------");
    
    ledTallyOff();

    // Subscribe to the tally events
    client.println("SUBSCRIBE TALLY");
  }
  else
  {
    Serial.println(" Not found!");
  }
}

void restart()
{
  saveSettings();

  Serial.println();
  Serial.println();
  Serial.println("------------");
  Serial.println("------------");
  Serial.println("RESTART");
  Serial.println("------------");
  Serial.println("------------");
  Serial.println();
  Serial.println();

  start();
}

void start()
{
  tallyConnectAtur();


  loadSettings();
  sprintf(deviceName, ssidTally, settings.tallyNumber);
  //sprintf(apPass, "%s%s", deviceName, "_access");
    sprintf(apPass, passwordTally, deviceName, "pro");

  connectToWifi();

  if (WiFi.status() == WL_CONNECTED)
  {
    vMixConnectt();
    
  }
}

void setup()
{
  Serial.begin(9600);
  EEPROM.begin(512);
  SPIFFS.begin();
  
   
  httpServer.on("/", HTTP_GET, rootPageHandler);
  httpServer.on("/save", HTTP_POST, handleSave);
  httpServer.serveStatic("/", SPIFFS, "/", "max-age=315360000");
  httpServer.begin();

  start();
  pinMode(ledSignal, OUTPUT);//----------------------------------------
   pinMode(ledRed, OUTPUT);//----------------------------------------
  pinMode(greenRed, OUTPUT);//------------------------------------------
  // Set outputs to LOW
  digitalWrite(ledSignal, LOW);//---------------------------------------------
  digitalWrite(ledRed, LOW);//---------------------------------------------
  digitalWrite(greenRed, LOW);//------------------------------------------------

  digitalWrite(ledRed, HIGH);
  delay(200);
  digitalWrite(ledRed, LOW);
  delay(200);
    digitalWrite(ledRed, HIGH);
    delay(200);
  digitalWrite(ledRed, LOW);
  delay(200);
      digitalWrite(ledRed, HIGH);
    delay(200);
  digitalWrite(ledRed, LOW);
}

void loop()
{
  
  httpServer.handleClient();

  while (client.available())
  {
    String data = client.readStringUntil('\r\n');
    handleData(data);
  }

  if (!client.connected() && !apEnabled && millis() > lastCheck + interval)
  {
    tallyConnectAtur();

    client.stop();

    vMixConnectt();
    
    lastCheck = millis();
  }

}

