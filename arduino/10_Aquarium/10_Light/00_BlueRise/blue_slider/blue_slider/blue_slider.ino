/* Touchscreen Display pinout:
 *
 * Display
*     Display ->    ESP32 
 * 1. Vcc =         5V
 * 2. GND =         GND
 * 3. CS =          15
 * 4. RESET =       -1 (RST)
 * 5. DC/RS =       2
 * 6. SDI (MOSI) =  23 
 * 7. SCK =         18
 * 8. LED (BG) =    5V
 * 9. SDO (MISO) =  19 /NC
 * Touch 
 * 10. T_CLK =      18
 * 11. T_CS =       21
 * 12. T_DIN =      23
 * 13. T_DO =       19
 * 14. T_IRQ =      NC
 * 



TFT_eSPI/User_Setup.h:

#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   15  // Chip select control pin
#define TFT_DC    2  // Data Command control pin
#define TFT_RST   4  // Reset pin (could connect to RST pin)
//#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST
 */









#include "FS.h"
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library





#define CALIBRATION_FILE "/calibrationData"

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library


#define TFT_W   320
#define TFT_H   480




#define PIN_LED_BG    2
#define PWM_RESOL     12
#define PWM_FREQ      5000


typedef enum 
{
  TOUCH_ITEM_SLIDER_NA = 0,
  TOUCH_ITEM_SLIDER_MIN,
  TOUCH_ITEM_SLIDER_UV = TOUCH_ITEM_SLIDER_MIN,
  TOUCH_ITEM_SLIDER_VI,
  TOUCH_ITEM_SLIDER_BL,
  TOUCH_ITEM_SLIDER_BH,
  TOUCH_ITEM_SLIDER_IB,
  TOUCH_ITEM_SLIDER_CW,
  TOUCH_ITEM_SLIDER_RB,
  TOUCH_ITEM_SLIDER_WW,
  TOUCH_ITEM_SLIDER_BR,
  TOUCH_ITEM_SLIDER_MAX = TOUCH_ITEM_SLIDER_BR,
  TOUCH_ITEM_BUTTON_MIN,
  TOUCH_ITEM_BUTTON_AM = TOUCH_ITEM_BUTTON_MIN,
  TOUCH_ITEM_BUTTON_MAX = TOUCH_ITEM_BUTTON_AM,
  TOUCH_ITEM_MAX
} Touch_Item_e;


typedef enum
{
  BUTTON_STATE_OFF = 0,     // State OFF
  BUTTON_STATE_ON,          // State ON
  BUTTON_STATE_MAX
} Button_State_e;


typedef enum
{
  BUTTON_AM = 0,      // Auto/Manual
  BUTTON_MAX
} Button_e;

typedef struct
{
  uint16_t x;           // x position
  uint16_t y;           // y position
  uint16_t w;           // width
  uint16_t h;           // height
  uint16_t bx;          // border on x axis
  uint16_t by;          // border on y axis
  uint16_t v;           // value: manual
  uint16_t cb[BUTTON_STATE_MAX];  // color background/state
  uint16_t ct[BUTTON_STATE_MAX];  // color text/state
  String   l[BUTTON_STATE_MAX];   // label/state
} Button_t;




#define NUM_SLIDER          9
#define SLIDER_HIGHT        430
#define SLIDER_WIDTH        34
#define SLIDER_BORDER_X     6
#define SLIDER_BORDER_Y     1
#define SLIDER_X_INIT       0
#define SLIDER_Y_INIT       40      // LCD_W /10
#define SLDIER_SPACING      0
#define SLIDER_TRACK_WIDTH  2
#define SLIDER_BAR_H        8
#define SLIDER_BAR_W        30
#define SLIDER_VAL_H        40
#define SLIDER_VAL_A_Y      24
#define SLIDER_VAL_M_Y      4

