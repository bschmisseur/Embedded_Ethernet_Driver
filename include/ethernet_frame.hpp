/**
 * @file ethernet_frame.hpp
 * 
 * @brief Declaration of public and private interfaces for ethernet frames
 * 
 * @author Bryce Schmisseur
 * 
 */

#ifndef EDS_ETHERNET_FRAME_HPP__
#define EDS_ETHERNET_FRAME_HPP__

// Standard Includes
#include <array> 
#include <cstdint> 
#include <string> 
#include <cstring>
#include <iostream>
#include <format> 
#include <stdexcept>

// Project Includes
#include "error_code.hpp"

// Declare namespace
namespace EthernetDriverSimulation
{
    class EthernetFrame {
        public:
            /**
             * @defgroup Public const class variables
             * @{
             */
            static constexpr std::size_t   MAX_FRAME_SIZE   = 1040;
            static constexpr std::size_t   MAX_PAYLOAD_SIZE = 1024;
            static constexpr std::uint16_t EDS_ETHERTYPE    = 0xC0AF;
            static constexpr std::uint16_t DELIMITER        = 0xABAB;
            /**
             * @}
             */

            /**
             * @defgroup Public class variables
             * @{
             */
            std::array<uint8_t, 4>destinationAddress;
            std::array<uint8_t, 4>sourceAddress;
            std::array<uint8_t, MAX_PAYLOAD_SIZE> payload;
            std::uint16_t payloadLength = 0;
            /**
             * @}
             */

            /**
             * @defgroup Public function declarations
             * @{
             */
            EthernetFrame() = default;

            /**
             * @brief Overloaded Constructor
             * 
             * @param[in] dest - array<uint8_t, 4>
             * @param[in] source - array<uint8_t, 4>
             * @param[in] data - uint8_t *
             * @param[in] length - uint16_t
             */
            EthernetFrame(std::array<uint8_t, 4>dest, std::array<uint8_t, 4>source, const uint8_t *data, std::uint16_t length):
                destinationAddress(dest),
                sourceAddress(source),
                payloadLength(length)
            {
                std::memcpy(payload.data(), data, payloadLength);
            }

            /**
             * @brief Generate a string to display the packet header
             * 
             * @return string representing the packet
             */
            std::string frameHeaderToString() const{
                return "Ethernet Packet:\n\tDestination Address: 0x" + displayHex(destinationAddress) +
                    "\n\tSource Address: 0x" + displayHex(sourceAddress) +
                    "\n\tEtherType: 0x" + displayHex(EDS_ETHERTYPE) +
                    "\n\tDelimiter: 0x" + displayHex(DELIMITER) +
                    "\n\tPayload Length: 0x" + displayHex(payloadLength) +
                    "\n\tPayload Data Start: 0x" + displayHex(payload[0]) + "...";
            }

            /**
             * @brief function to convert frame to hex stream (serialize)
             * 
             * @param[in] finalStreamSize - size_t&: pointer to size variable
             * 
             * @return uint8_t *: pointer to start of hex stream
             */
            const uint8_t *toHexStream(std::size_t& finalStreamSize) const{
                // Initalized Function Variables
                std::size_t hexStreamSize = 0;

                // Add Destination Address
                std::memcpy(buffer_.data() + hexStreamSize, destinationAddress.data(), 4);
                hexStreamSize+=4;

                // Add Source Address
                std::memcpy(buffer_.data() + hexStreamSize, sourceAddress.data(), 4);
                hexStreamSize+=4;
                
                // Add Ether Type
                writeUint16(buffer_, hexStreamSize, EDS_ETHERTYPE);
                writeUint16(buffer_, hexStreamSize, static_cast<uint16_t>(payloadLength));
                writeUint16(buffer_, hexStreamSize, DELIMITER);

                std::memcpy(buffer_.data() + hexStreamSize, payload.data(), payloadLength);
                hexStreamSize += payloadLength;

                hexStreamSize = hexStreamSize;
                return buffer_.data();
            }

            /**
             * @brief Convert a hex stream to a Frame object
             * 
             * @param[in] buffer - uint8_t *: pointer to frame start
             * 
             * @return ErrorCode: status of the conversion process
             */
            ErrorCode fromHexSteam(const uint8_t* buffer){
                    // If the buffer is NULL that return error
                    if (!buffer) {
                        return ErrorCode::MALFORMED_FRAME;
                    }

                    // Assume the Destination and Source Address are in the correct place 
                    std::memcpy(destinationAddress.data(), buffer, 4);
                    std::memcpy(sourceAddress.data(), buffer + 4, 4);

                    // Retrieve EtherType and ensure it matches the expected value
                    uint16_t etherType = (buffer[8] << 8) | buffer[9];
                    if (etherType != EDS_ETHERTYPE) {
                        return ErrorCode::INVALID_ETHER_TYPE;
                    }
                    
                    // Retrieve length and ensure its no greater than max
                    uint16_t len = (buffer[10] << 8) | buffer[11];
                    if (len > MAX_PAYLOAD_SIZE) {
                        return ErrorCode::MALFORMED_FRAME;;
                    }

                    // Retrieve delimiter and ensure it matches the expected value
                    uint16_t delimiter = (buffer[12] << 8) | buffer[13];
                    if (delimiter != DELIMITER) {
                        return ErrorCode::MALFORMED_FRAME;;
                    }
                    
                    // Set the payload Length
                    payloadLength = len;
                    std::memcpy(payload.data(), buffer + 14, payloadLength);

                    // If there was no errors when building the packet; return success
                    return ErrorCode::SUCCESS;
            }
            /**
             * @}
             */

        private:
            mutable std::array<uint8_t, MAX_FRAME_SIZE> buffer_;

            /**
             * @brief Helper function to display a 16 bit int as hex value
             */
            static std::string displayHex(uint16_t value) {
                char buffer[5];
                std::snprintf(buffer, sizeof(buffer), "%04X", value);
                return std::string(buffer);
            }

            /**
             * @brief Helper function to display a 16 bit int as hex value
             */
            static std::string displayHex(uint8_t value) {
                char buffer[3]; // 2 digits + null terminator
                std::snprintf(buffer, sizeof(buffer), "%02X", value);
                return std::string(buffer);
            }

            /**
             * @brief Helper function to display a 16 bit int as hex value
             */
            static std::string displayHex(const std::array<uint8_t, 4>& addr) {
                char buffer[9]; // 4 bytes * 2 hex chars + null terminator
                std::snprintf(buffer, sizeof(buffer), "%02X%02X%02X%02X",
                            addr[0], addr[1], addr[2], addr[3]);
                return std::string(buffer);
            }

            /**
             * @brief Helper function to write 16 bit values to the hex stream
             */
            static void writeUint16(std::array<uint8_t, MAX_FRAME_SIZE>& buf, std::size_t& hexStreamSize, uint16_t value) {
                // Shift the bits right to put the first byte in the buffer
                buf[hexStreamSize++] = static_cast<uint8_t>((value >> 8) & 0xFF);
                buf[hexStreamSize++] = static_cast<uint8_t>(value & 0xFF);
            }
    };
};

#endif // EDS_ETHERNET_FRAME_HPP__