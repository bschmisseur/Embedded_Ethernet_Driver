/**
 * @file host.app
 * 
 * @brief Declaration of public and private interfaces for the host
 * 
 * @author Bryce Schmisseur
 * 
 */

#ifndef EDS_HOST_HPP__
#define EDS_HOST_HPP__

// Standard Imports
#include <array>
#include <vector>
#include <cstdint>

// Project Includes
#include "error_code.hpp"
#include "ethernet_driver.hpp"
#include "ethernet_frame.hpp"
#include "packet_receiver.hpp"
#include "ethernet_protocol.hpp"

namespace EthernetDriverSimulation{
    class Host : public PacketReceiver {
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

            // Enumeration to hold the type for error injection
            enum class InjectionType : uint8_t{
                MALFORMED_PACKET = 1,
                INCORRECT_ETHERTYPE = 2,
            };


            /**
             * @defgroup Public Function declarations
             * @{
             */
            Host(EthernetDriver* driver, uint32_t address);
            ~Host() = default;

            void clearRxBuffer();
            void receiveFrame(const uint8_t *data, size_t size) override;
            void sendFrame(const std::vector<uint8_t>& payload);
            void setDestinationAddress(uint32_t address);
            void performHandshake( void );
            void requestVideo( void );
            void injectFrameError( InjectionType injectionType );
            
            ErrorCode processReceivedFrames();

            /**
             * @brief Function to convert the injection type to a user friendly
             * string to represent the error the user specifies to inject
             * 
             * @param[in] injectionType - InjectionType
             * 
             * @return string - user friendly string representing the error code
             */
            inline std::string injectionTypeTostring(InjectionType injectionType) {
                switch (injectionType) {
                    case InjectionType::MALFORMED_PACKET :   return "Malformed Packet";
                    case InjectionType::INCORRECT_ETHERTYPE: return "Incorrect Ethernet Packet";
                    default:                                 return "INVALID INJECTION TYPE";
                }
            }
            /**
             * @}
             */
        private:

            /**
             * @defgroup Private constant definitions
             * @{
             */
            const std::string RECREATED_VIDEO_FILE_PATH = "../output/recreated.gif";
            const std::string ENCODED_FROM_DEVICE_VIDEO_FILE_PATH = "../output/encode_from_device.mp4";
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

#endif // EDS_HOST_HPP__