# https://opensource.com/article/18/8/what-how-makefile
# https://www3.ntu.edu.sg/home/ehchua/programming/cpp/gcc_make.html
# phony target all can run multiple other targets
all: build run

build:
# https://medium.com/@edkins.sarah/set-up-sdl2-on-your-mac-without-xcode-6b0c33b723f7
	@echo "Building..."
	@g++ -std=c++17 -Wall -Werror -O0 source/*.cpp -I"source/*.hpp" -I"include" -L"lib" -l SDL2-2.0.0 -l SDL2_image-2.0.0 -o dungeon
	
run:
	@echo "Running..."
	./dungeon

clean:
	@rm dungeon
	@echo "Clean complete"
