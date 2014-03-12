
$(CC)=/usr/bin/gcc


all_tests : test2 test1


test1 : test1.o continuations.o definitions.o
	$(CC) -o  $@  $^ 

test1.o : test1.c
	$(CC) -o  $@ -c  $< -Wall -pedantic

test2 : tests.o continuations.o definitions.o
	$(CC) -o  $@  $^ 

tests.o : tests.c
	$(CC) -o  $@ -c  $< -Wall -pedantic

continuations.o : continuations.c continuations.h
	$(CC) -o  $@ -c  $< -Wall -pedantic	

definitions.o :  definitions.c
	$(CC) -o  $@ -c  $< -Wall -pedantic	

cleanAll : clean
	rm -f *.o test1 test2
clean :
	rm -f *~