#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_MPR121.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

Adafruit_MPR121 cap = Adafruit_MPR121();

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }
    Serial.println("SSD1306 found!");

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("URU Card");
    display.display();

    // Default address is 0x5A, if tied to 3.3V its 0x5B
    // If tied to SDA its 0x5C and if SCL then 0x5D
    if (!cap.begin(0x5A))
    {
        Serial.println("MPR121 not found, check wiring?");
        while (1)
            ;
    }
    Serial.println("MPR121 found!");
}

char getTouched()
{
    static uint16_t lasttouched = 0;
    uint16_t currtouched = cap.touched();
    uint8_t index = 0;
    static const char keystrokes[] = {'\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'C', '0', '\n'};

    for (uint8_t i = 0; i < 12; i++)
    {
        // it if *is* touched and *wasnt* touched before, alert!
        if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)))
        {
            index = i + 1;
            Serial.print(i);
            Serial.println(" touched");
        }
    }

    // reset our state
    lasttouched = currtouched;

    return keystrokes[index];
}

void loop()
{
    char touched = getTouched();

    static String input;

    if (touched != 0)
    {
        if (touched == 'C')
        {
            input.clear();
        }
        else if (touched == '\n')
        {
            input = "Ok!";
        }
        else
        {
            input += touched;
        }

        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println(input);
        display.display();
    }

    delay(100);
}