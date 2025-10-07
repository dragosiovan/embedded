#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>

// Define SPI pins for the display
#define TFT_CS   15  // Chip select for display
#define TFT_DC   2  // Data/Command for display
#define TFT_RST  4  // Reset for display

// Define SPI pins for the touchscreen
#define TOUCH_CS 15 // Chip select for touchscreen
#define TOUCH_IRQ 14 // Optional, interrupt pin for touchscreen



#define USE_EXAMPLE   0


typedef struct
{
  uint16_t x;           // x position
  uint16_t y;           // y position
  uint16_t w;           // width
  uint16_t h;           // height
  uint16_t b;           // border
  uint16_t v;           // init value
  uint16_t c;           // color
  String   l;           // label
} Slider_t;


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
  SLDIER_MAX
} Slider_e;

// Initialize the display
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Initialize the touchscreen
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

// Slider parameters
const int numSliders = 9;
const int sliderWidth =20;
const int sliderHeight = 200;
const int sliderSpacing = 15;
const int sliderStartX = 10;
const int sliderStartY = 30;
int sliderValues[numSliders] = {70, 100, 20, 30, 40, 50, 80, 20};


#define NUM_SLIDER        8
#define SLIDER_HIGHT      200
#define SLIDER_WIDTH      20
#define SLIDER_BORDER     4
#define SLIDER_X_INIT     10
#define SLIDER_Y_INIT     30
#define SLDIER_SPACING    15

Slider_t sliders[SLDIER_MAX] =
{
  [SLIDER_UV] = {
    .x = SLIDER_X_INIT,
    .y = SLIDER_Y_INIT,
    .w = SLIDER_WIDTH,
    .h = SLIDER_HIGHT,
    .b = SLIDER_BORDER,
    .v = 90,
    .c = ILI9341_PURPLE,
  },
  [SLIDER_VI] = {
    .x = SLIDER_X_INIT+SLIDER_VI*(SLIDER_WIDTH+SLDIER_SPACING),
    .y = SLIDER_Y_INIT,
    .w = SLIDER_WIDTH,
    .h = SLIDER_HIGHT,
    .b = SLIDER_BORDER,
    .v = 90,
    .c = ILI9341_MAGENTA,
  },
  [SLIDER_BL] = {
    .x = SLIDER_X_INIT+SLIDER_BL*(SLIDER_WIDTH+SLDIER_SPACING),
    .y = SLIDER_Y_INIT,
    .w = SLIDER_WIDTH,
    .h = SLIDER_HIGHT,
    .b = SLIDER_BORDER,
    .v = 90,
    .c = ILI9341_BLUE,
  },
  [SLIDER_BH] = {
    .x = SLIDER_X_INIT+SLIDER_BH*(SLIDER_WIDTH+SLDIER_SPACING),
    .y = SLIDER_Y_INIT,
    .w = SLIDER_WIDTH,
    .h = SLIDER_HIGHT,
    .b = SLIDER_BORDER,
    .v = 90,
    .c = ILI9341_BLUE,
  },
  [SLIDER_IB] = {
    .x = SLIDER_X_INIT+SLIDER_IB*(SLIDER_WIDTH+SLDIER_SPACING),
    .y = SLIDER_Y_INIT,
    .w = SLIDER_WIDTH,
    .h = SLIDER_HIGHT,
    .b = SLIDER_BORDER,
    .v = 0,
    .c = ILI9341_CYAN,
  },
  [SLIDER_CW] = {
    .x = SLIDER_X_INIT+SLIDER_CW*(SLIDER_WIDTH+SLDIER_SPACING),
    .y = SLIDER_Y_INIT,
    .w = SLIDER_WIDTH,
    .h = SLIDER_HIGHT,
    .b = SLIDER_BORDER,
    .v = 90,
    .c = ILI9341_LIGHTGREY,
  },
  [SLIDER_RB] = {
    .x = SLIDER_X_INIT+SLIDER_RB*(SLIDER_WIDTH+SLDIER_SPACING),
    .y = SLIDER_Y_INIT,
    .w = SLIDER_WIDTH,
    .h = SLIDER_HIGHT,
    .b = SLIDER_BORDER,
    .v = 100,
    .c = ILI9341_PINK,
  },
  [SLIDER_WW] = {
    .x = SLIDER_X_INIT+SLIDER_WW*(SLIDER_WIDTH+SLDIER_SPACING),
    .y = SLIDER_Y_INIT,
    .w = SLIDER_WIDTH,
    .h = SLIDER_HIGHT,
    .b = SLIDER_BORDER,
    .v = 90,
    .c = ILI9341_DARKGREY,
  },
    [SLIDER_BR] = {
    .x = SLIDER_X_INIT+SLIDER_BR*(SLIDER_WIDTH+SLDIER_SPACING),
    .y = SLIDER_Y_INIT,
    .w = SLIDER_WIDTH,
    .h = SLIDER_HIGHT,
    .b = SLIDER_BORDER,
    .v = 90,
    .c = ILI9341_GREEN,
  }
};


