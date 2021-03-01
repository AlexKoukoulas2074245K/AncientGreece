///------------------------------------------------------------------------------------------------
///  Timer.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef Timer_h
#define Timer_h

///-----------------------------------------------------------------------------------------------

#include <functional> 

///-----------------------------------------------------------------------------------------------
/// A class dedicated to counting down from a given period and exposing a HasTicked and other utility methods
/// to the client to indicate when the timer has finished, started, running, etc.
class Timer final
{
public:    
    ///-----------------------------------------------------------------------------------------------
    /// @param[in] period the period of the timer in seconds, i.e. for a 1 second timer the period would be 1.0f
    explicit Timer(const float period)
        : mPeriod(period)
        , mTimeCounter(mPeriod)
        , mHasTicked(false)
        , mIsRunning(true)
    
    {
    }

    ///-----------------------------------------------------------------------------------------------
    /// The update method of the timer.
    ///
    /// This must be called once per frame, otherwise the timer will not update.
    /// @param[in] dt the inter-frame delta time.
    inline void Update(const float dt)
    {
        if (!mIsRunning)
        {
            return;
        }
        
        mTimeCounter -= dt;
        if (mTimeCounter <= 0.0f)
        {
            mHasTicked = true;
        }
    }
    
    ///-----------------------------------------------------------------------------------------------
    /// Gets the raw float timer value.
    /// @returns the raw float timer value.
    inline float GetTimerValue() const
    {
        return mTimeCounter;
    }

    ///-----------------------------------------------------------------------------------------------
    /// Accessor to whether the timer has finished.
    /// @returns whether or not the timer has finished.
    inline bool HasTicked() const
    {
        return mHasTicked;
    }

    ///-----------------------------------------------------------------------------------------------
    /// Sets the current timer value (not period).
    /// @param[in] value the new value for this Timer's running value.    
    inline void SetTimerValue(const float value)
    {
        mTimeCounter = value;
    }
    
    ///-----------------------------------------------------------------------------------------------
    /// Pauses this Timer's execution.   
    inline void Pause()
    {
        mIsRunning = false;
    }
    
    ///-----------------------------------------------------------------------------------------------    
    /// Resumes this Timer's execution.   
    inline void Resume()
    {
        mIsRunning = true;
    }
    
    ///-----------------------------------------------------------------------------------------------
    /// Resets the timer's value to the period.
    inline void Reset()
    {
        mTimeCounter = mPeriod;
        mHasTicked   = false;
    }
    
private:
    const float mPeriod;
    float       mTimeCounter;
    bool        mHasTicked;
    bool        mIsRunning;    
};

///-----------------------------------------------------------------------------------------------

#endif /* Timer_h */
