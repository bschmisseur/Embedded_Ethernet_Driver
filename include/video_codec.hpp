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

// Declare namespace
namespace EthernetDriverSimulation
{
    class VideoCodec {
        public:
            /**
             * @defgroup Public Function declarations
             * @{
             */
            VideoCodec();
            ~VideoCodec();

            std::string encodeGifToH264(const std::string& gifPath);
            std::string decodeH264ToGit(const std::string& h264Path);
            /**
             * @}
             */
    };
};

#endif // EDS_VIDEO_CODEC_HPP__