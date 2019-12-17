/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>

#include <sal/core/thread.h>
#include <sal/appl/config.h>
#include <linux-uk-proxy.h>

#include "proxy-service.h"
#include "socintf.h"

/*
 * This is the proxy service the kernel diag shell uses. 
 */
static char _input_proxy_service[64]    = "BCM DIAG SHELL";
static char _output_proxy_service[64]   = "BCM DIAG SHELL";

#define OUTPUT_LINE(_ctrl, _buf) \
do { \
    if (_ctrl->ctrl.output_cb) { \
        unsigned int _len = strlen(_buf)+1; \
        _ctrl->ctrl.output_cb((proxy_ctrl_t*)_ctrl, (unsigned char*)_buf, &_len); \
    } \
} while (0)

#define COPY_AND_OUTPUT_CMD(_ctrl, _buf, _cmd) \
do { \
    strcpy(_buf, _cmd); \
    strcat(_buf, "\n"); \
    OUTPUT_LINE(_ctrl, _buf); \
} while (0)

#define LINE_BUF_SIZE 256

#ifdef INCLUDE_EDITLINE
extern char *readline(const char *prompt);
extern void add_history(char *p);
#endif
static int use_editline = 0;

static char *progname;

/*
 * Proxy control structure passed to all of our callbacks. 
 */
typedef struct proxy_ctrl_t {
    proxy_ctrl_t ctrl;
    
    const char *service;        /* Service pointer for kernel proxy callbacks */

    int output_fd;              /* File descriptor for the output callback */
    int input_fd;               /* File descriptor for the input callback */

    proxy_ctrl_t *sibling;      /* Other proxies which should stop when this one does */
} our_proxy_ctrl_t;
    
    
/*
 * Function: skip_whitespace
 *
 * Purpose:
 *      Skip over leading whitespace in string
 * Parameters:
 *      s - string to parse
 * Returns:
 *      pointer to first non-whitespace character in input string
 */ 
static char *
skip_whitespace(char *s)
{ 
    /* Skip leading whitespace */
    while (*s && *s <= ' ') {
        s++;
    }
    return s;
}

/*
 * Function: load_config
 *
 * Purpose:
 *    Load BCM configuration file
 * Parameters:
 *    ctrl - proxy service control structure
 *    fname - name of configuration file
 * Returns:
 *    0 - configuration file found and loaded
 *    -1 - configuration file not found
 */ 
static int
load_config(our_proxy_ctrl_t *ctrl, 
            char *fname)
{
    char cmd[64];
    char line[LINE_BUF_SIZE];
    FILE *fp;
    int offs;;

    fp = fopen(fname, "r");
    if (!fp) {
        printf("cannot open '%s'\n", fname);
        return -1;
    }
    COPY_AND_OUTPUT_CMD(ctrl, cmd, "local proxy_cmd_ enter_batchmode");
    line[LINE_BUF_SIZE-1] = 0;
    strcpy(line, "config add ");
    offs = strlen(line);
    while (fgets(&line[offs], LINE_BUF_SIZE-1-offs, fp)) {
        if (line[offs] != '#' && strchr(&line[offs], '=') != NULL) {
            OUTPUT_LINE(ctrl, line);
        }
    }
    COPY_AND_OUTPUT_CMD(ctrl, cmd, "local proxy_cmd_ exit_batchmode");
    fclose(fp);
    return 0;
}

/*
 * Function: run_script
 *
 * Purpose:
 *    Run soc script by passing file content through proxy
 * Parameters:
 *    ctrl - proxy service control structure
 *    script - name of script file to run
 * Returns:
 *    0 - script found and executed
 *    -1 - script not found
 */ 
