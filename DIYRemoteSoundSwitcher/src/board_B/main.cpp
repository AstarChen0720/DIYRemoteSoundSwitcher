#include <Arduino.h>
#include <Servo.h>
#include <IRremote.hpp> // 引入紅外線函式庫

Servo myServo;
int currentAngle = 0;   
const int stepAngle = 9; 

// 【新增設定】紅外線接收腳位
const int IR_RECEIVE_PIN = 2; 

const int RELAY_IN1 = 7; 
const int RELAY_IN2 = 8; 
const int LED_PIN   = 13;

void setup() {
    myServo.attach(9);   
    myServo.write(currentAngle); 
    
    pinMode(RELAY_IN1, OUTPUT);
    pinMode(RELAY_IN2, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    
    digitalWrite(RELAY_IN1, LOW);
    digitalWrite(RELAY_IN2, LOW);
    
    Serial.begin(9600);
    
    // 【啟動對講機】初始化紅外線接收器
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    
    Serial.println("--- 綜合控制系統 (含紅外線接收) 已啟動 ---");
    Serial.println("可用電腦指令 (a/d/r/1/2) 或 紅外線遙控器操作");
}

// 將原本重複的控制邏輯包成「執行動作」的函式，方便 Serial 和 IR 共用
void executeAction(char cmd) {
    if (cmd == 'd') {
        currentAngle = min(currentAngle + stepAngle, 180);
        myServo.write(currentAngle);
        Serial.print(">>> 增加音量 ");
    } 
    else if (cmd == 'a') {
        currentAngle = max(currentAngle - stepAngle, 0);
        myServo.write(currentAngle);
        Serial.print(">>> 減少音量 ");
    } 
    else if (cmd == 'r') {
        currentAngle = 0;
        myServo.write(currentAngle);
        Serial.print(">>> 重設音量 ");
    }
    else if (cmd == '1') {
        digitalWrite(RELAY_IN1, LOW);
        digitalWrite(RELAY_IN2, LOW);
        Serial.print(">>> 切換至 [設備 1] ");
    }
    else if (cmd == '2') {
        digitalWrite(RELAY_IN1, HIGH);
        digitalWrite(RELAY_IN2, HIGH);
        Serial.print(">>> 切換至 [設備 2] ");
    }

    // 顯示當前狀態
    Serial.print("| 目前角度: ");
    Serial.print(currentAngle);
    Serial.println(" 度");
}

void loop() {
    // 1. 檢查 Serial（電腦指令）
    if (Serial.available() > 0) {
        char incomingByte = Serial.read();
        executeAction(incomingByte);
    }

    // 2. 檢查 IR（紅外線對講機）
    if (IrReceiver.decode()) {
        // 確保不是雜訊 (UNKNOWN)，且地址是我們設定的 0x00
        if (IrReceiver.decodedIRData.protocol != UNKNOWN && IrReceiver.decodedIRData.address == 0x00) {
            uint16_t command = IrReceiver.decodedIRData.command;
            
            // 根據發射端定義的 0x01 ~ 0x04 進行動作轉換
            switch (command) {
                case 0x01: executeAction('d'); break; // 音量+
                case 0x02: executeAction('a'); break; // 音量-
                case 0x03: executeAction('1'); break; // 設備 1
                case 0x04: executeAction('2'); break; // 設備 2
                default: 
                    Serial.print("收到未知 IR 指令: ");
                    Serial.println(command, HEX);
                    break;
            }
        }
        // 準備接收下一個訊號
        IrReceiver.resume(); 
    }
}