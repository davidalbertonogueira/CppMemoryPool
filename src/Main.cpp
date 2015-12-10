#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "MemoryPool.h"
#include "Chrono.h"

#ifdef _WIN32
#include <crtdbg.h> //For memory leak debug
#endif

class Part {
public:
  Part() { position_ = tag_ = -1; };
  Part(int position, int tag) :
    position_(position), tag_(tag) {};
  virtual ~Part() {};

public:
  int position() { return position_; };
  int tag() { return tag_; };

private:
  int position_; // Position.
  int tag_; // Tag ID.
};

int main() {
  chrono::Chronometer chrono;

#ifdef _WIN32
  //For memory leak debug
  _CrtMemState memState_;
  _CrtMemCheckpoint(&memState_);
#endif

  chrono.GetTime();
  {
    MemPool<Part, 1000, 1> part_pool;

    for (int i = 0; i < 10000; i++) {
      // First, raw memory is requested from the memory pool
      Part * allocated_space = static_cast<Part*>(part_pool.GetNextBuffer());

      //Once that memory is granted, the new object is constructed in it
      allocated_space->Part::Part(2, 3);
      Part * one_part = allocated_space;
      //previous sintax is equivalent to using a "placement new"
      //Part * one_part = new (allocated_space) Part(2, 3);

      //object deconstruction
      one_part->~Part();
    }
    //discard alocated memory
    part_pool.Cleanup();
  }
  chrono.StopTime();
  std::cout << "Time: " << chrono.GetElapsedTime() << " sec." << std::endl;

#ifdef _WIN32
  //For memory leak debug
  _CrtMemState stateNow, stateDiff;
  _CrtMemCheckpoint(&stateNow);
  int diffResult = _CrtMemDifference(&stateDiff, &memState_, &stateNow);
  if (diffResult) {
    std::cerr << "Memory leak of " << stateDiff.lSizes[1]
      << " byte(s) detected." << std::endl;
  } else {
    std::cout << "No memory leak detected." << std::endl;
  }
#endif
  return 0;
}
