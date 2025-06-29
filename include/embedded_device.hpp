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

// Project Includes
#include "error_code.hpp"

// Declare namespace
namespace EthernetDriverSimulation
{
    class EmbeddedDevice {
        public:
            /**
             * @defgroup Public Function declarations
             * @{
             */
            EmbeddedDevice();
            ~EmbeddedDevice();

            ErrorCode performHandshakeRequest( void );
            ErrorCode sendVideoCapture( void );
            /**
             * @}
             */
        private:
            /**
             * @defgroup Private constant definitions
             * @{
             */
            const std::string VIDEO_FILE_PATH = "../data/embedded_video_capture.gif";
            /**
             * @}
             */
    };
};

#endif // EDS_EMBEDDED_DEVICE_HPP__