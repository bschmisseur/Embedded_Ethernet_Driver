/**
 * @file host.cpp
 * 
 * @brief Implementation of public and private interfaces for the simulated host
 * 
 * @author Bryce Schmisseur
 * 
 */

// Header Includes
#include "host.hpp"

// Standard Incudes
#include <cstring>
#include <fstream>
#include <cstdint>
#include <iostream>

// Project Includes
#include "ethernet_protocol.hpp"
#include "video_codec.hpp"
#include "ethernet_driver.hpp"

namespace EthernetDriverSimulation {

    Host::Host(EthernetDriver* driver, uint32_t address)
        : driver(driver), 
          address(address) 
    {
        driver->registerReceiver(address, this);
    }    

    void Host::receiveFrame(const uint8_t *data, size_t size) {
        // Put frame in rx buffer
        std::array<uint8_t, EthernetFrame::MAX_FRAME_SIZE> frame;
        std::memcpy(frame.data(), data, size);
        rxBuffer.push_back(frame);
    }

    void Host::sendFrame(const std::vector<uint8_t>& payload) {

        // Initialize method variables
        std::array<uint8_t, 4> dest;
        std::array<uint8_t, 4> source;
        const uint8_t *dataPtr;  

        // Convert dest
        std::memcpy(dest.data(), &destinationAddress, 4);

        // Convert source
        std::memcpy(source.data(), &address, 4);

        // Convert payload
        dataPtr = payload.data();

        // Construct Handshake packet
        EthernetFrame frame(
            dest,
            source,
            dataPtr,
            payload.size()
        ) ;

        std::size_t size = 0;
        driver->storeSerializedFrame(frame.toHexStream(size));
    }

    ErrorCode Host::processReceivedFrames() {

        // Initialize method variables
        bool started_video_processing = false; 

        // Clear File
        std::ofstream clearFile(ENCODED_FROM_DEVICE_VIDEO_FILE_PATH, std::ios::out | std::ios::trunc);
        clearFile.close();

        // Open File Handler
        std::ofstream encodedFile(ENCODED_FROM_DEVICE_VIDEO_FILE_PATH, std::ios::out | std::ios::binary | std::ios::app);

        // Loop through all frames in the buffer
        for (const auto& frame : rxBuffer) {
            EthernetFrame currentFrame;

            ErrorCode parseFrameStatus = currentFrame.fromHexSteam(frame.data());

            if (ErrorCode::SUCCESS != parseFrameStatus)
            {
                return parseFrameStatus;
            }

            ExpectedPayloadData packetType = findFrameType(currentFrame);

            // If acknowledgement packet; display header and return
            if (packetType == ExpectedPayloadData::ACKNOWLEDGEMENT){
                std::cout << "ACKNOWLEDGEMENT FRAME FROM DEVICE: " << "\n";
                std::cout << currentFrame.frameHeaderToString() << "\n\n";
            }

            // If video data; write to file
            if (packetType == ExpectedPayloadData::VIDEO_DATA){
                started_video_processing = true; 

                // Append Data of each 
                encodedFile.write(reinterpret_cast<const char*>(currentFrame.payload.data()), currentFrame.payloadLength);
            }
            
        }

        // If video was received print out path to GIF file
        if (started_video_processing)
        {
            // Close encoded file
            encodedFile.close();

            // Create Decoder Object
            VideoCodec videoDecoder;

            // Decode Video and save to file
            videoDecoder.decodeH264ToGif(ENCODED_FROM_DEVICE_VIDEO_FILE_PATH, RECREATED_VIDEO_FILE_PATH);

            // Output file path
            std::cout << "Received and decoded video can be found out this path: " << RECREATED_VIDEO_FILE_PATH << "\n";

            // Reset the video processing variables
            started_video_processing = false; 
        }

        // Clear Buffer
        clearRxBuffer();

        return ErrorCode::SUCCESS;
    }

    void Host::performHandshake( void ) {
        // Set Payload
        ExpectedPayloadData data = ExpectedPayloadData::HANDSHAKE;

        sendFrame(convertEnumToBytes(data));
    }

    void Host::requestVideo( void ) {
        // Set Payload
        ExpectedPayloadData data = ExpectedPayloadData::VIDEO_REQUEST;

        sendFrame(convertEnumToBytes(data));

    }

    void Host::injectFrameError( InjectionType injectionType ) {

        const uint8_t *dataPtr;

        if (injectionType == InjectionType::MALFORMED_PACKET) {
            // Convert Data
            uint8_t malformedFrame[] = {
                0xDE, 0xAD, 0xBE, 0xEF,      // Destination
                0x12, 0x34, 0x56, 0x78,      // Source
                0xC0, 0xAF,                  //EtherType 
                0x00, 0x02,                  // Payload Length (2 bytes)
                0xBB, 0xBB,                  // [X] Delimiter 
                0x11, 0x22                   // Payload (2 bytes)
            };

            dataPtr = malformedFrame;
        } 
        else {
            // Convert Data
            uint8_t invalidEtherTypeFrame[] = {
                0xDE, 0xAD, 0xBE, 0xEF,      // Destination
                0x12, 0x34, 0x56, 0x78,      // Source
                0x08, 0x00,                  // [X] EtherType 
                0x00, 0x02,                  // Payload Length (2 bytes)
                0xAB, 0xAB,                  // Delimiter 
                0x11, 0x22                   // Payload (2 bytes)
            };

            dataPtr = invalidEtherTypeFrame;
        }

        std::size_t size = 0;
        driver->storeSerializedFrame(dataPtr);
    }

    const std::vector<uint8_t> Host::convertEnumToBytes(ExpectedPayloadData data) {
        uint32_t rawData = static_cast<uint32_t>(data);
        std::vector<uint8_t> bytes = {
                                        static_cast<uint8_t>((rawData >> 0) & 0xFF),
                                        static_cast<uint8_t>((rawData >> 8) & 0xFF),
                                        static_cast<uint8_t>((rawData >> 16) & 0xFF),
                                        static_cast<uint8_t>((rawData >> 24) & 0xFF)};

        const std::vector<uint8_t>& refBytes = bytes;
    }

    void Host::clearRxBuffer() {
        rxBuffer.clear();
    }

    void Host::setDestinationAddress(uint32_t address) {
        destinationAddress = address;
    }
}