static int
run_script(our_proxy_ctrl_t *ctrl, 
           char *script,
           char *init_script,
           int recursion_level)
{
    char line[LINE_BUF_SIZE];
    char cmd[64];
    char *s;
    FILE *fp;
    int cont = 0;
    int reinit = 0;
    int offset = 0;

    if (++recursion_level > 4) {
        printf("recursion too deep to run script '%s'\n", script);
        return -1;
    }
    fp = fopen(script, "r");
    if (!fp) {
	if(strchr(script, '$')) {
	    sprintf(line, "rcload %s", script); 
	    COPY_AND_OUTPUT_CMD(ctrl, cmd, line);
	    sal_usleep(500000); 
	    return 0; 
	}
	else {
	    printf("cannot open script '%s'\n", script);
	    return -1;
	}
    }
    if (recursion_level == 1) {
        COPY_AND_OUTPUT_CMD(ctrl, cmd, "local proxy_cmd_ enter_batchmode");
    }
    COPY_AND_OUTPUT_CMD(ctrl, cmd, "local proxy_cmd_ push_scope");
    line[LINE_BUF_SIZE-1] = 0;
    while (fgets(line, LINE_BUF_SIZE-1, fp)) {
        s = skip_whitespace(line);
        if (!strncmp(line, "rcload ", 7)) {
            if ((s = strchr(line, '\n')) != NULL) {
                *s = 0;
            }
            s = skip_whitespace(line+7);
            run_script(ctrl, s, init_script, recursion_level);
        }
        if (init_script && 
            (!strncmp(s, "tr ", 3) || !strncmp(s, "testrun ", 8))) {
            /* 
             * Call init script after each testrun. Ideally we would
             * run this script *before* the testrun command, but we may
             * be in the middle of a multi-line command.
             */
            reinit = 1;
            /*
             * Exceptions to speed up sanity tests
             * Skip rc.soc for a few tests that don't have TRC flag set
             * to prevent linkscan task from getting enabled. 
             */
            offset = 0;
            if (!strncmp(s, "tr ", 3)) {
                offset = 3;
            } else if (!strncmp(s, "testrun ", 8)) {
                offset = 8;
            }
            if (((s[offset] >= '1') && (s[offset] <= '8') &&
                 ((s[offset + 1] < '0') || (s[offset + 1] > '9'))) ||
                !strncmp(&s[offset], "15", 2) ||
                !strncmp(&s[offset], "16", 2) ||
                !strncmp(&s[offset], "21", 2) ||
                !strncmp(&s[offset], "30", 2) ||
                !strncmp(&s[offset], "31", 2) ||
                !strncmp(&s[offset], "32", 2) ||
                !strncmp(&s[offset], "33", 2) ||
                !strncmp(&s[offset], "34", 2) ||
                !strncmp(&s[offset], "35", 2) ||
                !strncmp(&s[offset], "36", 2) ||
                !strncmp(&s[offset], "37", 2) ||
                !strncmp(&s[offset], "38", 2) ||
                !strncmp(&s[offset], "50", 2) ||
                !strncmp(&s[offset], "51", 2) ||
                !strncmp(&s[offset], "52", 2) ||
                !strncmp(&s[offset], "71", 2) ||
                !strncmp(&s[offset], "85", 2) ||
                !strncmp(&s[offset], "86", 2) ||
                !strncmp(&s[offset], "87", 2) ||
                !strncmp(&s[offset], "88", 2)
                ) {
                reinit = 0;
            }
        }
        if (*s != '#' && strncmp(s, "rccache", 7) != 0) {
            /* Send the line directly to output callback */
            OUTPUT_LINE(ctrl, line);
        }
        /* Check for line continuation */
        cont = (strstr(line, "\\\n")) ? 1 : 0;
        if (reinit && !cont) {
            /* 
             * Run init script as soon as possible after completing
             * the testrun command. Since command line concatenation
             * is handled remotely, we need to wait until we have sent
             * a complete command line.
             */
            COPY_AND_OUTPUT_CMD(ctrl, cmd, "local proxy_cmd_ push_scope");
            COPY_AND_OUTPUT_CMD(ctrl, cmd, "set rctest=0");
            run_script(ctrl, init_script, NULL, recursion_level);
            COPY_AND_OUTPUT_CMD(ctrl, cmd, "local proxy_cmd_ pop_scope");
            reinit = 0;
        }
    }
    COPY_AND_OUTPUT_CMD(ctrl, cmd, "local proxy_cmd_ pop_scope");
    if (recursion_level == 1) {
        COPY_AND_OUTPUT_CMD(ctrl, cmd, "local proxy_cmd_ exit_batchmode");
    }
    fclose(fp);
    return 0;
}

