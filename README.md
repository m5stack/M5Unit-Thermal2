# M5Unit-Thermal2

## Overview

Contains case programs of M5Stack UNIT THERMAL2. 

### SKU:U149
![Unit Thermal 2](./docs/unit-thermal2.webp)

Contains M5Stack-**UNIT THERMAL 2** series related case programs.

THERMAL2 is an infrared sensor with integrated MLX90640 internally to detect temperature data.

## Related Link

- [Link](https://docs.m5stack.com/en/unit/UNIT%20Thermal2)

## How To Use

```
#include <M5_Thermal2.h>

M5_Thermal2 thermal2;

void setup(void) {

    // prepare I2C.
    Wire.begin((int)SDA, (int)SCL, 400000L);
//  Wire.begin(21, 22, 400000L);

    // prepare sensor. ( default I2C addr : 0x32 )
    thermal2.begin();
//  thermal2.begin(&Wire, 0x32);
}

void loop(void) {
    delay(1);

    // Update button status and image data.
    if (thermal2.update()) {
        auto &tempdata = thermal2.getTemperatureData();

        printf("avg :%5.2f  ", tempdata.average_temp);
        printf("med :%5.2f  ", tempdata.median_temp);
        printf("high:%5.2f  ", tempdata.highest_temp);
        printf("low :%5.2f\n", tempdata.lowest_temp);
    }
}
```

## License

- [M5Unit-THERMAL2 - MIT](LICENSE)