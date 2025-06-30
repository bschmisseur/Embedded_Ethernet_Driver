/**
 * @file ethernet_driver.hpp
 * 
 * @brief Declaration of public and private interfaces for the simulated ethernet driver
 * 
 * @author Bryce Schmisseur
 * 
 */

#pragma once
#ifndef EDS_ETHERNET_DRIVER_HPP__
#define EDS_ETHERNET_DRIVER_HPP__

// Standard Includes
#include <vector>
#include <cstdint>
#include <unordered_map>

// Project Includes
#include "error_code.hpp"
#include "ethernet_frame.hpp"
#include "host.hpp"
#include "embedded_device.hpp"
#include "packet_receiver.hpp"

// Declare namespace
namespace EthernetDriverSimulation
{
    class PacketReceiver {
        public:
            /**
             * @defgroup Public constant declarations
             * @{
             */
            virtual void receiveFrame(const uint8_t* data, size_t size) = 0;
            virtual ~PacketReceiver() = default;
            /**
             * @}
             */
    };

    class EthernetDriver {
        public:

            /**
             * @defgroup Public constant declarations
             * @{
             */
            static constexpr size_t MAX_BUFFER_SIZE = 8192;
            static constexpr size_t MAX_BUFFERED_FRAMES = MAX_BUFFER_SIZE / EthernetFrame::MAX_FRAME_SIZE;
            /**
             * @}
             */

            /**
             * @defgroup Public function declarations
             * @{
             */
            EthernetDriver();
            ~EthernetDriver() = default;

            ErrorCode storeSerializedFrame(const uint8_t* frameData);
            
            void registerReceiver(uint32_t address, PacketReceiver* receiver);
            void processStoredFrame();
            /**
             * @}
             */

        private:
            /**
             * @defgroup Private variable declarations
             * @{
             */
            std::unordered_map<uint32_t, PacketReceiver*> addressToReceiverMap;
            std::vector<std::vector<uint8_t>> frameBuffer;
            /**
             * @}
             */

            /**
             * @defgroup Private function declarations
             * @{
             */
            void forwardSerializedFrame(const std::vector<uint8_t>& frame);
            /**
             * @}
             */
    };
};

#endif // EDS_ETHERNET_DRIVER_HPP__