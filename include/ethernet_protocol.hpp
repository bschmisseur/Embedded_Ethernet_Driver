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

    /**
     * @brief Function to generate a string representing the error code value
     * 
     * @param[in] data - ExpectedPayloadData
     * 
     * @return string - string representing the error code value
     */
    std::string packetDataToTypeString(ExpectedPayloadData data) {
        switch (data) {
            case ExpectedPayloadData::HANDSHAKE: return "Handshake Packet";
            case ExpectedPayloadData::ACKNOWLEDGEMENT: return "Acknowledgment Packet";
            case ExpectedPayloadData::ERROR: return "Error Packet";
            case ExpectedPayloadData::VIDEO_REQUEST: return "Video Request Packet";
            case ExpectedPayloadData::VIDEO_DATA: return "Video Data Packet";
            default: return "INVALID PACKET";
        }
    }

    /**
     * @brief Function to take in frame and given the length and data return enumeration signifying the type of frame
     * 
     * @param[in] frame - EthernetFrame
     * 
     * @return ExpectedPayloadData
     */
    ExpectedPayloadData findFrameType( EthernetFrame& frame) {
        
        // If the payload length is greater than 3 bytes -> Video Data Packet
        if (frame.payloadLength > 3){
            return ExpectedPayloadData::VIDEO_DATA;
        } 
        // If the payload length is exactly 3 it should match one expected payload data
        else if(frame.payloadLength == 3) {

            switch (frame.payload[0]) {
                case 0x10: return ExpectedPayloadData::HANDSHAKE;
                case 0x20: return ExpectedPayloadData::ACKNOWLEDGEMENT;
                case 0x30: return ExpectedPayloadData::VIDEO_REQUEST;
                case 0xFF: return ExpectedPayloadData::ERROR;
                default:   return ExpectedPayloadData::UNKNOWN_PACKET;
            }
        } 
        // Any other length is invalid
        else {
            return ExpectedPayloadData::UNKNOWN_PACKET;
        } 
    };
};

#endif // EDS_ETHERNET_PROTOCOL_HPP__