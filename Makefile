tools:
	make -C src

test:
	make -C tests

clean:
	make clean -C src
	make clean -C tests
