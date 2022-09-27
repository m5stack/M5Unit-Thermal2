/*!
 * @brief An thermocouple sensor with I2C communication From M5Stack
 * @copyright Copyright (c) 2022 by M5Stack[https://m5stack.com]
 *
 * @Links [Unit Thermal2](https://docs.m5stack.com/en/unit/thermal2)
 * @version  V0.0.1
 * @date  2022-09-25
 */
#ifndef _M5_THERMAL2_H_
#define _M5_THERMAL2_H_

#include <Arduino.h>
#include <Wire.h>

#include <stdint.h>

class M5_Thermal2 {
   public:
    static constexpr uint8_t i2c_default_addr          = 0x32;
    static constexpr uint8_t reg_index_status          = 0x00;
    static constexpr uint8_t reg_index_config          = 0x08;
    static constexpr uint8_t reg_index_lowest_alarm    = 0x20;
    static constexpr uint8_t reg_index_highest_alarm   = 0x30;
    static constexpr uint8_t reg_index_refresh_control = 0x6E;
    static constexpr uint8_t reg_index_overview        = 0x70;
    static constexpr uint8_t reg_device_id_0           = 0x90;
    static constexpr uint8_t reg_device_id_1           = 0x64;

    static inline float convertRawToCelsius(uint16_t rawdata) {
        return ((float)rawdata / 128) - 64.0f;
    }

    static inline uint16_t convertCelsiusToRaw(float temperature) {
        int res = (temperature + 64) * 128;
        return (res < 0) ? 0 : ((res > UINT16_MAX) ? UINT16_MAX : res);
    }

    enum refresh_rate_t : uint8_t {
        rate_0_5Hz,
        rate_1Hz,
        rate_2Hz,
        rate_4Hz,
        rate_8Hz,
        rate_16Hz,
        rate_32Hz,
        rate_64Hz,
    };

    // alarm enable flag.
    // [7] High temp reached high threshold
    // [6] Ave  temp reached high threshold
    // [5] Med  temp reached high threshold
    // [4] Low  temp reached high threshold
    // [3] High temp reached low threshold
    // [2] Ave  temp reached low threshold
    // [1] Med  temp reached low threshold
    // [0] Low  temp reached low threshold
    enum alarm_bitmask_t : uint8_t {
        alarm_lowest_low   = 1 << 0,
        alarm_lowest_med   = 1 << 1,
        alarm_lowest_avg   = 1 << 2,
        alarm_lowest_high  = 1 << 3,
        alarm_highest_low  = 1 << 4,
        alarm_highest_med  = 1 << 5,
        alarm_highest_avg  = 1 << 6,
        alarm_highest_high = 1 << 7,
    };

    // button state flag.
    // [4]: button holded flag. ( 500msec hold )
    // [3]: button clicked flag. (short click)
    // [2]: button was released flag.
    // [1]: button was pressed flag.
    // [0]: button is pressed. (current state)
    enum button_bitmask_t : uint8_t {
        button_is_pressed   = 1 << 0,
        button_was_pressed  = 1 << 1,
        button_was_released = 1 << 2,
        button_was_clicked  = 1 << 3,
        button_was_hold     = 1 << 4,
    };

    struct temperature_data_t;

    /*! @brief Initialize the Unit Thermal2.
        @param wire Pointer to Wire to be used.
        @param i2c_addr I2C address of UnitThermal2.
        @param i2c_freq I2C communication frequency.
        @param i2c_freq_pixelread I2C communication frequency (for read pixel).
                ※ If the sampling rate of the camera is increased,
                    read pixel speed should also be increased.
                    16Hz=200kHz  /  32Hz=400kHz /  64Hz=800kHz

                ※ Note that if the frequency is higher than 400 kHz, the rate of
                    communication errors will increase depending on conditions
                    such as the quality of the communication cable and the
                    presence of other devices.
        @return true:success / false:failure */
    int begin(TwoWire* wire = &Wire, uint8_t i2c_addr = i2c_default_addr,
              uint32_t i2c_freq = 0, uint32_t i2c_freq_pixelread = 0);

