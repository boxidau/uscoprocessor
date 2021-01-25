#include "gaugeoutput.h"
#include "constants.h"

uint16_t GaugeOutput::inputMin = 0;
uint16_t GaugeOutput::inputMax = 100;

void GaugeOutput::setup(uint16_t _inputMin, uint16_t _inputMax)
{
    pinMode(EXTERNAL_DAC_1, OUTPUT);
    analogWriteDAC1(MIN_DAC_VAL);
    inputMin = _inputMin;
    inputMax = _inputMax;
}

void GaugeOutput::update(uint16_t inputValue)
{
    inputValue = constrain(inputValue, inputMin, inputMax);
    analogWriteDAC1(
        map(inputValue, inputMin, inputMax, MIN_DAC_VAL, MAX_DAC_VAL));
}