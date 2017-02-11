CFLAGS = -Wall -g

all:
	gcc -c sha256.c $(CFLAGS)
	gcc -c rsa.c $(CFLAGS)
	gcc -c gen_header.c $(CFLAGS)
	gcc -o ../bin/gen_header gen_header.o sha256.o rsa.o -lcrypto
	gcc -c gen_rootkey.c $(CFLAGS)
	gcc -o ../bin/gen_rootkey gen_rootkey.o rsa.o -lcrypto

clean:
	rm -f *.o ./bin/gen_header ../bin/gen_rootkey
