//By Rohan Dhananjaya

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

MDNSResponder mdns;
WiFiServer server(80);

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

int pO1=12; //for relay 1
int pO2=13; //for relay 2
int pS1=4;  //for switch
int pS2=5;  //for switch
int sT_e=14; // Error LED
int sT_s=16; // Working LED
//bool l1S=false;
//bool l2S=false;
String l1s="";
String l2s="";
/////////////////
boolean light1_change;
boolean light2_change;

void setup() {
  
  pinMode(pO1, OUTPUT); // Config pin mods as inputs and outputs
  pinMode(pO2, OUTPUT);
  pinMode(sT_e,OUTPUT);
  pinMode(sT_s,OUTPUT);
  pinMode(pS1, INPUT);
  pinMode(pS2, INPUT);
  
  digitalWrite(pO1,LOW);
  digitalWrite(pO2,LOW); 
   
  
  Serial.begin(115200);  // Set boud and print info..
  Serial.println("net Light v0.0.6");
  Serial.println("BT_ST");
  
  digitalWrite(sT_e,HIGH);
  digitalWrite(sT_s,HIGH);

  WiFi.mode(WIFI_STA);  // WLAN Setup..
  WiFi.begin(ssid, password);
 
 while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    digitalWrite(sT_e,LOW);
    delay(500);
    digitalWrite(sT_e,HIGH);
    //ESP.restart();
  }

 /*-------- server started---------*/ 
  server.begin();
  Serial.println("SRV_STRT");
  Serial.println(WiFi.localIP());
  /*------printing ip address--------*/
  
  // --- OTA setup start --- //
  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname("net Light v1.8");
  ArduinoOTA.setPassword("WRzPBpXq@X3+sqSf");
  //MD5(admin) = bc1c02d4450dc9ec449264590da6ae47
  //ArduinoOTA.setPasswordHash("bc1c02d4450dc9ec449264590da6ae47");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    //Serial.println("Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
      digitalWrite(sT_e,HIGH);
      digitalWrite(sT_s,LOW);
  });
  
  ArduinoOTA.begin();
// --- OTA setup end. --- //
 
  digitalWrite(sT_e,LOW);   // Status
   for (int i=0; i <= 6; i++){
    digitalWrite(sT_s,HIGH);
    delay(100);
    digitalWrite(sT_s,LOW);
   }
  
  digitalWrite(sT_s,LOW);
  Serial.println("RDY");
  Serial.print("IP_ADR: ");
  Serial.println(WiFi.localIP());
  EEPROM.begin(512);
  digitalWrite(pO1,EEPROM.read(1));
  digitalWrite(pO2,EEPROM.read(2)); 
  
  if(EEPROM.read(1)){
    l1s="OFF";
  }else{
    l1s="ON";
  }
  
  if(EEPROM.read(2)){
    l2s="OFF";
  }else{
    l2s="ON";
  }
}



void loop() {
   ArduinoOTA.handle();

   WiFiClient client = server.available();    
    if (!client) 
    {
      return;
    }
  Serial.println("W_FR_NC");   
  while(!client.available())
  {
    delay(1);
  }
   if(digitalRead(pS1)==LOW)
  {
    light1_change = true;
  }
  if(digitalRead(pS2)==HIGH)
  {
    light2_change = true;
  }
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
 
  int value = LOW;
  if (request.indexOf("/L1=ON") != -1)  //GET Light 1 requests
  {
    digitalWrite(pO1, HIGH);
    //l1S=true;

    EEPROM.write(1,digitalRead(pO1));
    EEPROM.commit();
    
    digitalWrite(sT_s,HIGH);
    delay(100);
    digitalWrite(sT_s,LOW);
    l1s="OFF";
    }
    if (request.indexOf("/L1=OFF") != -1)  
    {
    digitalWrite(pO1, LOW);
    //l1S=false;
    EEPROM.write(1,digitalRead(pO1));
    EEPROM.commit();
    
    digitalWrite(sT_s,HIGH);
    delay(100);
    digitalWrite(sT_s,LOW);
    l1s="ON";
  }
  
   if (request.indexOf("/L2=ON") != -1)  //GET Light 2 requests
  {
    digitalWrite(pO2, HIGH);
   // l2S=true;

    EEPROM.write(2,digitalRead(pO2));
    EEPROM.commit();
    
    digitalWrite(sT_s,HIGH);
    delay(100);
    digitalWrite(sT_s,LOW);
    l2s="OFF";
  }
  if (request.indexOf("/L2=OFF") != -1)  
  {
    digitalWrite(pO2, LOW);
    //l2S=false;
    
    EEPROM.write(2,digitalRead(pO2));
    EEPROM.commit();
    
    digitalWrite(sT_s,HIGH);
    delay(100);
    digitalWrite(sT_s,LOW);
    l1s="ON";
  }
/*------------------Creating html page---------------------*/

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("<center><font size='22'>");
  client.println(""); 
  client.println("net Light v0.0.6");
  client.println("");

  client.println("<br><br>");
  client.print("<p>Light 2 :</p>");
  client.print("<a href=\"/L1="+l1s+"\"\"><button>Turn "+l1s+"</button></a>");
  client.println("<br>");
  client.print("<p>Light 2 :</p>");
  client.print("<a href=\"/L2="+l2s+"\"\"><button>Turn "+l2s+"</button></a>");  
  client.println("</font></center>");
 // Serial.println("CL_DSC");
 // Serial.println("");

   if(light1_change)
  {
    light1_change = false;
    if(digitalRead(pO1)){
      digitalWrite(pO1,0);
      l1s="ON";
    }
    else{
      digitalWrite(pO1,1);
      l1s="OFF";
    }

    EEPROM.write(1,digitalRead(pO1));
    EEPROM.commit();
    
    digitalWrite(sT_s,HIGH);
    delay(100);
    digitalWrite(sT_s,LOW);
  }
  
  if(light2_change)
  {
    light2_change = false;
    if(digitalRead(pO2)){
      digitalWrite(pO2,0);
      l2s="ON";
    }
    else{
      digitalWrite(pO2,1);
      l1s="OFF";
    }
    
    EEPROM.write(2,digitalRead(pO2));
    EEPROM.commit();
    
    digitalWrite(sT_s,HIGH);
    delay(100);
    digitalWrite(sT_s,LOW);    
  }
  

  
  delay(100);
}
