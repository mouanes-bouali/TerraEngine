# Compiler and flags
CXX       = g++
CXXFLAGS  = -std=c++17 -Isrc -IC:/msys64/mingw64/include/SDL2
LDFLAGS   = -L/mingw64/lib -lSDL2 -Wl,--subsystem,console

# Source files (add new .cpp files here as your engine grows)
SRCS      = src/core/main.cpp src/core/GameLoop.cpp src/platform/Window.cpp

# Target executable
TARGET    = engine.exe

# SDL2 DLL location (for running)
SDL_DLL   = /mingw64/bin/SDL2.dll

# Default target: build the engine
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

# Run the engine (copies required DLL and launches)
run: $(TARGET)
	cp $(SDL_DLL) ./
	./$(TARGET)

# Remove built files
clean:
	rm -f $(TARGET) SDL2.dll

.PHONY: all run clean