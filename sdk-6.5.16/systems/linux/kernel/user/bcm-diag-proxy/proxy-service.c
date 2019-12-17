/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <sal/core/thread.h>

#include "proxy-service.h"

/*
 * Function: _proxy_thread
 *
 * Purpose:
 *    Implements the proxy data loop
 * Parameters:
 *    ctrl - proxy control structure
 * Returns:
 *    Nothing
 */
static void
_proxy_thread(proxy_ctrl_t* ctrl)
{
    unsigned char* data = malloc(ctrl->max_data_size); 
    assert(data); 
    assert(ctrl->input_cb); 

    for (;;) {
	unsigned int len = ctrl->max_data_size; 
	memset(data, 0, len);

	/* Receive packets from the given input callback */
	if (ctrl->input_cb(ctrl, data, &len) >= 0) {
	    /* Send it to the output callback */
	    if (ctrl->output_cb) {
		ctrl->output_cb(ctrl, data, &len); 
	    }
	}
	
	if (ctrl->exit) {
            if (ctrl->exit_cb) {
		ctrl->exit_cb(ctrl, NULL, 0); 
            }
            free(data);
	    ctrl->exit = 0; 
	    return; 
	}
    }
}

/*
 * Function: proxy_service_start
 *
 * Purpose:
 *    Start a proxy thread/loop
 * Parameters:
 *    ctrl - proxy control structure
 *    fork - indicates whether a new thread should be created. 
 *       TRUE: run loop in a new thread.
 *       FALSE: Run loop (blocking)
 * Returns:
 *    Nothing
 */
int 
proxy_service_start(proxy_ctrl_t* ctrl, int fork)
{
    if (fork) {
	sal_thread_create("_proxy_thread", 0, 0, 
			  (void (*)(void*))_proxy_thread, ctrl); 
    } else {
	_proxy_thread(ctrl); 
    }
    return 0; 
}
