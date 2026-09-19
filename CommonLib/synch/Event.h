#pragma once

#include <mutex>
#include <condition_variable>

namespace CommonLib
{

class Event
{
public:
    explicit Event()
    {
    }

    explicit Event(bool isSet)
            : _isSet(isSet)
    {
    }

    inline void Set()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if (!_release)
        {
            _isSet = true;
            _cv.notify_all();
        }
    }

    inline void Unset()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if (!_release)
        {
            _isSet = false;
        }
    }

    inline bool IsSet() const
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _isSet;
    }

    inline bool Wait()
    {
        return Wait(0);
    }

    inline bool Wait(std::uint32_t timeoutMs)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        while (!_release && !_isSet)
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

        return _isSet;
    }

    inline void Release()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _release = true;
        _cv.notify_all();
    }

private:
    mutable std::mutex _mutex;
    std::condition_variable _cv;
    volatile bool _isSet = false;
    volatile bool _release = false;
};

}
