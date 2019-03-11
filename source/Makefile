target: main.o ui.o map.o trie.o pos.o scores.o maxheap.o
	gcc -o minisearch main.o ui.o map.o trie.o pos.o scores.o maxheap.o -lm

main.o: main.c
	gcc -c main.c

ui.o: ui.c
	gcc -c ui.c

map.o: map.c
	gcc -c map.c

trie.o: trie.c
	gcc -c trie.c

pos.o: pos.c
	gcc -c pos.c

scores.o: scores.c
	gcc -c scores.c

maxheap.o: maxheap.c
	gcc -c maxheap.c

clean:
	rm -f main.o ui.o map.o trie.o pos.o scores.o maxheap.o minisearch
