#pragma once

// lang::Cpp
// Code was borrowed from the Concurrency lecture provided on Piazza.

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <mutex>
#include <sstream>
#include <thread>

#include "custom_object.h"
#include "custom_string.h"

/** A Thread wraps the thread operations in the standard library.
 *  author: vitekj@me.com */
class Thread : public CustomObject {
public:
  std::thread thread_;

  /** Starts running the thread, invoked the run() method. */
  void start() {
    thread_ = std::thread([this] { this->run(); });
  }

  /** Wait on this thread to terminate. */
  void join() { thread_.join(); }

  // Detaches a thread
  void detach() { thread_.detach(); }

  /** Yield execution to another thread. */
  static void yield() { std::this_thread::yield(); }

  /** Sleep for millis milliseconds. */
  static void sleep(size_t millis) {
    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
  }

  /** Subclass responsibility, the body of the run method */
  virtual void run() { assert(false); }
};

/** A convenient lock and condition variable wrapper. */
class Lock : public CustomObject {
public:
  std::mutex mtx_;
  std::condition_variable_any cv_;

  /** Request ownership of this lock.
   *
   *  Note: This operation will block the current thread until the lock can
   *  be acquired.
   */
  void lock() { mtx_.lock(); }

  /** Release this lock (relinquish ownership). */
  void unlock() { mtx_.unlock(); }

  /** Sleep and wait for a notification on this lock.
   *
   *  Note: After waking up, the lock is owned by the current thread and
   *  needs released by an explicit invocation of unlock().
   */
  void wait() { cv_.wait(mtx_); }

  // Notify all threads waiting on this lock
  void notify_all() { cv_.notify_all(); }
};
