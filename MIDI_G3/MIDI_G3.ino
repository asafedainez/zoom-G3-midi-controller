
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <EEPROM.h>

/*
Display Nokia 5110
width: 84px (x coordenate)
height: 48px (y coordenate)


*/

#define width_text_size_base 6
#define height_text_size_base 7

#define memory_address_ref_patch 0
#define maximum_patches 99

#define contrast 45
#define bias 1

#define sclk_pin 12
#define din_pin 11
#define dc_pin 10
#define cs_pin 9
#define rst_pin 8

#define pin_FS1 2
#define pin_FS2 3
#define pin_FS3 4
#define pin_BANK_UP 5
#define pin_BANK_DOWN 6

int FS1;
int FS2;
int FS3;
int BANK_UP;
int BANK_DOWN;
char BANKS[] = "ABCDEFGHIJ";

byte valorFS1;
byte valorFS2;
byte valorFS3;

byte referencePatch;
byte activePatch;

Adafruit_PCD8544 display = Adafruit_PCD8544(sclk_pin, din_pin, dc_pin, cs_pin, rst_pin);

void setup() {
  pinMode(pin_FS1, INPUT_PULLUP);
  pinMode(pin_FS2, INPUT_PULLUP);
  pinMode(pin_FS3, INPUT_PULLUP);
  pinMode(pin_BANK_UP, INPUT_PULLUP);
  pinMode(pin_BANK_DOWN, INPUT_PULLUP);

  if (EEPROM.read(memory_address_ref_patch) > maximum_patches) {
    EEPROM.write(0, memory_address_ref_patch);
  }

  activePatch = getActivePatch();
  referencePatch = sumPatch(-1);
  valorFS1 = referencePatch;
  valorFS2 = sumPatch(1);
  valorFS3 = sumPatch(2);

  Serial.begin(9600);
  display.begin(contrast, bias);
  display.initDisplay();
  display.setCursor(0, 0);
  display.setTextColor(BLACK, WHITE);
}

void loop() {
  FS1 = digitalRead(pin_FS1);
  FS2 = digitalRead(pin_FS2);
  FS3 = digitalRead(pin_FS3);
  BANK_UP = digitalRead(pin_BANK_UP);
  BANK_DOWN = digitalRead(pin_BANK_DOWN);

  display.clearDisplay();
  printActivePatch();
  drawMidLine();
  printFootSwitchPatches();
  display.display();
  delay(100);

  if (BANK_UP == LOW) {
    increasePatch();
  }

  if (BANK_DOWN == LOW) {
    decreasePatch();
  }

  if (FS1 == LOW) {
    activePatch = valorFS1;
    updateActivePatch(valorFS1);
  }

  if (FS2 == LOW) {
    activePatch = valorFS2;
    updateActivePatch(valorFS2);
  }

  if (FS3 == LOW) {
    activePatch = valorFS3;
    updateActivePatch(valorFS3);
  }
}



byte getActivePatch() {
  return EEPROM.read(memory_address_ref_patch);
}

void updateActivePatch(byte patch) {
  EEPROM.write(patch, memory_address_ref_patch);
}

void printActivePatch() {
  display.setTextSize(3);
  display.setCursor(25, 0);  // centralizar número
  display.print(getPatchName(activePatch));
}

void printFootSwitchPatches() {
  byte xCursor = (width_text_size_base * 2 * 2) + 6;
  byte yCursor = (display.height() / 2) + 10;

  display.setTextSize(2);

  display.setCursor(0, yCursor);
  display.print(getPatchName(valorFS1));

  display.setCursor(xCursor, yCursor);
  display.print(getPatchName(valorFS2));

  display.setCursor(xCursor * 2, yCursor);
  display.print(getPatchName(valorFS3));
}

String getPatchName(byte patch) {
  byte bankNumber = patch / 10;
  byte number = patch % 10;

  return BANKS[bankNumber] + String(number);
}

void drawMidLine() {
  byte y = display.height() / 2;
  display.drawLine(0, y, display.width(), y, BLACK);
}

void increasePatch() {
  referencePatch = sumPatch(1);
  valorFS1 = referencePatch;
  valorFS2 = sumPatch(1);
  valorFS3 = sumPatch(2);
}

void decreasePatch() {
  referencePatch = sumPatch(-1);
  valorFS1 = referencePatch;
  valorFS2 = sumPatch(1);
  valorFS3 = sumPatch(2);
}

byte sumPatch(int number) {
  int ref = referencePatch + number;
  if (ref < 0) {
    return 99;
  }

  if (ref > maximum_patches) {
    return ref - maximum_patches - 1;
  }

  return ref;
}
