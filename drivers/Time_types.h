// -----------------------------------------------------------------------------
// Time_types.h
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

#ifndef TIME_TYPES_H_
#define TIME_TYPES_H_

#include <cstdint>

namespace Time {

    // tick_type represents the smallest unit
    // of time.
    //
    using tick_type = std::uint32_t;

    // A Duration represents a period of time in ticks.
    // Durations can be compared, added and
    // subtracted.
    //
    class Duration {
    public:
        constexpr Duration() = default;
        constexpr Duration(tick_type init) : value { init } 
        {
        }

        constexpr tick_type as_ticks() const
        {
            return value;
        }

        constexpr bool operator==(const Duration& rhs) const 
        { 
            return this->value == rhs.value; 
        }

        constexpr bool operator!=(const Duration& rhs) const 
        { 
            return !(*this == rhs); 
        }
        
        constexpr bool operator< (const Duration& rhs) const 
        { 
            return this->value < rhs.value; 
        }

        constexpr bool operator> (const Duration& rhs) const 
        { 
            return this->value > rhs.value; 
        }

        constexpr Duration operator+(const Duration& rhs) const 
        { 
            return Duration { this->value + rhs.value }; 
        }

        Duration& operator+=(const Duration& rhs)
        {
            value += rhs.value;
            return *this;
        }

        constexpr Duration operator-(const Duration& rhs) const 
        {
            // You can't have a negative duration, 
            // so round down to zero.
            //
            if (rhs.value <= this->value) {
                return Duration { this->value - rhs.value };
            }
            else {
                return Duration { 0 };
            } 
        }

        Duration& operator-=(const Duration& rhs)
        {
            // You can't have a negative duration, 
            // so round down to zero.
            //
            if (rhs.value <= this->value) {
                this->value -= rhs.value;
            }
            else {
                this->value = 0;
            }
            return *this;
        }

    private:
        tick_type value { 0 };
    };


    inline constexpr Duration abs_diff(const Duration& lhs, const Duration& rhs)
    {
        if (lhs > rhs) return (lhs - rhs);
        else           return (rhs - lhs);
    }


    inline constexpr Duration operator"" _ms(unsigned long long int t)
    {
        return Duration { static_cast<tick_type>(t) };
    }


    inline constexpr Duration operator"" _sec(unsigned long long int t)
    {
        return Duration { static_cast<tick_type>(t * 1000) };
    }

 
    // An Observation represents a point in time.
    // Observations can be compared and have the following
    // arithmetic behaviours:
    // Subtracting to Observations yields a Duration, which is
    // the absolute time (in ticks) between the two Observations
    // A Duration may be added to an Observation, yielding a new
    // Observation.  Observations may not be added.
    // An Observation may be converted to a Duration, which yields
    // the number of ticks since the epoch (when the clock started
    // running).
    //
    class Observation {
    public:
        constexpr Observation() = default;
        constexpr Observation(tick_type init) : value { init } 
        {
        }

        constexpr Duration as_duration() const
        {
            return Duration { value };
        }

        constexpr bool operator==(const Observation& rhs) const 
        { 
            return this->value == rhs.value; 
        }

        constexpr bool operator!=(const Observation& rhs) const 
        { 
            return !(*this == rhs); 
        }
        
        constexpr bool operator< (const Observation& rhs) const 
        { 
            return this->value < rhs.value; 
        }

        constexpr bool operator> (const Observation& rhs) const 
        { 
            return this->value > rhs.value; 
        }

        constexpr Observation operator+(const Duration& duration) const
        {
            return Observation { this->value + duration.as_ticks() };
        }

        Observation& operator+=(const Duration& rhs)
        {
            value += rhs.as_ticks();
            return *this;
        }

        constexpr Duration operator-(const Observation& rhs) const 
        {
            return Duration { 
                    (this->value >= rhs.value) ? (this->value - rhs.value) : (rhs.value - this->value)
                }; 
        }

    private:
        tick_type value { 0 };
    };

} // namespace Time


#endif // TIME_TYPES_H_
