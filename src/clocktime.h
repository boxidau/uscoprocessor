#include <TimeLib.h>

class ClockTimeClass {
public:
  static void setup();
  static double getMillisTime();
};


extern ClockTimeClass ClockTime;