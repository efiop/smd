#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>

#include "log.h"

#define SMD_REGION_NAME "/kinvolk"
#define SMD_TIMEOUT 1
#define SMD_MAX_TRIES 10

static int smd_server(void)
{
	char *buf = NULL;
	int fd = -1, ret;
	unsigned char c = 0;
	bool need_unlink = true;

	ret = shm_unlink(SMD_REGION_NAME);
	if (ret == -1 && errno != ENOENT) {
		pr_perror("Failed to unlink %s", SMD_REGION_NAME);
		goto out;
	}

	fd = shm_open(SMD_REGION_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		pr_perror("Failed to create %s", SMD_REGION_NAME);
		ret = -1;
		goto out;

	}

	ret = ftruncate(fd, sizeof(*buf));
	if (ret == -1) {
		pr_perror("Failed to truncate fd %d(%s)", fd, SMD_REGION_NAME);
		goto out;
	}

	buf = mmap(NULL, sizeof(*buf), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (buf == MAP_FAILED) {
		pr_perror("Failed to map fd %d(%s)", fd, SMD_REGION_NAME);
		ret = -1;
		goto out;
	}

	while (c < SMD_MAX_TRIES) {
		*buf = c;
		pr_info("Wrote: %u", *buf);
		sleep(SMD_TIMEOUT);
		c++;
	}

	ret = 0;
out:
	if (fd >= 0)
		close(fd);
	if (need_unlink)
		shm_unlink(SMD_REGION_NAME);
	return ret;
}

static int smd_client(void)
{
	unsigned char *buf;
	unsigned char c = 0;
	int fd = -1, ret = -1;

again:
	fd = shm_open(SMD_REGION_NAME, O_RDWR, 0);
	if (fd == -1) {
		if (errno == ENOENT && c < SMD_MAX_TRIES) {
			sleep(SMD_TIMEOUT);
			c++;
			goto again;
		}
		pr_perror("Failed to shm_open %s", SMD_REGION_NAME);
		goto out;
	}

	buf = mmap(NULL, sizeof(*buf), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (buf == MAP_FAILED) {
		pr_perror("Failed to map fd %d(%s)", fd, SMD_REGION_NAME);
		goto out;
	}

	c = 0;
	while (c < SMD_MAX_TRIES) {
		pr_info("Read: %u", *buf);
		sleep(SMD_TIMEOUT);
		c++;
	}

	return 0;
out:
	if (fd >= 0)
		close(fd);
	return ret;
}

int main(int argc, char *argv[])
{
	if (argc != 2)
		goto usage;

	if (!strcmp(argv[1], "client"))
		return smd_client();

	if (!strcmp(argv[1], "server"))
		return smd_server();

usage:
	pr_info("Usage:");
        pr_info("  smd client|server");

	return 1;
}
