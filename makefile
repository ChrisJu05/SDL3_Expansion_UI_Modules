# Makefile for Linux Project Template (may need to be removed for IOS)

# IMPORTANT:
# define the name of the output programm Here
Output_Program_Name := my_prog.out

# define extra compiler options
Extra_Compiler_Options := -g -Wall -Wextra # $(nullstring) if no extra options

# diffrent variabels should be set depending on how the SDL3 libuarys are provided:
# comment out the variant you are using

# SDL3 Libuarys installed as Packege:
#SDL3_Libuary := -lSDL3
#SDL3_ttf_Libuary := -lSDL3_ttf
#SDL3_image := -lSDL3_image
#SDL3_net_Libuary := $(nullstring) # Make a variable with a single space.

# SDL3 Libuarys custom compilled:
SDL3_Libuary := -L./LSD3_Libuarys/SDL/build -lSDL3
SDL3_ttf_Libuary := -L./LSD3_Libuarys/SDL_ttf/build -lSDL3_ttf
SDL3_image := -L./LSD3_Libuarys/SDL_image/build -lSDL3_image
#SDL3_net_Libuary := $(nullstring) # Make a variable with a single space. -L./LSD3_Libuarys/SDL_net/build -lSDL3_net



all:

	# build all the expansion modules
	gcc -c inc/dynamic_lines.c $(SDL3_Libuary) $(SDL3_ttf_Libuary) $(SDL3_image) $(SDL3_net_Libuary) -lm $(Extra_Compiler_Options)
	gcc -c inc/edit_box.c $(SDL3_Libuary) $(SDL3_ttf_Libuary) $(SDL3_image) $(SDL3_net_Libuary) -lm $(Extra_Compiler_Options)
	gcc -c inc/glass.c $(SDL3_Libuary) $(SDL3_ttf_Libuary) $(SDL3_image) $(SDL3_net_Libuary) -lm $(Extra_Compiler_Options)
	gcc -c inc/mini_animations.c $(SDL3_Libuary) $(SDL3_ttf_Libuary) $(SDL3_image) $(SDL3_net_Libuary) -lm $(Extra_Compiler_Options)
	gcc -c inc/mini_labels.c $(SDL3_Libuary) $(SDL3_ttf_Libuary) $(SDL3_image) $(SDL3_net_Libuary) -lm $(Extra_Compiler_Options)
	gcc -c inc/window.c $(SDL3_Libuary) $(SDL3_ttf_Libuary) $(SDL3_image) $(SDL3_net_Libuary) -lm $(Extra_Compiler_Options)
	gcc -c inc/QTS_Text.c $(SDL3_Libuary) $(SDL3_ttf_Libuary) $(SDL3_image) $(SDL3_net_Libuary) -lm $(Extra_Compiler_Options)
	gcc -c inc/QTS_Line.c $(SDL3_Libuary) $(SDL3_ttf_Libuary) $(SDL3_image) $(SDL3_net_Libuary) -lm $(Extra_Compiler_Options)
	gcc -c inc/QTS_EditLine.c $(SDL3_Libuary) $(SDL3_ttf_Libuary) $(SDL3_image) $(SDL3_net_Libuary) -lm $(Extra_Compiler_Options)
	gcc -c inc/QTS_EditLine_Functions.c $(SDL3_Libuary) $(SDL3_ttf_Libuary) $(SDL3_image) $(SDL3_net_Libuary) -lm $(Extra_Compiler_Options)
	gcc -c inc/QTS_Utils.c $(SDL3_Libuary) $(SDL3_ttf_Libuary) $(SDL3_image) $(SDL3_net_Libuary) -lm $(Extra_Compiler_Options)
	gcc -c inc/QTS_Box.c $(SDL3_Libuary) $(SDL3_ttf_Libuary) $(SDL3_image) $(SDL3_net_Libuary) -lm $(Extra_Compiler_Options)
	gcc -c inc/QTS_EditBox.c $(SDL3_Libuary) $(SDL3_ttf_Libuary) $(SDL3_image) $(SDL3_net_Libuary) -lm $(Extra_Compiler_Options)
	gcc -c inc/QTS_EditBox_Functions.c $(SDL3_Libuary) $(SDL3_ttf_Libuary) $(SDL3_image) $(SDL3_net_Libuary) -lm $(Extra_Compiler_Options)

	# build the main file
	gcc -c main.c $(SDL3_Libuary) $(SDL3_ttf_Libuary) $(SDL3_image) $(SDL3_net_Libuary) -lm $(Extra_Compiler_Options)

	# link all components
	gcc main.o dynamic_lines.o edit_box.o glass.o mini_animations.o mini_labels.o window.o QTS_Text.o QTS_Line.o QTS_EditLine.o QTS_EditLine_Functions.o QTS_Utils.o QTS_Box.o QTS_EditBox.o QTS_EditBox_Functions.o \
	$(SDL3_Libuary) $(SDL3_ttf_Libuary) $(SDL3_image) $(SDL3_net_Libuary) -lm -o $(Output_Program_Name) $(Extra_Compiler_Options)

	# clean up all inc modules
	rm dynamic_lines.o
	rm edit_box.o
	rm glass.o
	rm mini_animations.o
	rm mini_labels.o
	rm window.o
	rm QTS_Text.o
	rm QTS_Line.o
	rm QTS_EditLine.o
	rm QTS_EditLine_Functions.o
	rm QTS_Utils.o
	rm QTS_Box.o
	rm QTS_EditBox.o
	rm QTS_EditBox_Functions.o

	# clean up main
	rm main.o

clean:

	rm $(Output_Program_Name)
