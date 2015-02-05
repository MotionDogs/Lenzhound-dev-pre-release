#ifndef rxr_constants_h
#define rxr_constants_h

const char kBitShift = 15;
const int kSerialBaud = 9600;

// ISR constants
const long kSecondsInMicroseconds = 1000000L;
const long kIsrFrequency          = 6000L;
const long kPeriod                = kSecondsInMicroseconds/kIsrFrequency;

#endif // rxr_constants_h