/*
 * Function: _fd_input_cb
 *
 * Purpose:
 *    Proxy input function from a file descriptor
 * Parameters:
 *    ctrl - Proxy service control structure
 *    data - (OUT) buffer to receive the data
 *    len - (OUT) receives the data length
 * Returns:
 *    0 - data is ready to send.
 *    -1 - do not send data any data.
 * Notes:
 *    It reads commands from the descriptor.
 *    The following commands are interpreted locally:
 *
 *    quit - Quit the proxy program.
 *       Any data AFTER the quit is sent to the remote. Thus,
 *       "quit exit" will quit the local, and send "exit" to 
 *       the remote. "quit" by itself will quit the local, and 
 *       send nothing to the remote. This functionality prevents 
 *       the user from accidentally exiting the diag shell at 
 *       the remote end.
 *
 *    exit - Quit the proxy program.
 *       Same functionality as the quit command.
 *
 *    rcload <file>
 *       Lines in file <file> will be sent to the remote end.
 *       This provides a rudimentary approximation of the real
 *       diag shell 'rcload' function. 
 */


/*
 * Sometimes an input line (an instruction from the kernel shell) 
 * needs to be injected into the input read buffer. 
 */
static char _proxy_input_line[64]; 

static int
_fd_input_cb(our_proxy_ctrl_t *ctrl, 
             unsigned char *udata, 
             unsigned int *len)
{
    char *config_file;
    char *init_script;
    char *s = NULL;
    char *data = (char *)udata;
    FILE *fp;
    int input_fd;
    
    /* Check for locally specified input first */
    if(_proxy_input_line[0]) {
	strcpy(data, _proxy_input_line); 
	_proxy_input_line[0] = 0;
    } else {
	/* Retrieve input from the file descriptor */
#ifdef INCLUDE_EDITLINE
    if (use_editline) {
	    s = readline("");
        if (s) {
            add_history(s);
            strncpy(data, s, *len-1);
            data[*len-1] = 0;
            strcat(data, "\n");
        } else {
            strcpy(data, "quit\n");
        }
    }
#endif
	if (!use_editline) {
        /*
         * Duplicate file descriptor when using fdopen() to
         * prevent underlying socket from being closed by
         * fclose().
         */
        input_fd = dup(ctrl->input_fd);
        if (input_fd < 0) {
            printf("failed to dup input_fd %d\n", ctrl->input_fd);
            return -1;
        }

        /* Get a line from the descriptor */
        fp = fdopen(input_fd, "r");
	    assert(fp);
	    if (fgets(data, *len, fp) == NULL) {
		/* Broken pipe. Lets quit */
            strcpy(data, "quit\n");
	    }
	    /* Strip CR characters (e.g. from telnet) */
	    if ((s = strchr(data, '\r')) != NULL) {
		*s++ = '\n';
		*s = 0;
	    }
	    /* Support backspace */
	    while ((s = strchr(data, 0x08)) != NULL) {
		if (s == (char *)data) {
		    memcpy(s, &s[1], strlen(s));
		} else {
		    memcpy(&s[-1], &s[1], strlen(s));
		}
	    }
            fclose(fp);
	}
    }

    /* Special commands? */
    if (!strncmp(data, "quit", 4) || !strncmp(data, "exit", 4)) {
        /* Tell our service thread to exit */
        /* This will happen AFTER the data result from this function is sent. */
        ctrl->ctrl.exit = 1;
	    return -1;
    }
    
    if (!strncmp(data, "rcload ", 7)) {
        /* Read lines from the given file */
        if ((s = strchr(data, '\n')) != NULL) {
            *s = 0;
        }
        s = skip_whitespace(data+7);
        init_script = "rc.soc";
        if (!strncmp(s, "-i", 2)) {
            /* 
             * For backwards compatibility we need to silently
             * ignore the -i switch, which used to be required
             * for rc.soc to be executed in between testruns.
             */
            s = skip_whitespace(s+2);
        } else if (!strncmp(s, "-i-", 3)) {
            /*
             * Turn off init script feature (normally not used).
             */
            init_script = NULL;
            s = skip_whitespace(s+3);
        }
        run_script(ctrl, s, init_script, 0);
        return -1;
    }
    
    if (!strncmp(data, "config refresh", 14)) {
        /* Read config.bcm */
        if ((config_file = getenv("BCM_CONFIG_FILE")) == NULL) {
            config_file = SAL_CONFIG_FILE;
        }
        load_config(ctrl, config_file);
        return -1;
    }
    
    *len = strlen(data) + 1;

    /* Send this buffer */
    return 0;
}

/*
 * Function: _fd_output_cb
 *
 * Purpose:
 *    Proxy output function to a file descriptor
 * Parameters:
 *    ctrl - proxy service control structure
 *    data - data which was read from the proxy
 *    len - length of the data. 
 * Returns:
 *    Always 0
 * Notes:
 *    This is an output callback for the proxy service. 
 */ 
