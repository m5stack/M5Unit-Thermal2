#include "M5_Thermal2.h"

class i2c_clock_changer {
    TwoWire* _wire;
    uint32_t _prev_freq;

   public:
    i2c_clock_changer(TwoWire* wire, uint32_t freq) : _wire{wire} {
        _prev_freq = wire->getClock();
        wire->setClock(freq);
    }
    virtual ~i2c_clock_changer() {
        _wire->setClock(_prev_freq);
    }
};

int M5_Thermal2::begin(TwoWire* wire, uint8_t addr, uint32_t freq) {
    _wire      = wire;
    _addr      = addr;
    _freq      = freq;
    _init_step = 1;
    int retry  = 16;
    while (!_checkInit() && --retry) {
        delay(16);
    }
    return retry;
}

bool M5_Thermal2::_checkInit(void) {
    if (_init_step > 1) return true;

    i2c_clock_changer i2cc = {_wire, _freq};

    _wire->beginTransmission(_addr);
    _wire->write(reg_index_status);
    if ((0 == _wire->endTransmission(false)) &&
        (sizeof(unit_thermal2_reg_t) ==
         _wire->requestFrom(_addr, sizeof(unit_thermal2_reg_t)))) {
        unit_thermal2_reg_t reg;
        _wire->readBytes((uint8_t*)&reg, sizeof(unit_thermal2_reg_t));
        if (reg.status.device_id_0 == reg_device_id_0 &&
            reg.status.device_id_1 == reg_device_id_1) {
            _init_step     = 2;
            _status        = reg.status;
            _config        = reg.config;
            _lowest_alarm  = reg.lowest_alarm;
            _highest_alarm = reg.highest_alarm;
        }
    }
    return (_init_step > 1);
}

bool M5_Thermal2::_updateConfig(void) {
    if (!_checkInit()) return false;

    i2c_clock_changer i2cc = {_wire, _freq};

    _wire->beginTransmission(_addr);
    _wire->write(reg_index_config);
    _wire->write((uint8_t*)&_config, sizeof(_config));
    _init_step = (0 == _wire->endTransmission()) ? 2 : 1;
    return (_init_step > 1);
}

bool M5_Thermal2::update(void) {
    if (!_checkInit()) return false;

    uint32_t need_clock = 6250u << getRefreshRate();
    if (need_clock < _freq) {
        need_clock = _freq;
    }
    i2c_clock_changer i2cc = {_wire, need_clock};

    _wire->beginTransmission(_addr);
    _wire->write(reg_index_status);
    if (0 != _wire->endTransmission(false)) return 0;
    if (2u == _wire->requestFrom(_addr, 2u)) {
        _wire->readBytes((uint8_t*)&_status, 2);
        if (_status.button & ~1u) {
            _wire->beginTransmission(_addr);
            _wire->write(reg_index_status);
            _wire->write(_status.button);
            _wire->endTransmission(true);
        }
    }

    _wire->beginTransmission(_addr);
    _wire->write(reg_index_refresh_control);
    if (0 != _wire->endTransmission(false)) return false;
    if (2 != _wire->requestFrom(_addr, 2u, true)) return false;
    int reg_0x6E = _wire->read();
    bool subpage = _wire->read();
    if (reg_0x6E < 0 || (0 == (reg_0x6E & 1))) return false;

    _wire->beginTransmission(_addr);
    _wire->write(reg_index_overview);
    bool result = (0 == _wire->endTransmission(false));
    if (result) {
        result = (sizeof(temperature_reg_t) ==
                  _wire->requestFrom(_addr, sizeof(temperature_reg_t))) &&
                 (sizeof(temperature_reg_t) ==
                  _wire->readBytes((uint8_t*)&(_latest_raw.temperature_reg),
                                   sizeof(temperature_reg_t)));

        _latest_raw.subpage = subpage;

        static constexpr uint8_t i2c_once_read = 128;
        static constexpr uint8_t read_count    = 768 / i2c_once_read;

        auto dst = (uint8_t*)_latest_raw.pixel_raw;
        for (uint_fast8_t i = 0; result && i < read_count; ++i) {
            result =
                (i2c_once_read == _wire->requestFrom(_addr, i2c_once_read)) &&
                (i2c_once_read == _wire->readBytes(dst, i2c_once_read));
            dst += i2c_once_read;
        }
    }

    if (result && (0 == (_config.function_ctrl & 0x04))) {
        _wire->beginTransmission(_addr);
        _wire->write(reg_index_refresh_control);
        _wire->write(0);
        result = (0 == _wire->endTransmission(true)) &&
                 (_latest_raw.temperature_reg.lowest_raw <
                  _latest_raw.temperature_reg.highest_raw);
    }
    return result;
}

