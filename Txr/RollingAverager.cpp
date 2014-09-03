#include "RollingAverager.h"

namespace rollingaveragernamespace {
  
RollingAverager::RollingAverager(int startVal) {
  buffer_index_ = 0;
  sum = 0;
  for (int i=0; i<kBufferSize; i++) {
    buffer_[i]=startVal; 
    sum += startVal;
  }
}

long RollingAverager::Roll(long next_input) {
  ++buffer_index_;
  buffer_index_ &= (kBufferSize - 1);
  sum += next_input;
  sum -= buffer_[buffer_index_];
  buffer_[buffer_index_] = next_input;
  return sum / kBufferSize;
}

}