typedef enum
{
  SLIDER_UV = 0,      // uv
  SLIDER_VI,          // violet
  SLIDER_BL,          // blue low
  SLIDER_BH,          // blue high
  SLIDER_IB,          // ice blue
  SLIDER_CW,          // cold white
  SLIDER_RB,          // red blue
  SLIDER_WW,          // cold white, warm white
  SLIDER_BR,          // brightnes
  SLIDER_MAX
} Slider_e;

typedef struct
{
  int x;           // x position
  int y;           // y position
  int w;           // width
  int h;           // height
  int bx;          // border on x axis
  int by;          // border on y axis
  int vm;          // value: manual
  int va;          // value: auto
  int vo;          // value: manual old
  uint16_t c;           // color
  String   l;           // label
} Slider_t;




Slider_t sliders[SLIDER_MAX] =
{
  [SLIDER_UV] = {
    .x = SLIDER_X_INIT,
    .y = SLIDER_Y_INIT,
    .w = SLIDER_WIDTH,
    .h = SLIDER_HIGHT,
    .bx = SLIDER_BORDER_X,
    .by = SLIDER_BORDER_Y,
    .vm = 0,
    .va = 100,
    .vo = -1,
    .c = TFT_PURPLE,
  },
  [SLIDER_VI] = {
    .x = SLIDER_X_INIT+SLIDER_VI*(SLIDER_WIDTH+SLDIER_SPACING),
    .y = SLIDER_Y_INIT,
    .w = SLIDER_WIDTH,
    .h = SLIDER_HIGHT,
    .bx = SLIDER_BORDER_X,
    .by = SLIDER_BORDER_Y,
    .vm = 100,
    .va = 1,
    .vo = -1,
    .c = TFT_MAGENTA,
  },
  [SLIDER_BL] = {
    .x = SLIDER_X_INIT+SLIDER_BL*(SLIDER_WIDTH+SLDIER_SPACING),
    .y = SLIDER_Y_INIT,
    .w = SLIDER_WIDTH,
    .h = SLIDER_HIGHT,
    .bx = SLIDER_BORDER_X,
    .by = SLIDER_BORDER_Y,
    .vm = 1,
    .va = 5,
    .vo = -1,
    .c = TFT_BLUE,
  },
  [SLIDER_BH] = {
    .x = SLIDER_X_INIT+SLIDER_BH*(SLIDER_WIDTH+SLDIER_SPACING),
    .y = SLIDER_Y_INIT,
    .w = SLIDER_WIDTH,
    .h = SLIDER_HIGHT,
    .bx = SLIDER_BORDER_X,
    .by = SLIDER_BORDER_Y,
    .vm = 4,
    .va = 10,
    .vo = -1,
    .c = TFT_BLUE,
  },
  [SLIDER_IB] = {
    .x = SLIDER_X_INIT+SLIDER_IB*(SLIDER_WIDTH+SLDIER_SPACING),
    .y = SLIDER_Y_INIT,
    .w = SLIDER_WIDTH,
    .h = SLIDER_HIGHT,
    .bx = SLIDER_BORDER_X,
    .by = SLIDER_BORDER_Y,
    .vm = 20,
    .va = 20,
    .vo = -1,
    .c = TFT_CYAN,
  },
  [SLIDER_CW] = {
    .x = SLIDER_X_INIT+SLIDER_CW*(SLIDER_WIDTH+SLDIER_SPACING),
    .y = SLIDER_Y_INIT,
    .w = SLIDER_WIDTH,
    .h = SLIDER_HIGHT,
    .bx = SLIDER_BORDER_X,
    .by = SLIDER_BORDER_Y,
    .vm = 10,
    .va = 40,
    .vo = -1,
    .c = TFT_LIGHTGREY,
  },
  [SLIDER_RB] = {
    .x = SLIDER_X_INIT+SLIDER_RB*(SLIDER_WIDTH+SLDIER_SPACING),
    .y = SLIDER_Y_INIT,
    .w = SLIDER_WIDTH,
    .h = SLIDER_HIGHT,
    .bx = SLIDER_BORDER_X,
    .by = SLIDER_BORDER_Y,
    .vm = 30,
    .va = 80,
    .vo = -1,
    .c = TFT_PINK,
  },
  [SLIDER_WW] = {
    .x = SLIDER_X_INIT+SLIDER_WW*(SLIDER_WIDTH+SLDIER_SPACING),
    .y = SLIDER_Y_INIT,
    .w = SLIDER_WIDTH,
    .h = SLIDER_HIGHT,
    .bx = SLIDER_BORDER_X,
    .by = SLIDER_BORDER_Y,
    .vm = 40,
    .va = 99,
    .vo = -1,
    .c = TFT_DARKGREY,
  },
  [SLIDER_BR] = {
    .x = SLIDER_X_INIT+SLIDER_BR*(SLIDER_WIDTH+SLDIER_SPACING),//+SLIDER_WIDTH/2,
    .y = SLIDER_Y_INIT+SLIDER_HIGHT/2,
    .w = SLIDER_WIDTH,
    .h = SLIDER_HIGHT/2,
    .bx = SLIDER_BORDER_X,
    .by = SLIDER_BORDER_Y,
    .vm = 90,
    .va = 20,
    .vo = -1,
    .c = TFT_GREEN,
  }
};




