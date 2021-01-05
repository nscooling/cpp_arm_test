// -----------------------------------------------------------------------------
// Time_utils.cpp
//
// DISCLAIMER:
// Feabhas is furnishing this item "as is". Feabhas does not provide any
// warranty of the item whatsoever, whether express, implied, or statutory,
// including, but not limited to, any warranty of merchantability or fitness
// for a particular purpose or any warranty that the contents of the item will
// be error-free.
// In no respect shall Feabhas incur any liability for any damages, including,
// but limited to, direct, indirect, special, or consequential damages arising
// out of, resulting from, or any way connected to the use of the item, whether
// or not based upon warranty, contract, tort, or otherwise; whether or not
// injury was sustained by persons or property or otherwise; and whether or not
// loss was sustained from, or arose out of, the results of, the item, or any
// services that may be provided by Feabhas.
// -----------------------------------------------------------------------------

#include "Time_utils.h"
#include "cmsis_device.h"

extern "C"
void SysTick_Handler(void);

// from Timer.cpp

extern "C" 
void xPortSysTickHandler(void);

namespace STM32F407
{
  volatile Timer::ticks Timer::delay_count;


  Timer::Timer()
  {
    start();
  }


  void Timer::start()
  {
    // Use SysTick as reference for the delay loops.
    SysTick_Config(SystemCoreClock / freq_Hz);
  }


  void Timer::stop()
  {
    // To Do...
  }


  #ifndef RTOS
  void Timer::sleep(ticks t)
  {
    delay_count = t;

    // Busy wait until the SysTick decrements the counter to zero.
    while (delay_count != 0)
    {
    }
  }
  #endif


  void Timer::ISR()
  {
    // Decrement to zero the counter used by the delay routine.
    if (delay_count != 0)
    {
      --delay_count;
    }
  }

} // namespace STM32F407


// from Time_utils.cpp

namespace {

    using namespace Time;

    class Timer {
    public:
        Timer();
        ~Timer();

        Timer(const Timer&)            = delete;
        Timer(Timer&&)                 = delete;
        Timer& operator=(const Timer&) = delete;
        Timer& operator=(Timer&&)      = delete;

        void start();
        void stop();
        
        void sleep(Duration duration);
        void sleep(Observation abs_time);
        Observation now() const;

    private:
        static void ISR();
        static Timer* self;
        static constexpr Time::tick_type  freq_Hz { 1000 };
        
        volatile tick_type  current_time { 0 };

        friend void ::SysTick_Handler(void);
    };

    Timer* Timer::self { nullptr };


    Timer::Timer()
    {
        self = this;
        start();
    }


    Timer::~Timer()
    {
        stop();
    }


    void Timer::start()
    {
        SysTick_Config(SystemCoreClock / freq_Hz);
    }


    void Timer::stop()
    {
        // To Do...
    }


    void Timer::sleep(Duration duration)
    {
        sleep(now() + duration);
    }


    void Timer::sleep(Observation abs_time)
    {
        auto wakeup_time = abs_time.as_duration().as_ticks();

        while (current_time < wakeup_time) { /* spin... */ }
    }


    Observation Timer::now() const
    {
        return Observation { current_time };
    }


    void Timer::ISR()
    {
        self->current_time++;
    }

    // Create a file-scope instance of the Timer
    // that the external sleep() functions can
    // use.
    //
    Timer system_clock { };

}

namespace Time {

    void sleep(Duration duration)
    {
        system_clock.sleep(duration);
    }


    void sleep(Observation wakeup_time)
    {
        system_clock.sleep(wakeup_time);
    }


    Observation from(Observation marker, Duration duration)
    {
        auto current_time = now();
        duration -= (current_time - marker);
        return Observation { current_time + duration };
    }


    Observation now()
    {
        return system_clock.now();
    }
}

// -----------------------------------------------------------------------------
// CMSIS ISR Handler.  This function uses weak linkage
// to override the default ISR handler (provided by CMSIS)
//
extern "C"
void SysTick_Handler(void)
{
#ifdef RTOS
  xPortSysTickHandler();
#else
  STM32F407::Timer::ISR();
  Timer::ISR();
#endif
}

#ifndef RTOS
void sleep(std::uint32_t sleep_for)
{
  static STM32F407::Timer sys_tick { };
  sys_tick.sleep(sleep_for);
}
#endif


