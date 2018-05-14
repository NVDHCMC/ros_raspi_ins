#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#define XDDP_PORT 0 

int main(int argc, char const *argv[])
{
    char buf[128], *devname;
    int fd, ret;

    if (asprintf(&devname, "/dev/rtp%d", XDDP_PORT) < 0)
            perror("asprintf");

    fd = open(devname, O_RDWR);
    free(devname);

    if (fd < 0)
            perror("open");

    for (;;) {
        /* Get the next message from realtime_thread. */
        ret = read(fd, buf, sizeof(buf));
        if (ret <= 0) {
            perror("read");
        }
        /* Echo the message back to realtime_thread. */
        sleep(0.5);
    }

	return 0;
}