#include <esp_now.h>
#include <WiFi.h>
#include <WiFiType.h>
#include <ESP32Servo.h>
#include "WiFi.h"

#define LeftLPWM 12
#define LeftRPWM 14
#define rightLPWM 26
#define rightRPWM 25
#define cRPWM 33
#define cLPWM 32
#define ESC 19
#define servo_thuong 18
Servo esc;
Servo thuong;

const int freq = 30000;
const float freq_ESC = 71.18;
const int resolution = 8;
const int RPWM1 = 8;
const int LPWM1 = 9;
const int RPWM2 = 10;
const int LPWM2 = 11;
const int cRPWM1 = 12;
const int cLPWM2 = 13;

int buttonStateA = 0;
int lastButtonStateA = 0;
int buttonStateB = 0;
int lastButtonStateB = 0;
int buttonStateC = 0;
int lastButtonStateC = 0;
bool motorRunningc = false;
bool motorRunningt = false;
int buttonStateD = 0;     // Trạng thái của nút nhấn
int lastButtonStateD = 0; // Trạng thái trước đó của nút nhấn

int angle = 180; // Góc quay ban đầu của servo

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message
{

  int rightButtonAvalue; // SW3 default =1
  int rightButtonBvalue; // Sw4
  int rightButtonCvalue; // SW2
  int rightButtonDvalue; // SW1

  int leftJoyXvalue;
  int leftJoyYvalue;
  int leftJoySWvalue;
} struct_message;

int joystickvaleX;
int joystickvaleY;
int rightbuttonA;
int rightbuttonB;
int rightbuttonC;
int rightbuttonD;
int leftJoySWvalue;

void control(int joystickvaleY, int joystickvaleX)
{
  int motorspeedfowr = map(joystickvaleY, 1910, 4095, 0, 255);
  int motorspeedback = map(joystickvaleY, 1800, 0, 0, 255);
  int speedmotorr = map(joystickvaleX, 1910, 4095, 0, 255);
  int speedmotorl = map(joystickvaleX, 1800, 0, 0, 255);

  if (joystickvaleY > 1910)
  {
    // Forward
    ledcWrite(RPWM1, motorspeedfowr - 10);
    ledcWrite(LPWM1, 0);
    ledcWrite(RPWM2, motorspeedfowr);
    ledcWrite(LPWM2, 0);
    Serial.println("Forward");
  }
  else if (joystickvaleY < 1800)
  {
    // Backward
    ledcWrite(RPWM1, 0);
    ledcWrite(LPWM1, motorspeedback - 10);
    ledcWrite(RPWM2, 0);
    ledcWrite(LPWM2, motorspeedback);
    Serial.println("Backward");
  }
  else
  {
    // Stop
    ledcWrite(RPWM1, 0);
    ledcWrite(LPWM1, 0);
    ledcWrite(RPWM2, 0);
    ledcWrite(LPWM2, 0);
    Serial.println("Stop");
  }

  if (joystickvaleX > 2500)
  {
    // Turn right
    ledcWrite(RPWM1, 0);
    ledcWrite(LPWM1, speedmotorr - 100);
    ledcWrite(RPWM2, speedmotorr);
    ledcWrite(LPWM2, 0);
    Serial.println("Turn Right");
  }
  else if (joystickvaleX < 1000)
  {
    // Turn left
    ledcWrite(RPWM1, speedmotorl);
    ledcWrite(LPWM1, 0);
    ledcWrite(RPWM2, 0);
    ledcWrite(LPWM2, speedmotorl - 100);
    Serial.println("Turn Left");
  }
  else
  {
    // No turn
    Serial.println("No Turn");
  }
}