Button_t buttons[BUTTON_MAX] =
{
  [BUTTON_AM] = {
    .x = SLIDER_X_INIT+(SLIDER_MAX-1)*(SLIDER_WIDTH+SLDIER_SPACING),
    .y = 0,
    .w = TFT_W - (SLIDER_MAX-1)*SLIDER_WIDTH,
    .h = SLIDER_Y_INIT,
    .bx = 1,
    .by = 1,
    .v = BUTTON_STATE_ON,    // OFF = AUTO, ON = MANU
    .cb = {TFT_BLUE, TFT_RED},
    .ct = {TFT_WHITE, TFT_WHITE},
    .l = {"AUTO", "MANU"}
  }
};



void setupTouch()
{
  uint16_t calibrationData[5];
  uint8_t calDataOK = 0;
  bool calibAvail = false;

  tft.fillScreen((0xFFFF));

  tft.setCursor(20, 0, 2);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);  tft.setTextSize(1);
  tft.println("calibration run");

  Serial.println("Init FS ...");
  // check file system
  if (!SPIFFS.begin()) {
    Serial.println("Formatting file system ...");

    SPIFFS.format();
    SPIFFS.begin();
    Serial.println("...done.");
  }
  Serial.println("...done.");

  // check if calibration file exists
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    File f = SPIFFS.open(CALIBRATION_FILE, "r");
    if (f) {
      if (f.readBytes((char *)calibrationData, 14) == 14) {
        calDataOK = 1;
        Serial.println("Calibration data loaded from FS");
      }
      f.close();
    }
  }
  if (calDataOK) {
    // calibration data valid
    Serial.println("Touch init ...");
    tft.setTouch(calibrationData);
  } else {
    // data not valid. recalibrate
    Serial.println("Calibration run ...");
    tft.calibrateTouch(calibrationData, TFT_WHITE, TFT_RED, 15);
    // store data
    Serial.printf("Saving calibration data ...");
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calibrationData, 14);
      f.close();
      Serial.println("...done.");
    }
  }

  tft.fillScreen((0xFFFF));
}




/*****************************************************************************
 *
 */
void setup() {
  Serial.begin(115200);

  pinMode(PIN_LED_BG, OUTPUT);

  // analogWriteResolution(PIN_LED_BG, PWM_RESOL);
  // analogWriteFrequency(PIN_LED_BG, PWM_FREQ);
  // analogWrite(PIN_LED_BG, 100);

  ledcAttach(PIN_LED_BG, PWM_FREQ, PWM_RESOL);
  // ledcAttach(PIN_LED_BG);
  ledcWrite(PIN_LED_BG, 100);

  // Initialize the display
  tft.begin();
  tft.init();

  tft.setRotation(0); // Landscape mode

  // // Initialize the touchscreen
  setupTouch();

  // Clear the screen
  tft.fillScreen(TFT_BLACK);

  // Draw all sliders
  drawSliders();

  // Draw all buttons
  drawButtons();

}



