#include <Arduino.h>
#include <IRremote.hpp> 

// 定義按鈕引腳
const int BTN_VOL_UP   = 4;
const int BTN_VOL_DOWN = 5;
const int BTN_DEV_1    = 6;
const int BTN_DEV_2    = 12;
const int LED_PIN      = 13; 

// 儲存上一次按鈕的狀態 (預設都是 HIGH，因為是上拉電阻)
bool lastVolUpState   = HIGH;
bool lastVolDownState = HIGH;
bool lastDev1State    = HIGH;
bool lastDev2State    = HIGH;

const uint16_t IR_ADDRESS = 0x00; 

void blinkLED(int times) {
    for (int i = 0; i < times; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(100);
        digitalWrite(LED_PIN, LOW);
        delay(100);
    }
}

void setup() {
    Serial.begin(9600);
    
    pinMode(BTN_VOL_UP, INPUT_PULLUP);
    pinMode(BTN_VOL_DOWN, INPUT_PULLUP);
    pinMode(BTN_DEV_1, INPUT_PULLUP);
    pinMode(BTN_DEV_2, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);

    IrSender.begin(3, DISABLE_LED_FEEDBACK);
    
    Serial.println("--- 遙控器邊緣偵測版已就緒 ---");
}

void loop() {
    // 讀取當前所有按鈕狀態
    bool currentVolUp   = digitalRead(BTN_VOL_UP);
    bool currentVolDown = digitalRead(BTN_VOL_DOWN);
    bool currentDev1    = digitalRead(BTN_DEV_1);
    bool currentDev2    = digitalRead(BTN_DEV_2);

    // 偵測按鈕 1 (音量+)：當現在是 LOW 且上次是 HIGH 時才觸發
    if (currentVolUp == LOW && lastVolUpState == HIGH) {
        Serial.println("發送: 音量+ (0x01)");
        IrSender.sendNEC(IR_ADDRESS, 0x01, 0);
        blinkLED(1);
        delay(50); // 小段延遲避開機械彈跳
    }

    // 偵測按鈕 2 (音量-)
    if (currentVolDown == LOW && lastVolDownState == HIGH) {
        Serial.println("發送: 音量- (0x02)");
        IrSender.sendNEC(IR_ADDRESS, 0x02, 0);
        blinkLED(2);
        delay(50);
    }

    // 偵測按鈕 3 (設備 1)
    if (currentDev1 == LOW && lastDev1State == HIGH) {
        Serial.println("發送: 設備 1 (0x03)");
        IrSender.sendNEC(IR_ADDRESS, 0x03, 0);
        blinkLED(3);
        delay(50);
    }

    // 偵測按鈕 4 (設備 2)
    if (currentDev2 == LOW && lastDev2State == HIGH) {
        Serial.println("發送: 設備 2 (0x04)");
        IrSender.sendNEC(IR_ADDRESS, 0x04, 0);
        blinkLED(4);
        delay(50);
    }

    // 核心關鍵：把現在的狀態存起來，留給下一次循環比較
    lastVolUpState   = currentVolUp;
    lastVolDownState = currentVolDown;
    lastDev1State    = currentDev1;
    lastDev2State    = currentDev2;

    // 稍微喘息一下，增加系統穩定度
    delay(10);
}