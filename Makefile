SOURCES=ysh.c
TEST=test.c
FLAGS= -Wall -pedantic -ggdb
MAIN=main

all: $(MAIN)

test: $(TEST)
	gcc $(FLAGS) $(TEST) -o test

main: $(SOURCES)
	gcc $(FLAGS) $(SOURCES) -o ysh

clean:
	rm ysh debug

zip: 
	zip Emilio_Canton-YSH_Project.zip ./*