/*****************************************************************************
 *
 */
void loop() {

  touchLoop();
}




Touch_Item_e touchGetItem(uint16_t x, uint16_t y)
{
  uint8_t ti = TOUCH_ITEM_SLIDER_NA;

  // check the list of sliders
  for (uint8_t i = 0; i < SLIDER_MAX; i++)
  {
    Slider_t s = sliders[i];
    if (   (x >= s.x) 
        && (x <= (s.x+s.w))
        && (y >= s.y)
        && (y <= (s.y+s.h)))
      {
        ti = TOUCH_ITEM_SLIDER_UV + i;
      }
  }

  // check the list of buttons
  for (uint8_t i = 0; i < BUTTON_MAX; i++)
  {
    Button_t b = buttons[i];
    if (   (x >= b.x) 
        && (x <= (b.x+b.w))
        && (y >= b.y)
        && (y <= (b.y+b.h)))
      {
        ti = TOUCH_ITEM_BUTTON_AM + i;
      }
  }

  return (Touch_Item_e)ti;
}


void touchItemHandle(uint16_t x, uint16_t y, Touch_Item_e ti)
{
  // handle sliders
  if (ti >= TOUCH_ITEM_SLIDER_MIN && ti <= TOUCH_ITEM_SLIDER_MAX )
  {
    uint8_t si = ti - TOUCH_ITEM_SLIDER_MIN;
    Slider_t *s = &sliders[si];
    int val;


    // Serial.printf("Slider changed [%d] yr [%03d:%03d] y [%03d]\n", si, s->y, s->y+s->h, y);
    val = map(y, s->y, s->y+s->h, 100, 0);
    // Serial.printf("... map val [%d]\n", val);
    val = constrain(val, 0, 100);
    s->vm = val;
    // Serial.printf("... normalized val [%d]\n", s->vm);

    // drawSlider(si);
    redrawSlider(si);

    // handle LED BG 
    if (ti == TOUCH_ITEM_SLIDER_BR)
    {
      uint16_t v = map(val, 0, 100, 0, 4095);
      // analogWrite(PIN_LED_BG, v);
      ledcWrite(PIN_LED_BG, v);
    }
  }

  // handle buttons
  if (ti >= TOUCH_ITEM_BUTTON_MIN && ti <= TOUCH_ITEM_BUTTON_MAX)
  {
    uint8_t bi = ti - TOUCH_ITEM_BUTTON_MIN;
    Button_t *b = &buttons[bi];

    // Serial.printf("Button pressed [%d] val [%d]\n", bi, b->v);
    if (b->v == BUTTON_STATE_OFF)
      b->v = BUTTON_STATE_ON;
    else
      b->v = BUTTON_STATE_OFF; 
    
    Serial.printf("... new val [%d]\n", b->v);

    drawButton(bi);
    
    if (bi == BUTTON_AM)
    {
      drawSliders();
    }
  }
}



void touchLoop(void) {

  uint16_t x, y;
  static uint16_t color;
  Touch_Item_e ti = TOUCH_ITEM_SLIDER_NA;

  if (tft.getTouch(&x, &y)) // get touch coordinates
  {
    ti = touchGetItem(x,y);
    // Serial.printf("Touched item #[%d] x [%03d] y [%03d]\n", ti, x, y);

    touchItemHandle(x, y, ti);
  }
}




/*****************************************************************************
 *
 */
void drawButtons() {
  for (int i = 0; i < BUTTON_MAX; i++) {
    drawButton(i);
  }
}




/*****************************************************************************
 *
 */
