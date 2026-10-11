### SDL3 Linux install Guide
This Document Explains the process of Installing SDL3 on Linux. This also includes how to do so for SDL3_image and SDL3_ttf.

There are two ways to install SDL3 Libuary on linux:
A. Install via apt (my result in an older version).
B. Compile the libuary and link against it.

## For Method A: Installing via APT
For Method A on Debian 13 and Later (or debian 13 based systems) install the following:
libsdl3-dev, libsdl3-image-dev, libsdl3-ttf-dev, 
(for documentation you may want to install libsdl3-doc, libsdl3-image-doc, libsdl3-ttf-doc so you have the man pages availabel)

$ sudo apt install libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev libsdl3-doc libsdl3-image-doc libsdl3-ttf-doc

Note: installing this way may result in you getting an older version of SDL3 (see the SDL3 Github page for the latest version information: https://github.com/libsdl-org/SDL/releases
(At time of writing the latest version of SDL3 is version 3.4.2 and the version on APT is version 3.2.10)


## For Method B: Compiling the Libuary and Linking against it
For Method B on Debian 13 (or debian 13 based systems) Compiling the SDL3 project.

The Goal is to create the SDL3 Libuary and the coresponding sublibuarys (SDL3_image and SDL3_ttf) as shared object files on system.

# Compiling SDL3
1. Create a working Directory witch to store your Project in and enter the directory.
In this Example the working directory will be called SDL3_TestCompilation.

2. Download the source code of SDL3 by cloning the GitHub Repository:
$ git clone https://github.com/libsdl-org/SDL

Enter the SDL directory that resulted from the clone.

3. Install all neccecary dependencys using apt:

you can find an updated list of dependencys here:
https://github.com/libsdl-org/SDL/blob/main/docs/README-linux.md#build-dependencies

if you use the dependency list from github note that you may also need to install libwayland-dev libx11-dev seperatly:
$ sudo apt isntall libwayland-dev libx11-dev

$ sudo apt-get install build-essential git make \
pkg-config cmake ninja-build gnome-desktop-testing libasound2-dev libpulse-dev \
libaudio-dev libjack-dev libsndio-dev libx11-dev libxext-dev \
libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev libwayland-dev libx11-dev

4. Create a build directory (in the SDL floder):

$ mkdir build

and Enter the build directory

5. Runn CMake:

$ cmake ..

(".." tels cmake to look in the directory above the working directory)

Upon Sucsess CMkae will tell you that the build files have been written to your build directory and there should be buildfiles there.
(if an error occurs it will most likely tell you that a dependency or libuary is missing, install the packege
 using apt then delete the build directory and make a new build directory (resetting the process) and runn cmake .. again)
 you should now have a "Makefile" in your directory (among other things))
 
6. Runn Make to build the shared object:

$ make -j i

with i being the number of threads/cores that should be used for the build (or just runn make).

The Result should be that tere is a shared Libuary object called "libSDL3.so.<version>" in the build directory.
This is the completed targed fully build, to use it you still need the api header #include <SDL3/SDL.h> #include <SDL3/SDL_main.h> to work.

To do that you need to install the libuary with the following steps:

7. Install the Libuary using make install

$ sudo make install libSDL3.so.<version number>

this copys the needet Libuary files to /usr/local/lib : cmake  libSDL3.so  libSDL3.so.0  libSDL3.so.0.5.0  libSDL3_test.a  pkgconfig

8. Update the libuary with ldconfig:

$ sudo ldconfig

You have now installed SDL3, you can test it with a simple example:
http://wiki.libsdl.org/SDL3/SDL_CreateWindow

To compile Code using SDL3: 
C++:
$ g++ main.cpp -o Test.out -L./SDL/build -lSDL3
the -L./SDL/build is not requiured as the libuary is already installed in a directory where the compiler is looking (/usr/local/lib done by make install)

GCC:
$ gcc main.c -o Test.out -L./SDL/build -lSDL3
the -L./SDL/build is not requiured as the libuary is already installed in a directory where the compiler is looking (/usr/local/lib done by make install)




2. Installation of SDL3_ttf for True type fonts

If you want to use True fonts and usefull text features with your SDL3 Project you wil need SDL3_ttf
To install it in your project (asuming you have taken the steps mentioned in 1):

2.1 Clone the source reposetory (in working directory):
git clone --recurse-submodules -j4 https://github.com/libsdl-org/SDL_ttf.git

you should now have a SDL_ttf directory

2.2 Go in to the SDL_ttf directory and create a directory called build, and enter it:
cd SDL_ttf
mkdir build
cd build

2.3 Install the extra dependencys:
sudo apt install libfreetype-dev

2.4 runn cmake ..
cmake ..
(Note that the .. is important as it represents the git clone directory)

you should now have a "Makefile" in your directory (among other things)

2.5 Runn make
make

(this may take a view minutes)

you should now have a libSDL3_ttf.so.0.3.0 object in your directory
(is a shared libuary)

2.6 Install the libuary using make install:
sudo make install libSDL3_ttf.so.0.3.0

2.7 update the libuarys using ldconfig:
sudo ldconfig

You have now installed the SDL3_ttf libuary, you can test it with examples you can find in the
git repository. to compile put the in the working direktory (the one containing the SDL and SDL_ttf folder)
and compile using:

C++:
g++ Test.cpp -o Test.out -L./SDL_ttf/build -lSDL3_ttf -L./SDL/build -lSDL3

GCC:
gcc Test.cpp -o Test.out -L./SDL_ttf/build -lSDL3_ttf -L./SDL/build -lSDL3


Tipp you will need fonts for using SDL_ttf (as .ttf files), you can find fonts here:
https://www.1001freefonts.com/




This is some information on how to properly uses this text box example

(must be runn in folder above this one)

1. You need to habe SDL3 and SDL3_ttf installed and must link it with your project
compiler command:
gcc showfont.c -o Test.out -L./SDL_ttf/build -lSDL3_ttf -L./SDL/build -lSDL3

2. you need a ttf font file so a font can be rendered, you can find free ttf fonts here:
https://www.1001freefonts.com/

3. when running the programm you must give the font.ttf file and pointsize as arguments
Example:
./Test.out orbitron/Orbitron-Bold.ttf 15
you can change the Text Alingment by pressing a

Note the code is free, you can use it in your project




3. Installation of SDL3_image for image use

If you want to use immage formats outside of bitmap in your SDL3 project you need SDL3_image,
to install:

3.1 clone the repository (in a working directory)
git clone --recurse-submodules -j4 https://github.com/libsdl-org/SDL_image.git

you should now have a SDL_image directory

3.2 Go in to the SDL_image directory and create a build directory, and go in it
cd SDL_image
mkdir build
cd build

3.3 Runn cmake ..
cmake ..
(Note tht the .. is important as it represents the directory above)

you should now have a "Makefile" in your directory

3.4 Runn make
make

you should now have a libSDL3_image.so.0.3.0 object in your directory
(is a shared libuary)

3.5 Install the Libuary using make install
sudo make install libSDL3_image.so.0.3.0

3.6 Update the libuarys using ldconfig
sudo ldconfig

You have now installed SDL3_image, to compile with SDL3_ttf and SDL3, use:

C++:
g++ Test.c -o Test.out -L./SDL_ttf/build -lSDL3_ttf -L./SDL/build -lSDL3 -L./SDL_image/build -lSDL3_image

GCC:
gcc Test.c -o Test.out -L./SDL_ttf/build -lSDL3_ttf -L./SDL/build -lSDL3 -L./SDL_image/build -lSDL3_image



// End













