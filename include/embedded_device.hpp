/**
 * @file embedded_device.hpp
 * 
 * @brief Declaration of public and private interfaces for the simulated embedded device
 * 
 * @author Bryce Schmisseur
 * 
 */

#ifndef EDS_EMBEDDED_DEVICE_HPP__
#define EDS_EMBEDDED_DEVICE_HPP__

// Standard Includes
#include <string>
#include <array>
#include <vector>
#include <cstdint>

// Project Includes
#include "error_code.hpp"
#include "ethernet_driver.hpp"
#include "ethernet_frame.hpp"
#include "packet_receiver.hpp"
#include "ethernet_protocol.hpp"

// Declare namespace
namespace EthernetDriverSimulation
{
    class EmbeddedDevice : public PacketReceiver {
        public:
            
            /**
             * @defgroup Public constant declarations
             * @{
             */
            static constexpr size_t RX_BUFFER_SIZE = 8192;
            static constexpr size_t MAX_PACKETS = RX_BUFFER_SIZE / EthernetFrame::MAX_FRAME_SIZE;
            /**
             * @}
             */

            /**
             * @defgroup Public Function declarations
             * @{
             */
            EmbeddedDevice(EthernetDriver* driver, uint32_t address);
            ~EmbeddedDevice() = default;

            void clearRxBuffer();
            void receiveFrame(const uint8_t *data, size_t size) override;
            void sendFrame(const std::vector<uint8_t>& payload);
            void setDestinationAddress(uint32_t address);

            ErrorCode processReceivedFrames();

            /**
             * @}
             */
        private:
        
            /**
             * @defgroup Private constant definitions
             * @{
             */
            const std::string ORIGINAL_VIDEO_FILE_PATH = "../data/original.gif";
            const std::string ENCODED_ON_DEVICE_VIDEO_FILE_PATH = "../output/encode_on_device.mp4";
            /**
             * @}
             */

            /**
             * @defgroup Private variable declarations
             * @{
             */
            EthernetDriver* driver;
            uint32_t address;
            std::vector<std::array<uint8_t, EthernetFrame::MAX_FRAME_SIZE>> rxBuffer;
            uint32_t destinationAddress;
            /**
             * @}
             */

            /**
             * @defgroup Private Function declarations
             * @{
             */
            const std::vector<uint8_t> convertEnumToBytes(ExpectedPayloadData data);
            /**
             * @}
             */
    };
};

#endif // EDS_EMBEDDED_DEVICE_HPP__