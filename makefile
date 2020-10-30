src = main.c chat.c friendbox.c msg_queue.c client.c client_test.c
hello: $(src)
	gcc $(src) -o hello -g -pthread `pkg-config --cflags --libs gtk+-3.0`
