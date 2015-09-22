#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <chrono>
#include <iostream>
#include <array>
#include "Pool.h"
//#include "MultisetPool.h"

class BigObject {

public:

  //Each BigObject occupies 1MB of RAM
  static const int32_t NUM_BYTES{1000 *1024};

  void reset() {} //Required by the Pool class template

  void setFirstChar(char c) {byteArray[0] = c;}

private:

  std::array<char, NUM_BYTES> byteArray{};

};

TEST_CASE( "Allocations/Deallocations", "[Pool]" ) {

    constexpr int32_t NUM_ALLOCS{2};
    Pool<BigObject, NUM_ALLOCS> objPool;

    BigObject* obj1{objPool.acquire()};
    REQUIRE(obj1 not_eq nullptr);

    BigObject* obj2 = objPool.acquire();
    REQUIRE(obj2 not_eq nullptr);

    objPool.release(obj1);
    BigObject* obj3 = objPool.acquire();
    REQUIRE(obj3 == obj1);

    BigObject* obj4 = objPool.acquire();
    REQUIRE(obj4 == nullptr);

    objPool.release(obj2);
    BigObject* obj5 = objPool.acquire();
    REQUIRE(obj5 == obj2);

    void measurePerformance();
    measurePerformance();

}


//Compare the performance of three types of memory allocation:
//Stack, free store (heap), custom-written pool
void measurePerformance() {

  using namespace std::chrono; //Reduce verbosity of subsequent code
  constexpr int32_t NUM_ALLOCS{1000};
  void processObj(BigObject* obj);

  //Time heap allocations
  auto ts = system_clock::now();
  for(int32_t i=0; i<NUM_ALLOCS; ++i) {
    //BigObject* obj{new BigObject{}};
    auto obj = std::make_shared<BigObject>();
    //Do stuff.......
    processObj(obj.get());
    //delete obj;
  }
  auto te = system_clock::now();
  std::cout << "Heap Allocations took "
            << duration_cast<milliseconds>(te-ts).count()
            << " milliseconds"
            << std::endl;

  //Time stack allocations
  ts = system_clock::now();
  for(int32_t i=0; i<NUM_ALLOCS; ++i) {
    BigObject obj{};
    //Do stuff.......
    processObj(&obj);
  } //obj is auto-destroyed here
  te = system_clock::now();
  std::cout << "Stack Allocations took "
            << duration_cast<milliseconds>(te-ts).count()
            << " milliseconds"
            <<  std::endl;

  ts = system_clock::now();
  Pool<BigObject, NUM_ALLOCS> objPool;
  te = system_clock::now();
  std::cout << "Pool construction took "
            << duration_cast<milliseconds>(te-ts).count()
            << " milliseconds"
            <<  std::endl;

  //Time pool allocations
  ts = system_clock::now();
  for(int32_t i=0; i<NUM_ALLOCS; ++i) {
    BigObject* poolObj{objPool.acquire()};
    //Do Stuff.......
    processObj(poolObj);
    //objPool.release(poolObj);
  }
  te = system_clock::now();
  std::cout << "Pool Allocations took "
            << duration_cast<milliseconds>(te-ts).count()
            << " milliseconds"
            << std::endl;

}

void processObj(BigObject* obj) {

  obj->setFirstChar('a');

}


