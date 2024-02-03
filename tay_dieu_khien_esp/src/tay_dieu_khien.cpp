#include <esp_now.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiType.h>
// #include <Arduino.h>

#define rightButtonA 27
#define rightButtonB 26
#define rightButtonC 18
#define rightButtonD 19  

#define leftJoyX 35
#define leftJoyY 32
#define leftJoySW 25

int lcdColumns = 16;
int lcdRows = 2;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

int rightButtonAstate;
int rightButtonBstate;
int rightButtonCstate;
int rightButtonDstate;

int leftJoyXstate;
int leftJoyYstate;
int leftJoySWstate;

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xC8 , 0xF0 ,0x9E , 0x72 , 0xFA , 0x80};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message
{
  int rightButtonAvalue;
  int rightButtonBvalue;
  int rightButtonCvalue;
  int rightButtonDvalue;

  int leftJoyXvalue;
  int leftJoyYvalue;
  int leftJoySWvalue;
} struct_message;

// Create a struct_message called myData
struct_message handControllerData;

esp_now_peer_info_t peerInfo;

void scrollText(int row, String message, int delayTime, int lcdColumns)
{
  for (int i = 0; i < lcdColumns; i++)
  {
    message = " " + message;
  }
  message = message + " ";
  for (int pos = 0; pos < message.length(); pos++)
  {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
  }
}
// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  lcd.setCursor(0, 1);
  lcd.print(status == ESP_NOW_SEND_SUCCESS ? " Delivery Success         " : " Delivery Fail        ");
  // scrollText(1, status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail",2,lcdColumns);
}


void setup()
{
  // Init Serial Monitor
  Serial.begin(115200);

  pinMode(rightButtonA, INPUT);
  pinMode(rightButtonB, INPUT);
  pinMode(rightButtonC, INPUT);
  pinMode(rightButtonD, INPUT);

  pinMode(leftJoyX, INPUT);
  pinMode(leftJoyY, INPUT);
  pinMode(leftJoySW, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("AML XIN CHAO!");

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop()
{
  // Set values to send

  rightButtonAstate = digitalRead(rightButtonA);
  handControllerData.rightButtonAvalue = rightButtonAstate;
  rightButtonBstate = digitalRead(rightButtonB);
  handControllerData.rightButtonBvalue = rightButtonBstate;
  rightButtonCstate = digitalRead(rightButtonC);
  handControllerData.rightButtonCvalue = rightButtonCstate;
  rightButtonDstate = digitalRead(rightButtonD);
  handControllerData.rightButtonDvalue = rightButtonDstate;
  Serial.println(rightButtonDstate);

  leftJoyXstate = analogRead(leftJoyX);
  Serial.print("leftJoyXstate:   ");
  Serial.println(leftJoyXstate);
  handControllerData.leftJoyXvalue = leftJoyXstate;

  leftJoyYstate = analogRead(leftJoyY);
  Serial.print("leftJoyYstate:   ");
  Serial.println(leftJoyYstate);
  handControllerData.leftJoyYvalue = leftJoyYstate;
  
  leftJoySWstate = digitalRead(leftJoySW);
  handControllerData.leftJoySWvalue = leftJoySWstate;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&handControllerData, sizeof(handControllerData));

  if (result == ESP_OK)
  {
    Serial.println("Sent with success");
  }
  else
  {
    Serial.println("Error sending the data");
  }
  delay(50);
}
