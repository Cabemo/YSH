SOURCES=ysh.c
TEST=test.c
FLAGS= -Wall -pedantic -ggdb
MAIN=main
MANUAL=manual

all: $(MAIN) $(MANUAL)

manual:
	pandoc -o Manual.pdf Manual.md

main: $(SOURCES)
	gcc $(FLAGS) $(SOURCES) -o ysh

clean:
	rm ysh debug

zip: 
	zip Emilio_Canton-YSH_Project.zip ./*