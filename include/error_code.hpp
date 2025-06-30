/**
 * @file error_code.hpp
 * 
 * @brief File to contain all error code/function returns of the program
 * 
 * @author Bryce Schmisseur
 * 
 */

#ifndef EDS_ERROR_CODE_HPP__
#define EDS_ERROR_CODE_HPP__

// Standard Includes
#include <cstdint>
#include <string>

// Declare namespace
namespace EthernetDriverSimulation
{

    // Enumeration to hold all error code functions can return
    enum class ErrorCode : uint16_t{
        SUCCESS = 0x0000,

        MALFORMED_FRAME = 0x1000,
        INVALID_ETHER_TYPE = 0x1001,

        ETHERNET_BUFFER_FULL = 0x2000,
        HOST_BUFFER_FULL = 0x2001,
        DEVICE_BUFFER_FULL = 0x2002,
        INVALID_BUFFER_INDEX = 0x2003,

        INVALID_INPUT = 0x3000,
        INVALID_RECEIVER = 0x3001,

        ENCODE_CANNOT_OPEN_FILE = 0x4000,
        ENCODE_FAILED_STREAM_INFO = 0x4002,
        ENCODE_NO_VIDEO_STREAM = 0x4003,
        ENCODE_OUTPUT_ALLOC_FAIL = 0x4004,
        ENCODE_ENCODER_NOT_FOUND = 0x4005,
        ENCODE_ENCODER_OPEN_FAIL = 0x4006,
        ENCODE_CANNOT_OPEN_OUTPUT = 0x4007,
        ENCODE_HEADER_WRITE_FAIL = 0x4008,

        DECODE_CANNOT_OPEN_FILE = 0x5000,
        DECODE_FAILED_STREAM_INFO = 0x5002,
        DECODE_NO_VIDEO_STREAM = 0x5003,
        DECODE_OUTPUT_ALLOC_FAIL = 0x5004,
        DECODE_ENCODER_NOT_FOUND = 0x5005,
        DECODE_ENCODER_OPEN_FAIL = 0x5006,
        DECODE_CANNOT_OPEN_OUTPUT = 0x5007,
        DECODE_HEADER_WRITE_FAIL = 0x5008,
    };

    /**
     * @brief Function to convert the error code to an error message to display
     * a user friendly version of the error when necessary 
     * 
     * @param[in] errorCode - ErrorCode
     * 
     * @return string - user friendly string representing the error code
     */
    inline std::string errorCodeToString(ErrorCode code) {
        switch (code) {
            case ErrorCode::SUCCESS: return "Success";

            case ErrorCode::MALFORMED_FRAME: return "Malformed Packet";
            case ErrorCode::INVALID_ETHER_TYPE: return "Invalid Ethernet Type";

            case ErrorCode::ETHERNET_BUFFER_FULL: return "Ethernet Buffer Overflow";

            case ErrorCode::DEVICE_BUFFER_FULL: return "Device Buffer Overflow";
            case ErrorCode::INVALID_BUFFER_INDEX: return "Invalid Buffer Index";

            case ErrorCode::INVALID_INPUT: return "Invalid Input";
            case ErrorCode::INVALID_RECEIVER: return "Invalid Receiver";

            case ErrorCode::ENCODE_CANNOT_OPEN_FILE: return "Could not find original gif for encoding";
            case ErrorCode::ENCODE_FAILED_STREAM_INFO: return "Failed to get steam info for encoding";
            case ErrorCode::ENCODE_NO_VIDEO_STREAM: return "No video steam found when encoding";
            case ErrorCode::ENCODE_OUTPUT_ALLOC_FAIL: return "Not enough memory for encoding output";
            case ErrorCode::ENCODE_ENCODER_NOT_FOUND: return "Could not find specified encoding";
            case ErrorCode::ENCODE_ENCODER_OPEN_FAIL: return "Could not open/use encoder";
            case ErrorCode::ENCODE_CANNOT_OPEN_OUTPUT: return "Could not open output file when encoding";
            case ErrorCode::ENCODE_HEADER_WRITE_FAIL: return "Failed to write header for encoding";

            case ErrorCode::DECODE_CANNOT_OPEN_FILE: return "Could not find original gif for decoding";
            case ErrorCode::DECODE_FAILED_STREAM_INFO: return "Failed to get steam info for decoding";
            case ErrorCode::DECODE_NO_VIDEO_STREAM: return "No video steam found when decoding";
            case ErrorCode::DECODE_OUTPUT_ALLOC_FAIL: return "Not enough memory for decoding output";
            case ErrorCode::DECODE_ENCODER_NOT_FOUND: return "Could not find specified decoding";
            case ErrorCode::DECODE_ENCODER_OPEN_FAIL: return "Could not open/use decoder";
            case ErrorCode::DECODE_CANNOT_OPEN_OUTPUT: return "Could not open output file when decoding";
            case ErrorCode::DECODE_HEADER_WRITE_FAIL: return "Failed to write header for decoding";
            
            default: return "INVALID ERROR";
        }
    }

};

#endif // EDS_ERROR_CODE_HPP__