bool M5_Thermal2::buzzerOn(void) {
    _config.function_ctrl |= 1 << 0;
    return _updateConfig();
}

bool M5_Thermal2::buzzerOff(void) {
    _config.function_ctrl &= ~(1 << 0);
    return _updateConfig();
}

bool M5_Thermal2::ledOn(void) {
    _config.function_ctrl |= (1 << 1);
    return _updateConfig();
}

bool M5_Thermal2::ledOff(void) {
    _config.function_ctrl &= ~(1 << 1);
    return _updateConfig();
}

bool M5_Thermal2::setBuzzer(uint16_t freq, uint8_t volume) {
    _config.buzzer_freq   = freq;
    _config.buzzer_volume = volume;
    return _updateConfig();
}

bool M5_Thermal2::setBuzzerFreq(uint16_t freq) {
    _config.buzzer_freq = freq;
    return _updateConfig();
}

bool M5_Thermal2::setBuzzerVolume(uint8_t volume) {
    _config.buzzer_volume = volume;
    return _updateConfig();
}

bool M5_Thermal2::setLed(uint8_t r, uint8_t g, uint8_t b) {
    _config.led.r = r;
    _config.led.g = g;
    _config.led.b = b;
    return _updateConfig();
}

bool M5_Thermal2::setAlarmHighTemp(float temp_threshold,
                                   uint16_t interval_msec10,
                                   uint16_t buzzer_freq, uint8_t led_r,
                                   uint8_t led_g, uint8_t led_b) {
    _highest_alarm.temp_threshold  = convertCelsiusToRaw(temp_threshold);
    _highest_alarm.buzzer_interval = interval_msec10;
    _highest_alarm.buzzer_freq     = buzzer_freq;
    _highest_alarm.led.r           = led_r;
    _highest_alarm.led.g           = led_g;
    _highest_alarm.led.b           = led_b;

    if (!_checkInit()) return false;

    i2c_clock_changer i2cc = {_wire, _freq};

    _wire->beginTransmission(_addr);
    _wire->write(reg_index_highest_alarm);
    _wire->write((uint8_t*)&_highest_alarm, sizeof(_highest_alarm));
    _init_step = (0 == _wire->endTransmission()) ? 2 : 1;
    return (_init_step > 1);
}

bool M5_Thermal2::setAlarmLowTemp(float temp_threshold,
                                  uint16_t interval_msec10,
                                  uint16_t buzzer_freq, uint8_t led_r,
                                  uint8_t led_g, uint8_t led_b) {
    _lowest_alarm.temp_threshold  = convertCelsiusToRaw(temp_threshold);
    _lowest_alarm.buzzer_interval = interval_msec10;
    _lowest_alarm.buzzer_freq     = buzzer_freq;
    _lowest_alarm.led.r           = led_r;
    _lowest_alarm.led.g           = led_g;
    _lowest_alarm.led.b           = led_b;

    if (!_checkInit()) return false;

    i2c_clock_changer i2cc = {_wire, _freq};

    _wire->beginTransmission(_addr);
    _wire->write(reg_index_lowest_alarm);
    _wire->write((uint8_t*)&_lowest_alarm, sizeof(_lowest_alarm));
    _init_step = (0 == _wire->endTransmission()) ? 2 : 1;
    return (_init_step > 1);
}

bool M5_Thermal2::alarmOn(alarm_bitmask_t alarm_bitmask) {
    _config.temp_alarm_enable |= alarm_bitmask;
    return _updateConfig();
}

bool M5_Thermal2::alarmOff(alarm_bitmask_t alarm_bitmask) {
    _config.temp_alarm_enable &= ~alarm_bitmask;
    return _updateConfig();
}

bool M5_Thermal2::setRefreshRate(refresh_rate_t rate) {
    _config.refresh_rate = (refresh_rate_t)(rate & 7);
    return _updateConfig();
}

bool M5_Thermal2::setNoiseFilterLevel(uint8_t level) {
    _config.noise_filter = level & 15;
    return _updateConfig();
}

bool M5_Thermal2::setMonitorArea(uint8_t width, uint8_t height) {
    if (width > 15) width = 15;
    if (height > 15) height = 15;
    _config.temp_monitor_area = width | height << 4;
    return _updateConfig();
}

bool M5_Thermal2::changeI2CAddr(uint8_t new_i2c_addr) {
    if (new_i2c_addr < 8 || new_i2c_addr > 0x77) {
        return false;
    }
    if (!_checkInit()) return false;

    _config.i2c_addr     = new_i2c_addr;
    _config.i2c_addr_inv = ~new_i2c_addr;
    return _updateConfig();
}
