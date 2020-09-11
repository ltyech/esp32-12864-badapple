#include "SPIFFS.h"
#include <U8g2lib.h>
#include <Wire.h>

#define LCD_WIDTH 128
#define LCD_HEIGHT 64
#define WIDTH_COUNT 9
#define HEIGHT 54
#define SIZE (WIDTH_COUNT*HEIGHT)
#define top (LCD_HEIGHT - HEIGHT) / 2
#define left (LCD_WIDTH - WIDTH_COUNT * 8) / 2

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
uint8_t buf[SIZE];

File root;
File myFile;

void draw() {
  u8g2.drawXBMP(left, top, WIDTH_COUNT*8, HEIGHT,buf);
}

void welcomepage() {
  u8g2.setFont(u8g2_font_timR08_tf);
  u8g2.setFontPosTop();
  u8g2.setCursor(0,20);
  u8g2.print("Welcone to ESP32 BA Player");
}

void setup(){
  Serial.begin(1000000);
  pinMode(10, OUTPUT);
  pinMode(15,INPUT_PULLUP);
  u8g2.begin();
  u8g2.enableUTF8Print();

  u8g2.firstPage();
  do{
      welcomepage();
  }while(u8g2.nextPage());
  
  bool x=1;
  while(x){
    if (!SPIFFS.begin(true)) {
      Serial.println("Initialization Failed");
    }
    else
      Serial.println("Initialization OK");
    myFile = SPIFFS.open("/bad.bin");
    if(!myFile){
      Serial.println("Open file error");
    }
    else
      x=0;
  while(x&&digitalRead(15)==1);
  }  
  delay(500);
  u8g2.firstPage();
  do{
      u8g2.setFont(u8g2_font_timR08_tf);
      u8g2.setFontPosTop();
      u8g2.setCursor(0,20);
      u8g2.print("Start to play BadApple");
  }while(u8g2.nextPage());
  delay(1000);
  u8g2.setCursor(0,0);
  //myFile.seek(SIZE/WIDTH_COUNT);
}
int cnt=0;
double fps=0;
int stime=millis();
int lasttime=stime;
double zt=0;
void loop(){
  if((millis()-stime)/33.3<=cnt){
    u8g2.firstPage();
    do {
      draw();
    } while(u8g2.nextPage());
  }
  

  if  (myFile && myFile.available()) {
    myFile.read(buf,SIZE);
    for(int i=0;i<SIZE;i++){
      buf[i]=(buf[i]<<4)|(buf[i]>>4);  
      buf[i]=((buf[i]<<2)&0xcc)|((buf[i]>>2)&0x33);  
      buf[i]=((buf[i]<<1)&0xaa)|((buf[i]>>1)&0x55); 
    }
    
  } else {
    myFile.close();
    myFile = SPIFFS.open("bad.BIN");
    Serial.println("perss to Replay.");
    u8g2.firstPage();
    do {
      u8g2.print("End.Press to replay");
    } while(u8g2.nextPage());
    while(digitalRead(15));
    delay(25);
    while(digitalRead(15));
    delay(100);
    cnt=0;
  }
  ++cnt;
  int t=millis();
  Serial.println("img:"+String(cnt)+" fps:"+String((double)1000/(t-lasttime))+" time:"+String(t-stime));
  lasttime=t;
}
