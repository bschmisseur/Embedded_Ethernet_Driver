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
        default: return "INVALID ERROR";
    }
}

};

#endif // EDS_ERROR_CODE_HPP__