#define         KNOCK_MIN_INTEG_TIME            0b11000000    /* Defines the time constant at high speed to 40 μs.*/
#define         KNOCK_MAX_INTEG_TIME            0b11011000    /* Defines the time constant at low speed to 320 μs.*/


class KnockDetectorClass {
public:
    static void setup();
    static void loop();
    static bool error;
private:
    static char SPIWrite(const char tx_data);
};

extern KnockDetectorClass Knock;