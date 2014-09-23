#ifndef rxr_constants_h
#define rxr_constants_h

enum MicrostepInterval {
  EIGHTH_STEPS     = 3,
  QUARTER_STEPS    = 2,
  HALF_STEPS       = 1,
  FULL_STEPS       = 0
};

const char kBitShift = 15;
const int kSerialBaud = 9600;
const MicrostepInterval kMicrosteps = EIGHTH_STEPS;

// ISR constants
const long kSecondsInMicroseconds = 1000000L;
const long kIsrFrequency          = 6000L;
const long kPeriod                = kSecondsInMicroseconds/kIsrFrequency;

#endif // rxr_constants_h
