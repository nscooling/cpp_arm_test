// -----------------------------------------------------------------------------
// peripherals.cpp
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


#include "Peripherals.h"
#include "Memory_map.h"

#include <cstdint>
using std::uint32_t;

namespace STM32F407
{
    // Use static consts to give the compiler
    // the best opportunity to optimise
    //
    constexpr uint32_t RCC_base { AHB1_base + 0x3800 };

    static volatile uint32_t* RCC_AHB1_enable  { reinterpret_cast<uint32_t*>(RCC_base + 0x30) };
    static volatile uint32_t* RCC_APB1_enable  { reinterpret_cast<uint32_t*>(RCC_base + 0x40) };
    static volatile uint32_t* RCC_APB2_enable  { reinterpret_cast<uint32_t*>(RCC_base + 0x44) };

    void enable(AHB1_Device device)
    {
      uint32_t rcc = *RCC_AHB1_enable;
      rcc |= (0x1u << device);
      *RCC_AHB1_enable = rcc;
    }

    void enable(APB1_Device device)
    {
      uint32_t rcc = *RCC_APB1_enable;
      rcc |= (0x1u << device);
      *RCC_APB1_enable = rcc;
    }

    void enable(APB2_Device device)
    {
      uint32_t rcc = *RCC_APB2_enable;
      rcc |= (0x1u << device);
      *RCC_APB2_enable = rcc;
    }

} // namespace STM32F407
