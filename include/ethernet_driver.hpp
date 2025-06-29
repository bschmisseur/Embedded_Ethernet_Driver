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

// Project Includes
#include "error_code.hpp"
#include "ethernet_frame.hpp"

// Declare namespace
namespace EthernetDriverSimulation
{
    class EthernetDriver {
        public:

            /**
             * @defgroup Public constant declarations
             * @{
             */
            static constexpr size_t MAX_BUFFERED_FRAMES = 8192 / EthernetFrame::MAX_FRAME_SIZE;
            /**
             * @}
             */

            /**
             * @defgroup Public function declarations
             * @{
             */
            EthernetDriver();
            ~EthernetDriver();

            ErrorCode storeSerializedFrame(const uint8_t* frameData);
            void processStoredFrame();
            /**
             * @}
             */

        private:
            /**
             * @defgroup Private variable declarations
             * @{
             */
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