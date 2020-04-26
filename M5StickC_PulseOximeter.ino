/*
Arduino-MAX30100 oximetry / heart rate integrated sensor library
Copyright (C) 2016  OXullo Intersecans <x@brainrapers.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* This program is modified to operate with M5StickC and MAX30100 */
/* by attosci April 26, 2020 */

#include <M5StickC.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS 2000 // Display period is set to 2 seconds

// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;

uint32_t tsLastReport = 0;

// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
    Serial.println("Beat!");
}

void setup()
{
    M5.begin();

    // Use M5StickC Grove port as I2C 
    Wire.begin(32, 33);

    //M5.Lcd.setRotation(3);
    //M5.Lcd.setTextColor(WHITE);
    //M5.Lcd.setTextSize(3);
    //M5.Lcd.print("MAX30100");
    //M5.Lcd.clear(BLACK);
    //M5.Lcd.setCursor(0, 40);

    Serial.begin(115200);

    Serial.print("Initializing pulse oximeter..");

    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }

    // The default current for the IR LED is 50mA and it could be changed
    //   by uncommenting the following line. Check MAX30100_Registers.h for all the
    //   available options.
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA); // Comment out for better operation.

    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop()
{
    // Make sure to call update as fast as possible
    pox.update();

    // Asynchronously dump heart rate and oxidation levels to the serial
    // For both, a value of 0 means "invalid"
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        float pulse_rate = pox.getHeartRate();
        int spo2 = pox.getSpO2();
        
        Serial.print("Heart rate:");
        Serial.print(pulse_rate);
        Serial.print("bpm / SpO2:");
        Serial.print(spo2);
        Serial.println("%");

        M5.Lcd.fillScreen(BLACK);

        M5.Lcd.setTextSize(2);
        M5.Lcd.setCursor(10, 10);
        M5.Lcd.print("SpO2%");
        M5.Lcd.setCursor(10, 100);
        M5.Lcd.print("PRbpm");
        M5.Lcd.setTextSize(5);
        M5.Lcd.setCursor(15, 40);
        M5.Lcd.printf("%2d", spo2);
        M5.Lcd.setTextSize(3);
        M5.Lcd.setCursor(20, 130);
        M5.Lcd.printf("%3.0f", pulse_rate);
        
        tsLastReport = millis();
    }
}
