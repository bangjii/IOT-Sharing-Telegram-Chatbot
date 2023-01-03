#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>
#include "DHT.h"

const char* ssid = "";
const char* password = "";

#define BOTtoken "xxxxxxxx"
#define DHTPIN 14
#define DHTTYPE DHT11
#define trigPin 5
#define echoPin 4
#define ledA 16
#define ledB 13
#define buz 12

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
DHT dht(DHTPIN, DHTTYPE);

long duration;
int distance;
String url = "https://raw.githubusercontent.com/bangjii/IOT-Sharing-Telegram-Chatbot/main/bot-tele-removebg-preview.png";
int botRequestDelay = 250;
unsigned long lastTimeBotRan;
String chat_id;
bool ledAState = LOW;
bool ledBState = LOW;
bool buzState = LOW;

int getJarak(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034/2;
  return distance;
}

float getSuhu(String temp){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)) {
    h = 0;
    t = 0;
  }  
  if (temp == "suhu"){
    return t;
  } else if(temp == "humid"){
    return h;
  }
}
void beep(){
  for (int k = 0; k < 2; k++){
    for (int i = 0; i < 3; i++){
      digitalWrite(buz, HIGH);
      delay(20);
      digitalWrite(buz, LOW);
      delay(5);
    }
    delay(50);
  }
}

void handleNewMessages(int numNewMessages) {
  for (int i=0; i<numNewMessages; i++) {
    chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    Serial.println(text);
    String from_name = bot.messages[i].from_name;
    
    if (text == "/start"){
      String welcome = "Hay " + from_name + ".\n";
      welcome += "Selamat datang di Tele-Bot.\n\n";
      welcome += "Bot ini dibuat untuk mengakses sensor dan mengendalikan output melalu chat telegram.\n";
      welcome += "Untuk list perintah bisa ketik /perintah.";
      bot.sendPhoto(chat_id, url, welcome);
    }

    if (text == "/ledAon") {
      bot.sendMessage(chat_id, "LED A state set to ON", "");
      ledAState = HIGH;
      digitalWrite(ledA, ledAState);
    }
    
    if (text == "/ledAoff") {
      bot.sendMessage(chat_id, "LED A state set to OFF", "");
      ledAState = LOW;
      digitalWrite(ledA, ledAState);
    }
    if (text == "/ledBon") {
      bot.sendMessage(chat_id, "LED B state set to ON", "");
      ledBState = HIGH;
      digitalWrite(ledB, ledBState);
    }
    
    if (text == "/ledBoff") {
      bot.sendMessage(chat_id, "LED B state set to OFF", "");
      ledBState = LOW;
      digitalWrite(ledB, ledBState);
    }
    if (text == "/buzon") {
      bot.sendMessage(chat_id, "Buz state set to ON", "");
      buzState = HIGH;
      digitalWrite(buz, buzState);
    }
    
    if (text == "/buzoff") {
      bot.sendMessage(chat_id, "Buz state set to OFF", "");
      buzState = LOW;
      digitalWrite(buz, buzState);
    }
    if (text == "/alloff") {
      bot.sendMessage(chat_id, "All state set to OFF", "");
      buzState = LOW;
      ledAState = LOW;
      ledBState = LOW;
      digitalWrite(ledA, ledAState);
      digitalWrite(ledB, ledBState);
      digitalWrite(buz, buzState);
    }
    if (text == "/allon") {
      bot.sendMessage(chat_id, "All state set to ON", "");
      buzState = HIGH;
      ledAState = HIGH;
      ledBState = HIGH;
      digitalWrite(ledA, ledAState);
      digitalWrite(ledB, ledBState);
      digitalWrite(buz, buzState);
    }
    if (text == "/state") {
      float t = getSuhu("suhu");
      float h = getSuhu("humid");
      int jarak = getJarak();
      String stat = "Status Perangkat.\n";
      stat += "Led A Status: ";
      stat += String(ledAState) + " \n";
      stat += "Led B Status: ";
      stat += String(ledBState) + " \n";
      stat += "Buzzer Status: ";
      stat += String(buzState) + " \n";
      stat += "Jarak Status: ";
      stat += String(jarak) + " CM\n";
      stat += "Suhu Ruangan: ";
      stat += String(t) + "C / " +  String(h) + "%";
      bot.sendMessage(chat_id, stat, "");
    }
    if (text == "/perintah"){
      String cmd = "Berikut ini adalah list perintah yang berlaku.\n\n";
      cmd += "/ledAon : Menyalakan lampu Led A.\n";
      cmd += "/ledBon : Menyalakan lampu Led A.\n";
      cmd += "/ledAoff : Mematikan lampu Led A.\n";
      cmd += "/ledBoff : Mematikan lampu Led A.\n";
      cmd += "/buzon : Menyalakan buzzer.\n";
      cmd += "/buzoff : Mematikan buzzer.\n";
      cmd += "/allon : Menyalakan semua OUTPUT.\n";
      cmd += "/alloff : Mematikan semua OUTPUT.\n";
      cmd += "/state : Melihat semua kondisi OUTPUT dan Sensor.\n";
      bot.sendMessage(chat_id, cmd, "Markdown");
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledA, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(buz, OUTPUT);
  digitalWrite(ledA, LOW);
  digitalWrite(ledB, LOW);
  digitalWrite(buz, LOW);
  
  
  digitalWrite(ledA, HIGH);
  delay(1000);  
  digitalWrite(ledA, LOW);
  digitalWrite(ledB, HIGH);
  delay(1000);  
  digitalWrite(ledB, LOW);
  beep();
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
