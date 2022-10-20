/*
 * M5StickC_TVremote
 * 赤外線リモコン
 * 
 * 必要なライブラリ
 * https://github.com/SensorsIot/Arduino-IRremote
 * https://github.com/tobozo/M5Stack-SD-Updater
 * 
 * 警告をすべてにしているとIRのライブラリでエラーが出るので、警告を初期値にしてビルド 2020/11/13
 */

#include <M5StickC.h>
#include <IRremote.h>
#include <M5StackUpdater.h> // M5Stack SD-Updater

byte SEND_PIN = 9; // IR:9
IRsend irsend(SEND_PIN);

float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;
float gyroX = 0.0F;
float gyroY = 0.0F;
float gyroZ = 0.0F;
float pitch = 0.0F;
float roll  = 0.0F;
float yaw   = 0.0F;

void setup()
{
  M5.begin();

  //M5Stack SD-Updater
  if(digitalRead(BUTTON_A_PIN) == 0) {
    //Serial.println("Will Load menu binary");

    //updateFromFS(SD);
    updateFromFS(SPIFFS);
    ESP.restart();
  }
  
  M5.Lcd.fillScreen(BLACK);

  M5.Axp.ScreenBreath(9); //7 - 16 暗めにしたい

  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(4, 4);
  M5.Lcd.print("M5C TVremote");

  M5.Lcd.setCursor(4, 20);
  M5.Lcd.print("UP");
  M5.Lcd.setCursor(4, 30);
  M5.Lcd.print("DOWN\n (tilt left)");

  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(4, 140);
  M5.Lcd.print("VOLUME");

  M5.Lcd.setCursor(50, 70);
  M5.Lcd.print("CH");

  Serial.begin(115200);

  // LED
  pinMode(M5_LED, OUTPUT);
  digitalWrite(M5_LED, LOW); //これで点灯
  digitalWrite(M5_LED, HIGH); //これで消灯

  // 5V OUT OFF  https://lang-ship.com/blog/work/m5stickc-power-saving/
  Wire1.beginTransmission(0x34);
  Wire1.write(0x10);
  Wire1.endTransmission();
  Wire1.requestFrom(0x34, 1);
  uint8_t state = Wire1.read() & ~(1 << 2);
  Wire1.beginTransmission(0x34);
  Wire1.write(0x10);
  Wire1.write(state);
  Wire1.endTransmission();
  // CPU周波数下げる
  setCpuFrequencyMhz(80);
  
  M5.IMU.Init();
}

void loop() {
  M5.update();
  
  // データ取得
  M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);
  M5.IMU.getAccelData(&accX, &accY, &accZ);
  M5.IMU.getAhrsData(&pitch, &roll, &yaw);
  
  if(M5.BtnA.wasPressed() || M5.BtnB.wasPressed()){
    Serial.printf("------------------ \n");
    Serial.printf("gyroX,gyroY,gyroZ\t");
    Serial.printf("%6.2f,%6.2f,%6.2f\n", gyroX, gyroY, gyroZ);
    Serial.printf("accX,accY,accZ\t\t");
    Serial.printf("%5.2f,%5.2f,%5.2f\n", accX, accY, accZ);
    Serial.printf("pitch,roll,yaw\t\t");
    Serial.printf("%5.2f,%5.2f,%5.2f\n\n", pitch, roll, yaw);
  }
  
  if(M5.BtnB.wasPressed()){
    digitalWrite(M5_LED, LOW);  //LED ON

    if(pitch < -40){
      Serial.println("CH DOWN");
      irsend.sendNEC(0x02fdf807, 32); //東芝REGZA CH DOWN 02FDF807 
      delay(30);
      irsend.sendSony(0x555AF1480900F6FF, 64); //SONY     555AF1480900F6FF
      delay(30);
      irsend.sendPanasonic(0x4004, 0x0100ACAD); //Panasonic 40040100ACAD
      delay(30);
      irsend.sendPanasonic(0x555A, 0xF1484889); //SHARP     555AF1484889
      delay(30);
      irsend.sendNEC(0x0AF518E7, 32); //HITACHI 0AF518E7

    } else if(pitch < 40){
      Serial.println("CH UP");
      irsend.sendNEC(0x02fdd827, 32); //東芝REGZA CH UP 02FDD827 
      delay(30);
      irsend.sendSony(0x555AF148890076FF, 64); //SONY     555AF148890076FF
      delay(30);
      irsend.sendPanasonic(0x4004, 0x01002C2D); //Panasonic 400401002C2D
      delay(30);
      irsend.sendPanasonic(0x555A, 0xF1488885); //SHARP     555AF1488885
      delay(30);
      irsend.sendNEC(0x0AF59867, 32); //HITACHI 0AF59867

    } else {  // 入力切り替え
      Serial.println("INPUT SELECT");
      irsend.sendNEC(0x02fdf00f, 32); //東芝REGZA INPUT SELECT 02FDF00F 
      delay(30);
      irsend.sendSony(0x555AF148A5005AFF, 64); //SONY     555AF148A5005AFF
      delay(30);
      irsend.sendPanasonic(0x4004, 0x0100A0A1); //Panasonic 40040100A0A1
      delay(30);
      irsend.sendPanasonic(0x555A, 0xF148C881); //SHARP     555AF148C881
      delay(30);
      irsend.sendNEC(0x0AF5906F, 32); //HITACHI 0AF5906F
    }
    digitalWrite(M5_LED, HIGH);  //LED OFF
  }
  
  if(M5.BtnA.wasPressed()){
      digitalWrite(M5_LED, LOW);  //LED ON

    if(pitch > -40){  
      // VOL UP 音量アップ
      Serial.println("VOL UP");
  
      irsend.sendNEC(0x02fd58a7, 32); //東芝REGZA VOLUME UP 58a7 
      delay(30);
      irsend.sendSony(0x555AF1484900B6FF, 64); //SONY     555AF1484900B6FF
      delay(30);
      irsend.sendPanasonic(0x4004, 0x01000405); //Panasonic 400401000405
      delay(30);
      irsend.sendPanasonic(0x555A, 0xF148288F); //SHARP     555AF148288F
      delay(30);
      irsend.sendNEC(0x0AF548B7, 32); //HITACHI 0AF548B7

    } else {

      //VOL DOWN 音量ダウン
      Serial.println("VOL DOWN");
  
      irsend.sendNEC(0x02fd7887, 32); //東芝REGZA VOL DOWN 7887
      delay(30);
      irsend.sendSony(0x555AF148C90036FF, 64); //SONY     555AF148C90036FF
      delay(30);
      irsend.sendPanasonic(0x4004, 0x01008485); //Panasonic 400401008485
      delay(30);
      irsend.sendPanasonic(0x555A, 0xF148A887); //SHARP     555AF148A887
      delay(30);
      irsend.sendNEC(0x0AF5A857, 32); //HITACHI 0AF5A857
    }
      digitalWrite(M5_LED, HIGH);  //LED OFF
  }
  delay(1);
}
