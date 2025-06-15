# === Configuration ===
CXX = g++
SRC = main.cpp global.cpp menu.cpp map.cpp
OUT = game

UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
    CXXFLAGS = -std=c++17 -I/opt/homebrew/include
    LDFLAGS = -L/opt/homebrew/lib -lraylib \
              -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo
else
    CXXFLAGS =
    LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

# === Rules ===
all:
	$(CXX) $(SRC) -o $(OUT) $(CXXFLAGS) $(LDFLAGS) && ./$(OUT)

clean:
	rm -f $(OUT)
