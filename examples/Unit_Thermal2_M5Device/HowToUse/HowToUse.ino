
#include <M5Unified.h>  // https://github.com/m5stack/M5Unified/

#include <M5_Thermal2.h>

M5_Thermal2 thermal2;

void setup(void) {
    M5.begin();
    if (M5.Display.width() < M5.Display.height()) {
        M5.Display.setRotation(M5.Display.getRotation() ^ 1);
    }
    // Start of Wire by M5Unified
    M5.Ex_I2C.begin();

    // execute Wire.begin if needed.
    // Wire.begin((int)SDA, (int)SCL, 100000u);

    // Call begin once first.
    // Arguments can specify the instance of Wire, Unit's I2C address, and
    // communication frequency. thermal2.begin(&Wire, 0x32, 100000);
    thermal2.begin();

    // Sampling rate can be specified with values from 0~7.
    // The higher the sampling rate, the more noise is generated.
    // 0=1/2Hz  1=1Hz  2=2Hz  3=4Hz  4=8Hz  5=16Hz  6=32Hz  7=64Hz
    thermal2.setRefreshRate(6);

    // Noise filter strength can be specified from 0~15.  (0=disable noise
    // filter.)
    thermal2.setNoiseFilterLevel(8);

    // Led off , Buzzer off
    thermal2.ledOff();
    thermal2.buzzerOff();

    // Buzzer frequency and volume can be set.
    thermal2.setBuzzer(2000, 64);

    // RGB LED color can be set.
    thermal2.setLed(0, 4, 0);

    // Set alarm information when high temperature is detected.
    // 50.0 ℃ , 250 msec cycle , buzzer 6000 Hz, LED 4,0,0(RED)
    thermal2.setAlarmHighTemp(50.0, 25, 6000, 4, 0, 0);

    // 20.0 ℃ , 250 msec cycle , buzzer 2000 Hz, LED 0,0,4(BLUE)
    thermal2.setAlarmLowTemp(20.0, 25, 2000, 0, 0, 4);

    // Sets the monitor area to be used for alarms.
    // If 11 is set, a range of (11+1)x2=24 is used.
    // Smaller values use only the center portion of the camera.
    thermal2.setMonitorArea(15, 11);

    // All alarm on
    thermal2.alarmOn();

    /* // ※ Temperature alarms can be based on four types of temperature
    information.
    // Lowest temp reached low threshold
      thermal2.alarmOn(thermal2.alarm_lowest_low);

    // Median temp reached low threshold
      thermal2.alarmOn(thermal2.alarm_lowest_med);

    // Average temp reached low threshold
      thermal2.alarmOn(thermal2.alarm_lowest_avg);

    // Highest temp reached low threshold
      thermal2.alarmOn(thermal2.alarm_lowest_high);

    // Lowest temp reached high threshold
      thermal2.alarmOn(thermal2.alarm_highest_low);

    // Median temp reached high threshold
      thermal2.alarmOn(thermal2.alarm_highest_med);

    // Average temp reached high threshold
      thermal2.alarmOn(thermal2.alarm_highest_avg);

    // Highest temp reached high threshold
      thermal2.alarmOn(thermal2.alarm_highest_high);

    // All alarm off
    thermal2.alarmOff();
    //*/
}

