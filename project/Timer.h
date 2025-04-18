#if !defined(TESTUNIT_TICTOC_H)
#define TESTUNIT_TICTOC_H

#include <chrono>
#include <iostream>
#include <stack>
#include <string>

#define TIC(arg) TestUnit::Timer::Tic(arg);
#define TOC() TestUnit::Timer::Toc();

#include <chrono>
#include <stack>
#include <string>

namespace TestUnit {
class Timer {
 public:
  static void Tic(const std::string &name = "") {
    static int index = 0;
    TICTOCVec().push(TICTOCDS(name.empty() ? std::to_string(++index) : name));
  }

  static void Toc() {
    auto start = TICTOCVec().top();
    TICTOCVec().pop();

    std::cout << "TIMER " << start.name << " :\t"
              << std::chrono::duration_cast<std::chrono::microseconds>(
                     std::chrono::steady_clock::now() - start.time)
                         .count() /
                     1000.f
              << " ms" << std::endl;
  }

 private:
  struct TICTOCDS {
    TICTOCDS(const std::string &name)
        : time(std::chrono::steady_clock::now()), name(name) {}

    std::chrono::time_point<std::chrono::steady_clock> time;
    std::string name;
  };

  static std::stack<TICTOCDS> &TICTOCVec() {
    static std::stack<TICTOCDS> TICTOCVec;
    return TICTOCVec;
  }
};
}  // namespace TestUnit

#endif  // TESTUNIT_TICTOC_H
