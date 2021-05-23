
#include "Arduino.h"
#include "SoftwareSerial.h"        // 採用SoftwareSerial程式庫
#include "DFRobotDFPlayerMini.h"   // 採用DFRobotDFPlayerMini程式庫

#define RX 9
#define TX 10
#define LED1 14 //A0 
#define LED2 15 //A1
#define LED3 16 //A2
#define LED4 17 //A3
#define LED5 18 //A4
#define LED6 12 
#define BUTTON1 8 
#define BUTTON2 7 
#define BUTTON3 6 
#define BUTTON4 5 
#define BUTTON5 4 
#define BUTTON6 3
#define NUM_BUTTON 6
#define KNOB 19 //A5
//#define POWER_RESET 11  //電源板每隔30秒一定要拉低一次，不然會斷電

#define DEBOUNCE_DELAY 50

int buttonState[NUM_BUTTON];
int lastButtonState[NUM_BUTTON];
int buttonPin[NUM_BUTTON] = {BUTTON1, BUTTON2, BUTTON3, BUTTON4, BUTTON5, BUTTON6};
int ledPin[NUM_BUTTON] = {LED1, LED2, LED3, LED4, LED5, LED6};
int mp3_no = 0;
int mp3_no_old = 0;
int new_vol = 0;
int old_vol = 0;
//long iTimeToResetPower;


SoftwareSerial mySoftwareSerial(RX, TX);     // mySoftwareSerial(RX, TX), 宣告軟體序列傳輸埠
DFRobotDFPlayerMini myDFPlayer;         //宣告MP3 Player
void printDetail(uint8_t type, int value);  //印出詳情

void setup() {
    pinMode(BUTTON1, INPUT_PULLUP);   
    pinMode(BUTTON2, INPUT_PULLUP);  
    pinMode(BUTTON3, INPUT_PULLUP);  
    pinMode(BUTTON4, INPUT_PULLUP); 
    pinMode(BUTTON5, INPUT_PULLUP); 
    pinMode(BUTTON6, INPUT_PULLUP); 
    pinMode(LED1, OUTPUT);   
    pinMode(LED2, OUTPUT);  
    pinMode(LED3, OUTPUT);  
    pinMode(LED4, OUTPUT); 
    pinMode(LED5, OUTPUT); 
    pinMode(LED6, OUTPUT); 
    pinMode(KNOB, INPUT);    
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(LED5, LOW);
    digitalWrite(LED6, LOW);    
    
    //pinMode(POWER_RESET, OUTPUT); 
    //digitalWrite(POWER_RESET, LOW); 
    //iTimeToResetPower = millis();
    
    for(int i=0; i<NUM_BUTTON; i++){
      buttonState[i] = false;
      lastButtonState[i] = false;
    }    
    
    mySoftwareSerial.begin(9600); 
    Serial.begin(115200);    
    Serial.println();
    Serial.println(F("DFRobot DFPlayer Mini Demo"));   // 印出DFRobot DFPlayer Mini Demo字串到Serial通訊埠
    Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));    // 以下用法依此類通, 不再贅述喔

    if (!myDFPlayer.begin(mySoftwareSerial, true, false)){       
    //if (!myDFPlayer.begin(mySoftwareSerial)){ 
        Serial.println(F("Unable to begin:"));
        Serial.println(F("1.Please recheck the connection!"));
        Serial.println(F("2.Please insert the SD card!"));
        while (true);                                     
    }       
    Serial.println(F("DFPlayer Mini online."));
    myDFPlayer.stop();
    myDFPlayer.setTimeOut(500); // 設定通訊逾時為500ms
    myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
    myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
    myDFPlayer.enableDAC();     //Enable On-chip DAC

}

void loop(){
  //if(millis() - iTimeToResetPower > 20000){
  //  iTimeToResetPower = millis();
  //  digitalWrite(POWER_RESET, LOW);
  //  delay(1); 
  //  digitalWrite(POWER_RESET, HIGH); 
  //}
  
  int a = analogRead(KNOB);
  //Serial.print("set Vol:"); Serial.print(a); Serial.print("  "); Serial.println(new_vol);
  //return;
  
  if(a<300){
    new_vol = map(a, 180, 300, 0, 10);
  }
  else if(a<600){
    new_vol = map(a, 300, 600, 10, 22);
  }
  else{
    new_vol = map(a, 600, 1000, 22, 25);
  }
  if(new_vol<0) new_vol=0;
  if(new_vol>25) new_vol=25;
  
  if(new_vol != old_vol){
    old_vol = new_vol;
    myDFPlayer.volume(new_vol); 
    Serial.print("set Vol:"); Serial.print(a); Serial.print("  "); Serial.println(new_vol);
  }
  
  //按鈕1~6
  for(int i=0; i<6; i++){  
    buttonState[i] = checkButton(buttonPin[i]);    
  }  
  mp3_no = buttonState[0] + 2*buttonState[1] + 4*buttonState[2] + 
           8*buttonState[3] + 16*buttonState[4] + 32*buttonState[5];
  delay(DEBOUNCE_DELAY);
  
  for(int i=0; i<6; i++){  
    buttonState[i] = checkButton(buttonPin[i]);    
  }  
  int mp3_no_again = buttonState[0] + 2*buttonState[1] + 4*buttonState[2] + 
           8*buttonState[3] + 16*buttonState[4] + 32*buttonState[5];
           
  if(mp3_no!=mp3_no_again) return;  
  
  if (mp3_no>0 && mp3_no != mp3_no_old){ 
    //點亮LED
    for(int i=0; i<6; i++){
      if (buttonState[i]==true)  digitalWrite(ledPin[i], HIGH);
      else  digitalWrite(ledPin[i], LOW);
    }

    mp3_no_old = mp3_no;
    //myDFPlayer.pause();    
    myDFPlayer.stop();
    myDFPlayer.volume(new_vol);
    myDFPlayer.playMp3Folder(mp3_no);
    Serial.print("Play No: "); Serial.println(mp3_no);
    delay(500);
  }  

}

void printDetail(uint8_t type, int value) {
    switch (type) {
    case TimeOut:
        Serial.println(F("Time Out!"));
        break;
    case WrongStack:
        Serial.println(F("Stack Wrong!"));
        break;
    case DFPlayerCardInserted:
        Serial.println(F("Card Inserted!"));
        break;
    case DFPlayerCardRemoved:
        Serial.println(F("Card Removed!"));
        break;
    case DFPlayerCardOnline:
        Serial.println(F("Card Online!"));
        break;
    case DFPlayerPlayFinished:
        Serial.print(F("Number:"));
        Serial.print(value);
        Serial.println(F(" Play Finished!"));
        break;
    case DFPlayerError:
        Serial.print(F("DFPlayerError:"));
        switch (value) {
        case Busy:
            Serial.println(F("Card not found"));
            break;
        case Sleeping:
            Serial.println(F("Sleeping"));
            break;
        case SerialWrongStack:
            Serial.println(F("Get Wrong Stack"));
            break;
        case CheckSumNotMatch:
            Serial.println(F("Check Sum Not Match"));
            break;
        case FileIndexOut:
            Serial.println(F("File Index Out of Bound"));
            break;
        case FileMismatch:
            Serial.println(F("Cannot Find File"));
            break;
        case Advertise:
            Serial.println(F("In Advertise"));
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

bool checkButton(int iPinNo){
  if (!digitalRead(iPinNo)){
    //delay(DEBOUNCE_DELAY);
    if (!digitalRead(iPinNo)){
      return true;       
    }
    else{
      return false;   
    }
  }
  else{
    return false;   
  }
}
