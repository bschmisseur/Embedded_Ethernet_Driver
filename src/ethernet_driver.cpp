/**
 * @file ethernet_driver.cpp
 * 
 * @brief Implementation of public and private interfaces for the simulated ethernet driver
 * 
 * @author Bryce Schmisseur
 * 
 */

// Header Includes
#include "ethernet_driver.hpp"

// Standard Incudes

// Project Includes

namespace EthernetDriverSimulation{

    ErrorCode EthernetDriver::storeSerializedFrame(const uint8_t *frameData){

        // Ensure the buffer is not to full 
        if(frameBuffer.size() >= MAX_BUFFERED_FRAMES){
            return ErrorCode::ETHERNET_BUFFER_FULL;
        }

        // Move the frame in the buffer
        std::vector<uint8_t> frame(frameData, frameData + EthernetFrame::MAX_FRAME_SIZE);
        frameBuffer.emplace_back(std::move(frame));
        return ErrorCode::SUCCESS;
    }

    void EthernetDriver::processStoredFrame() {

        // Loop and send each frame
        for (const auto& frame : frameBuffer){
            forwardSerializedFrame(frame);
        }

        // Clear frame buffer
        frameBuffer.clear();
    }

    void EthernetDriver::forwardSerializedFrame(const std::vector<uint8_t>& frame) {
        
        // Retrieve the destination address from the packets
        // !!There is an assumption here that the memory address is always correct!! 
        uint32_t destinationMemoryAddress = 0;
        for (size_t i = 0; i < 4; ++i) {
            destinationMemoryAddress |= (static_cast<uint32_t>(frame[i]) << (8 * i));
        }

        // Convert address to pointer
        uint8_t* destBuffer = reinterpret_cast<uint8_t*>(destinationMemoryAddress);

        // Copy the packet to the destination bugger
        std::memcpy(destBuffer, frame.data(), EthernetFrame::MAX_FRAME_SIZE);
    }
}