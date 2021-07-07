CC=gcc
COMMON= -Wall -Wextra -Wfloat-equal -pedantic -std=c90 -Werror
DEBUG= -g3
SANITIZE= $(COMMON) -fsanitize=undefined -fsanitize=address $(DEBUG)
VALGRIND= $(COMMON) $(DEBUG)
PRODUCTION= $(COMMON) -O3
LDLIBS = -lm

testrealloc : assoc.h Realloc/specific.h Realloc/realloc.c testassoc.c ../../ADTs/General/general.h ../../ADTs/General/general.c
	$(CC) testassoc.c Realloc/realloc.c ../../ADTs/General/general.c -o testrealloc -I./Realloc $(PRODUCTION) $(LDLIBS)

testrealloc2 : assoc.h Realloc/specific.h Realloc/realloc.c testassoc2.c ../../ADTs/General/general.h ../../ADTs/General/general.c
	$(CC) testassoc2.c Realloc/realloc.c ../../ADTs/General/general.c -o testrealloc2 -I./Realloc $(PRODUCTION) $(LDLIBS)

testrealloc_s : assoc.h Realloc/specific.h Realloc/realloc.c testassoc.c ../../ADTs/General/general.h ../../ADTs/General/general.c
	$(CC) testassoc.c Realloc/realloc.c ../../ADTs/General/general.c -o testrealloc_s -I./Realloc $(SANITIZE) $(LDLIBS)

testrealloc_v : assoc.h Realloc/specific.h Realloc/realloc.c testassoc.c ../../ADTs/General/general.h ../../ADTs/General/general.c
	$(CC) testassoc.c Realloc/realloc.c ../../ADTs/General/general.c -o testrealloc_v -I./Realloc $(VALGRIND) $(LDLIBS)

clean:
	rm -f testrealloc_s testrealloc_v testrealloc testcuckoo_s testcuckoo_v testcuckoo testrealloc2

basic: testrealloc_s testrealloc_v
	./testrealloc_s
	valgrind ./testrealloc_v

extra:	testrealloc2
	./testrealloc2
