/* 
   1 - GND
   2 - VCC 3.3V !!! NOT 5V
   3 - CE to Arduino pin 9
   4 - CSN to Arduino pin 10
   5 - SCK to Arduino pin 13
   6 - MOSI to Arduino pin 11
   7 - MISO to Arduino pin 12
   8 - UNUSED 
*/
#include <SPI.h>
#include <DigitalIO.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <avr/wdt.h>

#define CE_PIN   9
#define CSN_PIN 10

const uint64_t pipe01 = 0xE8E8F0F0E1LL; // идентификатор передачи, "труба"
                      //0xE8E8F0F0E1LL
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

byte data;
int strob1;
#define sizeBuff 40
#define maxBuff 20
int ArrayStrob1[sizeBuff];
int ArrayData[sizeBuff];
int arraytemp[maxBuff];
int arrayind1[maxBuff];
int arrayind2[maxBuff];
int arrayind3[maxBuff];
int pretemp = 99;
int temp = 0;
int temp1 = 0;
int temp2 = 0;
int ind1 = 0;
int ind2 = 0;
int ind3 = 0;
int dl = 0;
int Transmit[4]; //передача
int Receive[2]; //прием
int button;
int premaxtemp = 0;
///////////////////////////////////////////////////////////////////////////////
void setup(){ 
  wdt_enable(WDTO_4S);
  Serial.end();                                                                                                                                                                      
  radio.begin();
  //radio.setChannel(9); // канал (0-127)
  // radio.setDataRate(RF24_250KBPS);    
  //radio.setPALevel(RF24_PA_HIGH);   
  //  radio.openWritingPipe(pipe01); // открываем трубу на передачу.
  radio.openReadingPipe(1,pipe01); //для приема
  radio.startListening();; //для приема
//DDRC = 0b00000000;
pinMode(14, INPUT);
pinMode(15, INPUT);
pinMode(16, OUTPUT);
pinMode(17, OUTPUT);
pinMode(18, OUTPUT);
digitalWrite(16, 1);
digitalWrite(17, 1);
digitalWrite(18, 1);
DDRD = B00000000; //настройка всех выводов порта D как входов
PORTD = B00000000;
}
/////////////////////////////////////////////////////////////////////
void loop(){
  Priem();
  //Buffer(); //Снимаем показания с водонагревателя
}

