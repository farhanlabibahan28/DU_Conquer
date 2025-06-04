# === Configuration ===
CXX = g++
CXXFLAGS = -std=c++17 -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lraylib \
-framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo

SRC = main.cpp 
OUT = game

# === Rules ===
all:
	$(CXX) $(SRC) -o $(OUT) $(CXXFLAGS) $(LDFLAGS) && ./$(OUT)

clean:
	rm -f $(OUT)