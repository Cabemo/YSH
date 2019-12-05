SOURCES=ysh.c
FLAGS= -Wall -pedantic -ggdb
MAIN=main
DEBUG=debug

all: $(MAIN) $(DEBUG)

main: $(SOURCES)
	gcc $(FLAGS) $(SOURCES) -o ysh

debug: $(SOURCES)
	gcc $(FLAGS) -DDEBUG $(SOURCES) -o debug

clean:
	rm ysh debug

zip: 
	zip Emilio_Canton-YSH_Project.zip ./*