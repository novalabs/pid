/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <float.h>

#include <core/os/SysLock.hpp>

class PID
{
private:
    float _i;
    float _d;
    float _setpoint;
    float _k;
    float _ki;
    float _kd;
    float _min;
    float _max;

public:
    inline
    PID(
        void
    );
    inline void
    config(
        float k,
        float ti,
        float td,
        float ts,
        float min,
        float max
    );

    inline void
    set(
        float setpoint
    );

    inline void
    setI(
        float setpoint
    );

    inline float
    update(
        float measure
    );
};


PID::PID(
    void
)
{
    _i = 0;
    _d = 0;
    _setpoint = 0;
    _k   = 0;
    _ki  = 0;
    _kd  = 0;
    _min = -FLT_MAX;
    _max = FLT_MAX;
}

void
PID::config(
    float k,
    float ti,
    float td,
    float ts,
    float min = -FLT_MAX,
    float max = FLT_MAX
)
{
    core::os::SysLock::Scope lock;
    _k   = k;
    _ki  = (ti == 0) ? 0 : k * (ts / ti);
    _kd  = k * (td / ts);
    _min = min;
    _max = max;
    _i   = 0;
    _d   = 0;
}

void
PID::set(
    float setpoint
)
{
    core::os::SysLock::Scope lock;

    setI(setpoint);
}

void
PID::setI(
    float setpoint
)
{
     // Reset integral and derivative components if sign has changed or setpoint is 0
    if ((setpoint > 0) != (_setpoint > 0)) {
        _i = 0;
        _d = 0;
    }

    _setpoint = setpoint;
}
float
PID::update(
    float measure
)
{
    float error;
    float output;

    /* calculate error */
    error = _setpoint - measure;

    /* proportional term */
    output = (_k * error);

    /* integral term */
    _i     += _ki * error;
    output += _i;

    /* derivative term */
    output += _kd * (error - _d);
    _d      = error;

    /* saturation filter */
    if (output > _max) {
        output = _max;
        /* anti windup: cancel error integration */
        _i -= _ki * error;
    } else if (output < _min) {
        output = _min;
        /* anti windup: cancel error integration */
        _i -= _ki * error;
    }

    return output;
} // PID::update
