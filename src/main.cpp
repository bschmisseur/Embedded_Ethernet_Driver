/**
 * @file main.cpp
 * 
 * @brief File to contain the main entry point of the project
 * 
 * @author Bryce Schmisseur
 * 
 */

// Project Includes
#include "version.hpp"

//Standard Includes
#include <cstdio>
#include <iostream>

/**
 * @brief Main entry point of application
 * 
 * @return exitCode - int: value to represent the success status of the application
 */
int main(){

    printf("Starting Ethernet Ethernet Driver Application - Version: %d.%d.%d\n", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);

    return 0; 
}