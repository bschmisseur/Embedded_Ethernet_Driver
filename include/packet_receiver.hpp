/**
 * @file packet_receiver.hpp
 * 
 * @brief Declaration of public and private parent class for an object that receives packets
 * 
 * @author Bryce Schmisseur
 * 
 */

#pragma once

// Standard Includes
#include <cstdint>
#include <cstddef>

namespace EthernetDriverSimulation {

    class PacketReceiver {
    public:

        /**
         * @defgroup Public virtual function declarations
         * @{
         */
        virtual ~PacketReceiver() = default;
        virtual void receiveFrame(const uint8_t* data, size_t size) = 0;
        /**
         * @}
         */
    };

}