#ifndef __SMD_LOG_H__
#define __SMD_LOG_H__

#include <string.h>
#include <errno.h>
#include <stdio.h>

#define __pr_err(fmt, ...)						\
		fprintf(stderr, "ERROR (%s:%d): " fmt,			\
			__FILE__, __LINE__, ##__VA_ARGS__)

#define pr_err(fmt, ...)						\
	__pr_err(fmt "\n", ##__VA_ARGS__)

#define pr_perror(fmt, ...)						\
	__pr_err(fmt ": %s\n", ##__VA_ARGS__, strerror(errno))

#define pr_info(fmt, ...)						\
		fprintf(stdout, fmt "\n", ##__VA_ARGS__)

#endif /* __SMD_LOG_H__ */
