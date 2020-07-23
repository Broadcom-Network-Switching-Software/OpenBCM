/*! \file netserve.c
 *
 * netserve: proxy server for stdio.
 *
 * This daemon connects a network socket to stdio of a local
 * application through a pseudo TTY, thus enabling access to the
 * application from a telnet client.
 *
 * Terminating the telnet session does not terminate the application,
 * i.e. it is possible to connect several times to the same instance
 * of the application.
 *
 * Example launching sdklt on port 5611:
 *
 *   netserve -d 5611 sdklt
 *
 * Now connect to sdklt, e.g. like this:
 *
 *   telnet 127.0.0.1 5611
 *
 * When using non-interactive mode, the I/O performance can be
 * significantly enhanced by using direct network socket I/O instead
 * of going through a pseudo-TTY. This mode is recommended for any
 * kind of automated scripting.
 *
 * Example launching sdklt on port 5611 using direct socket I/O:
 *
 *   netserve -d -n 5611 sdklt
 *
 * Now connect to sdklt when using direct socket I/O, e.g. like this:
 *
 *   ncat 127.0.0.1 5611
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>

#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <pty.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

/* Telnet definitions */
#define T_CMD   255
#define T_DONT  254
#define T_DO    253
#define T_WONT  252
#define T_WILL  251
#define T_ECHO  1
#define T_SGA   3

static unsigned char will_sga[]  = { T_CMD, T_WILL, T_SGA  };
static unsigned char will_echo[] = { T_CMD, T_WILL, T_ECHO };
static unsigned char dont_echo[] = { T_CMD, T_DONT, T_ECHO };

/* Network server */
static volatile int _netfd = -1;
static int _server_socket;

static int
_setup_socket(int port)
{
    struct sockaddr_in addr;
    int i;
    int sockfd;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("server: can't open stream socket");
        exit(1);
    }

    /*
     * Set socket option to reuse local address. This is supposed
     * to have the effect of freeing up the local address.
     */
    i = 1;
    if (setsockopt(sockfd, SOL_SOCKET,
                   SO_REUSEADDR, (char *) &i, 4) < 0) {
        perror("setsockopt(SO_REUSEADDR)");
    }

    /*
     * Disable Nagle's algorithm to avoid the performance being impacted by
     * delayed-ACK.
     */
    i = 1;
    if (setsockopt(sockfd, SOL_TCP,
                   TCP_NODELAY, (char *)&i, sizeof(i)) < 0) {
        perror("setsockopt(TCP_NODELAY)");
    }

    /* Set up server address */
    memset((void *) &addr,0x0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    /* Bind our local address and port */
    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("server: can't bind local address");
        exit(1);
    }

    /* Only process one connection at a time */
    listen(sockfd, 1);

    return sockfd;
}

static int
_wait_for_connection(int sockfd)
{
    struct sockaddr_in addr;
    socklen_t len;
    int newsockfd;

    len = sizeof(addr);
    if ((newsockfd = accept(sockfd, (struct sockaddr *) &addr, &len)) < 0) {
        perror("accept");
        exit(1);
    }

    /* Telnet response required for correct interaction with app */
    write(newsockfd, will_sga, sizeof(will_sga));
    write(newsockfd, will_echo, sizeof(will_echo));
    write(newsockfd, dont_echo, sizeof(dont_echo));

    return newsockfd;
}

static void
_do_telnet(int ifd, int ofd)
{
    unsigned char cmd[8];
    int i;

    /* Read command */
    for (i = 1; i < 3; i++) {
        if (read(ifd, &cmd[i], 1) != 1) {
            return;
        }
    }

    /* Select negative response */
    switch (cmd[1]) {
    case T_WILL:
    case T_WONT:
        cmd[1] = T_DONT;
        break;
    case T_DO:
    case T_DONT:
        cmd[1] = T_WONT;
        break;
    default:
        /* Ignore */
        return;
    }

    /* Ignore responses to our own commands */
    switch (cmd[2]) {
    case T_ECHO:
    case T_SGA:
        return;
    }

    /* Send response */
    cmd[0] = T_CMD;
    write(ofd, &cmd, 3);
}

