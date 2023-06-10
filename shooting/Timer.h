#pragma once
#include <thread>
#include <chrono>
#include <functional>
#include <cstdio>
#include <atomic>
class Timer
{
public:
    ~Timer() {
        //if (mRunning) {
        //    stop(const Timeout& timeout);
        //}
    }
    typedef std::chrono::milliseconds Interval;
    typedef std::function<void(void)> Timeout;

    void start(const Interval& interval, const Timeout& timeout) {
        mRunning = true;

        mThread = std::thread([this, interval, timeout] {
            while (mRunning) {
                std::this_thread::sleep_for(interval);

                timeout();
            }
            });
    }
    void stop(const Timeout& timeout) {
        mRunning = true;
        mThread = std::thread([this, timeout] {
            while (mRunning) {
                std::this_thread::sleep_for(std::chrono::seconds(10));
                timeout();
                mRunning = false;
                mThread.join();
            }
            });
    }

private:
    std::thread mThread{};
    std::atomic_bool mRunning{};
};

