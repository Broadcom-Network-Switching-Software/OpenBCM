/***********************************************************************
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * This provides a simple implementation of a BCM C2C/NH transport driver. 
 * This driver uses a User/Kernel Proxy service to transmit and receive 
 * the data. 
 * This driver is compiled for both the kernel and user space. 
 */

#ifndef __KERNEL__
#include <stdlib.h>
#endif
#include <assert.h>

#include <sal/core/alloc.h>
#include <sal/core/thread.h>

#include <bcm/error.h>
#include <soc/cm.h>

#include <linux-uk-proxy.h>
#include <bcm-uk-trans.h>

/* 
 * This is the name of the proxy service this module will use 
 */
static char _input_proxy_service[] = "BCM UK PROXY TRANSPORT"; 
static char _output_proxy_service[] = "BCM UK PROXY TRANSPORT"; 


/* This is the stacking port number assigned to our transport */
static int _port = 0; 

/*
 * Simple Handler registration (emphasis on simple). 
 */
#define MAX_CALLBACKS 5
static struct callback_ctrl_s {
    bcm_rx_cb_f cb; 
    void* cookie; 
} _callback_ctrl[MAX_CALLBACKS]; 

/*
 * Function:
 *	_rx_reg
 * Purpose:
 *	Provides the transport's rx register function. Veru minimal. 
 */
static int
_rx_reg(int unit, const char* name, bcm_rx_cb_f callback, uint8 priority, 
	void* cookie, uint32 flags)
{
    int i; 
    for(i = 0; i < MAX_CALLBACKS; i++) {
	if(_callback_ctrl[i].cb == NULL) {
	    _callback_ctrl[i].cb = callback; 
	    _callback_ctrl[i].cookie = cookie; 
	    return 0; 
	}
    }
    return -1; 
}

/*
 * Function:
 *	_rx_unreg
 * Purpose:
 *	Provides the transport's rx unregister function. Very minimal. 
 */
static int
_rx_unreg(int unit, bcm_rx_cb_f callback, uint8 priority)
{
    int i; 
    for(i = 0; i < MAX_CALLBACKS; i++) {
	if(_callback_ctrl[i].cb == callback) {
	    _callback_ctrl[i].cb = NULL; 
	    _callback_ctrl[i].cookie = NULL; 
	}
    }
    return 0; 
}


/*
 * Function:
 *	_rx_handle
 * Purpose:
 *	Processes a received frame for the transport
 */
static int
_rx_handle(unsigned char* data, int len)
{
    int i; 
    bcm_pkt_t pkt;
    memset(&pkt, 0, sizeof(pkt)); 
    
    /* Create a bcm_pkt_t for the packet */
    pkt.pkt_data = &pkt._pkt_data; 
    
    pkt.pkt_data->data = data; 
    pkt.pkt_data->len = len; 
    pkt.pkt_len = len; 

    pkt.blk_count = 1; 
    pkt.unit = 0; 

    /* Assign our port as the ingress */
    pkt.rx_port = _port; 
    pkt.rx_unit = 0; 

    /* Call everyone. This does not implement the priority scheme. */
    for(i = 0; i < MAX_CALLBACKS; i++) {
	if(_callback_ctrl[i].cb) {
	    int rc = _callback_ctrl[i].cb(0, &pkt, _callback_ctrl[i].cookie); 
	    switch(rc)
		{
		case BCM_RX_HANDLED:		    
		    {
			/* We should try to reuse this */
			bcm_rx_free(0, data); 
			return 0; 
		    }
		case BCM_RX_HANDLED_OWNED:
		    {	
			return 0; 
		    }
		case BCM_RX_NOT_HANDLED:
		    {
			/* Next handler */
			break;
		    }
		default:
		    {
			assert(0); 
			break;
		    }
		}
	}
    }
    bcm_rx_free(0, data); 
    return 0; 
}


/*
 * Function:
 *	_rx_thread
 * Purpose:
 *	Receives and dispatches packets from the kernel proxy service. 
 */

typedef struct rx_thread_ctrl_s {
    const char* service; 
    int exit; 
} rx_thread_ctrl_t; 