static void *
_net2tty(void *arg)
{
    int fd = *((int *)arg);
    unsigned char data;

    while (1) {
        while (_netfd < 0) {
            usleep(100000);
        }
        if (read(_netfd, &data, 1) != 1) {
            /* Broken pipe -- client quit */
            close(_netfd);
            _netfd = -1;
            raise(SIGUSR2);
        } else if (data == T_CMD) {
            _do_telnet(_netfd, fd);
        } else {
            write(fd, &data, 1);
            fsync(fd);
        }
    }
    return NULL;
}

static void *
_tty2net(void *arg)
{
    int fd = *((int *)arg);
    ssize_t rdsize;
    unsigned char data[1024];

    while (1) {
        /* Provide the buffer to read/write data in blocks. */
        rdsize = read(fd, data, sizeof(data));
        if (rdsize <= 0) {
            /* Broken pipe -- app quit */
            close(fd);
            break;
        }
        if (_netfd >= 0) {
            write(_netfd, data, rdsize);
            fsync(_netfd);
        }
    }
    return NULL;
}

static int
_start_app(char **args, int s)
{
    pid_t pid;

    if ((pid = fork()) < 0) {
        perror ("fork");
        return -1;
    }
    if (pid > 0) {
        return pid;
    }
    dup2(s, 0);
    dup2(s, 1);
    dup2(s, 2);
    execv(*args, args);
    exit(0);
}

static void
_restart(void)
{
    int s;

    if ((s = _wait_for_connection(_server_socket)) < 0) {
        fprintf(stderr, "connection error\n");
        exit(0);
    }
    /* Reenable the proxies with the new fd */
    _netfd = s;
}

static void
_sig(int sig)
{
    if (sig == SIGUSR2) {
        _restart();
    }
}

int
main(int argc, char *argv[])
{
    int p;
    int pid;
    int do_fork = 0;
    int direct_netio = 0;
    int ttyfd, appfd;
    pthread_t id;

    argv++;

    if (!*argv || !strcmp(*argv, "--help") || !strcmp(*argv, "-h")) {
        printf("usage: netserve [-d] [-n] <port> <program> [args]\n");
        printf("\t-d: Run the application as a daemon.\n");
        printf("\t-n: Use socket descriptor as program's stdio instead of\n");
        printf("\t    a pseudo TTY. This mode is recommended for scripted\n");
        printf("\t    environments such as ncat.\n");
        exit(0);
    }

    do {
        if (!strcmp(*argv, "-d")) {
            do_fork = 1;
            argv++;
        } else if (!strcmp(*argv, "-n")) {
            direct_netio = 1;
            argv++;
        } else {
            break;
        }
    } while (1);

    if ((p = atoi(*argv++)) == 0) {
        fprintf(stderr, "bad port number\n");
        exit(0);
    }

    if (do_fork) {
        /* Daemonize */
        pid = fork();
        if (pid < 0) {
            perror("fork()");
            exit(0);
        }
        if (pid > 0) {
            exit(0);
        }
        setsid();
    }

    /* Broken pipes are not a problem */
    signal(SIGPIPE, SIG_IGN);

    if (!direct_netio) {
        /* Get a pseudo tty */
        if (openpty(&ttyfd, &appfd, NULL, NULL, NULL) < 0) {
            perror("open pty");
            exit(0);
        }

        /* Start the application up with sv[1] as its stdio */
        pid = _start_app(argv, appfd);

        /* Start proxy for input */
        if (pthread_create(&id, NULL, _net2tty, (void *)&ttyfd) < 0) {
            perror("pthread_create");
            exit(0);
        }

        /* Start proxy for output */
        if (pthread_create(&id, NULL, _tty2net, (void *)&ttyfd) < 0) {
            perror("pthread_create");
            exit(0);
        }
    }

    /* Setup server */
    _server_socket = _setup_socket(p);

    /* SIGUSR2 restarts the server when the client connection closes */
    signal(SIGUSR2, _sig);

    /* Start the first server */
    raise(SIGUSR2);

    if (direct_netio) {
        /* Start the application up with socket descriptor as its stdio */
        pid = _start_app(argv, _netfd);
    }

    /* Wait for our child to exit */
    waitpid(pid, &p, 0);

    return 0;
}
