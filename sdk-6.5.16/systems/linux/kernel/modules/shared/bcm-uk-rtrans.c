/***********************************************************************
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * This provides a simple implementation of a BCM C2C/NH transport driver. 
 * This driver uses a User/Kernel Proxy service to transmit and receive the
 * data. 
 * This driver is compiled for both the kernel and user space. 
 */

#include <shared/bsl.h>

#include <sal/core/alloc.h>
#include <sal/core/thread.h>

#include <bcm/error.h>

#include <soc/cm.h>

#include <bcm/tx.h>
#include <bcm/rx.h>
#include <assert.h>

#include <linux-uk-proxy.h>
#include <bcm-uk-trans.h>

/* 
 * This is the name of the proxy service this module will use 
 */
static char _input_proxy_service[] = "BCM UK PROXY REMOTE TRANSPORT"; 
static char _output_proxy_service[] = "BCM UK PROXY REMOTE TRANSPORT"; 


/* 
 * Receives packets from the HW, sends them to user space 
 */
static bcm_rx_t
_bcm_rx_cb(int unit, bcm_pkt_t* pkt, void* cookie)
{
    /* User space expects the bcm_pkt_t before the data */
    unsigned char data[LUK_MAX_DATA_SIZE]; 
    unsigned char* p = data; 
    
    /* The bcm_pkt_t structure is expected at the start of the data */
    /* The pointers will get fixed up in user space */
    memcpy(p, pkt, sizeof(*pkt)); 
    p+=sizeof(*pkt);     
    pkt->pkt_data->len = pkt->pkt_len; 
    memcpy(p, pkt->pkt_data->data, pkt->pkt_data->len); 
    
    /* Send it */    
    linux_uk_proxy_send(_output_proxy_service, data, pkt->pkt_data->len+sizeof(*pkt)+4); 
    /* Other people might want this at this point */
    return BCM_RX_NOT_HANDLED; 
}

/*
 * Function:
 *	_rx_handle
 * Purpose:
 *	Processes a received frame for the transport
 */
static int
_handle_user_tx(unsigned char* data, int len)
{
    bcm_pkt_t* pkt;
    unsigned char* p = data; 
    int l = len; 
    /* We received a packet from user space. Need to tx it */
    /* pkt structure is at the beginning of the packet */
    pkt = (bcm_pkt_t*)p; 
    p += sizeof(bcm_pkt_t); 
    l -= sizeof(bcm_pkt_t); 

    /* Set up the data block */
    pkt->pkt_data = &pkt->_pkt_data; 
    pkt->pkt_data->data = p; 
    pkt->pkt_data->len = l;     
    pkt->blk_count = 1; 
    
    bcm_tx(pkt->unit, pkt, NULL);     
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
} thread_ctrl_t; 

static void
_user_tx_thread(thread_ctrl_t* ctrl)
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
	    _handle_user_tx(data, len); 
	    
	    data = NULL; 
	}
	
	if(ctrl->exit) {
	    ctrl->exit = 0; 
	    return; 
	}
    }
}
	
/* RX thread control */
static thread_ctrl_t _ctrl; 

int 
bcm_uk_rtrans_create(const char* service, bcm_trans_ptr_t** trans)
{   
    if(service) {
	strcpy(_input_proxy_service, service); 
	strcpy(_output_proxy_service, service); 
    }

    linux_uk_proxy_service_create(_input_proxy_service, 64, 0); 

    _ctrl.service = _input_proxy_service; 
    _ctrl.exit = 0; 

    sal_thread_create("BCM UK RT", 0, 0, (void (*)(void*))_user_tx_thread, &_ctrl); 

    /* Register an rx handler to send packets back to user */
    {	
	int rc; 
	if((rc = bcm_rx_register(0, "", _bcm_rx_cb, 250, NULL, BCM_RCO_F_ALL_COS)) < 0) {
	    LOG_CLI((BSL_META("register failed: %d (%s)\n"), rc, bcm_errmsg(rc))); 
	}
	else {
	    LOG_CLI((BSL_META("registered callback\n"))); 
	}
	if((rc = bcm_rx_start(0, NULL)) < 0) {
	    LOG_CLI((BSL_META("start failed: %d (%s)\n"), rc, bcm_errmsg(rc))); 
	}
	else {
	    LOG_CLI((BSL_META("rx started\n"))); 
	}
    }

    *trans = NULL; 
    return 0; 
}
    
	 
       














