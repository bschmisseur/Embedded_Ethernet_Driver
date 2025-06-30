/**
 * @file <file_name>.hpp
 * 
 * @brief <Description>
 * 
 * @author Bryce Schmisseur
 * 
 */

#ifndef EDS_ETHERNET_PROTOCOL_HPP__
#define EDS_ETHERNET_PROTOCOL_HPP__

// Standard Includes
#include <array> 
#include <cstdint> 
#include <string> 
#include <cstring> 

// Project Includes
#include "ethernet_frame.hpp"

// Declare namespace
namespace EthernetDriverSimulation
{
    // Enumeration to hold the expected data of each frame
    enum class ExpectedPayloadData : uint32_t{
        HANDSHAKE = 0x101010,
        ACKNOWLEDGEMENT = 0x202020,
        VIDEO_REQUEST = 0x303030,
        ERROR = 0xFAFAFA,
        VIDEO_DATA = 0xAAAAAA, // This is just a signifier of a video 
                                // data packet type not the expected data
        UNKNOWN_PACKET = 0xFFFFF,
    };

    std::string packetDataToTypeString(ExpectedPayloadData type);
    ExpectedPayloadData findFrameType(EthernetFrame& frame);
};

#endif // EDS_ETHERNET_PROTOCOL_HPP__