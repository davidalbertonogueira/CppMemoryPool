#ifndef CHRONO_H
#define CHRONO_H

#include <chrono>

#ifdef _WIN32
#include <time.h>
#else
#include <sys/time.h>
#endif

namespace chrono {
//******** USAGE *********
//chrono::Chronometer chrono; //Declare a Chronometer
//chrono.GetTime(); //Start timer
//{
//  //do your code
//}
//chrono.StopTime(); //Stop timer
//std::cout << "Time: " << chrono.GetElapsedTime() << " sec." << std::endl; //Print duration
class Chronometer {
public:
  Chronometer() {
    time_span = std::chrono::steady_clock::duration::zero();
  };
  virtual ~Chronometer() {};

  void GetTime() {
    clock_begin = std::chrono::steady_clock::now();
  }
  void StopTime() {
    std::chrono::steady_clock::time_point clock_end = std::chrono::steady_clock::now();
    time_span += clock_end - clock_begin;
  }
  //Return elapsed time in seconds
  double GetElapsedTime() {
    return double(time_span.count()) *
      std::chrono::steady_clock::period::num / std::chrono::steady_clock::period::den;
  }
protected:
  std::chrono::steady_clock::time_point clock_begin;
  std::chrono::steady_clock::duration time_span;
};
}

#endif // CHRONO_H
