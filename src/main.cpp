#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#define PUSH_BUTTON 19

uint8_t stateControllerAddress[] = {0x3C, 0x61, 0x05, 0x03, 0xD4, 0xB0};//ส่งไปหาเฉพาะ mac address


typedef struct startGame {
  bool start;
  
} startGame;

startGame command;
esp_now_peer_info_t peerInfo;
int count = 0;

//เมื่อส่งข้อมูลมาทำฟังก์ชั่นนี้
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(9600);
  
  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  //ตั้งเป็นโหมด Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // สั่งให้เริ่ม ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  //เมื่อส่งให้ทำฟังก์ชั่น OnDataSend ที่เราสร้างขึ้น
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, stateControllerAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // เชื่อมต่ออุปกรณ์ที่ต้องการสื่อสาร
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  
}

void loop() {
  
  command.start = false;

  if (digitalRead(PUSH_BUTTON) == LOW) {
    count++;
    Serial.printf("%d\n", count);
    command.start = true;
    
    esp_err_t result = esp_now_send(stateControllerAddress, (uint8_t *) &command, sizeof(command));
    
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    } else {
      Serial.println("Error sending the data");
    }
    delay(2000);
    

  }

}

