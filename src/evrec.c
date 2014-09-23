/**
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org/>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <linux/input.h>

static struct timespec
timespec_sub(struct timespec a, struct timespec b)
{
	struct timespec ret;

	if (a.tv_nsec - b.tv_nsec < 0)
		return (struct timespec) {
			.tv_sec  = a.tv_sec - b.tv_sec - 1,
			.tv_nsec = 1000000000 + a.tv_nsec - b.tv_nsec
		};

	return (struct timespec) {
		.tv_sec  = a.tv_sec  - b.tv_sec,
		.tv_nsec = a.tv_nsec - b.tv_nsec
	};
}

static void
dump_events(int evdev, int to)
{
	struct timespec last_event, now, delta;
	struct input_event ev;

	last_event.tv_sec = 0;

	for (;;) {
		if (read(evdev, &ev, sizeof(ev)) != sizeof(ev)) {
			perror("unexpected read length");
			return;
		}

		clock_gettime(CLOCK_MONOTONIC, &now);
		if (!last_event.tv_sec)
			last_event = now;

		delta = timespec_sub(now, last_event);

		/* since `struct input_event` has a timeval in it, we'll just use
		 * that to store the delta from the last event. note that we're using
		 * nanoseconds for the delta even though fractional seconds in a
		 * `struct timeval` are in microseconds. */
		ev.time.tv_sec  = delta.tv_sec;
		ev.time.tv_usec = delta.tv_nsec;
		write(to, &ev, sizeof(ev));

		last_event = now;
	}
}

int
main(int argc, char **argv)
{
	int fd;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <event device>\n", argv[0]);
		return EXIT_FAILURE;
	}

	if ((fd = open(argv[1], O_RDONLY)) < 0) {
		perror("can't open event device");
		return EXIT_FAILURE;
	}

	dump_events(fd, STDOUT_FILENO);

	close(fd);
	return EXIT_SUCCESS;
}
