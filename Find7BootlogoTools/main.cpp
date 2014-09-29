//
//  main.cpp
//  Find7BootlogoTools
//
//  Created by Matteo Gaggiano on 28/09/14.
//  Copyright (c) 2014 Matteo Gaggiano. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "../lodepng/lodepng.h"

#define AUTHOR "Matteo Gaggiano"
#define VERSION "1.0b1"

#if DEBUG
#define DLOG(fmt, ...) printf("Debug: " fmt "\n", ##__VA_ARGS__);
#else
#define DLOG(fmt, ...) do{} while(0)
#endif

#ifdef __LINUX__
#define uint8_t unsigned char
#endif

#define usage(x) std::cout << getexecname(x) << "\n\t-b \t Overwrite the bootlogo\n\t-f \t Overwrite the fastboot image (Not supported yet)\n\t-s \t Save raw images\n\nVersion: " VERSION " by " AUTHOR << std::endl;

#define FILE_LEN 16777216L
#define LOGO_START 3977216L
#define LOGO_LEN 3240000L
#define LOGO_END (LOGO_START+LOGO_LEN)

#define FAST_START 0L
#define FAST_LEN 0L
#define FAST_END (FAST_START+FAST_LEN)

char* getexecname(const char * x)
{
    char* a,* b = NULL;
    a = strtok((char*)x, "/");
    while (a != NULL) {
		a = strtok(NULL, "/");
		b = (a==NULL) ? b : strdup(a);
	}
    return b;
}

void RGBToBGR(std::vector<unsigned char>& image)
{
    unsigned char temp;
    for (int i = 0; i < image.size(); i+=3) {
        temp = image[i];
        image[i] = image[i+2];
        image[i+2] = temp;
    }
}

int main(int argc, const char * argv[]) {
    
    long unsigned int i = 0;
    bool flashBoot = false, flashFast = false, saverawimages = false;
    
    for (int i = 0; i < argc; ++i) {
        if(strcmp(argv[i], "-b") == 0) flashBoot = true;
        if(strcmp(argv[i], "-f") == 0) flashFast = true;
        if(strcmp(argv[i], "-s") == 0) saverawimages = true;
    }
    
    if ( !flashBoot && !flashFast ) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    
    std::vector<unsigned char> logoF, fastF;
    unsigned int w, h, error;
    FILE *fileF = NULL;

    if (flashBoot) {
        std::cout << "Loading bootlogo.png" << std::endl;
        if ((error = lodepng::decode(logoF, w, h, "bootlogo.png", LCT_RGB, 8))) {
            std::cout << "Error " << error << ": " << lodepng_error_text(error) << std::endl;
            exit(EXIT_FAILURE);
        }
        if (logoF.size() != LOGO_LEN) {
            std::cerr << "Error: the w*h of the bootlogo.png isn't 1440x750 8 bit RGB" << std::endl;
            exit(EXIT_FAILURE);
        }
        RGBToBGR(logoF);
        if (saverawimages) {
            lodepng::save_file(logoF, "bootlogo.raw");
            std::cout<<"Saved bootlogo.raw"<<std::endl;
        }
    }
    
    if (flashFast) {
        std::cout << "Not supported yet. Exiting..."<<std::endl;
        exit(EXIT_FAILURE);
        std::cout << "Loading fastboot.png" << std::endl;
        if ((error = lodepng::decode(fastF, w, h, "fastboot.png", LCT_RGB, 8))) {
            std::cout << "Error " << error << ": " << lodepng_error_text(error) << std::endl;
            exit(EXIT_FAILURE);
        }
        if (fastF.size() != LOGO_LEN) {
            std::cerr << "Error: the w*h of the fastboot.png isn't UNKNOWN" << std::endl;
            exit(EXIT_FAILURE);
        }
        RGBToBGR(logoF);
        if (saverawimages) {
            lodepng::save_file(fastF, "fastboot.raw");
            std::cout<<"Loading fastboot.raw"<<std::endl;
        }
    }
    
    uint8_t *fileBin = (uint8_t *) malloc(sizeof(uint8_t) * FILE_LEN);
    
    fileF = fopen("logo.bin", "r");
    if (!fileF) {
        perror("Error while read logo.bin");
        return errno;
    }
    
    for ( i = 0; i < FILE_LEN; i++) {
        fileBin[i] = fgetc(fileF);
    }
    
    fclose(fileF);
    for ( i = 0; i < FILE_LEN; i++)
    {
        if (i >= LOGO_START && i < LOGO_END && flashBoot)
        {
            fileBin[i] = logoF[i - LOGO_START]; // bootlogo image
            if ( i == LOGO_START ) {
                std::cout << "Writing bootlogo image" << std::endl;
            }
        } else
        
        if (i >= FAST_START && i < FAST_END && flashFast && false /* not supported yet */)
        {
            fileBin[i] = fastF[i - FAST_START]; // fastboot logo image
            if ( i == FAST_START ) {
                std::cout << "Writing fastboot image" << std::endl;
            }
        }
    }
    
    fileF = fopen("logo-modified.bin", "w");
    if (!fileF) {
        perror("Error writing file logo-modified.bin");
        return errno;
    }
    
    if (fwrite(fileBin, sizeof(fileBin[0]), FILE_LEN, fileF) == FILE_LEN)
    {
        std::cout << "All jobs done! The file logo-modified.bin created correctly" << std::endl;
    } else {
        std::cout << "The file logo-modified.bin cannot be created correctly. Check directory permissions and/or disk space." << std::endl;
    }
    
    free(fileBin);
    
    return 0;
}
