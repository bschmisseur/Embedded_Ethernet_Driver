// ethernet_protocol.cpp
/**
 * @file ethernet_protocol.cpp
 * 
 * @brief Implementation of public and private interfaces for the ethernet protocol utility
 * 
 * @author Bryce Schmisseur
 * 
 */

// Header Includes
#include "ethernet_protocol.hpp"

// Standard Incudes

// Project Includes

namespace EthernetDriverSimulation {

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
    }
}