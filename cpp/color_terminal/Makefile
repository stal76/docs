CXX       := gcc
CXX_FLAGS := -Wall -Wextra

BIN     := build
SRC     := test_color.c
EXECUTABLE  := test_color


all: $(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)
	$(CXX) $(CXX_FLAGS) $(INCLUDE) $^ -o $@

clean:
	-rm $(BIN)/*