    /*! @brief Set the I2C communication speed.
        @param i2c_freq I2C communication frequency.
        @param i2c_freq_pixelread I2C communication frequency (for read pixel).
                ※ If the sampling rate of the camera is increased,
                    read pixel speed should also be increased.
                    16Hz=200kHz  /  32Hz=400kHz /  64Hz=800kHz

                ※ Note that if the frequency is higher than 400 kHz, the rate of
                    communication errors will increase depending on conditions
                    such as the quality of the communication cable and the
                    presence of other devices. */
    void setI2CFreq(uint32_t freq, uint32_t freq_pixelread = 0);

    /*! @brief Update temperature data and button state.
        @return true:success / false:failure */
    bool update(void);

    /*! @brief Get temperature data from the last update.
        @return true:success / false:failure */
    inline const temperature_data_t& getTemperatureData(void) {
        return _latest_raw;
    }

    /*! @brief Change the refresh rate of the camera.
        @param rate New refresh rate (0 ~ 7)
                    0=1/2Hz  1=1Hz  2=2Hz  3=4Hz  4=8Hz  5=16Hz  6=32Hz  7=64Hz
        @return true:success / false:failure */
    bool setRefreshRate(refresh_rate_t rate);
    inline bool setRefreshRate(int rate) {
        return setRefreshRate((refresh_rate_t)rate);
    }

    /*! @brief Change the noise filter level.
        @param level New level (0 ~ 15) (0=disabled)
        @return true:success / false:failure */
    bool setNoiseFilterLevel(uint8_t level);

    /*! @brief Set the area to be monitored.
               Smaller values monitor only the center of the camera.
               For example, specifying 7,7 will use the center 16x16.
        @param width width of area (0 ~ 15) default:15
        @param height height of area (0 ~ 11) default:11
        @return true:success / false:failure */
    bool setMonitorArea(uint8_t width, uint8_t height);

    /*! @brief Set high temperature alarm details.
        @param temp_threshold Temperature of threshold.
        @param interval_msec10 Buzzer and LED on/off cycle. (5 ~ 255)x10 msec.
        @param buzzer_freq Buzzer sound frequency. (0 = disable buzzer.)
        @param led_r Red component of LED.
        @param led_g Green component of LED.
        @param led_b Blue component of LED.
        @return true:success / false:failure */
    bool setAlarmHighTemp(float temp_threshold, uint16_t interval_msec10,
                          uint16_t buzzer_freq, uint8_t led_r, uint8_t led_g,
                          uint8_t led_b);

    /*! @brief Set low temperature alarm details.
        @param temp_threshold Temperature of threshold.
        @param interval_msec10 Buzzer and LED on/off cycle. (5 ~ 255)x10 msec.
        @param buzzer_freq Buzzer sound frequency. (0 = disable buzzer.)
        @param led_r Red component of LED.
        @param led_g Green component of LED.
        @param led_b Blue component of LED.
        @return true:success / false:failure */
    bool setAlarmLowTemp(float temp_threshold, uint16_t interval_msec10,
                         uint16_t buzzer_freq, uint8_t led_r, uint8_t led_g,
                         uint8_t led_b);

    /*! @brief Enable temperature alarm.
        @param alarm_bitmask Alarm type to be enabled.
        @return true:success / false:failure */
    bool alarmOn(alarm_bitmask_t alarm_bitmask);
    inline bool alarmOn(uint8_t alarm_bitmask = 0xFFu) {
        return alarmOn((alarm_bitmask_t)alarm_bitmask);
    }

    /*! @brief Disable temperature alarm.
        @param alarm_bitmask Alarm type to be disabled.
        @return true:success / false:failure */
    bool alarmOff(alarm_bitmask_t alarm_bitmask);
    inline bool alarmOff(uint8_t alarm_bitmask = 0xFFu) {
        return alarmOff((alarm_bitmask_t)alarm_bitmask);
    }

    /*! @brief Set the volume of the buzzer. (Alarm volume also depends on this)
        @param volume Buzzer volume (0~255)
        @return true:success / false:failure */
    bool setBuzzerVolume(uint8_t volume);