static void
_rx_thread(rx_thread_ctrl_t* ctrl)
{
    unsigned char* data = NULL; 

    for(;;) {
	unsigned int len = LUK_MAX_DATA_SIZE; 
	if(!data) {
	    /* Transport client expects data to be allocated from the rx buffer pool */
	    bcm_rx_alloc(0, LUK_MAX_DATA_SIZE, 0, (void*)&data); 
	    assert(data); 
	}
	memset(data, 0, LUK_MAX_DATA_SIZE);

	if(linux_uk_proxy_recv(ctrl->service, data, &len) >= 0) {
	    _rx_handle(data, len); 
	    
	    data = NULL; 
	}
	
	if(ctrl->exit) {
	    ctrl->exit = 0; 
	    return; 
	}
    }
}
	
	
/*
 * Function:
 *	_setup_tx
 * Purpose:
 *	setup_tx transport vector
 */
static int
_setup_tx(int unit, bcm_pkt_t* tx_pkt)
{
    /* Nothing needed here */
    return BCM_E_NONE; 
}

/*
 * Function:
 *	_tx
 * Purpose:
 *	tx transport vector
 */
static int
_tx(int unit, bcm_pkt_t* pkt, void* cookie)
{    
    int i; 
    unsigned char data[LUK_MAX_DATA_SIZE]; 
    unsigned char* p = data; 
    int len = 0; 

    /* Collect data blocks into one buffer for transport */
    for(i = 0; i < pkt->blk_count; i++) {
	memcpy(p, pkt->pkt_data[i].data, pkt->pkt_data[i].len); 
	p+=pkt->pkt_data[i].len; 
	len += pkt->pkt_data[i].len;	
    }
    
    len += 4; /* CRC_APPEND -- check flag */

    /* Send it */    
    linux_uk_proxy_send(_output_proxy_service, data, len+4); 
    
    if(pkt->call_back) {
	pkt->call_back(unit, pkt, cookie); 
    }

    return 0; 
}	

/*
 * Function:
 *	_tx_list
 * Purpose:
 *	tx_list transport vector
 */
static int
_tx_list(int unit, bcm_pkt_t* pkt, bcm_pkt_cb_f all_done_cb, void* cookie)
{
    bcm_pkt_t* p; 
    
    /* Just tx each one */
    for(p = pkt; p; p = p->next) {
	_tx(unit, p, cookie); 
    }

    if(all_done_cb) {
	all_done_cb(unit, pkt, cookie); 
    }

    return 0; 
}


/*
 * This is our transport structure
 */
static bcm_trans_ptr_t _trans_ptr = {
    bcm_rx_alloc,       /* All transports need to use bcm_rx_alloc() */
    bcm_rx_free,        /* and bcm_rx_free() at the moment */
    bcm_pkt_rx_alloc,
    bcm_pkt_rx_free,
    _rx_reg,
    _rx_unreg, 
    _setup_tx, 
    _tx,
    _tx_list,
    NULL, 
    NULL, 
    0                   /* Default unit for alloc/free */
};


/* RX thread control */
static rx_thread_ctrl_t _ctrl; 

int 
bcm_uk_trans_create(const char* service, int port, bcm_trans_ptr_t** trans)
{   
    if(service) {
	strcpy(_input_proxy_service, service); 
	strcpy(_output_proxy_service, service); 
    }

#ifndef __KERNEL__
    /* In user mode we need to open the proxy driver first */ 
    linux_uk_proxy_open(); 

    /* Special overrides */
    if(getenv("INPUT_PROXY_SERVICE")) {
	strcpy(_input_proxy_service, getenv("INPUT_PROXY_SERVICE")); 
    }
    if(getenv("OUTPUT_PROXY_SERVICE")) {
	strcpy(_output_proxy_service, getenv("OUTPUT_PROXY_SERVICE")); 
    }

#endif /* __KERNEL__ */


    linux_uk_proxy_service_create(_input_proxy_service, 64, 0); 

    _ctrl.service = _input_proxy_service; 
    _ctrl.exit = 0; 
    _port = 1; 

    sal_thread_create("BCM UK TR", 0, 0, (void (*)(void*))_rx_thread, &_ctrl); 

    *trans = &_trans_ptr; 
    return 0; 
}
