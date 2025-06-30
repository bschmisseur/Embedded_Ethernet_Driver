/**
 * @file video_codec.hpp
 * 
 * @brief Declaration of public and private interfaces for the video codec
 * 
 * @author Bryce Schmisseur
 * 
 */

#ifndef EDS_VIDEO_CODEC_HPP__
#define EDS_VIDEO_CODEC_HPP__

// Standard Includes
#include <string>

// Project includes
#include "error_code.hpp"

// Declare namespace
namespace EthernetDriverSimulation
{
    class VideoCodec {
        public:
            /**
             * @defgroup Public Function declarations
             * @{
             */
            VideoCodec() = default;
            ~VideoCodec() = default;

            ErrorCode encodeGifToH264(const std::string& gifPath, const std::string& h264Path);
            ErrorCode decodeH264ToGif(const std::string& h264Path, const std::string& gifPath);
            /**
             * @}
             */
    };
};

#endif // EDS_VIDEO_CODEC_HPP__