void drawButton(int index) {

  Button_t b = buttons[index];

  Serial.printf("Draw button [%d] val [%d]\n", index, b.v);

  // Draw the button background in specific color
  tft.fillRect(b.x, b.y, b.w, b.h, TFT_WHITE);
  tft.fillRect(b.x+b.bx, b.y+b.by, b.w-2*b.bx, b.h-2*b.by, b.cb[b.v]);

  // Draw the button value
  tft.setCursor(b.x+10, b.y+10);
  tft.setTextColor(b.ct[b.v]);
  tft.setTextSize(1);
  tft.print(b.l[b.v]);
}


/*****************************************************************************
 *
 */
void drawSliders() {
  for (int i = 0; i < SLIDER_MAX; i++) {
    drawSlider(i);
  }
}








/*****************************************************************************
 * just change the slider position: shade old position, draw to the new position
 */
void redrawSlider(int index) {

  Slider_t *s = &sliders[index];
  uint16_t vto; // value t offset
  uint16_t cva; // auto value bg color
  uint16_t cvm; // manual value bg color
  uint16_t sv;  // slider value

  Serial.printf("Redraw slider #[%d] old value [%d]\n", index, s->vo);

  if ((buttons[BUTTON_AM].v == BUTTON_STATE_OFF) &&  // auto mode
      (s->va != -1))  // auto value available
  {
    cva = TFT_BLUE;
  }
  else
  {
    cva = TFT_BLACK;
  }

  if (buttons[BUTTON_AM].v != BUTTON_STATE_OFF) // manual mode
  {
    cvm = TFT_RED;
  }
  else
  {
    cvm = TFT_BLACK;
  }


  // Clear the previous slider value  
  tft.fillRect(s->x, s->y-SLIDER_VAL_H, s->w, SLIDER_VAL_H/2, cvm);
  tft.fillRect(s->x, s->y-SLIDER_VAL_H/2, s->w, SLIDER_VAL_H/2, cva);

  // Draw the slider value
  // tft.setCursor(sliderX, sliderStartY - 20);
  if (s->va != -1) // value auto is available
  {
    if (s->va<10)
    {
      vto = 14;
    }
    else if (s->va<100)
    {
      vto = 10;
    }
    else
    {
      vto = 6;
    }
    tft.setCursor(s->x+vto, s->y - SLIDER_VAL_H + SLIDER_VAL_A_Y);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.print(s->va);
  }

  if (s->vm<10)
  {
    vto = 14;
  }
  else if (s->vm<100)
  {
    vto = 10;
  }
  else
  {
    vto = 6;
  }
  tft.setCursor(s->x+vto, s->y - SLIDER_VAL_H + SLIDER_VAL_M_Y);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.print(s->vm);



  int sliderBarHeight = map(s->vo, 0, 100, 0, s->h-SLIDER_BAR_H);


  // shade old slider head
  tft.fillRect(s->x, s->y + s->h - sliderBarHeight-SLIDER_BAR_H, s->bx, SLIDER_BAR_H, TFT_BLACK);   // border left -> black 
  tft.fillRect(s->x + s->w - s->bx, s->y + s->h - sliderBarHeight-SLIDER_BAR_H, s->bx, SLIDER_BAR_H, TFT_BLACK);   // border right -> black 

  tft.fillRect(s->x + s->bx, s->y + s->h - sliderBarHeight - SLIDER_BAR_H, s->w - 2*s->bx, SLIDER_BAR_H, s->c);   // center -> sldier color 




  // Draw the slider bar
  if (buttons[BUTTON_AM].v == BUTTON_STATE_OFF) // AUTO
  {
    sv = s->va;
  }
  else
  {
    sv = s->vm;
  }
  s->vo = sv; // save old value
  sliderBarHeight = map(sv, 0, 100, 0, s->h-SLIDER_BAR_H);


  // draw new slider head
  tft.drawRect(s->x + s->w/2 - (SLIDER_BAR_W/2 +1), s->y + s->h - sliderBarHeight-SLIDER_BAR_H, SLIDER_BAR_W, SLIDER_BAR_H, TFT_WHITE);
  tft.fillRect(s->x + s->w/2 - (SLIDER_BAR_W/2 +1)+1, s->y + s->h - sliderBarHeight-SLIDER_BAR_H+1, SLIDER_BAR_W-2, SLIDER_BAR_H-2, TFT_BLACK);
}


