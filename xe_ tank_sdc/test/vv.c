#include <esp_now.h>
#include <WiFi.h>
#include <WiFiType.h>
#include <ESP32Servo.h>
#include "WiFi.h"

#define LeftLPWM 12
#define LeftRPWM 14
#define rightLPWM 26
#define rightRPWM 25
#define cRPWM 32
#define cLPWM 33
#define ESC 19
Servo esc;

const int freq = 30000;
const float freq_ESC = 71.18;
const int resolution = 8;
const int RPWM1 = 1;
const int LPWM1 = 2;
const int RPWM2 = 3;
const int LPWM2 = 4;
const int cRPWM1 = 5;
const int cLPWM2 = 6;

int buttonStateA = 0;
int lastButtonStateA = 0;
int buttonStateB = 0;
int lastButtonStateB = 0;
bool motorRunning = false;

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
    if (joystickvaleY > 2000)
    {
        if (x > 1000 && x < 3000) // Đi tiến thẳng
        {
            ledcWrite(RPWM1, motorspeedfowr);
            ledcWrite(LPWM1, 0);
            ledcWrite(RPWM2, motorspeedfowr);
            ledcWrite(LPWM2, 0);
            Serial.print("tien");
            Serial.println();
        }
        if (x < 1000) // Đi tiến và rẽ trái
        {
            ledcWrite(RPWM1, motorspeedfowr);
            ledcWrite(LPWM1, 0);
            ledcWrite(RPWM2, 0);
            ledcWrite(LPWM2, motorspeedfowr - 100);
            Serial.print("trai tien");
            Serial.println();
        }
        if (x > 3000) // Đi tiến và rẽ phải
        {
            ledcWrite(RPWM1, 0);
            ledcWrite(LPWM1, motorspeedfowr - 100);
            ledcWrite(RPWM2, motorspeedfowr);
            ledcWrite(LPWM2, 0);
            Serial.print("phai tien");
            Serial.println();
        }
    }
    else if (y < 1800)
    {
        if (x > 1000 && x < 3000) // Đi lùi thẳng
        {
            ledcWrite(RPWM1, 0);
            ledcWrite(LPWM1, motorspeedback);
            ledcWrite(RPWM2, 0);
            ledcWrite(LPWM2, motorspeedback);
            Serial.print("lui ");
            Serial.println();
        }
        if (x < 1000) // Đi lùi và rẽ trái
        {
            ledcWrite(RPWM1, 0);
            ledcWrite(LPWM1, motorspeedback);
            ledcWrite(RPWM2, motorspeedback - 100);
            ledcWrite(LPWM2, 0);
            Serial.print("lui trai");
            Serial.println();
        }
        if (x > 3000) // Đi lùi và rẽ phải
        {
            ledcWrite(RPWM1, motorspeedback - 100);
            ledcWrite(LPWM1, 0);
            ledcWrite(RPWM2, 0);
            ledcWrite(LPWM2, motorspeedback);
            Serial.print("lui phai");
            Serial.println();
        }
    }
    else if (y > 1500 && y < 2300) // Đứng yên
    {
        ledcWrite(RPWM1, 0);
        ledcWrite(LPWM1, 0);
        ledcWrite(RPWM2, 0);
        ledcWrite(LPWM2, 0);
        Serial.print("stop");
        Serial.println();
    }
}
void cuon_bong(int buttonStateA1)
{
    static unsigned long buttonPressStartTime = 0;
    const int shortPressDuration = 100; // 1 second

    if (buttonStateA1 != lastButtonStateA)
    {
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

                // Kiểm tra trạng thái của ESC
                if (esc.read() == 0)
                {
                    esc.write(180); // Nếu servo ở vị trí 0 độ, di chuyển đến 180 độ
                }
                else
                {
                    esc.write(0); // Nếu servo ở vị trí khác 0 độ, di chuyển về 0 độ
                }
            }
            delay(50); // Chờ để tránh debounce
        }
    }

    // Nếu nút vẫn đang được giữ và đã qua 1s từ lúc nhấn
    if (buttonStateA1 == 0 && millis() - buttonPressStartTime >= shortPressDuration)
    {
        // LedcWrite 50 sau 1s đầu
        ledcWrite(cRPWM1, 70);
        ledcWrite(cLPWM2, 0);
    }
    lastButtonStateA = buttonStateA1;
    if (buttonStateC1 == 0)
    {
        ledcWrite(cRPWM1, 0);
        ledcWrite(cLPWM2, 0);
        motorRunningc = false;
        Serial.print("nt");
    }
}
void dong_cua(int buttonStateC1)
{
    if (buttonStateC1 == 0)
    {
        esc.write(180);
    }
}

void tha_bong(int buttonStateB1)
{
    if (buttonStateB1 != lastButtonStateB)
    {
        if (buttonStateB1 == 0)
        {
            // Nút vừa được nhấn
            if (esc.read() == 0)
            {
                esc.write(180); // Nếu servo ở vị trí 0 độ, di chuyển đến 180 độ
                ledcWrite(cRPWM1, 100);
                ledcWrite(cLPWM2, 0);
            }
            else
            {
                esc.write(0); // Nếu servo ở vị trí khác 0 độ, di chuyển về 0 độ
                ledcWrite(cRPWM1, 0);
                ledcWrite(cLPWM2, 100);
            }
            delay(50); // Chờ để tránh debounce
        }
    }

    lastButtonStateB = buttonStateB1;
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
    Serial.println(rightbuttonA);

    control(joystickvaleY, joystickvaleX);
    cuon_bong(0);
    // tha_bong(rightbuttonB);
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
    esc.attach(ESC, 1000, 2000); //  pulse width từ 1000 đến 2000

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
    tha_bong(1);
    delay(1000);
    tha_bong(0);
}