static int
_fd_output_cb(our_proxy_ctrl_t *ctrl, 
              unsigned char *udata, 
              unsigned int *len)
{    
    char *data = (char *)udata;

    /* Do we have an instruction from the shell to execute? */
    if(!strncmp("proxyecho", data, 9)) {
	strcpy(_proxy_input_line, data+10); 
    }
    else {
	write(ctrl->output_fd, data, *len);
	fsync(ctrl->output_fd);
    }
    return 0;
}


/*
 * Function: _kernel_proxy_output_cb
 *
 * Purpose:
 *    Proxy output function. 
 * Parameters:
 *    ctrl - proxy control structure
 *    data - output data
 *    len - length of data
 * Returns:
 *    Always 0
 */
static int
_kernel_proxy_output_cb(our_proxy_ctrl_t *ctrl, 
                        unsigned char *data, 
                        unsigned int *len)
{
    /* This data should be sent to the given proxy service */
    return linux_uk_proxy_send(ctrl->service, data, *len);
}

/*
 * Function: _kernel_proxy_input_cb
 *
 * Purpose:
 *    Proxy input function
 * Parameters:
 *    ctrl - proxy control structure
 *    data - (OUT) proxy data
 *    len - (OUT) length of the data
 * Returns:
 *    0 - data available
 *    -1 - no data 
 */
static int
_kernel_proxy_input_cb(our_proxy_ctrl_t *ctrl, 
                       unsigned char *data, 
                       unsigned int *len)
{
    return linux_uk_proxy_recv(ctrl->service, data, len);
}


/*
 * Function: _kernel_proxy_input_exit_cb
 *
 * Purpose:
 *    To be called when input proxy exits
 * Parameters:
 *    ctrl - proxy control structure
 *    data - unused
 *    len - unused
 * Returns:
 *    0 - data available
 *    -1 - no data 
 */
static int
_kernel_proxy_input_exit_cb(our_proxy_ctrl_t *ctrl, 
                            unsigned char *data, 
                            unsigned int *len)
{
    /* Suspend service to prevent kernel threads from blocking */
    linux_uk_proxy_suspend(_input_proxy_service);
    return 0;
}

static void
_trigger_read_proxy_thread_exit(proxy_ctrl_t* ctrl)
{
    unsigned char data[] = {'\n', '\0'}; 
    unsigned int len = 1;

	if (ctrl->output_cb) {
		ctrl->output_cb(ctrl, data, &len); 
	}
}

/*
 * Function: _kernel_fd_attach
 *
 * Purpose:
 *    Attach I/O file descriptors to proxy service
 * Parameters:
 *    input_fd - file descriptor for input callback
 *    output_fd - file descriptor for output callback
 * Returns:
 *    0
 */
static int
_kernel_fd_attach(int input_fd, int output_fd)
{
    our_proxy_ctrl_t reader;
    our_proxy_ctrl_t writer;

    memset(&reader, 0, sizeof(reader));
    memset(&writer, 0, sizeof(writer));

    if (getenv("INPUT_PROXY")) {
        strcpy(_input_proxy_service, getenv("INPUT_PROXY"));
    }
    if (getenv("OUTPUT_PROXY")) {
        strcpy(_output_proxy_service, getenv("OUTPUT_PROXY"));
    }

    linux_uk_proxy_open();

    /* Create the service, if it is not yet available. */
    /* We configure a queue size of 1 -- we do not want the io buffered */
    linux_uk_proxy_service_create(_input_proxy_service, 1, 0);

    reader.ctrl.input_cb = (proxy_data_cb_t)_kernel_proxy_input_cb;
    reader.ctrl.output_cb = (proxy_data_cb_t)_fd_output_cb;
    reader.ctrl.exit_cb = (proxy_data_cb_t)_kernel_proxy_input_exit_cb;
    reader.ctrl.max_data_size = LUK_MAX_DATA_SIZE;
    reader.ctrl.exit = 0;
    
    reader.service = _input_proxy_service;
    reader.output_fd = output_fd;
    reader.sibling = NULL;
    
    /* Fork off the reader loop */
    proxy_service_start(&reader.ctrl, 1);

    /* Resume in case service was suspended */
    linux_uk_proxy_resume(_input_proxy_service);

    /* Create the service, if it is not yet available. */
    /* We configure a queue size of 1 -- we do not want the io buffered */
    linux_uk_proxy_service_create(_output_proxy_service, 1, 0);

    writer.ctrl.input_cb = (proxy_data_cb_t)_fd_input_cb;
    writer.ctrl.output_cb = (proxy_data_cb_t)_kernel_proxy_output_cb;
    writer.ctrl.exit_cb = NULL;
    writer.ctrl.max_data_size = LUK_MAX_DATA_SIZE;
    writer.ctrl.exit = 0;
    
    writer.service = _output_proxy_service;
    writer.input_fd = input_fd;
    writer.sibling = &reader.ctrl;
    
    /* Start the writer loop (don't fork) */
    proxy_service_start(&writer.ctrl, 0);
    /* Tell our sibling thread to exit as well */
    writer.sibling->exit = 2;

    /* Wait for sibling to exit */
    while (writer.sibling->exit) {
        sal_usleep(150000);
        if (writer.sibling->exit) {
             _trigger_read_proxy_thread_exit(&writer.ctrl);
        }
    }

    return 0;
}