/*****************************************************************************
 *
 */
void drawSlider(int index) {

  Slider_t *s = &sliders[index];
  uint16_t vto; // value t offset
  uint16_t cva; // auto value bg color
  uint16_t cvm; // manual value bg color
  uint16_t sv;  // slider value

  if ((buttons[BUTTON_AM].v == BUTTON_STATE_OFF) &&  // auto mode
      (s->va != -1))  // auto value available
  {
    cva = TFT_BLUE;
  }
  else
  {
    cva = TFT_BLACK;
  }

  if (buttons[BUTTON_AM].v != BUTTON_STATE_OFF) // manual mode
  {
    cvm = TFT_RED;
  }
  else
  {
    cvm = TFT_BLACK;
  }


  // Clear the previous slider value  
  tft.fillRect(s->x, s->y-SLIDER_VAL_H, s->w, SLIDER_VAL_H/2, cvm);
  tft.fillRect(s->x, s->y-SLIDER_VAL_H/2, s->w, SLIDER_VAL_H/2, cva);

  // Draw the slider value
  // tft.setCursor(sliderX, sliderStartY - 20);
  if (s->va != -1) // value auto is available
  {
    if (s->va<10)
    {
      vto = 14;
    }
    else if (s->va<100)
    {
      vto = 10;
    }
    else
    {
      vto = 6;
    }
    tft.setCursor(s->x+vto, s->y - SLIDER_VAL_H + SLIDER_VAL_A_Y);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.print(s->va);
  }

    if (s->vm < 10)
    {
      vto = 14;
    }
    else if (s->vm < 100)
    {
      vto = 10;
    }
    else
    {
      vto = 6;
    }
  tft.setCursor(s->x + vto, s->y - SLIDER_VAL_H + SLIDER_VAL_M_Y);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.print(s->vm);

  // Draw the slider bar
  if (buttons[BUTTON_AM].v == BUTTON_STATE_OFF) // AUTO
  {
    sv = s->va;
  }
  else
  {
    sv = s->vm;
  }
  s->vo = sv;
  int sliderBarHeight = map(sv, 0, 100, 0, s->h-SLIDER_BAR_H);

  tft.fillRect(s->x, s->y, s->w, s->h, TFT_BLACK); // draw background
  tft.fillRect(s->x + s->bx, s->y + s->by, s->w - 2*s->bx, s->h - 2*s->by, s->c);

  // draw slider head
  // rectangle 
  tft.drawRect(s->x + s->w/2 - (SLIDER_BAR_W/2 +1), s->y + s->h - sliderBarHeight-SLIDER_BAR_H, SLIDER_BAR_W, SLIDER_BAR_H, TFT_WHITE);
  tft.fillRect(s->x + s->w/2 - (SLIDER_BAR_W/2 +1)+1, s->y + s->h - sliderBarHeight-SLIDER_BAR_H+1, SLIDER_BAR_W-2, SLIDER_BAR_H-2, TFT_BLACK);


  // Serial.printf("Draw bg  [%d] x [%d] y [%d] w [%d] h [%d] slideBar [%d]\n", index, s.x, s.y, s.w, s.h, sliderBarHeight);
  // Serial.printf("  slider x [%d] y [%d] w [%d] h [%d]\n", s.x+s.bx, s.y+s.h - sliderBarHeight - s.bx, s.w - 2*s.bx, sliderBarHeight - 2*s.bx);

}

