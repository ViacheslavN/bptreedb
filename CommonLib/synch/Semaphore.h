#pragma once

#include <mutex>
#include <condition_variable>

namespace CommonLib
{

class Semaphore
{
public:
    explicit Semaphore(size_t count = 0)
            : _count(count)
    {
    }

    inline void Up()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if (!_release)
        {
            _count++;
            _cv.notify_one();
        }
    }

    inline void Down()
    {
        Down(0);
    }

    inline bool Down(std::uint32_t timeoutMs)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        while (!_release && _count == 0)
        {
            if (timeoutMs == 0)
            {
                _cv.wait(lock);
            }
            else
            {
                std::cv_status status = _cv.wait_for(lock, std::chrono::milliseconds(timeoutMs));
                if (status == std::cv_status::timeout)
                    return false;
            }
        }
        if (!_release)
        {
            --_count;
        }

        return true;
    }

    inline void Release()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _release = true;
        _cv.notify_all();
    }

private:
    std::mutex _mutex;
    std::condition_variable _cv;
    volatile size_t _count = 0;
    volatile bool _release = false;
};

}
