# script for automaticaly building and installing SDL3 Libuarys
# The Libuarys will be installed in a follder called SDL3_Libuarys witch will be created by the script
# Requiurs sudo privileges
# This script only only build for Linux

#!/bin/bash

# Check for sudo privileges
if sudo -l &> /dev/null; then
    echo "User has sudo privileges."
else
    echo "User does not have sudo privileges."
    echo "exiting..."
    exit
fi


# Create folder as location for Building
mkdir SDL3_Libuarys
cd SDL3_Libuarys

# install dependency packeges
sudo apt install libwayland-dev libx11-dev
sudo apt-get install build-essential git make \
pkg-config cmake ninja-build gnome-desktop-testing libasound2-dev libpulse-dev \
libaudio-dev libjack-dev libsndio-dev libx11-dev libxext-dev \
libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev libwayland-dev libx11-dev

# Start with SDL3 Baselibuary
git clone https://github.com/libsdl-org/SDL
cd SDL

# Create Build directory inside SDL Folder
mkdir build
cd build

# Runn CMake
cmake ..

# (this may take some time)

# Runn make
make

# Install the Libuary
sudo make install libSDL3.so.*.*.*
sudo ldconfig





# Return to SDL3_Libuarys Directory
cd ..
cd ..




# Install SDL3_ttf
# Clone the git repository
git clone --recurse-submodules -j4 https://github.com/libsdl-org/SDL_ttf.git
cd SDL_ttf

# Create Build directory
mkdir build
cd build

# Install extra dependency
sudo apt install libfreetype-dev

# Run CMake
cmake ..

# Runn make
make

# Install the Libuary
sudo make install libSDL3_ttf.so.*.*.*
sudo ldconfig




# Return to SDL3_Libuarys Directory
cd ..
cd ..




# Install SDL3_image
# Clone the git repository
git clone --recurse-submodules -j4 https://github.com/libsdl-org/SDL_image.git
cd SDL_image

# Create Build directory
mkdir build
cd build

# Runn CMake
cmake ..

# Runn Make
make

# Install the Libuary
sudo make install libSDL3_image.so.*.*.*
sudo ldconfig
