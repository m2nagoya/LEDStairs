#include <MsTimer2.h>
#define HFSER1  (12)
#define HFSER2  (13)
#define HUESER  (0)
#define MAX_CH 2
#define TIMERSTOP     1000
#define TIMERLIGHTON  1000
#define TIMERLIGHTOFF 1000
#define LIMITHUE 200
#define COOLTIME 3000

const uint8_t SRCLK[MAX_CH] = { 10 , 4};
const uint8_t RCLK[MAX_CH]  = { 9 , 3};
const uint8_t SER[MAX_CH]   = { 8 , 2};

volatile boolean tim1msF; // タイマー 10msフラグ
volatile boolean LightOn1;
volatile boolean LightOn2;
volatile boolean LightOff1;
volatile boolean LightOff2;
volatile boolean LightStop;
volatile boolean EXIT1 = false;
volatile boolean EXIT2 = false;
int tim1msCnt = 0;

void setup() {
  pinMode(HFSER1, INPUT);
  pinMode(HFSER2, INPUT);
  for(int ch = 0;ch < MAX_CH;ch++){
     pinMode(SRCLK[ch], OUTPUT);
     pinMode(RCLK[ch],  OUTPUT);
     pinMode(SER[ch],   OUTPUT);
  }
  pinMode(HUESER, INPUT);

  MsTimer2::set(1, timer1mS);
  MsTimer2::start();
}
const uint16_t PATTERNS1[] = {
  0b111111000000000,
  0b111111000000000,
  0b111111000000000,
  0b111111000000000,  
  0b111111000000000,
  0b0111111000000000,
  0b0011111100000000,
  0b0001111110000000,
  0b0000111111000000,
  0b0000011111100000,
  0b0000001111110000,
  0b0000000111111000,
  0b0000000011111100,
  0b0000000011111100,
  0b0000000011111100,
  0b0000000011111100,
  0b0000000011111100,
  0b0000000000000000,
};
const uint16_t PATTERNS2[] = {
  0b0000000000000000,
};
const uint16_t PATTERNS3[] = {
  0b0000000011111100,
  0b0000000011111100,
  0b0000000011111100,
  0b0000000011111100,
  0b0000000011111100,
  0b0000000111111000,
  0b0000001111110000,
  0b0000011111100000,
  0b0000111111000000,
  0b0001111110000000,
  0b0011111100000000,
  0b0111111000000000,
  0b1111110000000000,
  0b1111110000000000,
  0b1111110000000000,
  0b1111110000000000,
  0b1111110000000000,
  0b0000000000000000,
};
const uint16_t PATTERNS4[] = {
  0b0000000000000000,
};

void FNC_LightOn1(uint8_t j) {
   for(int ch = 0;ch < MAX_CH;ch++){
     shiftOut(SER[ch], SRCLK[ch], LSBFIRST, PATTERNS1[j] >> (8*ch)); 
   }
   for(int ch = 0;ch < MAX_CH;ch++){
      digitalWrite(RCLK[ch],  LOW);
      digitalWrite(RCLK[ch],  HIGH);
   }
}
void FNC_LightOff1(uint8_t j) {
   for(int ch = 0;ch < MAX_CH;ch++){
     shiftOut(SER[ch], SRCLK[ch], LSBFIRST, PATTERNS2[j] >> (8*ch)); 
   }
   for(int ch = 0;ch < MAX_CH;ch++){
      digitalWrite(RCLK[ch],  LOW);
      digitalWrite(RCLK[ch],  HIGH);
   }
}
void FNC_LightOn2(uint8_t j) { 
   for(int ch = 0;ch < MAX_CH;ch++){
     shiftOut(SER[ch], SRCLK[ch], LSBFIRST, PATTERNS3[j] >> (8*ch)); 
   }
   for(int ch = 0;ch < MAX_CH;ch++){
      digitalWrite(RCLK[ch],  LOW);
      digitalWrite(RCLK[ch],  HIGH);
   }
}
void FNC_LightOff2(uint8_t j) {
   for(int ch = 0;ch < MAX_CH;ch++){
     shiftOut(SER[ch], SRCLK[ch], LSBFIRST, PATTERNS4[j] >> (8*ch)); 
   }
   for(int ch = 0;ch < MAX_CH;ch++){
      digitalWrite(RCLK[ch],  LOW);
      digitalWrite(RCLK[ch],  HIGH);
   }
}

