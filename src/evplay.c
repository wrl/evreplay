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

static void
play_events(int from, int evdev)
{
	struct timespec req, rem;
	struct input_event ev;

	for (;;) {
		if (read(from, &ev, sizeof(ev)) < sizeof(ev))
			return;

		req.tv_sec  = ev.time.tv_sec;
		req.tv_nsec = ev.time.tv_usec;

		clock_nanosleep(CLOCK_MONOTONIC, 0, &req, &rem);

		ev.time.tv_sec  = 0;
		ev.time.tv_usec = 0;
		write(evdev, &ev, sizeof(ev));
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

	if ((fd = open(argv[1], O_WRONLY)) < 0) {
		perror("can't open event device");
		return EXIT_FAILURE;
	}

	play_events(STDIN_FILENO, fd);

	close(fd);
	return EXIT_SUCCESS;
}
