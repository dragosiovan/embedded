#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET D4
/* Object named display, of the class Adafruit_SSD1306 */
Adafruit_SSD1306 display(OLED_RESET);

//#if (SSD1306_LCDHEIGHT != 64)
////#if (SSD1306_LCDHEIGHT != 32)
//#error("Height incorrect, please fix Adafruit_SSD1306.h!");
//#endif

uint8_t s = 0, m = 0, h = 0;
uint8_t cnt = 0;

// 5x7 font

void setup()   {
  Serial.begin(115200);
  Serial.println ("Initializing...");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); /* Initialize display with address 0x3C */
//  display.invertDisplay(1);
  display.clearDisplay();  /* Clear display */
  display.setTextSize(1);  /* Select font size of text. Increases with size of argument. */
  display.setTextColor(WHITE);  /* Color of text*/
}

void loop() 
{
  updateWatch();/* Every second increment clock and display */
  delay(1000);
}

void updateWatch() 
{
  s = s + 1;
  if (s == 60) {
    s = 0;
    m = m + 1;
  }
  if (m == 60) {
    m = 0;
    h = h + 1;
  }
  if (h == 13)
    h = 1;

  writeOLED();
}

void writeOLED()
{
  char time[30];
  int sel1 = 0;
  int sel2 = 0;
  int sel3 = 0;
  int sel4 = 0;
  int pos = 0;
  int col = 0;
  int raw = 0;
  int spa = 0; // menu spacing left/right
  int menu_w = 0;
  int sel_w = 0;
  
  display.clearDisplay();  /* Clear display */
  sprintf(time, "%02d:%02d:%02d", h, m, s);
  
  drawStr(0, 0, time);

  Serial.print ("Write OLED CNT=");
  Serial.println (cnt);

  drawBarBright (8, (cnt%10)*10, (cnt%3) == 0);
  
  drawBarWhiteTemp (16, 100/*(cnt%10)*10*/, (cnt%3) == 1);
  

  if (cnt & 1) sel1 = 1;
  if (cnt & 2) sel2 = 1;
  if (cnt & 3) sel3 = 1;
  if (cnt & 4) sel4 = 1;
  
  menu_w = 128/4;
  raw = 50+13;
  pos = 0;
  spa = 4;
  sel_w = 11;

  int y = 24;
  if (cnt%3 == 2)
    display.drawRect (pos+spa,y, menu_w-spa, 14, sel1);
  else
    display.drawLine (pos+spa,raw, pos+menu_w-spa, raw, sel1);
  drawStr (pos+sel_w, 53, "<<");
  
  pos += menu_w;
  if (cnt%3 == 2)
    display.drawRect (pos+spa,y, menu_w-spa, 14, sel2);
  else
    display.drawLine (pos+spa,raw, pos+menu_w-spa, raw, sel2);
  drawStr (pos+sel_w, y, "^^");
  
  pos += menu_w;
  if (cnt%3 == 2)
    display.drawRect (pos+spa,y, menu_w-spa, 14, sel3);
  else
    display.drawLine (pos+spa,raw, pos+menu_w-spa, raw, sel3);
  drawStr (pos+sel_w, y, "vv");
  
  pos += menu_w;
  if (cnt%3 == 2)
    display.drawRect (pos+spa,y, menu_w-spa, 14, sel4);
  else
    display.drawLine (pos+spa,raw, pos+menu_w-spa, raw, sel4);
  drawStr (pos+sel_w, y, ">>");
  

//  drawStr (3, 53, "Left");
//  drawStr (48, 53, "Table");
//  drawStr (93, 53, "Right");


//  drawStr (3, 53, "<<  ^^  vv  >>");
  
  display.display();
    cnt++;
}


void drawBarWhiteTemp(int y, int level, int sel)
{
  int x = 0;
//  int y = 35;
  int w = 128;
  int h = 5;
  int f = 0;
  int c = 4;     // cursor width 

  drawStr (x, y, "W");
  drawStr (w-6, y, "C");

  x = x + 9;
  w = w - 18;
  f = map (level, 0, 100, x, w);

  /* draw line (not active) or frame (active) */
  
  if (sel)
    display.drawRect (x, y, w, h, 1);
  else
    display.drawLine (x, y+h-1, x+w, y+h-1, 1);

  display.fillRect (f, y+1, c, h-2, 1);
}

void drawBarBright(int y, int level, int sel)
{
  int x = 0;
//  int y = 20;
  int w = 128;
  int h = 5;
  int f = 0;

  drawStr (x, y, "*");

  x = x + 9;
  w = w - 18;
  f = map (level, 0, 100, 0, w);

  Serial.println ("Draw bright");
  Serial.print ("X=");
  Serial.println (x);
  Serial.print ("y=");
  Serial.println (y);
  Serial.print ("w=");
  Serial.println (w);
  Serial.print ("h=");
  Serial.println (h);
  Serial.print ("F=");
  Serial.println (f);


  if (sel)
    display.drawRect (x, y, w, h, 1);
  else
    display.drawLine (x, y+h-1, x+w-1, y+h-1, 1);
    
  display.fillRect (x, y, f, h, 1);  
}


void drawBar (int x, int y, int w, int h, int level, int sel)
{
//  int x = 0;
//  int y = 35;
//  int w = 128;
//  int h = 8;
  int f = 0;

  f = map (level, 0, 100, 0, w);

  if (sel)
    display.drawRect (x, y, w, h, 1);
  else
    display.drawLine (x, y+h, x+w, y+h, 1);
  display.fillRect (x, y, f, h, 1);

}

void drawStr(uint8_t x, uint8_t y, char* str)
{
  display.setCursor(x, y);  /* Set x,y coordinates */
  display.println(str);
}
