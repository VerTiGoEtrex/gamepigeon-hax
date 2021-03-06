CXX		  := clang++
CXX_FLAGS := -Wall -Wextra -std=c++17 -O3

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib

LIBRARIES	:=
EXECUTABLE	:= main


all: $(BIN)/$(EXECUTABLE)

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) -lprofiler $^ -o $@ $(LIBRARIES)

clean:
	-rm -rf $(BIN)/*