///////////////////////////////////////////////////////////////////////
void Buffer(){
  wdt_reset();
for (int m = 0; m < maxBuff; m++) {  
  for (int i = 0; i < sizeBuff; i++) {
    ArrayStrob1[i] = (PINC & (1<<0));
    ArrayData[i] = PIND;
     delay(1); //38
  }
  Priem();
      for (int x = 0; x < sizeBuff - 1; x++) { 
       int tempbuf = ArrayData[x];
       strob1 = ArrayStrob1[x];
             if (tempbuf == 144 && strob1 == 1){temp1 = 0; } 
        else if (tempbuf == 255 && strob1 == 1){temp1 = 0; } 
        else if (tempbuf == 221 && strob1 == 1){temp1 = 1; } 
        else if (tempbuf == 161 && strob1 == 1){temp1 = 2; }
        else if (tempbuf == 137 && strob1 == 1){temp1 = 3; }
        else if (tempbuf == 204 && strob1 == 1){temp1 = 4; }
        else if (tempbuf == 138 && strob1 == 1){temp1 = 5; }
        else if (tempbuf == 130 && strob1 == 1){temp1 = 6; }
        else if (tempbuf == 157 && strob1 == 1){temp1 = 7; }
        else if (tempbuf == 128 && strob1 == 1){temp1 = 8; }
        else if (tempbuf == 136 && strob1 == 1){temp1 = 9; }
        else if (tempbuf == 162){temp1 = 9; }
        else if (tempbuf == 144 && strob1 == 0){temp2 = 0; }  
        else if (tempbuf == 221 && strob1 == 0){temp2 = 1; } 
        else if (tempbuf == 161 && strob1 == 0){temp2 = 2; }
        else if (tempbuf == 137 && strob1 == 0){temp2 = 3; }
        else if (tempbuf == 204 && strob1 == 0){temp2 = 4; }
        else if (tempbuf == 138 && strob1 == 0){temp2 = 5; }
        else if (tempbuf == 130 && strob1 == 0){temp2 = 6; }
        else if (tempbuf == 157 && strob1 == 0){temp2 = 7; }
        else if (tempbuf == 128 && strob1 == 0){temp2 = 8; }
        else if (tempbuf == 136 && strob1 == 0){temp2 = 9; }
        
        else if (tempbuf == 253){
            ind1 = 1;
            ind2 = 0;
            ind3 = 0;
        }
        else if (tempbuf == 191){
            ind1 = 0;
            ind2 = 1;
            ind3 = 0;
        }
        else if (tempbuf == 254){
            ind1 = 0;
            ind2 = 0;
            ind3 = 1;
        }
        if (tempbuf == 255){
            ind1 = 0; 
            ind2 = 0; 
            ind3 = 0;
        }
        String Strtemp =  String(temp1) + String(temp2);
        temp = Strtemp.toInt();    
        }        
 ////////////     
        if (temp > 15){arraytemp[m] = temp;}
        arrayind1[m] = ind1;
        arrayind2[m] = ind2;
        arrayind3[m] = ind3;  
       } 
 Priem();
 //////////////
    int maxtemp=arraytemp[0]; // нулевой элемент будет.
    int maxind1=arrayind1[0];
    int maxind2=arrayind2[0];
    int maxind3=arrayind3[0];
    for (int mx = 1; mx < 10; mx++) // начинаем с 1, так как нулевой уже приняли за max
      {
         // if (arraytemp[mx] >= maxtemp){maxtemp = arraytemp[mx];}
          if (arrayind1[mx] >= maxind1){maxind1 = arrayind1[mx];}
          if (arrayind2[mx] >= maxind2){maxind2 = arrayind2[mx];}
          if (arrayind3[mx] >= maxind3){maxind3 = arrayind3[mx];}
      }
   wdt_reset();
//Находим максимальные значения для отсеивания мусора
  int confidence = 0;
  int* candidate = NULL;
  for (int i = 0; i<maxBuff; i++) {
    if (confidence == 0) {
      candidate = arraytemp+i;
      confidence++;
    }
    else if (*candidate == arraytemp[i]) {confidence++;}
    else {confidence--;}
  }
  
  Priem();
    if (confidence > 0 && candidate[1] > 15){
      maxtemp = candidate[1];
    }
        Transmit[0] = maxtemp;
        Transmit[1] = maxind1;
        Transmit[2] = maxind2;
        Transmit[3] = maxind3;
        radio.openWritingPipe(pipe01);
        radio.stopListening(); 
        radio.write(Transmit, sizeof(Transmit));
        radio.startListening(); 
}

void Obmen(){
  long int var = 0;
    while(var < 10){
      Buffer(); //Снимаем показания 
      Priem();  
      var++;
    }
}
void Priem(){
  wdt_reset();
    uint8_t pipeNum = 0; 
        if (radio.available(&pipeNum)){ // проверяем не пришло ли чего в буфер.
          if (pipeNum == 1){
            radio.read(&button, sizeof(button)); 
            if (button == 110){
              digitalWrite(16, 0);
              delay(200);
              digitalWrite(16, 1);
              Obmen();
            }
            else if (button == 220){
              digitalWrite(17, 0);
              delay(200);
              digitalWrite(17, 1);
              Obmen();
            }
            else if (button == 330){
              digitalWrite(18, 0);
              delay(200);
              digitalWrite(18, 1);
              Obmen();
            }
            else if (button == 777){
            Buffer();
            }
         }
        }
    }
