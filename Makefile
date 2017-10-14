CFLAGS	+= -Wall -Werror -Wextra
LDFLAGS += -lrt

TARGET_BIN = smd

SRCS	+= main.c
OBJS	= $(SRCS:.c=.o)

.PHONY: all clean test test-docker

all: $(TARGET_BIN)

$(TARGET_BIN): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

-include $(SRCS:.c=.d)

$(SRCS:.c=.d): %.d: %.c
	$(CC) $(CFLAGS) -MM $< >$@

%.o: %.d

test: $(TARGET_BIN)
	./smd server &
	./smd client

test-docker:
	echo -e "\e[31m\nWARNING: THIS WILL USE UPSTREAM GIT REPO, NOT YOUR LOCAL ONE\n\e[0m"
	docker build -t centos7:smd-server docker/smd/centos7/smd-server
	docker build -t centos7:smd-client docker/smd/centos7/smd-client
	docker run --detach -v $$(pwd):/smd -w /smd centos:latest bash -c '/smd/smd server' > server.sha
	docker run --rm -v $$(pwd):/smd -w /smd --ipc=container:$$(cat server.sha) centos:latest bash -c '/smd/smd client'
	docker wait $$(cat server.sha)
	docker logs $$(cat server.sha)
	docker rm $$(cat server.sha)

clean:
	$(RM) $(TARGET_BIN) $(OBJS) $(SRCS:.c=.d)