static int
_fd_stdio_attach(int fd)
{
    our_proxy_ctrl_t reader;
    our_proxy_ctrl_t writer;

    memset(&reader, 0, sizeof(reader));
    memset(&writer, 0, sizeof(writer));

    reader.ctrl.input_cb = (proxy_data_cb_t)_fd_input_cb;
    reader.input_fd = fd;
    reader.ctrl.output_cb = (proxy_data_cb_t)_fd_output_cb;    
    reader.output_fd = 0;
    reader.ctrl.max_data_size = LUK_MAX_DATA_SIZE;
    reader.ctrl.exit = 0;    
    reader.sibling = NULL;
    reader.ctrl.exit_cb = NULL;

    /* Fork off the reader loop */
    proxy_service_start(&reader.ctrl, 1);
    
    writer.ctrl.input_cb = (proxy_data_cb_t)_fd_input_cb;
    writer.input_fd = 1;
    writer.ctrl.output_cb = (proxy_data_cb_t)_fd_output_cb;
    writer.output_fd = fd;
    writer.ctrl.max_data_size = LUK_MAX_DATA_SIZE;
    writer.ctrl.exit = 0;   
    writer.sibling = &reader.ctrl;
    writer.ctrl.exit_cb = NULL;

    /* Start the writer loop (don't fork) */
    proxy_service_start(&writer.ctrl, 0);
    writer.sibling->exit = 2;
    return 0;
}

static int
_daemon(int port)
{
    int server_s;
    int client_s;

    server_s = setup_socket(port);    

    for (;;) {
        printf("waiting for connections...\n");
        client_s = wait_for_cnxn(server_s);
        printf("client connected.\n");
        _kernel_fd_attach(client_s, client_s);
        close(client_s);
    }
    return 0;
}

static void 
usage(void)
{
    printf("Usage: %s [-d port]\n", progname);
    printf("       %s [-s hostname port]\n", progname);
#ifdef INCLUDE_EDITLINE
    printf("       %s [-n]\n", progname);
#endif
}

static void
ctrl_c_handler(int sig)
{
    /* Suspend service to prevent kernel threads from blocking */
    linux_uk_proxy_suspend(_input_proxy_service);

    exit(0);
}

int 
main(int argc, char *argv[])
{
    if ((progname = strrchr(argv[0], '/')) == NULL) {
        progname = argv[0];
    } else {
        progname++;
    }

    /* Suspend proxy on exit */
    signal(SIGINT, ctrl_c_handler);

    /* Broken pipes are not a problem */
    signal(SIGPIPE, SIG_IGN);

    if (argc == 1) {
        /* Attach on stdio */
#ifdef INCLUDE_EDITLINE
        use_editline = 1;
#else
        use_editline = 0;
#endif
        _kernel_fd_attach(0, 1);
#ifdef INCLUDE_EDITLINE
    } else if (!strcmp(argv[1], "-n") && argc == 2) {
        /* Attach on stdio */
        _kernel_fd_attach(0, 1);
#endif
    } else if (!strcmp(argv[1], "-d") && argc == 3) {
        /* Port to listen on */
        int port = atoi(argv[2]);
        _daemon(port);
    } else if (!strcmp(argv[1], "-s") && argc == 4) {
        /* Connect to server */
        char *s = argv[2];
        int p = atoi(argv[3]);
        int sockfd = conn(s, p);
        _fd_stdio_attach(sockfd);
    } else {
        usage();
    }
    return 0;
}