void cuon_bong(int buttonStateA1, int buttonStateC1)
{
  static unsigned long buttonPressStartTime = 0;
  const int shortPressDuration = 100; // 1 second

  if (buttonStateA1 != lastButtonStateA)
  {
    esc.write(180);
    delay(1000);
    if (buttonStateA1 == 0)
    {
      // Nút vừa được nhấn
      buttonPressStartTime = millis(); // Lưu thời điểm bắt đầu nhấn nút

      if (!motorRunningc)
      {
        Serial.print("t");
        ledcWrite(cRPWM1, 100);
        ledcWrite(cLPWM2, 0); // LedcWrite 110 trong 1s đầu
        motorRunningc = true;
      }
      else
      {
        ledcWrite(cRPWM1, 0);
        ledcWrite(cLPWM2, 0);
        motorRunningc = false;
        Serial.print("nt");
      }
      delay(50); // Chờ để tránh debounce
    }
  }

  // Nếu nút vẫn đang được giữ và đã qua 1s từ lúc nhấn
  if (buttonStateA1 == 0 && millis() - buttonPressStartTime >= shortPressDuration)
  {
    // LedcWrite 50 sau 1s đầu
    ledcWrite(cRPWM1, 80);
    ledcWrite(cLPWM2, 0);
  }
  lastButtonStateA = buttonStateA1;
  if (buttonStateC1 == 0)
  {
    esc.write(90);
    ledcWrite(cRPWM1, 0);
    ledcWrite(cLPWM2, 0);
    motorRunningc = false;
    Serial.print("nt");
  }
}
void tha_bong(int buttonStateB1, int buttonStateC1)
{
  static unsigned long buttonPressStartTime = 0;
  const int shortPressDuration = 100; // 1 second

  if (buttonStateB1 != lastButtonStateB)
  {
    esc.write(180);
    delay(10);
    if (buttonStateB1 == 0)
    {
      // Nút vừa được nhấn
      buttonPressStartTime = millis(); // Lưu thời điểm bắt đầu nhấn nút

      if (!motorRunningt)
      {
        Serial.print("t");
        ledcWrite(cRPWM1, 0);
        ledcWrite(cLPWM2, 90); // LedcWrite 110 trong 1s đầu
        motorRunningt = true;
      }
      else
      {
        ledcWrite(cRPWM1, 0);
        ledcWrite(cLPWM2, 0);
        motorRunningt = false;
        Serial.print("nt");
      }
      delay(50); // Chờ để tránh debounce
    }

  }

  // Nếu nút vẫn đang được giữ và đã qua 1s từ lúc nhấn
  if (buttonStateB1 == 0 && millis() - buttonPressStartTime >= shortPressDuration)
  {
    // LedcWrite 50 sau 1s đầu
    ledcWrite(cRPWM1, 0);
    ledcWrite(cLPWM2, 73);
  }
  lastButtonStateB = buttonStateB1;
  if (buttonStateC1 == 0)
  {
    esc.write(90);
    ledcWrite(cRPWM1, 0);
    ledcWrite(cLPWM2, 0);
    motorRunningt = false;
    Serial.print("nt");
  }
}

void bong_thuong(int buttonStateD1)
{
  static bool isButtonPressed = false; // Biến để theo dõi trạng thái nhấn giữ
  buttonStateD = buttonStateD1;        // Đọc trạng thái của nút nhấn

  if (buttonStateD == LOW)
  { // Nếu nút nhấn được bấm
    if (!isButtonPressed)
    {
      if (angle == 180)
      {             // Nếu góc quay hiện tại của servo là 180 độ
        angle = 70; // Đặt góc quay mới là 70 độ
      }
      else if (angle == 5)
      {             // Nếu góc quay hiện tại của servo là 0 độ
        angle = 70; // Đặt góc quay mới là 70 độ
      }
      else if (angle == 70)
      {            // Nếu góc quay hiện tại của servo là 70 độ
        angle = 5; // Đặt góc quay mới là 5 độ
      }
      thuong.write(angle);    // Gửi yêu cầu servo xoay đến góc quay mới
      delay(500);             // Đợi 0.5 giây để servo hoàn thành quá trình xoay
      isButtonPressed = true; // Đánh dấu là nút đang được giữ
    }
  }
  else
  {
    isButtonPressed = false; // Đánh dấu là nút đã được thả ra
  }
}

// Create a struct_message called myData
struct_message readingData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&readingData, incomingData, sizeof(readingData));
  Serial.print("Bytes received: ");
  Serial.println(len);

  joystickvaleX = readingData.leftJoyXvalue;
  joystickvaleY = readingData.leftJoyYvalue;
  rightbuttonA = readingData.rightButtonAvalue;
  rightbuttonB = readingData.rightButtonBvalue;
  rightbuttonC = readingData.rightButtonCvalue;
  rightbuttonD = readingData.rightButtonDvalue;
  leftJoySWvalue = readingData.leftJoySWvalue;

  control(joystickvaleY, joystickvaleX);
  cuon_bong(rightbuttonA, rightbuttonC);
  tha_bong(rightbuttonD, rightbuttonC);
  bong_thuong(leftJoySWvalue);
}

void setup()
{
  // Initialize Serial Monitor
  Serial.begin(115200);

  pinMode(LeftLPWM, OUTPUT);
  pinMode(LeftRPWM, OUTPUT);
  pinMode(rightLPWM, OUTPUT);
  pinMode(rightRPWM, OUTPUT);
  pinMode(cRPWM, OUTPUT);
  pinMode(cLPWM, OUTPUT);

  ledcSetup(RPWM1, freq, resolution);
  ledcSetup(LPWM1, freq, resolution);
  ledcSetup(RPWM2, freq, resolution);
  ledcSetup(LPWM2, freq, resolution);
  ledcSetup(cRPWM1, freq, resolution);
  ledcSetup(cLPWM2, freq, resolution);

  ledcAttachPin(rightRPWM, RPWM1);
  ledcAttachPin(rightLPWM, LPWM1);
  ledcAttachPin(LeftRPWM, RPWM2);
  ledcAttachPin(LeftLPWM, LPWM2);
  ledcAttachPin(cRPWM, cRPWM1);
  ledcAttachPin(cLPWM, cLPWM2);

  esc.setPeriodHertz(50);      // Đặt tần số cho ESC
  esc.attach(ESC, 500, 2400); //  pulse width từ 1000 đến 2000
  esc.write(180);
  thuong.setPeriodHertz(50);
  thuong.attach(servo_thuong, 500, 2400);
  thuong.write(180);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{
}
