//
//  driver.hpp
//  GLFW Test
//
//  Created by Logan Pazol on 8/18/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef driver_hpp
#define driver_hpp

// Temp
#define TICKS_PER_SECOND 30.0

#include <iostream>
#include <math.h>

/******************************************************************************
 *  Definition for default value driver class                                 *
 ******************************************************************************/

template <class T>
class ValueDriver {
    
    public:
    
        ValueDriver(T* _value_location);
    
        virtual void updateValue();
        virtual T getRenderValue(double interpolation) = 0;
    
        void play();
        void stop();
        void reverse();
        void reset();
    
    protected:
    
        T* value_location;
        double duration_in_ticks;
        int ticks_elapsed;
    
        double direction = 1.0;
        bool playing = false;
    
};

/******************************************************************************
 *  Definition for the Sine driver, tween in between 0 and amplitude          *
 ******************************************************************************/

template <class T>
class SinDriver : public ValueDriver<T> {
    
    public:
    
        SinDriver(T* _value_location, double duration, double _amplitude, double _base_value);
    
        virtual void updateValue();
        virtual T getRenderValue(double interpolation);
    
    private:
    
        double amplitude;
        double base_value;
    
};

template <class T>
ValueDriver<T>::ValueDriver(T* _value_location) {
    
    value_location = _value_location;
    ticks_elapsed = 0;
    
}

/******************************************************************************
 *  Functions to control the playback of the driver                           *
 ******************************************************************************/

template <class T>
void ValueDriver<T>::updateValue() {
    
    if (playing) {
    
        // Update the tick we are on
        if (this->ticks_elapsed < ceil(this->duration_in_ticks) && this->direction == 1.0)
            this->ticks_elapsed++;
        else if (this->ticks_elapsed > 0 && this->direction == -1.0)
            this->ticks_elapsed--;
        else this->playing = false;
        
    }
    
}

template <class T>
void ValueDriver<T>::play() {
    
    direction = 1.0;
    playing = true;

}

template <class T>
void ValueDriver<T>::stop() { playing = false; }

template <class T>
void ValueDriver<T>::reverse() {
    
    direction = -1.0;
    playing = true;
    
}

template <class T>
void ValueDriver<T>::reset() { ticks_elapsed = 0; }

/******************************************************************************
 *  Functions for the Sine driver class                                       *
 ******************************************************************************/

template <class T>
SinDriver<T>::SinDriver(T* _value_location, double duration, double _amplitude, double _base_value) : ValueDriver<T>(_value_location) {
    
    this->duration_in_ticks = duration * TICKS_PER_SECOND;
    amplitude = _amplitude;
    base_value = _base_value;
    
}

template <class T>
void SinDriver<T>::updateValue() {
    
    // Perform the update logic for the base class
    ValueDriver<T>::updateValue();
    
    if (this->playing) {
    
        // Calculate the percentage complete we are
        double percent_complete = (double)this->ticks_elapsed / this->duration_in_ticks;
    
        if (percent_complete >= 1.0)
            *this->value_location = (T)(amplitude + base_value);
        else if (percent_complete <= 0.0)
            *this->value_location = (T)(base_value);
        else {
            
            // Calculate the value of the sign function
            double sin_value = sin(M_PI_2 * percent_complete);
    
            *this->value_location = (T)(sin_value * amplitude + base_value);
            
        }
    
    }

}

template <class T>
T SinDriver<T>::getRenderValue(double interpolation) {
    
    // Add interpolation only if we are playing
    double current_progress = (double)this->ticks_elapsed;
    if (this->playing)
        current_progress += interpolation * this->direction;
    
    // Calculate the percentage complete we are
    double percent_complete = current_progress / this->duration_in_ticks;

    if (percent_complete >= 1.0)
        return (T)(amplitude + base_value);
    
    if (percent_complete <= 0.0)
        return (T)base_value;
    
    // Calculate the value of the sign function
    double sin_value = sin(M_PI_2 * percent_complete);
    
    return (T)(sin_value * amplitude + base_value);
}

#endif /* driver_hpp */
