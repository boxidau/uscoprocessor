#define         MOCK_DATA                       false

// Engine params
#define         NUMBER_OF_CYLINDERS             4
#define         MIN_RPM                         800
#define         MAX_RPM                         6500


// Knock params
#define         KNOCK_SET_PRESCALER             0b01000110    /* 8MHz prescaler, SDO active. */
#define         KNOCK_SET_CHANNEL_1             0b11100000    /* Set active channel 1 */
#define         KNOCK_SET_CHANNEL_2             0b11100001    /* Set active channel 2 */
#define         KNOCK_SET_BAND_PASS_FREQ        0b00100100    /* 944 2.5L ~=5.7kHz */
#define         KNOCK_SET_GAIN                  0b10100100    /* Set gain 0.348 */
#define         KNOCK_SET_ADVANCED_MODE         0b01110001

// Knock pins
#define         KNOCK_TEST_PIN                  24
#define         KNOCK_HOLD_PIN                  25
#define         KNOCK_ADC_INPUT_PIN             PIN_A12
#define         EXTERNAL_DAC_0                  PIN_A21
#define         SPI_KNOCK_CS_PIN                9


// Device pins
#define         TACH_INPUT_PIN                  2
#define         EXTERNAL_DIGITAL_PIN            6
#define         EXTERNAL_DAC_1                  PIN_A22
#define         ERROR_LED                       23  // D23 A9
#define         TACH_LED                        22  // D22 A8
#define         EXTERNAL_PWM1                   7
#define         EXTERNAL_PWM2                   8
#define         EXTERNAL_ADC1                   PIN_A15
#define         EXTERNAL_ADC2                   PIN_A14
#define         EXTERNAL_NTC1                   PIN_A16
#define         EXTERNAL_NTC2                   PIN_A17




// CANBUS
#define         CAN_TX_PIN                      3
#define         CAN_RX_PIN                      4
#define         MEGASQUIRT_BROADCAST_CANID      1512
#define         BASE_CANID                      2024
#define         EGT_CANID                       2025
#define         ANALOG_CANID                    2026


// EGT
#define         SPI_EGT_CS_PIN                  15

