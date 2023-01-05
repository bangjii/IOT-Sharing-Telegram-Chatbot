#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

const char* ssid = "wifiku";
const char* password = "wifi123456";

#define BOTtoken "0000000000:AAA_A11111a111aaaaa11a11aa"
#define ledA 16


X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
int botRequestDelay = 250;
unsigned long lastTimeBotRan;
String chat_id;

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
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/ledAon") {
      bot.sendMessage(chat_id, "LED A state set to ON", "");
      digitalWrite(ledA, HIGH);
    }
    
    if (text == "/ledAoff") {
      bot.sendMessage(chat_id, "LED A state set to OFF", "");
      digitalWrite(ledA, LOW);
    }
  }
}

void setup() {
  Serial.begin(115200);
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());
  pinMode(ledA, OUTPUT);
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