void loop(void) {
    M5.update();

    M5.Display.startWrite();

    // Update button status and image data. If new image data is obtained, the
    // return value is true. ※ Button states are updated even if the image could
    // not be retrieved.
    if (thermal2.update()) {
        static int frame_count = 0;
        frame_count++;

        // Obtain temperature data structure.
        auto temp_data = thermal2.getTemperatureData();

        // `getLowestTemperature` and `getHighestTemperature` of type float has
        // temperature in degrees Celsius.
        float lowest_temp  = temp_data.getLowestTemperature();
        float highest_temp = temp_data.getHighestTemperature();
        float temp_diff    = highest_temp - lowest_temp;

        // Pixel data is held in an array. Array size is 384. (16x24)
        for (int idx = 0; idx < 384; ++idx) {
            /*
            The resolution of the camera is 32x24, but the data obtained at one
            time is half of that. The resulting `array_data` is stored
            alternately like a chessboard. You can use `subpage` to find out
            whether even or odd numbers were obtained

             ■ == subpage 0
             □ == subpage 1
              ０ １ ２ ３ … 28 29 30 31
            ０■ □ ■ □ … ■ □ ■ □
            １□ ■ □ ■ … □ ■ □ ■
            ２■ □ ■ □ … ■ □ ■ □
               ～～～～～～～～～～～
            21□ ■ □ ■ … □ ■ □ ■
            22■ □ ■ □ … ■ □ ■ □
            23□ ■ □ ■ … □ ■ □ ■
            */
            int y = idx >> 4;
            int x = ((idx & 15) << 1) + ((y & 1) != temp_data.getSubPage());

            float t   = temp_data.getPixelTemperature(idx) - lowest_temp;
            int level = t * 256 / temp_diff;

            level          = (level < 0) ? 0 : (level > 255) ? 255 : level;
            uint32_t color = M5.Display.color888(level, level, level);
            M5.Display.fillRect(64 + x * 3, y * 3, 3, 3, color);
        }

        // Lowest temperature pixel.
        M5.Display.drawRect(64 + temp_data.getLowestX() * 3,
                            temp_data.getLowestY() * 3, 3, 3, TFT_BLUE);

        // Highest temperature pixel.
        M5.Display.drawRect(64 + temp_data.getHighestX() * 3,
                            temp_data.getHighestY() * 3, 3, 3, TFT_YELLOW);

        // Most temperature differences pixel.
        M5.Display.drawRect(64 + temp_data.getMostDifferenceX() * 3,
                            temp_data.getMostDifferenceY() * 3, 3, 3,
                            TFT_PURPLE);

        static int prev_sec = ~0u;
        int sec             = millis() / 1000;
        if (prev_sec != sec) {
            prev_sec = sec;
            M5.Display.setCursor(0, 8);
            M5.Display.printf("avg: %5.2f\n",
                              temp_data.getAverageTemperature());
            M5.Display.printf("med: %5.2f\n", temp_data.getMedianTemperature());
            M5.Display.printf("high:%5.2f\n",
                              temp_data.getHighestTemperature());
            M5.Display.printf("low: %5.2f\n", temp_data.getLowestTemperature());
            M5.Display.printf("fps: %02d\n", frame_count);
            frame_count = 0;
        }
    } else {
        delay(1);
    }
    M5.Display.setCursor(0, 0);
    M5.Display.printf("btn: %02x\n", thermal2.getButtonRawState());
    M5.Display.endWrite();

    // Determines if a button was pressed. (True only when pressed.)
    if (thermal2.btnWasPressed()) {
        // Led on , Buzzer on
        thermal2.ledOn();
        thermal2.buzzerOn();
    }

    // Determines if a button was released. (True only when released.)
    if (thermal2.btnWasReleased()) {
        // Led off , Buzzer off
        thermal2.ledOff();
        thermal2.buzzerOff();
    }

    // Determines if a button is pressed. (True if currently pressed.)
    if (thermal2.btnIsPressed()) {
    }

    // Determines if a button is released. (True if currently released.)
    if (thermal2.btnIsReleased()) {
    }

    // Determines if a button was clicked. (True only when clicked.)
    if (thermal2.btnWasClicked()) {
        thermal2.setLed(0, 4, 0);
        thermal2.setBuzzer(1500, 64);
    }

    // Determines if a button was hold. (True only when pressed and held for 500
    // msec.)
    if (thermal2.btnWasHold()) {
        thermal2.setLed(2, 2, 2);
        thermal2.setBuzzer(2000, 64);
    }
}
