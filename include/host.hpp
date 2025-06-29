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

// Project Includes
#include "error_code.hpp"

namespace EthernetDriverSimulation{
    class Host {

        // Enumeration to hold the type for error injection
        enum class InjectionType : uint8_t{
            MALFORMED_PACKET = 1,
            INCORRECT_ETHERTYPE = 2,
        };

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

    public:

        /**
         * @defgroup Public Function declarations
         * @{
         */
        Host();
        ~Host();

        int *getHostAddress( void );
        ErrorCode performHandshakeRequest( void );
        ErrorCode requestVideoCapture( void );
        ErrorCode injectFrameError( InjectionType injectionType );
        /**
         * @}
         */
    };
};

#endif // EDS_HOST_HPP__