#include <Wire.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
// D6 = Rx & D5 = Tx
SoftwareSerial nodemcu(D6, D5);
//intiliazing wifi module to send the telegrm messages
const char* ssid = "Oneplus"; // Enter your WIFI SSID
const char* password = "shannu45"; // Enter your WIFI Password
#define BOTtoken "6617912176:AAFHA5NJxU5_ZFao77Ueb9rqV72Z0DN2WsY" // Enter the bottoken you got from botfather
#define CHAT_ID "5358296229" // Enter your chatID you got from chatid bot
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
//end of telegram messsnage intialization 
void setup() {
  // Initialize Serial port
  Serial.begin(9600);
  nodemcu.begin(9600);
  //intialize lcd port
  while (!Serial) continue;
  // Serial.begin(115200);
  configTime(0, 0, "pool.ntp.org");
  client.setTrustAnchors(&cert);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int a = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    a++;
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  bot.sendMessage(CHAT_ID, "Wifi Connected!", "");
  bot.sendMessage(CHAT_ID, "System has Started!!", "");
}
int sta=0;
String a;
int level=200;
void loop() {
  StaticJsonDocument<1000> jsonBuffer;
  DeserializationError error = deserializeJson(jsonBuffer, nodemcu);
  if (error) {
    Serial.println("Error parsing JSON");
    jsonBuffer.clear();
    return;
  }
  // Check if the received data is an object
  if (!jsonBuffer.is<JsonObject>()) {
    Serial.println("Invalid JSON Object");
    jsonBuffer.clear();
    return;
  }
  JsonObject data = jsonBuffer.as<JsonObject>();
  Serial.println("-----------------------------------------");
  //jsonBuffer.clear();
  Serial.println("Json object Recieved..");
  int s=data["status"];
  String dire=data["direction"];
  if(s==1){
    sta=sta+1;
    Serial.print("Recieved Status  :");
    Serial.println("fire");
    Serial.print("Direction ::");
    Serial.println(dire);
    Serial.print("status");
    Serial.print(sta);
    String message="Fire Detected on "+ dire+" side\n";
    int ks=level/2;
    String Level_mess="The Level of the Extinughsher "+String(ks)+" %\n";
    String Status="The Robot can extuiniguish fire for "+String(level)+" seconds\n";
    String Final=message+Level_mess+Status;
    if(sta==1){
        a="status is 🟢\nThe fires has been deployed";
        message=Final+a;
        bot.sendMessage(CHAT_ID, message,"");
        }
    if(sta==3)
    {
      a="status is 🔵\nMedium Fires is being detcted from 3 seconds";
      message=Final+a;
      bot.sendMessage(CHAT_ID, message,"");
    }
    if(sta>5){
      a="status is 🔴\nHeavy fires has been detecting from more than 5 seconds 🔴";
      message=Final+a;
      bot.sendMessage(CHAT_ID, message,"");
    }
    level--;
  }
  else{
    sta=0;
    // bot.sendMessage(CHAT_ID, "no fire","");
    Serial.print("Recieved Status  :");
    Serial.println("No fire");
  }
  jsonBuffer.clear();
}
