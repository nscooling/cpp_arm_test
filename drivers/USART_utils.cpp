// -----------------------------------------------------------------------------
// USART_utils.cpp
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

#include <cstdint>
#include "USART_utils.h"
#include "Peripherals.h"
#include "Memory_map.h"

using std::uint32_t;

namespace STM32F407 {

    namespace USART_config {

        // -----------------------------------------------------------------------------
        // Each UART requires two GPIO pins to be reconfigured
        // to act as the Tx and Rx pins.  These pins are on a
        // different port, and indeed different pins, for each
        // UART.
        // Tx pin is GPIO_B Pin 10
        // Rx pin is GPIO_B Pin 11

        struct GPIO_registers {
            uint32_t mode;
            uint32_t type;
            uint32_t speed;
            uint32_t pull_up_down;
            uint32_t input;
            uint32_t output;
            uint32_t set_reset;
            uint32_t lock;
            uint32_t alt_fn_low;
            uint32_t alt_fn_high;
        };


        void enable_usart_IO()
        {
            constexpr uint32_t GPIO_B_addr { STM32F407::AHB1_base + (0x400 * static_cast<int>(STM32F407::AHB1_Device::GPIO_B)) };
            auto const GPIO_B { reinterpret_cast<volatile GPIO_registers*>(GPIO_B_addr) };

            STM32F407::enable(STM32F407::AHB1_Device::GPIO_B);  // Enable GPIO port

            GPIO_B->alt_fn_high  |= (0x07 << (4 * (10 % 8)));   // ALT_FN7 - USART 3
            GPIO_B->alt_fn_high  |= (0x07 << (4 * (11 % 8)));   //

            GPIO_B->mode         |= (0x02 << (10 * 2));         // ALT FUNCTION
            GPIO_B->mode         |= (0x02 << (11 * 2));         //

            GPIO_B->speed        |= (0x03 << (10 * 2));         // HIGH SPEED
            GPIO_B->speed        |= (0x03 << (11 * 2));         //

            GPIO_B->pull_up_down |= (0x01 << (10 * 2));         // PULL UP
            GPIO_B->pull_up_down |= (0x01 << (11 * 2));         //
        }

    } // namespace USART_config

} // namespace STM32F407
