/**
 * @file main.cpp
 * 
 * @brief File to contain the main entry point of the project
 * 
 * @author Bryce Schmisseur
 * 
 */

//Standard Includes
#include <cstdio>
#include <iostream>

// Project Includes
#include "version.hpp"
#include "host.hpp"
#include "embedded_device.hpp"
#include "ethernet_driver.hpp"
#include "video_codec.hpp"

using namespace EthernetDriverSimulation;

void performHandshake(Host& host, EmbeddedDevice& device, EthernetDriver& driver) {
    std::cout << "\n--- Performing Handshake ---\n";

    // Call upon host to send handshake request
    host.performHandshake();

    // Call upon ethernet driver to send frames
    // TODO Frame -> Frames
    driver.processStoredFrame();
    
    // Call upon device to read frames
    device.processReceivedFrames();
    
    // Call upon ethernet driver to send frames
    driver.processStoredFrame();

    // Call upon host to read frames
    host.processReceivedFrames();

    std::cout << "\n--- Close Performing Handshake ---\n";
};

void performInjectedError(Host& host, EmbeddedDevice& device, EthernetDriver& driver) {
    std::cout << "\n--- Injected Error ---\n";
    std::cout << "Choose error type:\n";
    std::cout << "1. Malformed Frame\n";
    std::cout << "2. Incorrect EtherType\n";
    std::cout << "Enter option: ";
    int errorOption;
    std::cin >> errorOption;

    Host::InjectionType selectedType;
    
    if (errorOption == 1) 
    {
        selectedType = Host::InjectionType::MALFORMED_PACKET;
    } 
    else if (errorOption == 2) 
    {
        selectedType = Host::InjectionType::INCORRECT_ETHERTYPE;
    } 
    else 
    {
        std::cout << "Invalid option.\n";
        return;
    }

    // Call upon host to send invalid packet
    host.injectFrameError(selectedType);

    // Call upon ethernet driver to send frames
    driver.processStoredFrame();
    
    // Call upon device to read frames
    device.processReceivedFrames();
    
    // Call upon ethernet driver to send frames
    driver.processStoredFrame();

    // Call upon host to read frames
    host.processReceivedFrames();
    
    std::cout << "\n--- Ending Injected Error ---\n";
}

void performVideoCapture(Host& host, EmbeddedDevice& device, EthernetDriver& driver) {
    std::cout << "\n--- Video Capture Request ---\n";

    std::cout << "\n--- Ending Video Capture Request ---\n";

}

/**
 * @brief Main entry point of application
 * 
 * @return exitCode - int: value to represent the success status of the application
 */
int main() {
    printf("Starting Ethernet Ethernet Driver Application - Version: %d.%d.%d\n", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);

    // Instantiate Ethernet Driver
    EthernetDriver driver;

    // Simulated memory addresses (IP Address) for the Host and Device
    constexpr uint32_t HOST_ADDRESS   = 0x01020304;
    constexpr uint32_t DEVICE_ADDRESS = 0x0A0B0C0D;

    // Create Host and Device objects
    Host host(&driver, HOST_ADDRESS);
    EmbeddedDevice device(&driver, DEVICE_ADDRESS);

    // Set the dest address of the devices
    host.setDestinationAddress(DEVICE_ADDRESS);
    device.setDestinationAddress(HOST_ADDRESS);

    while (true) {
        std::cout << "\n========= Ethernet Protocol Simulator =========\n";
        std::cout << "1. Perform Handshake\n";
        std::cout << "2. Injected Errors\n";
        std::cout << "3. Request Video Capture\n";
        std::cout << "0. Exit\n";
        std::cout << "Choose an option: ";

        int option;
        std::cin >> option;

        switch (option) {
            case 1:
                performHandshake(host, device, driver);
                break;
            case 2:
                performInjectedError(host, device, driver);
                break;
            case 3:
                performVideoCapture(host, device, driver);
                break;
            case 0:
                std::cout << "Exiting...\n";
                return 0;
            default:
                std::cout << "Invalid option.\n";
                break;
        }
    }

    return 0;
};