# LonoBadge

<img width="819" height="543" alt="image" src="https://github.com/user-attachments/assets/134aefa0-e183-483c-bb20-69d08b7493ed" />

This project is a small custom electronic badge built around the **Seeed Studio XIAO nRF52840** and a **Waveshare 1.54-inch 200×200 e-Paper display**.

The main idea is simple: connect the badge to a computer using USB-C, send a picture to it, and have the picture appear on the e-Paper display. Because the display is e-Paper, the image stays visible even after the display stops refreshing.

## BOM

* 1x Seeed Studio XIAO nRF52840
* 1x 1.54-inch 200×200 monochrome e-Paper Module
* 1 × push button
* 1x CR2032 Coin Cell Battery
* 1x CR2032 Coin Cell Battery holder

## Pin Connections

The badge uses the following pin arrangement:

```text
XIAO nRF52840       Waveshare e-Paper
--------------------------------------
D1                  CS
D2                  DC
D3                  RST
D4                  BUSY
D8                  CLK
D10                 MOSI
3V3_OUT             3.3V
GND                 GND
```

The button is connected like this:

```text
D5 ───── Push Button ───── GND
```

The firmware uses the internal pull-up resistor, so the button is normally HIGH and becomes LOW when pressed.

## How It Works

The badge communicates with the computer through the XIAO nRF52840's USB connection.

When a picture is selected on the computer, it is converted into a **200×200 black-and-white image**. The image is then sent to the XIAO through USB.

The XIAO receives the image and stores it temporarily in memory. It then sends the image to the Waveshare e-Paper display through SPI.

The basic process is:

```text
Picture on PC
     ↓
Convert to 200×200
     ↓
Convert to black & white
     ↓
USB-C
     ↓
XIAO nRF52840
     ↓
SPI
     ↓
Waveshare e-Paper
     ↓
Picture displayed
```

## Image Format

The current firmware is designed for:

Resolution: 200 × 200 pixels
Colour:     Black & White
Format:     1-bit bitmap data
Data size:  5000 bytes

## Button

The single button is connected to **D5**.

When the button is pressed, the XIAO refreshes the current image on the e-Paper display.

This can be useful if the display needs to be refreshed or if you want the button to become part of the badge's user interface later.

## Arduino Library

The firmware uses the **GxEPD2** library to control the e-Paper display.

Install the library through the Arduino IDE:

```text
Arduino IDE
    ↓
Library Manager
    ↓
Search: GxEPD2
    ↓
Install
```

The sketch also uses the standard Arduino SPI functionality.

## Uploading the Firmware

Open the `.ino` file in Arduino IDE.

Select:

```text
Board:
Seeed XIAO nRF52840
```

Then select the COM port belonging to the XIAO.

Connect the XIAO to the computer using USB-C and upload the firmware.

After the upload finishes, the e-Paper should show the startup screen and the serial monitor should report that the badge is ready.

## Sending a Picture

The firmware expects the computer to send the following header:

```text
IMG200
```

followed by exactly **5000 bytes** of image data.

A separate PC-side uploader can take a normal PNG or JPG image, resize it to 200×200, convert it to black and white, and send it to the XIAO.

For example:

```text
python upload_badge.py COM5 badge.png
```

Replace `COM5` with the COM port assigned to your XIAO.

## Current Limitations

The current version is intentionally simple.

It supports one 200×200 monochrome image at a time. The image is transferred through USB rather than making the XIAO behave like a USB flash drive.

The current button also only refreshes the image. It can be expanded later to support different images, menus, animations, or other badge functions.

The exact GxEPD2 display driver also depends on the controller used by the specific Waveshare 1.54-inch module. If the display does not initialize correctly, the exact Waveshare model should be checked before changing the wiring or firmware.

# Picture

## Schematic

<img width="581" height="467" alt="image" src="https://github.com/user-attachments/assets/a7d334c6-bdaa-4b4e-bb7a-15ab01aa17cf" />

## PCB

<img width="804" height="459" alt="image" src="https://github.com/user-attachments/assets/3b4ad995-d930-4f2f-a846-aca9f6afe348" />


