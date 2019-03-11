#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <rtdm/ipc.h>

pthread_t rt, nrt;


#define XDDP_PORT 0     /* [0..CONFIG-XENO_OPT_PIPE_NRDEV - 1] */
static const char *msg[] = {
        "Surfing With The Alien",
        "Lords of Karma",
        "Banana Mango",
        "Psycho Monkey",
        "Luminous Flesh Giants",
        "Moroccan Sunset",
        "Satch Boogie",
        "Flying In A Blue Dream",
        "Ride",
        "Summer Song",
        "Speed Of Light",
        "Crystal Planet",
        "Raspberry Jam Delta-V",
        "Champagne?",
        "Clouds Race Across The Sky",
        "Engines Of Creation"
};
static void fail(const char *reason)
{
        perror(reason);
        exit(EXIT_FAILURE);
}
static void *realtime_thread(void *arg)
{
        struct sockaddr_ipc saddr;
        int ret, s, n = 0, len;
        struct timespec ts;
        size_t poolsz;
        char buf[128];
        /*
         * Get a datagram socket to bind to the RT endpoint. Each
         * endpoint is represented by a port number within the XDDP
         * protocol namespace.
         */
        s = socket(AF_RTIPC, SOCK_DGRAM, IPCPROTO_XDDP);
        if (s < 0) {
                perror("socket");
                exit(EXIT_FAILURE);
        }
        /*
         * Set a local 16k pool for the RT endpoint. Memory needed to
         * convey datagrams will be pulled from this pool, instead of
         * Xenomai's system pool.
         */
        poolsz = 16384; /* bytes */
        ret = setsockopt(s, SOL_XDDP, XDDP_POOLSZ,
                         &poolsz, sizeof(poolsz));
        if (ret)
                fail("setsockopt");
        /*
         * Bind the socket to the port, to setup a proxy to channel
         * traffic to/from the Linux domain.
         *
         * saddr.sipc_port specifies the port number to use.
         */
        memset(&saddr, 0, sizeof(saddr));
        saddr.sipc_family = AF_RTIPC;
        saddr.sipc_port = XDDP_PORT;
        ret = bind(s, (struct sockaddr *)&saddr, sizeof(saddr));
        if (ret)
                fail("bind");
        for (;;) {
                len = strlen(msg[n]);
                /*
                 * Send a datagram to the NRT endpoint via the proxy.
                 * We may pass a NULL destination address, since a
                 * bound socket is assigned a default destination
                 * address matching the binding address (unless
                 * connect(2) was issued before bind(2), in which case
                 * the former would prevail).
                 */
                ret = sendto(s, msg[n], len, 0, NULL, 0);
                if (ret != len)
                        fail("sendto");
                printf("%s: sent %d bytes, \"%.*s\"\n",
                       __FUNCTION__, ret, ret, msg[n]);
                /* Read back packets echoed by the regular thread */
                ret = recvfrom(s, buf, sizeof(buf), 0, NULL, 0);
                if (ret <= 0)
                        fail("recvfrom");
                printf("   => \"%.*s\" echoed by peer\n", ret, buf);
                n = (n + 1) % (sizeof(msg) / sizeof(msg[0]));
                /*
                 * We run in full real-time mode (i.e. primary mode),
                 * so we have to let the system breathe between two
                 * iterations.
                 */
                ts.tv_sec = 0;
                ts.tv_nsec = 500000000; /* 500 ms */
                clock_nanosleep(CLOCK_REALTIME, 0, &ts, NULL);
        }
        return NULL;
}
static void *regular_thread(void *arg)
{
        char buf[128], *devname;
        int fd, ret;
        if (asprintf(&devname, "/dev/rtp%d", XDDP_PORT) < 0)
                fail("asprintf");
        fd = open(devname, O_RDWR);
        free(devname);
        if (fd < 0)
                fail("open");
        for (;;) {
                /* Get the next message from realtime_thread. */
                ret = read(fd, buf, sizeof(buf));
                if (ret <= 0)
                        fail("read");
                /* Echo the message back to realtime_thread. */
                ret = write(fd, buf, ret);
                if (ret <= 0)
                        fail("write");
        }
        return NULL;
}
int main(int argc, char **argv)
{
        struct sched_param rtparam = { .sched_priority = 42 };
        pthread_attr_t rtattr, regattr;
        sigset_t set;
        int sig;
        sigemptyset(&set);
        sigaddset(&set, SIGINT);
        sigaddset(&set, SIGTERM);
        sigaddset(&set, SIGHUP);
        pthread_sigmask(SIG_BLOCK, &set, NULL);
        pthread_attr_init(&rtattr);
        pthread_attr_setdetachstate(&rtattr, PTHREAD_CREATE_JOINABLE);
        pthread_attr_setinheritsched(&rtattr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&rtattr, SCHED_FIFO);
        pthread_attr_setschedparam(&rtattr, &rtparam);
        errno = pthread_create(&rt, &rtattr, &realtime_thread, NULL);
        if (errno)
                fail("pthread_create");
        pthread_attr_init(&regattr);
        pthread_attr_setdetachstate(&regattr, PTHREAD_CREATE_JOINABLE);
        pthread_attr_setinheritsched(&regattr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&regattr, SCHED_OTHER);
        errno = pthread_create(&nrt, &regattr, &regular_thread, NULL);
        if (errno)
                fail("pthread_create");
        sigwait(&set, &sig);
        pthread_cancel(rt);
        pthread_cancel(nrt);
        pthread_join(rt, NULL);
        pthread_join(nrt, NULL);
        return 0;
}