    /*! @brief Set frequency of the buzzer when alarm is not activated.
        @param freq Buzzer sound frequency. (0 = disable buzzer.)
        @return true:success / false:failure */
    bool setBuzzerFreq(uint16_t freq);

    /*! @brief Set frequency and volume of the buzzer.
        @param freq Buzzer sound frequency. (0 = disable buzzer.)
        @param volume Buzzer volume (0~255)
        @return true:success / false:failure */
    bool setBuzzer(uint16_t freq, uint8_t volume);

    /*! @brief Turn on buzzer when alarm is not activated.
        @return true:success / false:failure */
    bool buzzerOn(void);

    /*! @brief Turn off buzzer when alarm is not activated.
        @return true:success / false:failure */
    bool buzzerOff(void);

    /*! @brief Set color of the LED when alarm is not activated.
        @return true:success / false:failure */
    bool setLed(uint8_t r, uint8_t g, uint8_t b);

    /*! @brief Turn on LED when alarm is not activated.
        @return true:success / false:failure */
    bool ledOn(void);

    /*! @brief Turn off LED when alarm is not activated.
        @return true:success / false:failure */
    bool ledOff(void);

    /*! @brief Get the state of the button when it was last updated.
        @return true: button is pressed */
    inline bool btnIsPressed(void) const {
        return _status.button & button_bitmask_t::button_is_pressed;
    }

    /*! @brief Get the state of the button when it was last updated.
        @return true: button is released */
    inline bool btnIsReleased(void) const {
        return !btnIsPressed();
    }

    /*! @brief Get the state of the button when it was last updated.
        @return true: button was pressed */
    inline bool btnWasPressed(void) const {
        return _status.button & button_bitmask_t::button_was_pressed;
    }

    /*! @brief Get the state of the button when it was last updated.
        @return true: button was released */
    inline bool btnWasReleased(void) const {
        return _status.button & button_bitmask_t::button_was_released;
    }

    /*! @brief Get the state of the button when it was last updated.
        @return true: button was clicked (short click) */
    inline bool btnWasClicked(void) const {
        return _status.button & button_bitmask_t::button_was_clicked;
    }

    /*! @brief Get the state of the button when it was last updated.
        @return true: button was hold (500msec pressed hold) */
    inline bool btnWasHold(void) const {
        return _status.button & button_bitmask_t::button_was_hold;
    }

    /*! @brief Get the state of the button when it was last updated.
        @return Raw value of button state */
    inline button_bitmask_t getButtonRawState(void) const {
        return (button_bitmask_t)_status.button;
    }

    /*! @brief Get the current camera refresh rate.
        @return camera refresh rate */
    inline refresh_rate_t getRefreshRate(void) const {
        return (refresh_rate_t)(_config.refresh_rate & 7);
    };

    /*! @brief Get the current noise filter level.
        @return noise filter level */
    inline uint8_t getNoiseFilterLevel(void) const {
        return _config.noise_filter & 15;
    }

    /*! @brief Change the I2C address of the unit.
        @brief The address is stored even when the power is turned off.
        @param new_i2c_addr New I2C address (0x08 ~ 0x77)
        @return true:success / false:failure
        @attention When the power is turned back on, it will operate at the new
       address. */
    bool changeI2CAddr(uint8_t new_i2c_addr);

#pragma pack(push)
#pragma pack(1)

    struct temperature_reg_t {
        uint16_t median_raw;
        uint16_t average_raw;
        uint16_t most_diff_raw;
        uint8_t most_diff_x;
        uint8_t most_diff_y;
        uint16_t lowest_raw;
        uint8_t lowest_x;
        uint8_t lowest_y;
        uint16_t highest_raw;
        uint8_t highest_x;
        uint8_t highest_y;
    };

    struct temperature_data_t {
        inline bool getSubPage(void) const {
            return subpage;
        }

