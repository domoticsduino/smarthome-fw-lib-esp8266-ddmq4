/* 1.0.0 VERSION */

#ifndef ddmq4_h
#define ddmq4_h

#include <Arduino.h>

struct DDMQ4Val {

    float sensorValue = 0.0;
    double ppm = 0.0;
    double percentage = 0.0;
    float realValue = 0.0;
    bool success = false;
    const char* errorMsg;

};

class DDMQ4 {
    
    private:
        int _analogPin;
        float _m = -0.318; //Slope
        float _b = 1.133; //Y-Intercept
        float _R0 = 11.820; //Sensor Resistance in fresh air from previous code
        float _valorecentrale = 512.0; //valore che corrisponde a 1000ppm - da ricavare in base allo schema elettrico

        float fscale(float, float, float, float, float, float);

    public:

        DDMQ4(int);
        
        DDMQ4Val getValue();

};

#endif