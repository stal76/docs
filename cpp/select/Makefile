CXX       := gcc
CXX_FLAGS := -Wall -Wextra

BIN     := build
SRC     := select.c
EXECUTABLE  := test_select


all: $(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)
	$(CXX) $(CXX_FLAGS) $(INCLUDE) $^ -o $@

clean:
	-rm $(BIN)/*