        inline float getPixelTemperature(uint_fast16_t index) const {
            return convertRawToCelsius((index < 384) ? pixel_raw[index] : 0);
        }
        inline float getLowestTemperature(void) const {
            return convertRawToCelsius(temperature_reg.lowest_raw);
        }
        inline float getHighestTemperature(void) const {
            return convertRawToCelsius(temperature_reg.highest_raw);
        }
        inline float getMedianTemperature(void) const {
            return convertRawToCelsius(temperature_reg.median_raw);
        }
        inline float getAverageTemperature(void) const {
            return convertRawToCelsius(temperature_reg.average_raw);
        }
        inline float getMostDifferenceTemperature(void) const {
            return ((float)temperature_reg.most_diff_raw) / 128;
        }

        inline uint16_t getPixelRaw(uint_fast16_t index) const {
            return (index < 384) ? pixel_raw[index] : 0;
        }
        inline uint16_t getLowestRaw(void) const {
            return temperature_reg.lowest_raw;
        }
        inline uint16_t getHighestRaw(void) const {
            return temperature_reg.highest_raw;
        }
        inline uint16_t getMedianRaw(void) const {
            return temperature_reg.median_raw;
        }
        inline uint16_t getAverageRaw(void) const {
            return temperature_reg.average_raw;
        }
        inline uint16_t getMostDifferenceRaw(void) const {
            return temperature_reg.most_diff_raw;
        }

        inline uint8_t getLowestX(void) const {
            return temperature_reg.lowest_x;
        }
        inline uint8_t getLowestY(void) const {
            return temperature_reg.lowest_y;
        }
        inline uint8_t getHighestX(void) const {
            return temperature_reg.highest_x;
        }
        inline uint8_t getHighestY(void) const {
            return temperature_reg.highest_y;
        }
        inline uint8_t getMostDifferenceX(void) const {
            return temperature_reg.most_diff_x;
        }
        inline uint8_t getMostDifferenceY(void) const {
            return temperature_reg.most_diff_y;
        }

        temperature_reg_t temperature_reg;
        uint16_t pixel_raw[384];
        bool subpage;
    };

    struct rgb_t {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        bool operator==(const rgb_t& rhs) {
            return r == rhs.r && g == rhs.g && b == rhs.b;
        }
        bool operator!=(const rgb_t& rhs) {
            return !(*this == rhs);
        }
    };

    struct alarm_reg_t {
        uint16_t temp_threshold;
        uint16_t buzzer_freq;
        uint8_t buzzer_interval;
        rgb_t led;
    };

    struct status_reg_t {
        uint8_t button;
        uint8_t alarm;
        uint16_t reserved_0x02;
        uint8_t device_id_0;
        uint8_t device_id_1;
        uint8_t version_major;
        uint8_t version_minor;
    };

    struct config_reg_t {
        uint8_t i2c_addr;
        uint8_t i2c_addr_inv;
        uint8_t function_ctrl;
        refresh_rate_t refresh_rate;
        uint8_t noise_filter;
        uint8_t reserved_0x0D;
        uint16_t reserved_0x0E;

        uint8_t temp_monitor_area;
        uint8_t temp_alarm_enable;
        uint16_t buzzer_freq;
        uint8_t buzzer_volume;
        rgb_t led;
    };

    struct unit_thermal2_reg_t {
        // 0x00 ~ 0x07
        status_reg_t status;

        // 0x08 ~ 0x1F
        config_reg_t config;
        uint8_t reserved_0x18[8];

        // 0x20 = 0x2F
        alarm_reg_t lowest_alarm;
        uint8_t reserved_0x28[8];

        // 0x30 = 0x37
        alarm_reg_t highest_alarm;
    };

#pragma pack(pop)

   private:
    TwoWire* _wire           = nullptr;
    uint32_t _freq           = 400000;
    uint32_t _freq_pixelread = 400000;
    uint16_t _addr           = i2c_default_addr;
    uint8_t _init_step       = 0;

    temperature_data_t _latest_raw;
    status_reg_t _status;
    config_reg_t _config;
    alarm_reg_t _lowest_alarm;
    alarm_reg_t _highest_alarm;

    bool _checkInit(void);
    bool _updateConfig(void);
};

#endif
