dijkstra.exe: dijkstra.o heap.o hash.o
	g++ -o dijkstra.exe dijkstra.o heap.o hash.o

dijkstra.o: dijkstra.cpp graph.h
	g++ -g -Wall -Wextra -Wshadow -c dijkstra.cpp

heap.o: heap.cpp heap.h
	g++ -g -Wall -Wextra -Wshadow -c heap.cpp

hash.o: hash.cpp hash.h
	g++ -g -Wall -Wextra -Wshadow -c hash.cpp

debug:
	g++ -Wall -Wextra -Wshadow -g -O1 -fsanitize=address,undefined \
	    -fno-omit-frame-pointer -o dijkstra_debug.exe dijkstra.cpp heap.cpp hash.cpp
clean:
	rm -f *.exe *.o *.stackdump *~

backup:
	test -d backups || mkdir backups
	cp *.cpp backups
	cp *.h backups
	cp Makefile backups