long int timer = 0;
uint8_t i=0;
int sum = 0;
void loop() {
  int max_pattern1 = sizeof(PATTERNS1)/sizeof(PATTERNS1[0]); 
  int max_pattern2 = sizeof(PATTERNS2)/sizeof(PATTERNS2[0]);
  int max_pattern3 = sizeof(PATTERNS3)/sizeof(PATTERNS3[0]); 
  int max_pattern4 = sizeof(PATTERNS4)/sizeof(PATTERNS4[0]);
  int HFValue1 = digitalRead(HFSER1);
  int HFValue2 = digitalRead(HFSER2);
  int HUEValue = analogRead(HUESER);
  if (!LightOn2 && HFValue1 == 1 && HUEValue < LIMITHUE) {
    LightOn1 = true;
  }
  if (!LightOn1 && HFValue2 == 1 && HUEValue < LIMITHUE) {
    LightOn2 = true;
  }
  if (LightOn1 && HFValue2 == 1 && !EXIT1) {
    i = 0; timer = 0; EXIT1 = true;
  }
  if (LightOn2 && HFValue1 == 1 && !EXIT2) {
    i = 0; timer = 0; EXIT2 = true;
  }
  if (LightOn1 && !LightOn2 && !EXIT1 && !EXIT2) {
    if (LightOn1 && !LightOff1 && !LightStop && tim1msF) {
      timer++;
      if (timer > TIMERLIGHTON) {
         if (i < max_pattern1){
            FNC_LightOn1(i++);
         } else {
           i = 0;
           LightStop = true;
         }
         timer = 0;
      }
      tim1msF = false ;
    }  
    else if (LightOn1 && !LightOff1 && LightStop && tim1msF) {
      timer++;
      if(timer > TIMERSTOP) {
        LightOff1 = true;
        timer = 0;
      }
      tim1msF = false ;
    }
    else if (LightOn1 && LightOff1 && LightStop && tim1msF) {
      timer++;
      if (timer > TIMERLIGHTOFF){
        if (i < max_pattern2){
          FNC_LightOff1(i++);
        } else {
          i = 0;
          LightOn1  = false;
          LightOff1 = false;
          LightStop = false;
        }
        timer = 0;
      }
      tim1msF = false ;
    }
  } else if (!LightOn1 && LightOn2 && !EXIT1 && !EXIT2) {
      if (LightOn2 && !LightOff2 && !LightStop && tim1msF) {
        timer++;
        if (timer > TIMERLIGHTON) {
           if (i < max_pattern3){
              FNC_LightOn2(i++);
           } else {
             i = 0;
             LightStop = true;
             delay(COOLTIME);
           }
           timer = 0;
        }
        tim1msF = false ;
      }  
      else if (LightOn2 && !LightOff2 && LightStop && tim1msF) {
        timer++;
        if(timer > TIMERSTOP) {
          LightOff2 = true;
          timer = 0;
        }
        tim1msF = false ;
      }
      else if (LightOn2 && LightOff2 && LightStop && tim1msF) {
        timer++;
        if (timer > TIMERLIGHTOFF){
          if (i < max_pattern4){
            FNC_LightOff2(i++);
          } else {
            i = 0;
            LightOn2  = false;
            LightOff2 = false;
            LightStop = false;
            delay(COOLTIME);
          }
          timer = 0;
        }
        tim1msF = false ;
      }
  } else if (EXIT1 && tim1msF) {
    timer++;
    if (timer > TIMERLIGHTOFF){
      if (i < max_pattern2){
        FNC_LightOff1(i++);
      } else {
        i = 0;
        LightOn1  = false; LightOff1 = false;
        LightOn2  = false; LightOff2 = false;
        LightStop = false;
        EXIT1 = false;
        delay(COOLTIME);
      }
      timer = 0;
    }
    tim1msF = false ;
  } else if (EXIT2 && tim1msF) {
    timer++;
    if (timer > TIMERLIGHTOFF){
      if (i < max_pattern4){
        FNC_LightOff2(i++);
      } else {
        i = 0;
        LightOn1  = false; LightOff1 = false;
        LightOn2  = false; LightOff2 = false;
        LightStop = false;
        EXIT2 = false;
        delay(COOLTIME);
      }
      timer = 0;
    }
    tim1msF = false ;
  }
}

void timer1mS() {
  tim1msF = true; // 1mSec毎
}