void setup() {
  Serial.begin(115200);

  // Initialize the display
  tft.begin();
  tft.setRotation(1); // Landscape mode

  // Initialize the touchscreen
  touch.begin();

  // Clear the screen
  tft.fillScreen(ILI9341_RED);

  // Draw the sliders
  drawSliders();
}

void loop() {
  // Check for touch input
  if (touch.touched()) {
    TS_Point p = touch.getPoint();

    // Map touch coordinates to screen coordinates
    int touchX = map(p.x, 0, 4095, 0, tft.width());
    int touchY = map(p.y, 0, 4095, 0, tft.height());

    // Check if touch is within any slider area
    for (int i = 0; i < numSliders; i++) {
      int sliderX = sliderStartX + i * (sliderWidth + sliderSpacing);
      if (touchX >= sliderX && touchX <= sliderX + sliderWidth && touchY >= sliderStartY && touchY <= sliderStartY + sliderHeight) {
        // Update slider value based on touch position
        sliderValues[i] = map(touchY, sliderStartY + sliderHeight, sliderStartY, 0, 100);
        sliderValues[i] = constrain(sliderValues[i], 0, 100);

        // Redraw the slider
        drawSlider(i);
      }
    }
  }
}

void drawSliders() {
  for (int i = 0; i < numSliders; i++) {
    drawSlider(i);
  }
}



#if USE_EXAMPLE
void drawSlider(int index) {
  int sliderX = sliderStartX + index * (sliderWidth + sliderSpacing);

  // Clear the previous slider value
  tft.fillRect(sliderX, sliderStartY - 20, sliderWidth, 20, ILI9341_BLACK);

  // Draw the slider value
  tft.setCursor(sliderX, sliderStartY - 20);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.print(sliderValues[index]);

  // Draw the slider background in specific color
  tft.fillRect(sliderX, sliderStartY, sliderWidth, sliderHeight, ILI9341_PURPLE);

  // Draw the slider bar
  int sliderBarHeight = map(sliderValues[index], 0, 100, 0, sliderHeight);
  tft.fillRect(sliderX+4, sliderStartY + sliderHeight - sliderBarHeight+4, sliderWidth-8, sliderBarHeight-8, ILI9341_WHITE);


  Serial.printf("Draw slider [%d] x [%d] y [%d]\n", index, sliderX, sliderStartY + sliderHeight - sliderBarHeight);
}


#else


void drawSlider(int index) {

  Slider_t s = sliders[index];

  // int sliderX = sliderStartX + index * (sliderWidth + sliderSpacing);

  // Clear the previous slider value
  //tft.fillRect(sliderX, sliderStartY - 20, sliderWidth, 20, ILI9341_BLACK);
  tft.fillRect(s.x, s.y, s.w, 20, ILI9341_BLACK);

  // Draw the slider value
  // tft.setCursor(sliderX, sliderStartY - 20);
  tft.setCursor(s.x, s.y - 20);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.print(s.v);

  // Draw the slider background in specific color
  // tft.fillRect(sliderX, sliderStartY, sliderWidth, sliderHeight, ILI9341_PURPLE);
  tft.fillRect(s.x, s.y, s.w, s.h, s.c);

  // Draw the slider bar
  // int sliderBarHeight = map(sliderValues[index], 0, 100, 0, sliderHeight);
  int sliderBarHeight = map(s.v, 0, 100, 0, s.h);
  // tft.fillRect(sliderX+4, sliderStartY + sliderHeight - sliderBarHeight+4, sliderWidth-8, sliderBarHeight-8, ILI9341_WHITE);
  tft.fillRect(s.x+s.b, s.y+s.h - sliderBarHeight + s.b, s.w - 2*s.b, s.h - 2*s.b, ILI9341_WHITE);


  Serial.printf("Draw slider [%d] x [%d] y [%d]\n", index, s.x, s.y);
}

#endif
