#include "Arduino.h"

#define MIN_DAC_VAL 0
#define MAX_DAC_VAL 990

class GaugeOutput {
public:
    static void setup(uint16_t _inputMin, uint16_t _inputMax);  // set input range, eg if used for boost 20kPa, 200kPa
    static void update(uint16_t inputValue);  // update current value in the above range
private:
    static uint16_t inputMin;
    static uint16_t inputMax;
};