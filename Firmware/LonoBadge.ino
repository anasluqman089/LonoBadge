#include <Arduino.h>
#include <SPI.h>
#include <GxEPD2_BW.h>

// Waveshare e-Paper
#define EPD_CS      D1
#define EPD_DC      D2
#define EPD_RST     D3
#define EPD_BUSY    D4

// User button
#define BUTTON_PIN  D5

GxEPD2_BW<
  GxEPD2_154_D67,
  GxEPD2_154_D67::HEIGHT
> display(
  GxEPD2_154_D67(
    EPD_CS,
    EPD_DC,
    EPD_RST,
    EPD_BUSY
  )
);


// IMAGE SETTINGS

#define IMAGE_WIDTH   200
#define IMAGE_HEIGHT  200

// 200 x 200 pixels / 8 bits per byte
#define IMAGE_BYTES   5000

uint8_t imageBuffer[IMAGE_BYTES];


// USB image header
const char IMAGE_HEADER[] = "IMG200\n";


// BUTTON STATE
bool previousButtonState = HIGH;


// STARTUP SCREEN
void showStartupScreen()
{
  display.setFullWindow();

  display.firstPage();

  do
  {
    display.fillScreen(GxEPD_WHITE);

    display.setTextColor(GxEPD_BLACK);

    display.setTextSize(2);
    display.setCursor(40, 80);
    display.print("BADGE");

    display.setTextSize(1);
    display.setCursor(48, 105);
    display.print("USB READY");

  }
  while (display.nextPage());
}


// FIND IMAGE HEADER
bool checkForImageHeader()
{
  static uint8_t headerIndex = 0;

  while (Serial.available())
  {
    char received = (char)Serial.read();

    if (received == IMAGE_HEADER[headerIndex])
    {
      headerIndex++;

      if (headerIndex == sizeof(IMAGE_HEADER) - 1)
      {
        headerIndex = 0;
        return true;
      }
    }
    else
    {
      headerIndex = 0;

      // Check whether this character starts a new header.
      if (received == IMAGE_HEADER[0])
      {
        headerIndex = 1;
      }
    }
  }

  return false;
}


// RECEIVE IMAGE FROM USB

bool receiveImage()
{
  uint32_t receivedBytes = 0;

  uint32_t lastDataTime = millis();

  while (receivedBytes < IMAGE_BYTES)
  {
    while (Serial.available())
    {
      int incomingByte = Serial.read();

      if (incomingByte >= 0)
      {
        imageBuffer[receivedBytes] =
          (uint8_t)incomingByte;

        receivedBytes++;

        lastDataTime = millis();
      }

      if (receivedBytes >= IMAGE_BYTES)
      {
        break;
      }
    }

    // 15-second timeout if USB transfer stops.
    if (millis() - lastDataTime > 15000)
    {
      Serial.println("ERROR:TRANSFER_TIMEOUT");
      return false;
    }

    delay(1);
  }

  Serial.println("IMAGE_RECEIVED");

  return true;
}


// DISPLAY IMAGE
void showImage()
{
  display.setFullWindow();

  display.firstPage();

  do
  {
    display.fillScreen(GxEPD_WHITE);

    for (int y = 0; y < IMAGE_HEIGHT; y++)
    {
      for (int x = 0; x < IMAGE_WIDTH; x++)
      {
        // Each byte contains 8 horizontal pixels.
        int byteIndex =
          y * (IMAGE_WIDTH / 8) +
          (x / 8);

        uint8_t bitMask =
          0x80 >> (x & 7);

        bool black =
          (imageBuffer[byteIndex] & bitMask) != 0;

        if (black)
        {
          display.drawPixel(
            x,
            y,
            GxEPD_BLACK
          );
        }
      }
    }

  }
  while (display.nextPage());

  Serial.println("DISPLAY_UPDATED");
}



// BUTTON HANDLER
void handleButton()
{
  bool currentButtonState =
    digitalRead(BUTTON_PIN);

  // Button is active LOW because it connects D5 to GND.
  if (
    previousButtonState == HIGH &&
    currentButtonState == LOW
  )
  {
    delay(30);

    // Debounce / confirm press.
    if (digitalRead(BUTTON_PIN) == LOW)
    {
      Serial.println("BUTTON_PRESSED");

      // Refresh the current image.
      showImage();

      // Wait for button release.
      while (digitalRead(BUTTON_PIN) == LOW)
      {
        delay(10);
      }
    }
  }

  previousButtonState =
    currentButtonState;
}


// SETUP
void setup()
{
  // Button connected to GND.
  pinMode(
    BUTTON_PIN,
    INPUT_PULLUP
  );

  // XIAO native USB serial.
  Serial.begin(115200);

  // Allow USB to enumerate.
  delay(1500);

  // Initialize e-Paper.
  display.init(115200);

  // Portrait orientation.
  display.setRotation(0);

  // Show startup screen.
  showStartupScreen();

  Serial.println();
  Serial.println("==============================");
  Serial.println("CUSTOM E-PAPER BADGE");
  Serial.println("==============================");

  Serial.println("BOARD: XIAO nRF52840");
  Serial.println("DISPLAY: Waveshare 1.54\"");
  Serial.println("RESOLUTION: 200x200");

  Serial.println();
  Serial.println("PIN CONFIGURATION:");
  Serial.println("CS   = D1");
  Serial.println("DC   = D2");
  Serial.println("RST  = D3");
  Serial.println("BUSY = D4");
  Serial.println("SW1  = D5");
  Serial.println("CLK  = D8");
  Serial.println("MOSI = D10");

  Serial.println();
  Serial.println("USB PROTOCOL:");
  Serial.println("IMG200 + 5000 BYTES");

  Serial.println();
  Serial.println("READY");
}


// MAIN LOOP
void loop()
{
  // Check physical button.
  handleButton();

  // Check USB for a new image.
  if (checkForImageHeader())
  {
    Serial.println("RECEIVING_IMAGE");

    if (receiveImage())
    {
      showImage();
    }
  }

  delay(2);
}
