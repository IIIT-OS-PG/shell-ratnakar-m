EXEC = myshell

LIBS = 

INCLUDES = .

SRC = \
	myshell.cpp \
	helpers.cpp \

build:
	g++ -g -w -std=c++14  $(SRC) -I $(INCLUDES) -o $(EXEC) $(LIBS) && chmod +x $(EXEC) 

run: build
	./$(EXEC)

debug: build
	gdb $(EXEC)

valgrind: build
	valgrind --leak-check=full --show-leak-kinds=all ./$(EXEC)

