all: test

ejsonrpc.o: ejsonrpc.c
	$(CC) -g -c $(CFLAGS) $^ -o $@
	
frozen.o: frozen.c
	$(CC) -c $(CFLAGS) $^ -o $@
	
munit.o: munit.c
	$(CC) -c $(CFLAGS) $^ -o $@
	
test: test.o ejsonrpc.o frozen.o munit.o
	$(CC) $(LDFLAGS) $^ -o out/$@
	
test.o: test.c 
	$(CC) -g -c $(CFLAGS) $^ -o $@
	
clean:
	rm ejsonrpc.o
	rm frozen.o
	rm munit.o
	rm test.o
	rm out/test