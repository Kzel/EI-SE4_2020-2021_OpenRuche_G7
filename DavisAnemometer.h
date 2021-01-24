/**
 * Copyright (c) 2019, Jan Jongboom
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"

#ifndef DAVIS_ANEMOMETER_H_
#define DAVIS_ANEMOMETER_H_

class DavisAnemometer {
public:
    /**
     * Create a new anemometer instance
     *
     * @param windDirection Analog pin used for wind direction
     * @param windSpeed Digital pin used for wind speed (requires interrupts to work)
     */
    DavisAnemometer(PinName windDirection, PinName windSpeed) :
        _windDirection(windDirection), _windSpeed(windSpeed), _windSpeedCount(0)
    {

    }

    /**
     * Enable the wind speed sensor
     */
    void enable() {
        _windSpeed.fall(callback(this, &DavisAnemometer::windSpeedIrq));
        _windSpeedTimer.start();
        _windSpeedTimer.reset();
        _windSpeedCount = 0;
    }

    /**
     * Disable the wind speed sensor
     */
    void disable() {
        _windSpeed.fall(NULL);
        _windSpeedTimer.stop();
        _windSpeedCount = 0;
    }

    /**
     * Read the wind direction
     *
     * @returns A number between 0 and 360 degrees
     */
    uint16_t readWindDirection() {
        float windDirection = _windDirection.read() * 360.0f;
        return static_cast<uint16_t>(windDirection);
    }

    /**
     * Read the wind speed.
     * Reading the wind speed requires some sampling time, e.g. 3 seconds.
     * The sampling timer is reset every time this function is called.
     * Thus, don't read this function too often or it won't return accurate results.
     *
     * @returns Wind speed in km/h
     */
    float readWindSpeed() {
        float timePassed = static_cast<float>(_windSpeedTimer.read_ms()) / 1000.0f;
        float speed = static_cast<float>(_windSpeedCount) * (2.25f / timePassed) * 1.609f;

        // reset the state
        _windSpeedTimer.reset();
        _windSpeedCount = 0;

        return speed;
    }

private:
    // this runs in an ISR
    void windSpeedIrq() {
        _windSpeedCount++;
    }

    AnalogIn _windDirection;
    InterruptIn _windSpeed;

    // use lpticker on devices that have it enabled
#if DEVICE_LPTICKER
    LowPowerTimer _windSpeedTimer;
#else
    Timer _windSpeedTimer;
#endif

    uint64_t _windSpeedCount;
};

#endif // DAVIS_ANEMOMETER_H_
