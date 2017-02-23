CC = $(CROSS_COMPILER)gcc
AR = $(CROSS_COMPILER)ar

CFLAGS := $(CFLAGS) -DAXTLS_LITE -Wall -I/opt/axtls-lite/include
LDFLAGS := $(LDFLAGS) -lsal_crypto -L. -laxtls-lite -L/opt/axtls-lite/src

sal_crypto:
	$(CC) -c sal_sha256.c $(CFLAGS)
	$(CC) -c sal_rsa.c $(CFLAGS)
	$(AR) rvs libsal_crypto.a sal_sha256.o sal_rsa.o

export CFLAGS
export LDFLAGS
test: sal_crypto
	make -C tests

clean:
	rm -f *.o *.a
	make clean -C tests
