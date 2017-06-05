test : ae.o test.o
	gcc -o test ae.o test.o

ae.o : ae.c ae_epoll.c ae_evport.c ae_kqueue.c ae_select.c
	gcc -c ae.c
test.o : test.c
	gcc -c test.c
clean :
	rm test.o  ae.o test

.PHONY: edit clean
