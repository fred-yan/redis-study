#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <errno.h>  
#include <sys/socket.h>  
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>  
#include <netdb.h>
#include <sys/epoll.h>  
#include <string.h> 

struct event_loop
{
	int epfd;
	struct epoll_event *events;
	int nevent;
};

struct event_monit
{
	int fd;
	short events;
	void *(*callback)(int fd, void *args);
	void *args;
};

int initEvent(struct event_loop *loop, int nevent)
{
	int epfd;
	struct epoll_event *events;
	epfd = epoll_create(nevent);
	if (epfd < 0) {
		return -1;
	}

	events = calloc(nevent, sizeof(struct epoll_event));
	if (events == NULL)
	{
		close(epfd);
		return -1;
	}

	if (loop != NULL)
	{
		memset(loop, 0, sizeof(struct event_loop));
		loop->epfd = epfd;
		loop->events = events;
		loop->nevent = nevent;
	}

	return 0;
}

int addEvent(struct event_loop *loop, struct event_monit *m)
{
	struct epoll_event event;
	event.data.ptr = m;
	event.events = m->events;
	
	int retCode = epoll_ctl(loop->epfd, EPOLL_CTL_ADD, m->fd, &event);
	if (retCode < 0) 
	{
		printf("epoll_ctl add event error\n");
		return retCode;
	}

	return 0;
}

int delEvent(struct event_loop *loop, struct event_monit *m)
{
	struct epoll_event event;
	event.data.ptr = m;
	event.events = m->events;

	int retCode = epoll_ctl(loop->epfd, EPOLL_CTL_DEL, m->fd, &event);
	if (retCode < 0)
	{
		printf("epoll_ctl del enent error\n");
		return retCode;
	}

	return 0;
}

int dispatcher(struct event_loop *loop, int timeout)
{
	int i, j, nevents;
	nevents = epoll_wait(loop->epfd, loop->events, loop->nevent, timeout);
	if (nevents < 0)
	{
		printf("epoll_wait error: %d\n", nevents);
		abort();
	}
	for (i=0; i<nevents; i++)
	{
		struct event_monit *em = (struct event_monit *)loop->events[i].data.ptr;
		em->events = loop->events[i].events;
		if (em->callback) 
		{
			printf("start exec callback function\n");
			em->callback(em->fd, em->args);
		}
	}

	return 0;
}

void *test(int fd, void *arg) {

	printf ("****************test: fd= %d\n", fd);
	char buff[256];
	int len = recv(fd, buff, sizeof(buff), 0);

	if (len > 0) {
		buff[len] = '\0';
		printf("%s\n", buff);
	}
	else {
		perror("recvfrom error:");
	}

	printf("****************test end **********\n");
}

void *inTest(int fd, void *arg) {
	printf ("****************inTest: fd= %d\n", fd);
	char buff[256];
	int len = read(fd, buff, sizeof(buff));

	if (len > 0) {
		buff[len] = '\0';
		printf("%s\n", buff);
	}
	else {
		perror("read stdin error:");
	}

	printf("****************inTest end **********\n");
}

int main(int argc, char **argv) {
	int listenFd = -1;
	int connFd = -1;
	struct sockaddr_in servAddr;
	listenFd = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(listenFd, F_SETFL, O_NONBLOCK);
	memset(&servAddr, 0, sizeof(servAddr));

	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(8080);
	servAddr.sin_addr.s_addr = INADDR_ANY;
	bind(listenFd, (struct sockaddr *)&servAddr, sizeof(servAddr));

	listen(listenFd, 5);

	struct event_monit event, inEvent;
	struct event_loop *loop = (struct event_loop*)malloc(sizeof(struct event_loop));


	event.fd = listenFd;
	event.events = EPOLLIN;
	event.args = NULL;
	event.callback = test;

	inEvent.fd = 0;
	inEvent.events = EPOLLIN;
	inEvent.args = NULL;
	inEvent.callback = inTest;

	initEvent(loop, 64);
	addEvent(loop, &event);
	addEvent(loop, &inEvent);

	while(1) {
		dispatcher(loop, -1);
	}

	return 0;
}