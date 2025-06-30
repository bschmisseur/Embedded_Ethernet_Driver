/**
 * @file embedded_device.cpp
 * 
 * @brief Implementation of public and private interfaces for the simulated embedded device
 * 
 * @author Bryce Schmisseur
 * 
 */

// Header Includes
#include "embedded_device.hpp"

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
    
    EmbeddedDevice::EmbeddedDevice(EthernetDriver* driver, uint32_t address)
        : driver(driver), 
          address(address) 
    {
        driver->registerReceiver(address, this);
    }

    void EmbeddedDevice::receiveFrame(const uint8_t *data, size_t size) {
        // Put frame in rx buffer
        std::array<uint8_t, EthernetFrame::MAX_FRAME_SIZE> frame;
        std::memcpy(frame.data(), data, size);
        rxBuffer.push_back(frame);
    }

    void EmbeddedDevice::sendFrame(const std::vector<uint8_t>& payload) {

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

    ErrorCode EmbeddedDevice::processReceivedFrames() {
        // Loop through all frames in the buffer
        for (const auto& frame : rxBuffer) {
            EthernetFrame currentFrame;

            ErrorCode parseFrameStatus = currentFrame.fromHexSteam(frame.data());

            if (ErrorCode::SUCCESS != parseFrameStatus)
            {
                // Send ERROR back
                ExpectedPayloadData data = ExpectedPayloadData::ERROR;
                sendFrame(convertEnumToBytes(data));

                return parseFrameStatus;
            }
            else 
            {
                ExpectedPayloadData packetType = findFrameType(currentFrame);
                
                // Received Packet is Handshake
                if (ExpectedPayloadData::HANDSHAKE == packetType)
                {
                    // Send ACKNOWLEDGEMENT back
                    ExpectedPayloadData data = ExpectedPayloadData::HANDSHAKE;
                    sendFrame(convertEnumToBytes(data));

                }
                // Received VIDEO_REQUEST
                else if (ExpectedPayloadData::VIDEO_REQUEST == packetType)
                {
                    // Encode File
                    // Create Decoder Object
                    VideoCodec videoEncoder;

                    // Decode Video and save to file
                    videoEncoder.encodeGifToH264(ORIGINAL_VIDEO_FILE_PATH, ENCODED_ON_DEVICE_VIDEO_FILE_PATH);

                    // Open encoded file
                    std::ifstream encodedFile(ENCODED_ON_DEVICE_VIDEO_FILE_PATH, std::ios::binary);

                    std::vector<uint8_t> buffer(EthernetFrame::MAX_FRAME_SIZE);
                    
                    // Loop through 1024 bytes at a type until no more data in file
                    while (encodedFile.read(reinterpret_cast<char *>(buffer.data()), EthernetFrame::MAX_FRAME_SIZE) || encodedFile.gcount() > 0)
                    {
                        size_t bytesRead = encodedFile.gcount();

                        // Shrink buffer to actual size (in case final chunk is < 1024 bytes)
                        buffer.resize(bytesRead);

                        // Now create a reference to the current chunk
                        const std::vector<uint8_t>& chunkRef = buffer;

                        // Send Packets
                        sendFrame(chunkRef);

                        // Resize back to full size for next iteration
                        buffer.resize(EthernetFrame::MAX_FRAME_SIZE);
                    }
                }
            }
        }

        // Clear Buffer
        clearRxBuffer();

        return ErrorCode::SUCCESS;
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
}