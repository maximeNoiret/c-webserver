COMPILER = gcc
CFLAGS = -Wall -Werror -g -fsanitize=address,undefined
LIBRARIES = -L/usr/local/lib -lpurpdynarray
INCLUDES = -I/usr/local/include

main: main.o server.o pages.o
	$(COMPILER) $(CFLAGS) -o $@ $^ $(LIBRARIES)

main.o: main.c server.h pages.h
	$(COMPILER) $(CFLAGS) -c $< -o $@ $(INCLUDES)

server.o: server.c server.h pages.h
	$(COMPILER) $(CFLAGS) -c $< -o $@ $(INCLUDES)

pages.o: pages.c pages.h
	$(COMPILER) $(CFLAGS) -c $< -o $@ $(INCLUDES)

clean:
	rm -f *.o main
