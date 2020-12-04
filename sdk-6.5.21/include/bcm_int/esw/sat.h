/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        sat.h
 * Purpose:     Function declarations for SAT bcm internal functions
 */

#ifndef _BCM_INT_SAT_H_
#define _BCM_INT_SAT_H_


/* SAT UP-MEP OLP header type/subtype */
#define BCM_SAT_UPMEP_OLP_HDR_COMPRESSED             0x000E

/* SAT Down-MEP OLP header type/subtype */
#define BCM_SAT_DOWNMEP_OLP_HDR_COMPRESSED           0x000E


struct _bcm_sat_hash_data_s {
    int             in_use;           /* Endpoint usage status.  */
    uint16          sglp;             /* Source GLP port */              
    uint16          dglp;             /* Destination GLP port */
	bcm_sat_endpoint_info_t  ep_info; /* End point information */
};  


typedef struct _bcm_sat_hash_data_s _bcm_sat_hash_data_t;  
 

/*
 * Typedef:
 *     _bcm_sat_control_t
 * Purpose:
 *     SAT module control structure. One structure for each XGS device.
 */
struct _bcm_sat_control_s {
    int                         init;               /* TRUE if SAT module has    */
                                                    /* been initialized.         */
    sal_mutex_t                 sat_lock;           /* Protection mutex          */
    uint32                      upsamp_ep_count;    /* Max number of UP-MEP      */
                                                    /* endpoints supported.      */
    uint32                      downsamp_ep_count;  /* Max number of DOWN_MEP    */
                                                    /* endpoints supported.      */
    shr_idxres_list_handle_t    upsamp_mep_pool;    /* SAT UP-MEP endpoint indices  */
                                                    /* pool.                        */
    shr_idxres_list_handle_t    downsamp_mep_pool;  /* SAT UP-MEP endpoint indices  */
	                                                /* pool.                        */
    shr_htb_hash_table_t        upsamp_htbl;        /* SAT UP-MEP endpoint hash table.      */
	_bcm_sat_hash_data_t       *upsamp_hash_data;   /* Pointer to UP-MEP SAT hash data memory. */
    shr_htb_hash_table_t        downsamp_htbl;      /* SAT Down-MEP endpoint hash table.      */
    _bcm_sat_hash_data_t       *downsamp_hash_data; /* Pointer to Down-MEP SAT hash data memory. */
};

typedef struct _bcm_sat_control_s _bcm_sat_control_t;

#endif  /* !_BCM_INT_SAT_H_ */
