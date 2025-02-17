//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: no-threads
// ALLOW_RETRIES: 2

// <mutex>

// class timed_mutex;

// void lock();

#include <cassert>
#include <chrono>
#include <cstdlib>
#include <mutex>
#include <thread>

#include "make_test_thread.h"
#include "test_macros.h"

std::timed_mutex m;

typedef std::chrono::system_clock Clock;
typedef Clock::time_point time_point;
typedef Clock::duration duration;
typedef std::chrono::milliseconds ms;
typedef std::chrono::nanoseconds ns;

void f()
{
    time_point t0 = Clock::now();
    m.lock();
    time_point t1 = Clock::now();
    m.unlock();
    ns d = t1 - t0 - ms(250);
#if TEST_SLOW_HOST()
    assert(d < ms(150));  // within 150ms
#else
    assert(d < ms(50));  // within 50ms
#endif
}

int main(int, char**)
{
    m.lock();
    std::thread t = support::make_test_thread(f);
    std::this_thread::sleep_for(ms(250));
    m.unlock();
    t.join();

  return 0;
}
