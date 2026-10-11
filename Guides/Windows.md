How to Install and Runn SDL3 (+SDL3_ttf +SDL3_image)on Windows with mingw (a gcc port to windows)

1. Installing the mingw-w64 compiler:

The mingw-w64 is a kit of basic linux applications including gcc ported to windows.
It is common use for C windows development and a SDL3 suported method.

1.1 Go to the mingw-w64 website and install mingw-w64

https://www.mingw-w64.org/

you can find the download under:
downloads>Prebuilt Toolchains
download the w64devkit via GitHub releses (the x64 .exe one)
(if you are working on an ARM mashine or 32bit mashine you may need a diffrent one)

1.2 Exicute the devkit (witch is basicly just a file extractor)
and extract the files to someware where you plan on keeping your compiler program.
(i put the min a folder in documents witch is where programms typicaly keep their data)

1.3 Set up Enviurment variables
So that when you enter gcc in to your comand prompt the comand prompt knows where to find gcc.
Search for "Enviro" or "Edit Enviroment variables" and open the systemcontroll:Edit Envirument variables.
there klick on Enviroment variables button
under system variables select the path variable and hit the edit button

add the path to where you put the compiler floder in step 1.2 using the new button (the compiler will be in the
  directory that was extracted, in the bin floder)(the path does not include the exe itself)
->copy the path to the bin folder

once done exit out completly using the ok buttons.

you can now check if the installation was sucsesfull by opening command prompt and running gcc
if gcc responds the installation was sucsesfull if the comand prompt lables gcc as unknown, check if your path variable was adjusted corectly.
(you can also now compile c programs including all the standerd libs)

To start a compiled program you can try "./<program name>" or "start <program name>"


2. Installing SDL3, SDL3_ttf, SDL3_image:
other SDL libuarys can be added adjacent to the SDL3_ttf or SDL3_image libuary

2.1 Get the libuarys from github

Got to the SDL3 github releses and download the latest version:
SDL3-devel-<version>-mingw.tar.gz

Got to the SDL3_ttf github releses and download the latest version:
SDL3_ttf-devel-<version>-mingw.tar.gz

Got to the SDL3_image github releses and download the latest version:
SDL3_image-devel-<version>-mingw.tar.gz



2.2 Extracting and placing the libuarys

Decide where you want to store the SDL3 libuarys and create a lib_SDL3 directory there.
this can be in your project directory or anywhere the compiler can acces.
you want to create the following folder structure:
(the name lib_SDL3 is optional and can be annything but the folders in the directory must be named same as here or you must later adjust all code #includes witch is not recomended)

lib_SDL3
|-SDL3
|-SDL3_ttf
|-SDL3_image

Now Extract the downloaded libuary .tar.gz files NOT yet in to the folder structure. (so you have the libuarys as uncompresed files)

Now FOR EACH LIBUARY:
go in to the extracted directory enter the x86_64-w64-mingw32 directory (if you have an i686 system go in to i686-w64-mingw32)
and copy all directorys (primarly bin, include, lib) in to the folder structure we just created in to that libuarys directory.

final stepp is adding the libuarys dll files.

you can ether put a copy/move the libuarys .dll file (witch is in the bin directory of each libuary in our folder structure) in to the directory of the code and where the resulting EXE file must be to be exicuted in this case,

or you can modefy your system path variable and add the paths to the .dll files of the libuarys to the path variable so that the resulting EXE can be exicuted annywhere on your PC and does not need to have the dll files in the same directory.

To modefy the system path variable it is the same proces as listed above:

Search for "Enviro" or "Edit Enviroment variables" and open the systemcontroll:Edit Envirument variables.
there klick on Enviroment variables button
under system variables select the path variable and hit the edit button.
Add the paths to the .dll files (all three or more) using the new button (each path requiures its own fild)

once done exit out using the ok buttons.



3. Actualy compiling your SDL3 project

to compile your SDL3 project we will be using cmake and a makefile (cmake should have been installed with mingw32 as it is included in the kit).

An Example makefile will follow where you can fill in the paths to where you put the libuarys.
Note that the makefile is structured in a way that mandates the lis_SDL3 floder structure to work.
you can also edit the output name if you want..., the output name in this case is my_game.exe

to build your project runn make
to destroy the build so you can make a nother runn make clean

Example Makefile:

# Set the paths for the libraries
SDL3_PATH = "<path to lib_SDL3/SDL3 floder>"
SDL3_TTF_PATH = "<path to lib_SDL3/SDL3_ttf floder"
SDL3_IMAGE_PATH = "<path to lib_SDL3/SDL3_image floder"
MINGW_PATH = C:/mingw

# Compiler and linker flags
CC = gcc
CFLAGS = -Wall -I$(SDL3_PATH)/include -I$(SDL3_TTF_PATH)/include -I$(SDL3_IMAGE_PATH)/include
LDFLAGS = -L$(SDL3_PATH)/lib -L$(SDL3_TTF_PATH)/lib -L$(SDL3_IMAGE_PATH)/lib -lSDL3 -lSDL3_ttf -lSDL3_image -lm -lmingw32 -lSDL3

# Project files
SRC = SDL3_Project_Template_with_example.c
OBJ = $(SRC:.c=.o)
EXEC = my_game.exe

# Build rules
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

# END OF Makefile

General File IO:

Generaly works simularly to Linux (paths relative to the application).
But for Production code the following functions should be used to answer relevant questions.

There are functions to answer necessary path questions:

    Where is my app's data? SDL_GetBasePath().
    Where can I safely write files? SDL_GetPrefPath().
    Where are paths like Downloads, Desktop, Music? SDL_GetUserFolder().
    What is this thing at this location? SDL_GetPathInfo().
    What items live in this folder? SDL_EnumerateDirectory().
    What items live in this folder by wildcard? SDL_GlobDirectory().
    What is my current working directory? SDL_GetCurrentDirectory().

When dealing with actualy opening and reading/writing to files it is recomended to use SDL functions
or use an abstraction as SDL functionsare in some ways similar to std file functions

















lawdawd
