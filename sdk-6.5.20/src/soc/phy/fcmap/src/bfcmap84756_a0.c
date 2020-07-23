/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bfcmap.h>
#include <bfcmap_int.h>
#include <bfcmap84756_a0.h>
#include <mmi_cmn.h>
#include <soc/phyctrl.h>
#include <shared/bsl.h>

#define BFCMAP84756_MAX_PORTS           1
             
#define BFCMAP84756_MAKE_FP_COOKIE(f,i,p)    (((f) << 8) | (p))


/*
 * Check if conflicting flags are set
 */
#define BFCMAP84756_CHECK_CONFLICTING_FLAGS2(v, f1, f2)    \
            ((((v) & ((f1) | (f2))) &&                  \
              (((v) & ((f1) | (f2))) == ((f1) | (f2)))) ? 1 : 0)

/*
 * Check if multiple flags set in the specified value (v).
 */
#define BFCMAP84756_CHECK_MULTIPLE_FLAGS_SET(f)         \
            (((f) && (((f) - 1) & (f))) ? 1 : 0)

/* 
 * Bitmap of valid flags for port config.
 */
#define BFCMAP84756_PORT_CONFIG_FLAG_VALID                      \
        (BFCMAP_PORT_ENABLE                               |\
         0)

/*
 * Validate if Port configuration flags are valid.
 */
#define BFCMAP84756_IS_PORT_CONFIG_VALID(f)                 \
            (((f) & ~BFCMAP84756_PORT_CONFIG_FLAG_VALID) ? 0 : 1)

#define BFCMAP84756_USER_DATA_OFFSET__COUNT     2

#define BFCMAP84756_MIB_CLEAR_THRESHOLD         100

#define BFCMAP84756_VALID_EGRESS_SECURE_MATCH_FLAGS            0

/*
 * LMI / UC communications.
 */
#define BFCMAP84756_ERROR_INTR_MASK          (0xfffc0000)
#define BFCMAP84756_ERROR_INTR_STATUS        (0xfffc)

#define BFCMAP84756_UC_INTR_EN               (0x0080)
#define BFCMAP84756_UC_INTR_STATUS           (0x0080)

#define BFCMAP84756_UC_CMD2_UC_ATTN          (0x2000)

#define BFCMAP_PORT_UC_INT_ENABLE_REG(c) (0x2e)
#define BFCMAP_PORT_UC_INT_STATUS_REG(c) (0x2d)
#define BFCMAP_PORT_UC_CMD_REG(c)        (0x22)
#define BFCMAP_PORT_UC_CMD2_REG(c)       (0x2a)
#define BFCMAP_PORT_LMI_UC_MB(c)         (0x28)
#define BFCMAP_PORT_UC_LMI_MB(c)         (0x29)
#define BFCMAP_PORT_UC_DATA_REG0(c)      (0x40)
#define BFCMAP_PORT_UC_DATA_REG14(c)     (0x4e)
#define BFCMAP_PORT_UC_DATA_REG15(c)     (0x4f)
#define BFCMAP_PORT_UC_DATA_REG16(c)     (0x50)
#define BFCMAP_PORT_UC_DATA_REG17(c)     (0x51)
#define BFCMAP_PORT_UC_DATA_REG18(c)     (0x52)
#define BFCMAP_PORT_UC_DATA_REG19(c)     (0x53)
#define BFCMAP_PORT_UC_DATA_REG21(c)     (0x55)
#define BFCMAP_PORT_UC_DATA_REG22(c)     (0x56)
#define BFCMAP_PORT_UC_DATA_REG31(c)     (0x5f)


/************************************************************************
 * Local functions
 */
/*
extern int bfcmap_int_mmi1_reset(bfcmap_dev_addr_t dev_addr);
*/
extern int bfcmap84756_a0_firmware_download(bfcmap_dev_addr_t dev_addr, int broadcast, int enable);

STATIC int bfcmap84756_port_config_selective_set(bfcmap_port_ctrl_t *mpc, 
                                                        bfcmap_port_config_t *pCfg);

STATIC int bfcmap84756_port_config_selective_get(bfcmap_port_ctrl_t *mpc, 
                                                        bfcmap_port_config_t *pCfg);
extern int 
bfcmap_event(bfcmap_port_ctrl_t *mpc, bfcmap_event_t event, 
             int chanId, int assocId);

/***********************************************************
 * Statistics
 */
STATIC bfcmap_counter_info_t bfcmap84756_counter_info_tbl[] = {
	/* fc_rxdebug0 */
	{ fc_rxdebug0,  0x00100083, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxdebug1 */
	{ fc_rxdebug1,  0x00100084, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxunicastpkts */
	{ fc_rxunicastpkts,  0x00100085, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxgoodframes */
	{ fc_rxgoodframes,  0x00100086, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxbcastpkts */
	{ fc_rxbcastpkts,  0x00100087, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
        /* fc_rxbbcredit0 */
	{ fc_rxbbcredit0,  0x00100088, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxinvalidcrc */
	{ fc_rxinvalidcrc,  0x00100089, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxframetoolong */
	{ fc_rxframetoolong,  0x0010008a, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxtruncframes */
	{ fc_rxtruncframes,  0x0010008b, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxdelimitererr */
	{ fc_rxdelimitererr,  0x0010008c, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxothererr */
	{ fc_rxothererr,  0x0010008d, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxruntframes */
	{ fc_rxruntframes,  0x0010008e, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxlipcount */
	{ fc_rxlipcount,  0x0010008f, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxnoscount */
	{ fc_rxnoscount,  0x00100090, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxerrframes */
	{ fc_rxerrframes,  0x00100091, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxdropframes */
	{ fc_rxdropframes,  0x00100092, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxlinkfail */
	{ fc_rxlinkfail,  0x00100093, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxlosssync */
	{ fc_rxlosssync,  0x00100094, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxlosssig */
	{ fc_rxlosssig,  0x00100095, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxprimseqerr */
	{ fc_rxprimseqerr,  0x00100096, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxinvalidword */
	{ fc_rxinvalidword,  0x00100097, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxinvalidset */
	{ fc_rxinvalidset,  0x00100098, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxencodedisparity */
	{ fc_rxencodedisparity,  0x00100099, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_rxbyt */
	{ fc_rxbyt,  0x0010009a, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_txdebug0 */
	{ fc_txdebug0,  0x001000d9, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_txdebug1 */
	{ fc_txdebug1,  0x001000da, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_txunicastpkts */
	{ fc_txunicastpkts,  0x001000db, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_txbcastpkts */
	{ fc_txbcastpkts,  0x001000dc, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_txbbcredit0 */
	{ fc_txbbcredit0,  0x001000dd, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_txgoodframes */
	{ fc_txgoodframes,  0x001000de, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_txfifounderrun */
	{ fc_txfifounderrun,  0x001000df, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_txdropframes */
	{ fc_txdropframes,  0x001000e0, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },
	/* fc_txbyt */
	{ fc_txbyt,  0x001000e1, 0,
               BFCMAP_COUNTER_F_PORT | BFCMAP_COUNTER_F_SIZE64 },

    { 0, 0, 0, 0 } /* last entry */
};


STATIC int cl45_dev = 30 ;
/*********************************************************
 * Local functions and structures
 */
STATIC int
_bfcmap_mdio_read(bfcmap_dev_addr_t dev_addr, 
                        buint32_t io_addr, buint16_t *data)
{
    io_addr = BLMI_IO_CL45_ADDRESS(cl45_dev, io_addr);
    return _blmi_mmi_rd_f(dev_addr, io_addr, data);
}

STATIC int
_bfcmap_mdio_write(bfcmap_dev_addr_t dev_addr, 
                        buint32_t io_addr, buint16_t data)
{
    io_addr = BLMI_IO_CL45_ADDRESS(cl45_dev, io_addr);
    return _blmi_mmi_wr_f(dev_addr, io_addr, data);
}

#define BFCMAP_MDIO_READ(dev_addr, r, pd)   \
                    _bfcmap_mdio_read((dev_addr), (r), (pd))

#define BFCMAP_MDIO_WRITE(dev_addr, r, d)   \
                    _bfcmap_mdio_write((dev_addr), (r), (buint16_t) (d))


#define MAX_WAIT_FOR_UC  1000

int 
_bfcmap84756_send_lmi_uc_msg(bfcmap_port_ctrl_t *mpc, 
                             bfcmap_lmi_uc_cmds_t  msg_id, buint16_t data)
{
    int rv = BFCMAP_E_NONE, i;
    buint16_t fval;
    bfcmap_dev_addr_t mdio_addr = mpc->uc_dev_addr ;

	BFCMAP_LOCK_PORT(mpc);

    if ( msg_id == BFCMAP_UC_FIRMWARE_INIT ) {
	    /* Write 16 bit date associated with the command */
	    BFCMAP_MDIO_WRITE(mdio_addr, BFCMAP_PORT_UC_DATA_REG0(mpc), data);
	    fval = msg_id;
	    BFCMAP_MDIO_WRITE(mdio_addr, BFCMAP_PORT_LMI_UC_MB(mpc), fval);
    }
	else {
		/* handshake with uC to make sure uC is ready for the next command */
		for ( i = 0 ; i < MAX_WAIT_FOR_UC; i++) {
	    	BFCMAP_MDIO_READ(mdio_addr, BFCMAP_PORT_UC_DATA_REG31(mpc), &fval);
			if (fval != 0) {
				BFCMAP_MDIO_WRITE(mdio_addr, BFCMAP_PORT_UC_DATA_REG31(mpc), 0);
				break;
			}
			else {
				BFCMAP_SAL_USLEEP(10);
			}
		}
		if (i == MAX_WAIT_FOR_UC)
			BFCMAP_SAL_PRINTF("BCM84756:FC uC at unit %d port %d is likely dead \n", 
			                  BFCMAP_UNIT(mpc), BFCMAP_PORT(mpc));
		
	    /* Write 16 bit data associated with the command */
	    BFCMAP_MDIO_WRITE(mdio_addr, BFCMAP_PORT_UC_DATA_REG0(mpc), data);
	    fval = msg_id;
	    BFCMAP_MDIO_WRITE(mdio_addr, BFCMAP_PORT_LMI_UC_MB(mpc), fval);

		/* Interrupt uc for all other cmd other than BFCMAP_UC_FIRMWARE_INIT */
	    /* Send interrupt to the microcontroller */
	    BFCMAP_MDIO_READ(mdio_addr, BFCMAP_PORT_UC_CMD2_REG(mpc), &fval);
	    fval |= BFCMAP84756_UC_CMD2_UC_ATTN;
	    BFCMAP_MDIO_WRITE(mdio_addr, BFCMAP_PORT_UC_CMD2_REG(mpc), fval);

	    fval &= ~BFCMAP84756_UC_CMD2_UC_ATTN;
	    BFCMAP_MDIO_WRITE(mdio_addr, BFCMAP_PORT_UC_CMD2_REG(mpc), fval);
	}
	
	BFCMAP_UNLOCK_PORT(mpc);

    return rv;
}


STATIC bfcmap_counter_cb_t bfcmap84756_counter_cb = {
    16,
    NULL,
    16,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};


STATIC int 
bfcmap84756_mp_vlanmap_move(bfcmap_dev_ctrl_t *mdc,
                    bfcmap_port_ctrl_t *mpc,
                    int from_idx, int to_idx, int num_en,
                    buint32_t cb_data)
{
    BFCMAP_COMPILER_SATISFY(mdc);
    BFCMAP_COMPILER_SATISFY(mpc);
    BFCMAP_COMPILER_SATISFY(from_idx);
    BFCMAP_COMPILER_SATISFY(to_idx);
    BFCMAP_COMPILER_SATISFY(num_en);
    BFCMAP_COMPILER_SATISFY(cb_data);

    return 0;
}

STATIC int 
bfcmap84756_mp_vlanmap_cmp(bfcmap_dev_ctrl_t *mdc,
                    bfcmap_port_ctrl_t *mpc,
                    bfcmap_int_vlanmap_entry_t* src,
                    bfcmap_int_vlanmap_entry_t* dest,
                    buint32_t cb_data)
{
    bfcmap_vlan_vsan_map_t *fl1, *fl2;

    BFCMAP_COMPILER_SATISFY(mdc);
    BFCMAP_COMPILER_SATISFY(cb_data);

    if (src->pc != dest->pc) {
        goto vlanmap_no_match;
    }

    fl1 = &src->vlanmap;
    fl2 = &dest->vlanmap;

    if (fl1->vlan_vid != fl2->vlan_vid) {
        goto vlanmap_no_match;
    }

    if (fl1->vsan_vfid != fl2->vsan_vfid) {
        goto vlanmap_no_match;
    }

    return 0;

vlanmap_no_match:
    return -1;
}


STATIC int 
bfcmap84756_mp_vlanmap_write(bfcmap_dev_ctrl_t *mdc,
                    bfcmap_port_ctrl_t *mpc,
                    bfcmap_int_vlanmap_entry_t* src,
                    int index, buint32_t cb_data)
{
    bfcmap_vlan_vsan_map_t *fl;
    buint32_t       mem_addr;
    buint32_t       fval;
    bcm84756_a0_VLAN_MAPTABLE_0r_t reg;

    BFCMAP_COMPILER_SATISFY(mdc);
    BFCMAP_COMPILER_SATISFY(cb_data);


    fl = &src->vlanmap;

    bcm84756_a0_VLAN_MAPTABLE_0r_CLR(reg);

    bcm84756_a0_VLAN_MAPTABLE_0r_VLAN_VIDf_SET(mpc, reg, fl->vlan_vid);
    bcm84756_a0_VLAN_MAPTABLE_0r_VSAN_VFIDf_SET(mpc, reg, fl->vsan_vfid);

    mem_addr = bcm84756_a0_VLAN_MAPTABLE_0r(mpc);
    mem_addr += index;

    fval = bcm84756_a0_VLAN_MAPTABLE_0r_GET(reg);
    return BFCMAP_REG32_WRITE(mpc, mem_addr, &fval);
}

#if defined (BCM_WARM_BOOT_SUPPORT)  /*  */

STATIC int 
bfcmap84756_mp_vlanmap_read(bfcmap_dev_ctrl_t *mdc,
                    bfcmap_port_ctrl_t *mpc,
                    bfcmap_int_vlanmap_entry_t* src,
                    int index,
                    bfcmap_int_vlanmap_entry_t** new_map,
                    buint32_t cb_data)
{
    bfcmap_vlan_vsan_map_t fl;
    buint32_t       mem_addr;
    buint32_t       fval;
    bcm84756_a0_VLAN_MAPTABLE_0r_t reg;

    BFCMAP_COMPILER_SATISFY(mdc);
    BFCMAP_COMPILER_SATISFY(cb_data);
    BFCMAP_COMPILER_SATISFY(src);

    bcm84756_a0_VLAN_MAPTABLE_0r_CLR(reg);

    mem_addr = bcm84756_a0_VLAN_MAPTABLE_0r(mpc);
    mem_addr += index;
    BFCMAP_REG32_READ(mpc, mem_addr, &fval);

    if (fval == 0) {
        *new_map  = NULL;
        return BFCMAP_E_NONE;
    }


    bcm84756_a0_VLAN_MAPTABLE_0r_SET(reg, fval);

    fl.vlan_vid = bcm84756_a0_VLAN_MAPTABLE_0r_VLAN_VIDf_GET(mpc, reg);
    fl.vsan_vfid = bcm84756_a0_VLAN_MAPTABLE_0r_VSAN_VFIDf_GET(mpc, reg);


    *new_map = bfcmap_int_vlanmap_add_or_update(mpc, &fl);
    if (*new_map == NULL) {
        return BFCMAP_E_RESOURCE;
    }

    (*new_map)->vlanmap.vlan_vid = fl.vlan_vid ;
    (*new_map)->vlanmap.vsan_vfid = fl.vsan_vfid;

    return BFCMAP_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */


STATIC int 
bfcmap84756_mp_vlanmap_clear(bfcmap_dev_ctrl_t *mdc,
                    bfcmap_port_ctrl_t *mpc,
                    int index, int num_en, buint32_t cb_data)
{
    buint32_t       mem_addr;
    buint32_t       fval;
    int ii;
    int rv;

    BFCMAP_COMPILER_SATISFY(mdc);
    BFCMAP_COMPILER_SATISFY(cb_data);

    mem_addr = bcm84756_a0_VLAN_MAPTABLE_0r(mpc);
    mem_addr += index;

    fval = 0;
    for (ii = 0 ; ii < num_en ; ii++) {
        rv = BFCMAP_REG32_WRITE(mpc, mem_addr, &fval);
        if (rv) {
            return rv;
        }
        mem_addr += 1;
    }

    return 0;
}


/*
 * Function:
 *      bfcmap84756_device_init
 * Purpose:
 *      Initializes a bfcmap device.
 * Parameters:
 *      mdc         - bfcmap device control strtucture
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */
STATIC int 
bfcmap84756_device_init(bfcmap_dev_ctrl_t *mdc)
{
    buint32_t       cb_arg;
    int             port, mp_size;
    bfcmap_port_ctrl_t *mpc;
    bfcmap_hw_mapper_vec_t mpvec;

    /* setup MP callback vectors.  */
    mpvec.mp_cmp    = bfcmap84756_mp_vlanmap_cmp;
    mpvec.mp_clear  = bfcmap84756_mp_vlanmap_clear;
    mpvec.mp_move   = bfcmap84756_mp_vlanmap_move;
    mpvec.mp_write  = bfcmap84756_mp_vlanmap_write;
#if defined (BCM_WARM_BOOT_SUPPORT)  /*  */
    mpvec.mp_read   = bfcmap84756_mp_vlanmap_read ;
#endif /* BCM_WARM_BOOT_SUPPORT */


    BFCMAP_LOCK_DEVICE(mdc);

    /*
     * Init all ports.
     */
    for (port = 0; port < BFCMAP84756_MAX_PORTS; port++) {
        mpc = BFCMAP_UNIT_PORT_CONTROL(mdc, port);

        if (mpc->mp == NULL) {
            cb_arg = 0;
            mp_size = 64;
            mpc->mp = bfcmap_int_hw_mapper_create(mpc, 
                                            mp_size, cb_arg, &mpvec);
        }
    }

    if (mdc->counter_info == NULL) {
        mdc->counter_info = bfcmap_int_stat_tbl_create(mdc, 
                                        bfcmap84756_counter_info_tbl);
        mdc->counter_cb = &bfcmap84756_counter_cb;
    }

    BFCMAP_UNLOCK_DEVICE(mdc);

    return BFCMAP_E_NONE;
}

/*
 * Function:
 *      bfcmap84756_device_uninit
 * Purpose:
 *      Initializes a bfcmap device.
 * Parameters:
 *      mdc         - bfcmap device control strtucture
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */
STATIC int 
bfcmap84756_device_uninit(bfcmap_dev_ctrl_t *mdc)
{
    bfcmap_port_ctrl_t         *mpc;
    int                         port;

    BFCMAP_LOCK_DEVICE(mdc);

    /* Destroy mp instances */
    for (port = 0; port < BFCMAP84756_MAX_PORTS; port++) {
        mpc = BFCMAP_UNIT_PORT_CONTROL(mdc, port);
        if(mpc->mp) {
            BFCMAP_SAL_FREE(mpc->mp);
            mpc->mp = NULL;
        }
    }
        

    if (mdc->counter_info) {
        BFCMAP_SAL_FREE(mdc->counter_info);
        mdc->counter_info = NULL;
    }

    BFCMAP_UNLOCK_DEVICE(mdc);

    return BFCMAP_E_NONE;
}

/*
 * Function:
 *      bfcmap84756_port_speed_set
 * Purpose:
 *      Sets speed on a bfcmap port
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      speed        - Speed to set
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */

#define AN_WITH_MAX_SPEED   0x8000
STATIC int 
bfcmap84756_port_speed_set(bfcmap_port_ctrl_t *mpc, bfcmap_port_speed_t speed)
{
    int rv = BFCMAP_E_NONE;
    int fval = 0;

    switch (speed) {
        case BFCMAP_PORT_SPEED_AN:
            fval = 0;
            break;
        case BFCMAP_PORT_SPEED_2GBPS:
            fval = 2;
            break;
        case BFCMAP_PORT_SPEED_4GBPS:
            fval = 4;
            break;
        case BFCMAP_PORT_SPEED_8GBPS:
            fval = 8;
            break;
        case BFCMAP_PORT_SPEED_AN_2GBPS:
            fval = AN_WITH_MAX_SPEED | 2;
            break;
        case BFCMAP_PORT_SPEED_AN_4GBPS:
            fval = AN_WITH_MAX_SPEED | 4;
            break;
        case BFCMAP_PORT_SPEED_AN_8GBPS:
            fval = AN_WITH_MAX_SPEED | 8;
            break;
        default:
            return BFCMAP_E_PARAM;
    }
    /* Send to UC - link Speed */
    rv = _bfcmap84756_send_lmi_uc_msg(mpc, BFCMAP_UC_LINK_SPEED, fval);

    return rv;
}

/*
 * Function:
 *      bfcmap84756_port_config_selective_set
 * Purpose:
 *      Initilaizes the bfcmap port corresponding to the configuration
 *      specified.
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      pCfg         - Pointer to port configuration
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */
STATIC int 
bfcmap84756_port_config_selective_set(bfcmap_port_ctrl_t *mpc, 
                                              bfcmap_port_config_t *pCfg)
{
    int                                          fval, rv = BFCMAP_E_NONE;
    bcm84756_a0_FCM_FCMON_RX_R_T_TOVr_t          reg_rxtov;
    bcm84756_a0_FCM_FCMON_RX_INTERRUPT_ENABLEr_t reg_ien;
    bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_HIr_t    reg_src_hi;
    bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_LOr_t    reg_src_lo;
    bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_HIr_t    reg_dst_hi;
    bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_LOr_t    reg_dst_lo;
    bcm84756_a0_FCM_FCMI_CONFIGr_t               reg_icfg;
    bcm84756_a0_FCM_FCME_CONFIGr_t               reg_ecfg;
    bcm84756_a0_FCM_FCMI_MAC_SRCr_t              reg_imm;
    bcm84756_a0_FCM_FCMI_VFTHDR_PROC_MODEr_t     reg_ivfm;
    bcm84756_a0_FCM_FCMI_VLANTAG_PROC_MODEr_t    reg_ivlm;
    bcm84756_a0_FCM_FCME_VFTHDR_PROC_MODEr_t     reg_evfm;
    bcm84756_a0_FCM_FCME_VLANTAG_PROC_MODEr_t    reg_evlm;
    bcm84756_a0_FCM_DEFAULT_VLAN_TAGr_t          reg_vlan_tag;
    bcm84756_a0_FCM_DEFAULT_VFTHDRr_t            reg_vft_tag; 
    bcm84756_a0_FCM_FCMAC_BBCr_t reg_bbc;
    bcm84756_a0_FCM_FCMAC_FRXB0r_t  reg_frxb0;
    bcm84756_a0_FCM_FCMAC_MAXFRMLEN_CONFIGr_t    reg_maxf;
    bcm84756_a0_FCM_FCMAC_BBCR_RECOVERYr_t       reg_bbscn;
    int maxfleng, buf_reserve, org_bbscn, rx_bbc;
    uint32  mask1, mask2;
    
    if (!pCfg) {
        BFCMAP_SAL_DBG_PRINTF("Port configuration is NULL\n");
        return BFCMAP_E_PARAM;
    }

    mask1 = pCfg->action_mask;
    mask2 = pCfg->action_mask2;

    /* These config parameters are not supported by MT1 */
    
    if ((mask2 & BFCMAP_ATTR2_USE_TTS_PCS_16G_MASK) ||
        (mask2 & BFCMAP_ATTR2_USE_TTS_PCS_32G_MASK) ||
        (mask2 & BFCMAP_ATTR2_TRAINING_ENABLE_16G_MASK) ||
        (mask2 & BFCMAP_ATTR2_TRAINING_ENABLE_32G_MASK) ||
        (mask2 & BFCMAP_ATTR2_FEC_ENABLE_16G_MASK) ||
        (mask2 & BFCMAP_ATTR2_FEC_ENABLE_32G_MASK) ||
        (mask2 & BFCMAP_ATTR2_INGRESS_FCS_CRRPT_EOF_ENABLE_MASK) ||
        (mask2 & BFCMAP_ATTR2_INGRESS_VLANTAG_PRESENCE_ENABLE_MASK) ||
        (mask2 & BFCMAP_ATTR2_EGRESS_HOPCNT_CHECK_MODE_MASK) ||
        (mask2 & BFCMAP_ATTR2_EGRESS_PASS_CTRL_FRAME_ENABLE_MASK) ||
        (mask2 & BFCMAP_ATTR2_EGRESS_PASS_PFC_FRAME_ENABLE_MASK) ||
        (mask2 & BFCMAP_ATTR2_EGRESS_PASS_PAUSE_FRAME_ENABLE_MASK) ||
        (mask2 & BFCMAP_ATTR2_EGRESS_FCOE_VER_CHK_DISABLE_MASK) ||
        (mask2 & BFCMAP_ATTR2_EGRESS_DEFAULT_COS_VALUE_MASK) ||
        (mask2 & BFCMAP_ATTR2_EGRESS_USE_IP_COS_MAP_MASK) ||
        (mask2 & BFCMAP_ATTR2_INGRESS_HOPCNT_DEC_ENABLE_MASK) ||
        (mask2 & BFCMAP_ATTR2_SCRAMBLING_ENABLE_MASK) || 
        (mask2 & BFCMAP_ATTR2_EGRESS_PAUSE_ENABLE_MASK) ||
        (mask2 & BFCMAP_ATTR2_EGRESS_PFC_ENABLE_MASK) ||
        (mask2 & BFCMAP_ATTR2_STAT_INTERVAL_MASK) ||
        (mask2 & BFCMAP_ATTR2_TRCM_ATTRIBS_MASK) ||
        (mask2 & BFCMAP_ATTR2_COS_TO_PRI_MASK)) {

        BFCMAP_SAL_DBG_PRINTF("Port configuration parameter is Not Supported\n");
        return BFCMAP_E_PARAM;
    }

    BFCMAP_LOCK_PORT(mpc);

    if ((mask1 & BFCMAP_ATTR_MAX_FRAME_LENGTH_MASK) ||
        (mask1 & BFCMAP_ATTR_RX_BB_CREDITS_MASK)) {
        /* Max frame length */
        maxfleng = pCfg->max_frame_length;

        /* RX buffer Credits based on max_frame_length (rounded up to the next 16 bytes)*/
        if ((maxfleng % 4) == 0) {
            buf_reserve = maxfleng;
        }
        else {
            buf_reserve = maxfleng + 4 -(maxfleng % 4);
        }
        rx_bbc = 0x2000/buf_reserve;
        if (rx_bbc > 31) {
            BFCMAP_SAL_DBG_PRINTF("max_frame_length too small\n");
            return BFCMAP_E_PARAM;
        }
            
        bcm84756_a0_FCM_FCMAC_MAXFRMLEN_CONFIGr_CLR(reg_maxf);
        bcm84756_a0_FCM_FCMAC_MAXFRMLEN_CONFIGr_MAXLENf_SET(mpc, reg_maxf, maxfleng);
        bcm84756_a0_WRITE_FCM_FCMAC_MAXFRMLEN_CONFIGr(mpc, reg_maxf);
    
        bcm84756_a0_FCM_FCMAC_FRXB0r_CLR(reg_frxb0);
        bcm84756_a0_FCM_FCMAC_FRXB0r_FRXB_FREE_RXB_CNTf_SET(mpc, reg_frxb0,rx_bbc);
        bcm84756_a0_WRITE_FCM_FCMAC_FRXB0r(mpc,reg_frxb0);
        
        /* send the command to uCode */
        rv = _bfcmap84756_send_lmi_uc_msg(mpc, BFCMAP_UC_RX_BBC_SET, rx_bbc);
        if (rv != BFCMAP_E_NONE)
            goto exit;
    }
    
    if (mask1 & BFCMAP_ATTR_TX_BB_CREDITS_MASK) {
        /* TX buffer Credits */
        bcm84756_a0_READ_FCM_FCMAC_BBCr(mpc, reg_bbc);
        bcm84756_a0_FCM_FCMAC_BBCr_BBC_BB_CREDITf_SET(mpc, reg_bbc, pCfg->tx_buffer_to_buffer_credits);
        bcm84756_a0_WRITE_FCM_FCMAC_BBCr(mpc, reg_bbc);
    }
    
    if (mask1 & BFCMAP_ATTR_BB_SC_N_MASK) {
        /* BB_SC_N */
        bcm84756_a0_READ_FCM_FCMAC_BBCR_RECOVERYr(mpc, reg_bbscn);
        org_bbscn = bcm84756_a0_FCM_FCMAC_BBCR_RECOVERYr_BB_SC_Nf_GET(mpc, reg_bbscn);
        /* Set BB_SC_N to 0 first if it changes from one non-zero value to another non-zero value */
        if (org_bbscn && pCfg->bb_sc_n && (org_bbscn != pCfg->bb_sc_n))
        {
            bcm84756_a0_FCM_FCMAC_BBCR_RECOVERYr_CLR(reg_bbscn);
            bcm84756_a0_FCM_FCMAC_BBCR_RECOVERYr_BB_SC_Nf_SET(mpc, reg_bbscn, 0);
            bcm84756_a0_WRITE_FCM_FCMAC_BBCR_RECOVERYr(mpc, reg_bbscn);
        }
        bcm84756_a0_FCM_FCMAC_BBCR_RECOVERYr_CLR(reg_bbscn);
        bcm84756_a0_FCM_FCMAC_BBCR_RECOVERYr_BB_SC_Nf_SET(mpc, reg_bbscn, pCfg->bb_sc_n);
        bcm84756_a0_WRITE_FCM_FCMAC_BBCR_RECOVERYr(mpc, reg_bbscn);
    }
    
    if (mask1 & BFCMAP_ATTR_R_T_TOV_MASK) {
        /* RX R_T_TOV  */
        if ((fval = pCfg->r_t_tov)) {
            bcm84756_a0_FCM_FCMON_RX_R_T_TOVr_CLR(reg_rxtov);
            bcm84756_a0_FCM_FCMON_RX_R_T_TOVr_R_T_TOVf_SET(mpc,reg_rxtov, fval);
            bcm84756_a0_WRITE_FCM_FCMON_RX_R_T_TOVr(mpc,reg_rxtov);
        }
    }

    if (mask1 & BFCMAP_ATTR_VLAN_TAG_MASK) {
        /* VLAN Tag */
        bcm84756_a0_FCM_DEFAULT_VLAN_TAGr_CLR(reg_vlan_tag);
        fval = pCfg->vlan_tag;
        bcm84756_a0_FCM_DEFAULT_VLAN_TAGr_SET(reg_vlan_tag, fval);
        bcm84756_a0_WRITE_FCM_DEFAULT_VLAN_TAGr(mpc, reg_vlan_tag);
    }

    if (mask1 & BFCMAP_ATTR_VFT_TAG_MASK) {
        /* VFT Tag */
        bcm84756_a0_FCM_DEFAULT_VFTHDRr_CLR(reg_vft_tag);
        fval = pCfg->vft_tag;
        bcm84756_a0_FCM_DEFAULT_VFTHDRr_SET(reg_vft_tag, fval);
        bcm84756_a0_WRITE_FCM_DEFAULT_VFTHDRr(mpc, reg_vft_tag);
    }

    if ((mask1 & BFCMAP_ATTR_PORT_MODE_MASK) || 
        (mask1 & BFCMAP_ATTR_INGRESS_MAPPER_BYPASS_MASK) || 
        (mask1 & BFCMAP_ATTR_INGRESS_MAP_TABLE_INPUT_MASK) || 
        (mask1 & BFCMAP_ATTR_INGRESS_FC_CRC_MODE_MASK)) {
        /* Mapper Ingress Config */
        bcm84756_a0_READ_FCM_FCMI_CONFIGr(mpc, reg_icfg);

        if ((mask1 & BFCMAP_ATTR_PORT_MODE_MASK)) {
            /* port mode */
            switch (pCfg->port_mode) {
                case BFCMAP_FCOE_TO_FCOE_MODE: /* FCoE Encap not done */
                    fval = 0;
                    break;
                case BFCMAP_FCOE_TO_FC_MODE:   /* FCoE encap done */
                    fval = 1;
                    break;
                default:
                    rv = BFCMAP_E_PARAM;
                    goto exit;
            }
            bcm84756_a0_FCM_FCMI_CONFIGr_ENCAP_FC2FCOE_ENf_SET(mpc, reg_icfg, fval);
        }
        
        if (mask1 & BFCMAP_ATTR_INGRESS_MAPPER_BYPASS_MASK) {
            fval = (pCfg->ingress_mapper_bypass) ? 1 : 0;
            bcm84756_a0_FCM_FCMI_CONFIGr_MAPPER_BYPASS_ENABLEf_SET(mpc, reg_icfg, fval);
        }
        if (mask1 & BFCMAP_ATTR_INGRESS_MAP_TABLE_INPUT_MASK) {
            switch (pCfg->ingress_map_table_input) {
            case BFCMAP_MAP_TABLE_INPUT_VID:
                fval = 1;
                break;
            case BFCMAP_MAP_TABLE_INPUT_VFID:
                fval = 0;
                break;
            default:
                rv = BFCMAP_E_PARAM;
                goto exit;
            }
            bcm84756_a0_FCM_FCMI_CONFIGr_MAP_TABLE_INPUTf_SET(mpc, reg_icfg, fval);
        }
        
        if (mask1 & BFCMAP_ATTR_INGRESS_FC_CRC_MODE_MASK) {
            switch (pCfg->ingress_fc_crc_mode) {
            case BFCMAP_FC_CRC_MODE_NORMAL:
                fval = 0;
                break;
            case BFCMAP_FC_CRC_MODE_NO_CRC_CHECK:
                fval = 1;
                break;
            default:
                rv = BFCMAP_E_PARAM;
                goto exit;
            }
            bcm84756_a0_FCM_FCMI_CONFIGr_FCS_REGEN_MODEf_SET(mpc, reg_icfg, fval);
        }
        bcm84756_a0_WRITE_FCM_FCMI_CONFIGr(mpc, reg_icfg);
    }
    
    if (mask1 & BFCMAP_ATTR_SRC_MAC_CONSTRUCT_MASK) {
        /* Source MAC */
        bcm84756_a0_READ_FCM_FCMI_MAC_SRCr(mpc, reg_imm);
        bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_HIr_CLR(reg_src_hi);
        bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_LOr_CLR(reg_src_lo);
        switch (pCfg->src_mac_construct) {
            case BFCMAP_ENCAP_FCOE_FPMA:
                fval = (pCfg->src_fcmap_prefix & 0xffff00) >> 8 ; 
                bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_HIr_SRCMACf_SET(mpc, reg_src_hi, fval);
                fval = (pCfg->src_fcmap_prefix & 0xff) << 24;
                bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_LOr_SRCMACf_SET(mpc, reg_src_lo, fval);
                fval = 0; /* Program the SA MAC construct to 0 */
                bcm84756_a0_FCM_FCMI_MAC_SRCr_FCOE_SA_MODEf_SET(mpc, reg_imm, fval);
                bcm84756_a0_WRITE_FCM_FCMI_MAC_SRCr(mpc, reg_imm);
                bcm84756_a0_WRITE_FCM_DEFAULT_FCOE_SRCMAC_HIr(mpc,reg_src_hi);
                bcm84756_a0_WRITE_FCM_DEFAULT_FCOE_SRCMAC_LOr(mpc,reg_src_lo);
                break;
            case BFCMAP_ENCAP_FCOE_ETH_ADDRESS_NULL:
                fval = 0;
                bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_HIr_SRCMACf_SET(mpc, reg_src_hi, fval);
                bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_LOr_SRCMACf_SET(mpc, reg_src_lo, fval);
                fval = 1; /* Program the SA MAC construct to 1 */
                bcm84756_a0_FCM_FCMI_MAC_SRCr_FCOE_SA_MODEf_SET(mpc, reg_imm, fval);
                bcm84756_a0_WRITE_FCM_FCMI_MAC_SRCr(mpc, reg_imm);
                bcm84756_a0_WRITE_FCM_DEFAULT_FCOE_SRCMAC_HIr(mpc,reg_src_hi);
                bcm84756_a0_WRITE_FCM_DEFAULT_FCOE_SRCMAC_LOr(mpc,reg_src_lo);
                break;
            case BFCMAP_ENCAP_FCOE_ETH_ADDRESS_USER:
                fval = BMAC_TO_32_HI(pCfg->src_mac_addr);
                bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_HIr_SRCMACf_SET(mpc, reg_src_hi, fval);
                fval = BMAC_TO_32_LO(pCfg->src_mac_addr);
                bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_LOr_SRCMACf_SET(mpc, reg_src_lo, fval);
                fval = 2; /* Program the SA MAC construct to 2 */
                bcm84756_a0_FCM_FCMI_MAC_SRCr_FCOE_SA_MODEf_SET(mpc, reg_imm, fval);
                bcm84756_a0_WRITE_FCM_FCMI_MAC_SRCr(mpc, reg_imm);
                bcm84756_a0_WRITE_FCM_DEFAULT_FCOE_SRCMAC_HIr(mpc,reg_src_hi);
                bcm84756_a0_WRITE_FCM_DEFAULT_FCOE_SRCMAC_LOr(mpc,reg_src_lo);
                break;
            default:
                rv = BFCMAP_E_PARAM;
                goto exit;
        }
    }
    if (mask1 & BFCMAP_ATTR_DST_MAC_CONSTRUCT_MASK) {
        /* Destination MAC */
        bcm84756_a0_READ_FCM_FCMI_MAC_SRCr(mpc, reg_imm);
        bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_HIr_CLR(reg_dst_hi);
        bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_LOr_CLR(reg_dst_lo);
        switch (pCfg->dst_mac_construct) {
            case BFCMAP_ENCAP_FCOE_FPMA:
                fval = (pCfg->dst_fcmap_prefix & 0xffff00) >> 8;
                bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_HIr_DSTMACf_SET(mpc, reg_dst_hi, fval);
                fval = (pCfg->dst_fcmap_prefix & 0xff) << 24;
                bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_LOr_DSTMACf_SET(mpc, reg_dst_lo, fval);
                fval = 0; /* Program the DA MAC construct to 0 */
                bcm84756_a0_FCM_FCMI_MAC_SRCr_FCOE_DA_MODEf_SET(mpc, reg_imm, fval);
                bcm84756_a0_WRITE_FCM_DEFAULT_FCOE_DSTMAC_HIr(mpc,reg_dst_hi);
                bcm84756_a0_WRITE_FCM_DEFAULT_FCOE_DSTMAC_LOr(mpc,reg_dst_lo);
                bcm84756_a0_WRITE_FCM_FCMI_MAC_SRCr(mpc, reg_imm);
                break;
            case BFCMAP_ENCAP_FCOE_ETH_ADDRESS_NULL:
                fval = 0;
                bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_HIr_DSTMACf_SET(mpc, reg_dst_hi, fval);
                bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_LOr_DSTMACf_SET(mpc, reg_dst_lo, fval);
                fval = 1; /* Program the DA MAC construct to 1 */
                bcm84756_a0_FCM_FCMI_MAC_SRCr_FCOE_DA_MODEf_SET(mpc, reg_imm, fval);
                bcm84756_a0_WRITE_FCM_DEFAULT_FCOE_DSTMAC_HIr(mpc,reg_dst_hi);
                bcm84756_a0_WRITE_FCM_DEFAULT_FCOE_DSTMAC_LOr(mpc,reg_dst_lo);
                bcm84756_a0_WRITE_FCM_FCMI_MAC_SRCr(mpc, reg_imm);
                break;
            case BFCMAP_ENCAP_FCOE_ETH_ADDRESS_USER:
                fval = BMAC_TO_32_HI(pCfg->dst_mac_addr);
                bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_HIr_DSTMACf_SET(mpc, reg_dst_hi, fval);
                fval = BMAC_TO_32_LO(pCfg->dst_mac_addr);
                bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_LOr_DSTMACf_SET(mpc, reg_dst_lo, fval);
                fval = 2; /* Program the DA MAC construct to 2 */
                bcm84756_a0_FCM_FCMI_MAC_SRCr_FCOE_DA_MODEf_SET(mpc, reg_imm, fval);
                bcm84756_a0_WRITE_FCM_DEFAULT_FCOE_DSTMAC_HIr(mpc,reg_dst_hi);
                bcm84756_a0_WRITE_FCM_DEFAULT_FCOE_DSTMAC_LOr(mpc,reg_dst_lo);
                bcm84756_a0_WRITE_FCM_FCMI_MAC_SRCr(mpc, reg_imm);
                break;
            default:
                rv = BFCMAP_E_PARAM;
                goto exit;
        }
    } 
    
    if ((mask1 & BFCMAP_ATTR_INGRESS_VLANTAG_PROC_MODE_MASK) ||
        (mask1 & BFCMAP_ATTR_INGRESS_VID_MAPSRC_MASK) ||
        (mask1 & BFCMAP_ATTR_INGRESS_VLAN_PRI_MAP_MODE_MASK)) {
        
        /* Ingress VLAN header processing mode */
        bcm84756_a0_READ_FCM_FCMI_VLANTAG_PROC_MODEr(mpc, reg_ivlm);
        if (mask1 & BFCMAP_ATTR_INGRESS_VLANTAG_PROC_MODE_MASK) {
            switch(pCfg->ingress_vlantag_proc_mode) {
            case BFCMAP_VLANTAG_PRESERVE:
                fval = 0;
                break;
            case BFCMAP_VLANTAG_INSERT:
                fval = 1;
                break;
            case BFCMAP_VLANTAG_DELETE:
                fval = 2;
                break;
            default:
                rv = BFCMAP_E_PARAM;
                goto exit;
            }
            bcm84756_a0_FCM_FCMI_VLANTAG_PROC_MODEr_VLANTAG_PRESENCEf_SET(mpc, reg_ivlm, fval);
        }
        if (mask1 & BFCMAP_ATTR_INGRESS_VID_MAPSRC_MASK) {
            switch (pCfg->ingress_vid_mapsrc) {
            case BFCMAP_VID_MACSRC_VFID:
                fval = 0;
                break;
            case BFCMAP_VID_MACSRC_PASSTHRU:
                fval = 1;
                break;
            case BFCMAP_VID_MACSRC_PORT_DEFAULT:
                fval = 2;
                break;
            case BFCMAP_VID_MACSRC_MAPPER:
                fval = 3;
                break;
            default:
                rv = BFCMAP_E_PARAM;
                goto exit;
            }
            bcm84756_a0_FCM_FCMI_VLANTAG_PROC_MODEr_VLANTAG_MAPSRCf_SET(mpc, reg_ivlm, fval);
        }
        if (mask1 & BFCMAP_ATTR_INGRESS_VLAN_PRI_MAP_MODE_MASK) {
            switch (pCfg->ingress_vlan_pri_map_mode) {
            case BFCMAP_VLAN_PRI_MAP_MODE_PORT_DEFAULT:
                 fval = 0;
                 break;
            case BFCMAP_VLAN_PRI_MAP_MODE_PASSTHRU:
                fval = 1;
                break;
            default:
                rv = BFCMAP_E_PARAM;
                goto exit;
            }
            bcm84756_a0_FCM_FCMI_VLANTAG_PROC_MODEr_PRI_MAP_MODEf_SET(mpc, reg_ivlm, fval);
        }
        bcm84756_a0_WRITE_FCM_FCMI_VLANTAG_PROC_MODEr(mpc, reg_ivlm);
    }

    if ((mask1 & BFCMAP_ATTR_INGRESS_VFTHDR_PROC_MODE_MASK) ||
        (mask1 & BFCMAP_ATTR_INGRESS_VFID_MAPSRC_MASK) ||
        (mask1 & BFCMAP_ATTR_INGRESS_HOPCNT_CHECK_MODE_MASK)) {
        
        /* Ingress VFT header processing mode */
        bcm84756_a0_READ_FCM_FCMI_VFTHDR_PROC_MODEr(mpc, reg_ivfm);
        if (mask1 & BFCMAP_ATTR_INGRESS_VFTHDR_PROC_MODE_MASK) {
            switch (pCfg->ingress_vfthdr_proc_mode) {
            case BFCMAP_VFTHDR_PRESERVE:
                fval = 0;
                break;
            case BFCMAP_VFTHDR_INSERT:
                fval = 1;
                break;
            case BFCMAP_VFTHDR_DELETE:
                fval = 2;
                break;
            default:
                rv = BFCMAP_E_PARAM;
                goto exit;
            }
            bcm84756_a0_FCM_FCMI_VFTHDR_PROC_MODEr_VFTHDR_PRESENCEf_SET(mpc, reg_ivfm, fval);
        }
        if (mask1 & BFCMAP_ATTR_INGRESS_VFID_MAPSRC_MASK) {
            switch (pCfg->ingress_vfid_mapsrc) {
            case BFCMAP_VFID_MACSRC_PASSTHRU:
                fval = 0;
                break;
            case BFCMAP_VFID_MACSRC_PORT_DEFAULT:
                fval = 2;
                break;
            case BFCMAP_VFID_MACSRC_VID:
                fval = 3;
                break;
            case BFCMAP_VFID_MACSRC_MAPPER:
                fval = 4;
                break;
            case BFCMAP_VFID_MACSRC_DISABLE:
                BFCMAP_SAL_DBG_PRINTF("Port config parameter VFID DISABLE on ingress is Not Supported\n");
                rv = BFCMAP_E_PARAM;
                goto exit;
            default:
                rv = BFCMAP_E_PARAM;
                goto exit;
            }
            bcm84756_a0_FCM_FCMI_VFTHDR_PROC_MODEr_VFID_MAPSRCf_SET(mpc, reg_ivfm, fval);
        }
        if (mask1 & BFCMAP_ATTR_INGRESS_HOPCNT_CHECK_MODE_MASK) {
            switch (pCfg->ingress_hopCnt_check_mode) {
                case BFCMAP_HOPCNT_CHECK_MODE_NO_CHK:
                    fval = 0;
                    break;
                case BFCMAP_HOPCNT_CHECK_MODE_FAIL_DROP:
                    fval = 2;
                    break;
                case BFCMAP_HOPCNT_CHECK_MODE_FAIL_EOFNI:
                default:    
                    fval = 3;
                    break;  
            }
            bcm84756_a0_FCM_FCMI_VFTHDR_PROC_MODEr_HOPCOUNT_DISCARD_ENABLEf_SET(mpc, reg_ivfm, fval);
        }
        bcm84756_a0_WRITE_FCM_FCMI_VFTHDR_PROC_MODEr(mpc, reg_ivfm);
    }

   
    /* Mapper Egress Config */
    if ((mask1 & BFCMAP_ATTR_PORT_MODE_MASK) || 
        (mask1 & BFCMAP_ATTR_EGRESS_MAPPER_BYPASS_MASK) || 
        (mask1 & BFCMAP_ATTR_EGRESS_MAP_TABLE_INPUT_MASK) || 
        (mask1 & BFCMAP_ATTR_EGRESS_FC_CRC_MODE_MASK)) {
        bcm84756_a0_READ_FCM_FCME_CONFIGr(mpc, reg_ecfg);
        if (mask1 & BFCMAP_ATTR_PORT_MODE_MASK) {
            /* port mode */
            switch (pCfg->port_mode) {
                case BFCMAP_FCOE_TO_FCOE_MODE: /* FCoE Decap not done */
                    fval = 0;
                    break;
                case BFCMAP_FCOE_TO_FC_MODE:   /* FCoE Decap done */
                default:
                    fval = 1;
                    break;
            }
            bcm84756_a0_FCM_FCME_CONFIGr_DECAP_FCOE2FC_ENABLEf_SET(mpc, reg_ecfg, fval);
        }
        
        if (mask1 & BFCMAP_ATTR_EGRESS_MAPPER_BYPASS_MASK) {
            fval = (pCfg->egress_mapper_bypass) ? 1 : 0;
            bcm84756_a0_FCM_FCME_CONFIGr_MAPPER_BYPASS_ENABLEf_SET(mpc, reg_ecfg, fval);
        }
        
        if (mask1 & BFCMAP_ATTR_EGRESS_MAP_TABLE_INPUT_MASK) {
            switch (pCfg->egress_map_table_input) {
                case BFCMAP_MAP_TABLE_INPUT_VID:
                    fval = 0;
                    break;
                case BFCMAP_MAP_TABLE_INPUT_VFID:
                default:
                    fval = 1;
                    break;
            }
            bcm84756_a0_FCM_FCME_CONFIGr_MAP_TABLE_INPUTf_SET(mpc, reg_ecfg, fval);
        }
        
        if (mask1 & BFCMAP_ATTR_EGRESS_FC_CRC_MODE_MASK) {
            switch (pCfg->egress_fc_crc_mode) {
            case BFCMAP_FC_CRC_MODE_NORMAL:
                fval = 0;
                break;
            case BFCMAP_FC_CRC_MODE_NO_CRC_CHECK:
                fval = 1;
                break;
            default:
                rv = BFCMAP_E_PARAM;
                goto exit;
            }
            bcm84756_a0_FCM_FCME_CONFIGr_FCS_REGEN_MODEf_SET(mpc, reg_ecfg, fval);
        }
        bcm84756_a0_WRITE_FCM_FCME_CONFIGr(mpc, reg_ecfg);
    }
    
    if ((mask1 & BFCMAP_ATTR_EGRESS_VLANTAG_PROC_MODE_MASK) ||
        (mask1 & BFCMAP_ATTR_EGRESS_VID_MAPSRC_MASK) ||
        (mask1 & BFCMAP_ATTR_EGRESS_VLAN_PRI_MAP_MODE_MASK)) {
        /* Egress VLAN header processing mode */
        bcm84756_a0_READ_FCM_FCME_VLANTAG_PROC_MODEr(mpc, reg_evlm);
        if (mask1 & BFCMAP_ATTR_EGRESS_VLANTAG_PROC_MODE_MASK) {
            switch(pCfg->egress_vlantag_proc_mode) {
            case BFCMAP_VLANTAG_PRESERVE:
                fval = 0;
                break;
            case BFCMAP_VLANTAG_INSERT:
                fval = 1;
                break;
            case BFCMAP_VLANTAG_DELETE:
                fval = 2;
                break;
            default:
                rv = BFCMAP_E_PARAM;
                goto exit;
            }
            bcm84756_a0_FCM_FCME_VLANTAG_PROC_MODEr_VLANTAG_PRESENCEf_SET(mpc, reg_evlm, fval);
        }
        if (mask1 & BFCMAP_ATTR_EGRESS_VID_MAPSRC_MASK) {
            switch (pCfg->egress_vid_mapsrc) {
            case BFCMAP_VID_MACSRC_VFID:
                fval = 0;
                break;
            case BFCMAP_VID_MACSRC_PASSTHRU:
                fval = 1;
                break;
            case BFCMAP_VID_MACSRC_PORT_DEFAULT:
                fval = 2;
                break;
            case BFCMAP_VID_MACSRC_MAPPER:
                fval = 3;
                break;
            default:
                rv = BFCMAP_E_PARAM;
                goto exit;
            }
            bcm84756_a0_FCM_FCME_VLANTAG_PROC_MODEr_VLANTAG_MAPSRCf_SET(mpc, reg_evlm, fval);
        }
        if (mask1 & BFCMAP_ATTR_EGRESS_VLAN_PRI_MAP_MODE_MASK) {
            switch (pCfg->egress_vlan_pri_map_mode) {
            case BFCMAP_VLAN_PRI_MAP_MODE_PORT_DEFAULT:
                 fval = 0;
                 break;
            case BFCMAP_VLAN_PRI_MAP_MODE_PASSTHRU:
                fval = 1;
                break;
            default:
                rv = BFCMAP_E_PARAM;
                goto exit;
            }
            bcm84756_a0_FCM_FCME_VLANTAG_PROC_MODEr_PRI_MAP_MODEf_SET(mpc, reg_evlm, fval);
        }
        bcm84756_a0_WRITE_FCM_FCME_VLANTAG_PROC_MODEr(mpc, reg_evlm);
    }

    if ((mask1 & BFCMAP_ATTR_EGRESS_VFTHDR_PROC_MODE_MASK) ||
        (mask1 & BFCMAP_ATTR_EGRESS_VFID_MAPSRC_MASK) ||
        (mask2 & BFCMAP_ATTR2_EGRESS_HOPCNT_DEC_ENABLE_MASK)) {
        
        /* Engress VFT header processing mode */
        bcm84756_a0_READ_FCM_FCME_VFTHDR_PROC_MODEr(mpc, reg_evfm);
        if (mask1 & BFCMAP_ATTR_EGRESS_VFTHDR_PROC_MODE_MASK) {
            switch (pCfg->egress_vfthdr_proc_mode) {
            case BFCMAP_VFTHDR_PRESERVE:
                fval = 0;
                break;
            case BFCMAP_VFTHDR_INSERT:
                fval = 1;
                break;
            case BFCMAP_VFTHDR_DELETE:
                fval = 2;
                break;
            default:
                rv = BFCMAP_E_PARAM;
                goto exit;
            }
            bcm84756_a0_FCM_FCME_VFTHDR_PROC_MODEr_VFTHDR_PRESENCEf_SET(mpc, reg_evfm, fval);
        }
        if (mask1 & BFCMAP_ATTR_EGRESS_VFID_MAPSRC_MASK) {
            switch (pCfg->egress_vfid_mapsrc) {
            case BFCMAP_VFID_MACSRC_PASSTHRU:
                fval = 0;
                break;
            case BFCMAP_VFID_MACSRC_PORT_DEFAULT:
                fval = 2;
                break;
            case BFCMAP_VFID_MACSRC_VID:
                fval = 3;
                break;
            case BFCMAP_VFID_MACSRC_MAPPER:
                fval = 4;
                break;
            case BFCMAP_VFID_MACSRC_DISABLE:
                BFCMAP_SAL_DBG_PRINTF("Port config parameter VFID DISABLE on egress is Not Supported\n");
                rv = BFCMAP_E_PARAM;
                goto exit;

            default:
                rv = BFCMAP_E_PARAM;
                goto exit;
            }
            bcm84756_a0_FCM_FCME_VFTHDR_PROC_MODEr_VFID_MAPSRCf_SET(mpc, reg_evfm, fval);
        }
        if (mask2 & BFCMAP_ATTR2_EGRESS_HOPCNT_DEC_ENABLE_MASK) {   
            fval = (pCfg->egress_hopCnt_dec_enable) ? 1 : 0;
            bcm84756_a0_FCM_FCME_VFTHDR_PROC_MODEr_HOPCOUNT_DECR_ENABLEf_SET(mpc, reg_evfm, fval);
        }
        bcm84756_a0_WRITE_FCM_FCME_VFTHDR_PROC_MODEr(mpc, reg_evfm);
    }

    if (mask1 & BFCMAP_ATTR_INTERRUPT_ENABLE_MASK) {
        /* Enable Port interrupts */
        BFCMAP_LOCK_DEVICE(mpc->parent);

        bcm84756_a0_FCM_FCMON_RX_INTERRUPT_ENABLEr_CLR(reg_ien);
        fval = pCfg->interrupt_enable ? 0x3ff : 0;
        bcm84756_a0_FCM_FCMON_RX_INTERRUPT_ENABLEr_SET(reg_ien, fval);
        bcm84756_a0_WRITE_FCM_FCMON_RX_INTERRUPT_ENABLEr(mpc,reg_ien);

        BFCMAP_UNLOCK_DEVICE(mpc->parent);
    }

    if (mask1 & BFCMAP_ATTR_FW_ON_ACTIVE_8G_MASK) {
        switch (pCfg->fw_on_active_8g) {
        case BFCMAP_8G_FW_ON_ACTIVE_ARBFF:
            BFCMAP_MDIO_WRITE(mpc->uc_dev_addr, BFCMAP_PORT_UC_DATA_REG19(mpc), 0);  /* ARBFF */
            break;
            
        case BFCMAP_8G_FW_ON_ACTIVE_IDLE:
        default:
            BFCMAP_MDIO_WRITE(mpc->uc_dev_addr, BFCMAP_PORT_UC_DATA_REG19(mpc), 1);  /* IDLE */
            break;
        }
    }
    /* Save the current config. */
    BFCMAP_SAL_MEMCPY(&mpc->cfg, pCfg, sizeof(bfcmap_port_config_t));

exit:
    BFCMAP_UNLOCK_PORT(mpc);
    return rv;
}


/*
 * Function:
 *      bfcmap84756_port_config_selective_get
 * Purpose:
 *      Returns the current bfcmap port configuration.
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      pCfg         - Pointer to port configuration
 * Returns:
 *      BFCMAP_E_NONE   - Success
 *      BFCMAP_E_PARAM  - if pCfg is NULL
 */
STATIC int 
bfcmap84756_port_config_selective_get(bfcmap_port_ctrl_t *mpc, 
                                              bfcmap_port_config_t *pCfg)
{
    buint32_t                  fval;
    buint32_t                  hi;
    buint32_t                  lo;
    bcm84756_a0_FCM_FCMON_RX_INTERRUPT_ENABLEr_t reg_ien;
    bcm84756_a0_FCM_FCMON_RX_R_T_TOVr_t          reg_rxtov;
    bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_HIr_t    reg_src_hi;
    bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_LOr_t    reg_src_lo;
    bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_HIr_t    reg_dst_hi;
    bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_LOr_t    reg_dst_lo;
    bcm84756_a0_FCM_FCMI_CONFIGr_t               reg_icfg;
    bcm84756_a0_FCM_FCMI_MAC_SRCr_t              reg_imm;
    bcm84756_a0_FCM_FCMI_VFTHDR_PROC_MODEr_t     reg_ivfm;
    bcm84756_a0_FCM_FCMI_VLANTAG_PROC_MODEr_t    reg_ivlm;
    bcm84756_a0_FCM_FCME_VFTHDR_PROC_MODEr_t     reg_evfm;
    bcm84756_a0_FCM_FCME_VLANTAG_PROC_MODEr_t    reg_evlm;
    bcm84756_a0_FCM_FCME_CONFIGr_t               reg_ecfg;
    bcm84756_a0_FCM_DEFAULT_VLAN_TAGr_t          reg_vlan_tag;
    bcm84756_a0_FCM_DEFAULT_VFTHDRr_t            reg_vft_tag;
    bcm84756_a0_FCM_FCMAC_BBCr_t                 reg_bbc;
    bcm84756_a0_FCM_FCMAC_FRXB0r_t               reg_frxb0;
    bcm84756_a0_FCM_FCMAC_MAXFRMLEN_CONFIGr_t    reg_maxf;
    bcm84756_a0_FCM_FCMAC_BBCR_RECOVERYr_t       reg_bbscn;
    bfcmap_dev_addr_t mdio_addr = mpc->uc_dev_addr ;
    buint16_t fval16, speed;
    uint32  mask1, mask2;
    int rc = BFCMAP_E_NONE;
    
    if (!pCfg) {
        return BFCMAP_E_PARAM;
    }
    mask1 = pCfg->action_mask;
    mask2 = pCfg->action_mask2;
    BFCMAP_SAL_MEMSET(pCfg, 0, sizeof(bfcmap_port_config_t));
    pCfg->action_mask = mask1;
    pCfg->action_mask2 = mask2;

    /* These config parameters are not supported by MT1 */
    
    if ((mask2 & BFCMAP_ATTR2_USE_TTS_PCS_16G_MASK) ||
        (mask2 & BFCMAP_ATTR2_USE_TTS_PCS_32G_MASK) ||
        (mask2 & BFCMAP_ATTR2_TRAINING_ENABLE_16G_MASK) ||
        (mask2 & BFCMAP_ATTR2_TRAINING_ENABLE_32G_MASK) ||
        (mask2 & BFCMAP_ATTR2_FEC_ENABLE_16G_MASK) ||
        (mask2 & BFCMAP_ATTR2_FEC_ENABLE_32G_MASK) ||
        (mask2 & BFCMAP_ATTR2_INGRESS_FCS_CRRPT_EOF_ENABLE_MASK) ||
        (mask2 & BFCMAP_ATTR2_INGRESS_VLANTAG_PRESENCE_ENABLE_MASK) ||
        (mask2 & BFCMAP_ATTR2_EGRESS_HOPCNT_CHECK_MODE_MASK) ||
        (mask2 & BFCMAP_ATTR2_EGRESS_PASS_CTRL_FRAME_ENABLE_MASK) ||
        (mask2 & BFCMAP_ATTR2_EGRESS_PASS_PFC_FRAME_ENABLE_MASK) ||
        (mask2 & BFCMAP_ATTR2_EGRESS_PASS_PAUSE_FRAME_ENABLE_MASK) ||
        (mask2 & BFCMAP_ATTR2_EGRESS_FCOE_VER_CHK_DISABLE_MASK) ||
        (mask2 & BFCMAP_ATTR2_EGRESS_DEFAULT_COS_VALUE_MASK) ||
        (mask2 & BFCMAP_ATTR2_EGRESS_USE_IP_COS_MAP_MASK) ||
        (mask2 & BFCMAP_ATTR2_INGRESS_HOPCNT_DEC_ENABLE_MASK) ||
        (mask2 & BFCMAP_ATTR2_SCRAMBLING_ENABLE_MASK) || 
        (mask2 & BFCMAP_ATTR2_EGRESS_PAUSE_ENABLE_MASK) ||
        (mask2 & BFCMAP_ATTR2_EGRESS_PFC_ENABLE_MASK) ||
        (mask2 & BFCMAP_ATTR2_STAT_INTERVAL_MASK) ||
        (mask2 & BFCMAP_ATTR2_TRCM_ATTRIBS_MASK) ||
        (mask2 & BFCMAP_ATTR2_COS_TO_PRI_MASK)) {

        BFCMAP_SAL_DBG_PRINTF("Port configuration parameter is Not Supported\n");
        return BFCMAP_E_PARAM;
    }
    
    BFCMAP_LOCK_PORT(mpc);

    if (mask1 & BFCMAP_ATTR_MAX_FRAME_LENGTH_MASK) {
        /* Max frame length */
        bcm84756_a0_READ_FCM_FCMAC_MAXFRMLEN_CONFIGr(mpc, reg_maxf);
        pCfg->max_frame_length = 
            bcm84756_a0_FCM_FCMAC_MAXFRMLEN_CONFIGr_MAXLENf_GET(mpc, reg_maxf);
    }
    
    if (mask1 & BFCMAP_ATTR_RX_BB_CREDITS_MASK) {
        /* RX buffer Credits */
        bcm84756_a0_READ_FCM_FCMAC_FRXB0r(mpc, reg_frxb0);
        pCfg->rx_buffer_to_buffer_credits = 
                   bcm84756_a0_FCM_FCMAC_FRXB0r_FRXB_FREE_RXB_CNTf_GET(mpc,
                   reg_frxb0);
    }
    if (mask1 & BFCMAP_ATTR_TX_BB_CREDITS_MASK) {
        /* TX buffer Credits */
        bcm84756_a0_READ_FCM_FCMAC_BBCr(mpc, reg_bbc);
        pCfg->tx_buffer_to_buffer_credits =
                      bcm84756_a0_FCM_FCMAC_BBCr_BBC_BB_CREDITf_GET(mpc, reg_bbc);
    }

    if (mask1 & BFCMAP_ATTR_BB_SC_N_MASK) {
        /* BB_SC_N */
        bcm84756_a0_READ_FCM_FCMAC_BBCR_RECOVERYr(mpc, reg_bbscn);
        pCfg->bb_sc_n = bcm84756_a0_FCM_FCMAC_BBCR_RECOVERYr_BB_SC_Nf_GET(mpc, reg_bbscn); 
    }
    if ((mask1 & BFCMAP_ATTR_PORT_STATE_MASK) ||
        (mask1 & BFCMAP_ATTR_SPEED_MASK)) {
        /* Read Link Speed from the LMI register */
        /* Firmware updates Register 0x50 once AN completes. */
        if ( BFCMAP_MDIO_READ(mdio_addr, 0x50, &fval16) != BLMI_E_NONE) {
            rc = BFCMAP_E_FAIL;
            goto exit;
        }
        if (mask1 & BFCMAP_ATTR_PORT_STATE_MASK) {
            if (fval16 != BFCMAP_UC_LINK_UP) {
                pCfg->port_state = BFCMAP_PORT_STATE_LINKDOWN;
            } else {
                pCfg->port_state = BFCMAP_PORT_STATE_ACTIVE;
            }
        }
        if (mask1 & BFCMAP_ATTR_SPEED_MASK) {
            if (fval16 == BFCMAP_UC_LINK_UP) {
                if (BFCMAP_MDIO_READ(mdio_addr, 0x52, &speed) != BLMI_E_NONE) {
                    rc = BFCMAP_E_FAIL;
                    goto exit;
                }
            }
            else {
                speed = 8;
            }
            /* port speed */
            switch (speed) {
                case 8:
                    pCfg->speed = BFCMAP_PORT_SPEED_8GBPS;
                    break;
                case 4:
                    pCfg->speed = BFCMAP_PORT_SPEED_4GBPS;
                    break;
                case 2:
                default:
                    pCfg->speed = BFCMAP_PORT_SPEED_2GBPS;
                    break;
            }
        }
    }   

    if (mask1 & BFCMAP_ATTR_R_T_TOV_MASK) {
        /* RX R_T_TOV  */
        bcm84756_a0_READ_FCM_FCMON_RX_R_T_TOVr(mpc,reg_rxtov);
        fval = bcm84756_a0_FCM_FCMON_RX_R_T_TOVr_R_T_TOVf_GET(mpc,reg_rxtov);
        pCfg->r_t_tov = fval ;
    }

    if (mask1 & BFCMAP_ATTR_VLAN_TAG_MASK) {
        /* VLAN Tag */
        bcm84756_a0_READ_FCM_DEFAULT_VLAN_TAGr(mpc, reg_vlan_tag);
        pCfg->vlan_tag =  (uint32)reg_vlan_tag.v[0];
    }

    if (mask1 & BFCMAP_ATTR_VFT_TAG_MASK) {
        /* VFT Tag */
        bcm84756_a0_READ_FCM_DEFAULT_VFTHDRr(mpc, reg_vft_tag);
        pCfg->vft_tag =  (uint32)reg_vft_tag.v[0];
    }

    /* Mapper Ingress Config */
    if ((mask1 & BFCMAP_ATTR_INGRESS_MAPPER_BYPASS_MASK) || 
        (mask1 & BFCMAP_ATTR_PORT_MODE_MASK) ||
        (mask1 & BFCMAP_ATTR_INGRESS_MAP_TABLE_INPUT_MASK) || 
        (mask1 & BFCMAP_ATTR_INGRESS_FC_CRC_MODE_MASK)) {
        bcm84756_a0_READ_FCM_FCMI_CONFIGr(mpc, reg_icfg);
        if (mask1 & BFCMAP_ATTR_PORT_MODE_MASK) {
            fval = bcm84756_a0_FCM_FCMI_CONFIGr_ENCAP_FC2FCOE_ENf_GET(mpc, reg_icfg);
            pCfg->port_mode =  (fval == 1) ? BFCMAP_FCOE_TO_FC_MODE : BFCMAP_FCOE_TO_FCOE_MODE ;
        }
        if (mask1 & BFCMAP_ATTR_INGRESS_MAPPER_BYPASS_MASK) {
            fval = bcm84756_a0_FCM_FCMI_CONFIGr_MAPPER_BYPASS_ENABLEf_GET(mpc, reg_icfg);
            pCfg->ingress_mapper_bypass = fval;
        }
        if (mask1 & BFCMAP_ATTR_INGRESS_MAP_TABLE_INPUT_MASK) {
            fval = bcm84756_a0_FCM_FCMI_CONFIGr_MAP_TABLE_INPUTf_GET(mpc, reg_icfg);
            pCfg->ingress_map_table_input = (fval == 1) ? BFCMAP_MAP_TABLE_INPUT_VID : BFCMAP_MAP_TABLE_INPUT_VFID;
        }
        if (mask1 & BFCMAP_ATTR_INGRESS_FC_CRC_MODE_MASK) {
            fval = bcm84756_a0_FCM_FCMI_CONFIGr_FCS_REGEN_MODEf_GET(mpc, reg_icfg);
            pCfg->ingress_fc_crc_mode = (fval == 1) ? BFCMAP_FC_CRC_MODE_NO_CRC_CHECK : BFCMAP_FC_CRC_MODE_NORMAL;
        }
    }
    if ((mask1 & BFCMAP_ATTR_SRC_MAC_CONSTRUCT_MASK) ||
        (mask1 & BFCMAP_ATTR_DST_MAC_CONSTRUCT_MASK)) {
        bcm84756_a0_READ_FCM_FCMI_MAC_SRCr(mpc, reg_imm);
        if (mask1 & BFCMAP_ATTR_SRC_MAC_CONSTRUCT_MASK) {
            fval = bcm84756_a0_FCM_FCMI_MAC_SRCr_FCOE_SA_MODEf_GET(mpc, reg_imm);
            switch (fval) {
                case 0:
                    pCfg->src_mac_construct = BFCMAP_ENCAP_FCOE_FPMA;
                    bcm84756_a0_READ_FCM_DEFAULT_FCOE_SRCMAC_HIr(mpc,reg_src_hi);
                    hi = bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_HIr_SRCMACf_GET(mpc, reg_src_hi);
                    pCfg->src_fcmap_prefix = (hi & 0xffff) << 8; 

                    bcm84756_a0_READ_FCM_DEFAULT_FCOE_SRCMAC_LOr(mpc,reg_src_lo);
                    lo = bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_LOr_SRCMACf_GET(mpc, reg_src_lo);
                    pCfg->src_fcmap_prefix |= (lo & 0xff000000) >> 24;
                    break;
                case 1:
                    pCfg->src_mac_construct = BFCMAP_ENCAP_FCOE_ETH_ADDRESS_NULL;
                    break;
                case 2:
                default:    
                    pCfg->src_mac_construct = BFCMAP_ENCAP_FCOE_ETH_ADDRESS_USER;
                    /* Source MAC */
                    bcm84756_a0_READ_FCM_DEFAULT_FCOE_SRCMAC_HIr(mpc,reg_src_hi);
                    hi = bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_HIr_SRCMACf_GET(mpc, reg_src_hi);
                    
                    bcm84756_a0_READ_FCM_DEFAULT_FCOE_SRCMAC_LOr(mpc,reg_src_lo);
                    lo = bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_LOr_SRCMACf_GET(mpc, reg_src_lo);
                    BMAC_BUILD_FROM_32(pCfg->src_mac_addr, hi, lo);
                    break;
            }
        }
        if (mask1 & BFCMAP_ATTR_DST_MAC_CONSTRUCT_MASK) {
            fval = bcm84756_a0_FCM_FCMI_MAC_SRCr_FCOE_DA_MODEf_GET(mpc, reg_imm);
            switch (fval) {
                case 0:
                    pCfg->dst_mac_construct = BFCMAP_ENCAP_FCOE_FPMA;
                    bcm84756_a0_READ_FCM_DEFAULT_FCOE_DSTMAC_HIr(mpc,reg_dst_hi);
                    hi = bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_HIr_DSTMACf_GET(mpc, reg_dst_hi);
                    pCfg->dst_fcmap_prefix = (hi & 0xffff) << 8; 

                    bcm84756_a0_READ_FCM_DEFAULT_FCOE_DSTMAC_LOr(mpc,reg_dst_lo);
                    lo = bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_LOr_DSTMACf_GET(mpc, reg_dst_lo);
                    pCfg->dst_fcmap_prefix |= (lo & 0xff000000) >> 24;
                    break;
                case 1:
                    pCfg->dst_mac_construct = BFCMAP_ENCAP_FCOE_ETH_ADDRESS_NULL;
                    break;
                case 2:
                default:    
                    pCfg->dst_mac_construct = BFCMAP_ENCAP_FCOE_ETH_ADDRESS_USER;
                    /* Destination MAC */
                    bcm84756_a0_READ_FCM_DEFAULT_FCOE_DSTMAC_HIr(mpc,reg_dst_hi);
                    hi = bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_HIr_DSTMACf_GET(mpc, reg_dst_hi);
                    
                    bcm84756_a0_READ_FCM_DEFAULT_FCOE_DSTMAC_LOr(mpc,reg_dst_lo);
                    lo = bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_LOr_DSTMACf_GET(mpc, reg_dst_lo);
                    
                    BMAC_BUILD_FROM_32(pCfg->dst_mac_addr, hi, lo);
                    break;
            }
        }
    }
    if ((mask1 & BFCMAP_ATTR_INGRESS_VLANTAG_PROC_MODE_MASK) ||
        (mask1 & BFCMAP_ATTR_INGRESS_VID_MAPSRC_MASK) ||
        (mask1 & BFCMAP_ATTR_INGRESS_VLAN_PRI_MAP_MODE_MASK)) {
        bcm84756_a0_READ_FCM_FCMI_VLANTAG_PROC_MODEr(mpc, reg_ivlm);
        if (mask1 & BFCMAP_ATTR_INGRESS_VLANTAG_PROC_MODE_MASK) {
            /* Ingress VLAN header processing mode */
            fval = bcm84756_a0_FCM_FCMI_VLANTAG_PROC_MODEr_VLANTAG_PRESENCEf_GET(mpc, reg_ivlm);
            pCfg->ingress_vlantag_proc_mode = ((fval == 0) ? BFCMAP_VLANTAG_PRESERVE : 
                                                             ((fval == 1)? BFCMAP_VLANTAG_INSERT : BFCMAP_VLANTAG_DELETE));
        }
        if (mask1 & BFCMAP_ATTR_INGRESS_VID_MAPSRC_MASK) {
            _shr_bfcmap_vid_mapsrc_t  tmp;
            fval = bcm84756_a0_FCM_FCMI_VLANTAG_PROC_MODEr_VLANTAG_MAPSRCf_GET(mpc, reg_ivlm);
            switch (fval) {
            case 0:
                tmp = BFCMAP_VID_MACSRC_VFID;
                break;
            case 1:
                tmp = BFCMAP_VID_MACSRC_PASSTHRU;
                break;
            case 2:
                tmp = BFCMAP_VID_MACSRC_PORT_DEFAULT;
                break;
            case 3:
            default:    
                tmp = BFCMAP_VID_MACSRC_MAPPER;
                break;
            }
            pCfg->ingress_vid_mapsrc = tmp;
        }
        if (mask1 & BFCMAP_ATTR_INGRESS_VLAN_PRI_MAP_MODE_MASK) {
            fval = bcm84756_a0_FCM_FCMI_VLANTAG_PROC_MODEr_PRI_MAP_MODEf_GET(mpc, reg_ivlm);
            pCfg->ingress_vlan_pri_map_mode = (fval == 1)? BFCMAP_VLAN_PRI_MAP_MODE_PASSTHRU : BFCMAP_VLAN_PRI_MAP_MODE_PORT_DEFAULT;
        }
    }

    if ((mask1 & BFCMAP_ATTR_INGRESS_VFTHDR_PROC_MODE_MASK) ||
        (mask1 & BFCMAP_ATTR_INGRESS_VFID_MAPSRC_MASK) ||
        (mask1 & BFCMAP_ATTR_INGRESS_HOPCNT_CHECK_MODE_MASK)) {
        /* Ingress VFT header processing mode */
        bcm84756_a0_READ_FCM_FCMI_VFTHDR_PROC_MODEr(mpc, reg_ivfm);
        if (mask1 & BFCMAP_ATTR_INGRESS_VFTHDR_PROC_MODE_MASK) {
            _shr_bfcmap_vfthdr_proc_mode_t tmp;
            fval = bcm84756_a0_FCM_FCMI_VFTHDR_PROC_MODEr_VFTHDR_PRESENCEf_GET(mpc, reg_ivfm);
            switch (fval) {
            case 0:
                tmp = BFCMAP_VFTHDR_PRESERVE;
                break;
            case 1:
                tmp = BFCMAP_VFTHDR_INSERT;
                break;
            case 2:
            default:    
                tmp = BFCMAP_VFTHDR_DELETE;
                break;
            }
            pCfg->ingress_vfthdr_proc_mode = tmp;
        }
        if (mask1 & BFCMAP_ATTR_INGRESS_VFID_MAPSRC_MASK) {
            _shr_bfcmap_vfid_mapsrc_t tmp;
            fval = bcm84756_a0_FCM_FCMI_VFTHDR_PROC_MODEr_VFID_MAPSRCf_GET(mpc, reg_ivfm);
            switch (fval) {
            case 0:
            case 1:    
                tmp = BFCMAP_VFID_MACSRC_PASSTHRU;
                break;
            case 2:
                tmp = BFCMAP_VFID_MACSRC_PORT_DEFAULT;
                break;
            case 3: 
                tmp = BFCMAP_VFID_MACSRC_VID;
                break;
            case 4:
            default:    
                tmp = BFCMAP_VFID_MACSRC_MAPPER;
                break;
            }
            pCfg->ingress_vfid_mapsrc = tmp;
        }
        if (mask1 & BFCMAP_ATTR_INGRESS_HOPCNT_CHECK_MODE_MASK) {
            _shr_bfcmap_hopcnt_check_mode_t tmp;
            fval = bcm84756_a0_FCM_FCMI_VFTHDR_PROC_MODEr_HOPCOUNT_DISCARD_ENABLEf_GET(mpc, reg_ivfm);
            switch (fval) {
                case 0:
                case 1:    
                    tmp = BFCMAP_HOPCNT_CHECK_MODE_NO_CHK;
                    break;
                case 2:
                    tmp = BFCMAP_HOPCNT_CHECK_MODE_FAIL_DROP;
                    break;
                case 3:
                default:    
                    tmp = BFCMAP_HOPCNT_CHECK_MODE_FAIL_EOFNI;
                    break;  
            }
            pCfg->ingress_hopCnt_check_mode = tmp;
        }
    }


    if (mask1 & BFCMAP_ATTR_MAPPER_LEN_MASK) {
        /* Mapper Egress Config */
        pCfg->mapper_len = 64 ;
    }

    if ((mask1 & BFCMAP_ATTR_EGRESS_MAPPER_BYPASS_MASK) || 
        (mask1 & BFCMAP_ATTR_PORT_MODE_MASK) ||
        (mask1 & BFCMAP_ATTR_EGRESS_MAP_TABLE_INPUT_MASK) || 
        (mask1 & BFCMAP_ATTR_EGRESS_FC_CRC_MODE_MASK)) {
        bcm84756_a0_READ_FCM_FCME_CONFIGr(mpc, reg_ecfg);
        
        if (mask1 & BFCMAP_ATTR_PORT_MODE_MASK) {
            fval = bcm84756_a0_FCM_FCME_CONFIGr_DECAP_FCOE2FC_ENABLEf_GET(mpc, reg_ecfg);
            pCfg->port_mode =  (fval == 1) ? BFCMAP_FCOE_TO_FC_MODE : BFCMAP_FCOE_TO_FCOE_MODE ;
        }
        if (mask1 & BFCMAP_ATTR_EGRESS_MAPPER_BYPASS_MASK) {
            fval = bcm84756_a0_FCM_FCME_CONFIGr_MAPPER_BYPASS_ENABLEf_GET(mpc, reg_ecfg);
            pCfg->egress_mapper_bypass = fval;
        }
        if (mask1 & BFCMAP_ATTR_EGRESS_MAP_TABLE_INPUT_MASK) {
            fval = bcm84756_a0_FCM_FCME_CONFIGr_MAP_TABLE_INPUTf_GET(mpc, reg_ecfg);
            pCfg->egress_map_table_input = (fval == 1) ? BFCMAP_MAP_TABLE_INPUT_VFID : BFCMAP_MAP_TABLE_INPUT_VID;
        }
        if (mask1 & BFCMAP_ATTR_EGRESS_FC_CRC_MODE_MASK) {
            fval = bcm84756_a0_FCM_FCME_CONFIGr_FCS_REGEN_MODEf_GET(mpc, reg_ecfg);
            pCfg->egress_fc_crc_mode = (fval == 1) ? BFCMAP_FC_CRC_MODE_NO_CRC_CHECK : BFCMAP_FC_CRC_MODE_NORMAL;
        }
    }

    if ((mask1 & BFCMAP_ATTR_EGRESS_VLANTAG_PROC_MODE_MASK) ||
        (mask1 & BFCMAP_ATTR_EGRESS_VID_MAPSRC_MASK) ||
        (mask1 & BFCMAP_ATTR_EGRESS_VLAN_PRI_MAP_MODE_MASK)) {
        bcm84756_a0_READ_FCM_FCME_VLANTAG_PROC_MODEr(mpc, reg_evlm);
        if (mask1 & BFCMAP_ATTR_EGRESS_VLANTAG_PROC_MODE_MASK) {
            /* Egress VLAN header processing mode */
            fval = bcm84756_a0_FCM_FCME_VLANTAG_PROC_MODEr_VLANTAG_PRESENCEf_GET(mpc, reg_evlm);
            pCfg->egress_vlantag_proc_mode = ((fval == 0) ? BFCMAP_VLANTAG_PRESERVE : 
                                                             ((fval == 1)? BFCMAP_VLANTAG_INSERT : BFCMAP_VLANTAG_DELETE));
        }
        if (mask1 & BFCMAP_ATTR_EGRESS_VID_MAPSRC_MASK) {
            _shr_bfcmap_vid_mapsrc_t  tmp;
            fval = bcm84756_a0_FCM_FCME_VLANTAG_PROC_MODEr_VLANTAG_MAPSRCf_GET(mpc, reg_evlm);
            switch (fval) {
            case 0:
                tmp = BFCMAP_VID_MACSRC_VFID;
                break;
            case 1:
                tmp = BFCMAP_VID_MACSRC_PASSTHRU;
                break;
            case 2:
                tmp = BFCMAP_VID_MACSRC_PORT_DEFAULT;
                break;
            case 3:    
            default:    
                tmp = BFCMAP_VID_MACSRC_MAPPER;
                break;
            }
            pCfg->egress_vid_mapsrc = tmp;
        }
        if (mask1 & BFCMAP_ATTR_EGRESS_VLAN_PRI_MAP_MODE_MASK) {
            fval = bcm84756_a0_FCM_FCME_VLANTAG_PROC_MODEr_PRI_MAP_MODEf_GET(mpc, reg_evlm);
            pCfg->egress_vlan_pri_map_mode = (fval == 1)? BFCMAP_VLAN_PRI_MAP_MODE_PASSTHRU : BFCMAP_VLAN_PRI_MAP_MODE_PORT_DEFAULT;
        }
    }

    if ((mask1 & BFCMAP_ATTR_EGRESS_VFTHDR_PROC_MODE_MASK) ||
        (mask1 & BFCMAP_ATTR_EGRESS_VFID_MAPSRC_MASK) ||
        (mask2 & BFCMAP_ATTR2_EGRESS_HOPCNT_DEC_ENABLE_MASK)) {
        
        /* Egress VFT header processing mode */
        bcm84756_a0_READ_FCM_FCME_VFTHDR_PROC_MODEr(mpc, reg_evfm);
        if (mask1 & BFCMAP_ATTR_EGRESS_VFTHDR_PROC_MODE_MASK) {
            _shr_bfcmap_vfthdr_proc_mode_t tmp;
            fval = bcm84756_a0_FCM_FCME_VFTHDR_PROC_MODEr_VFTHDR_PRESENCEf_GET(mpc, reg_evfm);
            switch (fval) {
            case 0:
                tmp = BFCMAP_VFTHDR_PRESERVE;
                break;
            case 1:
                tmp = BFCMAP_VFTHDR_INSERT;
                break;
            case 2:
            default:    
                tmp = BFCMAP_VFTHDR_DELETE;
                break;
            }
            pCfg->egress_vfthdr_proc_mode = tmp;
        }
        if (mask1 & BFCMAP_ATTR_EGRESS_VFID_MAPSRC_MASK) {
            _shr_bfcmap_vfid_mapsrc_t tmp;
            fval = bcm84756_a0_FCM_FCME_VFTHDR_PROC_MODEr_VFID_MAPSRCf_GET(mpc, reg_evfm);
            switch (fval) {
            case 0:
            case 1:    
                tmp = BFCMAP_VFID_MACSRC_PASSTHRU;
                break;
            case 2:
                tmp = BFCMAP_VFID_MACSRC_PORT_DEFAULT;
                break;
            case 3: 
                tmp = BFCMAP_VFID_MACSRC_VID;
                break;
            case 4:
            default:    
                tmp = BFCMAP_VFID_MACSRC_MAPPER;
                break;
            }
            pCfg->egress_vfid_mapsrc = tmp;
        }
        if (mask2 & BFCMAP_ATTR2_EGRESS_HOPCNT_DEC_ENABLE_MASK) {
            fval = bcm84756_a0_FCM_FCME_VFTHDR_PROC_MODEr_HOPCOUNT_DECR_ENABLEf_GET(mpc, reg_evfm);
            pCfg->egress_hopCnt_dec_enable = fval;
        }
    }

    if (mask1 & BFCMAP_ATTR_INTERRUPT_ENABLE_MASK) {
        bcm84756_a0_READ_FCM_FCMON_RX_INTERRUPT_ENABLEr(mpc,reg_ien);
        fval = bcm84756_a0_FCM_FCMON_RX_INTERRUPT_ENABLEr_GET(reg_ien);
        if (fval == 0x3ff) 
            pCfg->interrupt_enable = 1;
        else
            pCfg->interrupt_enable = 0;
    }

    if (mask1 & BFCMAP_ATTR_FW_ON_ACTIVE_8G_MASK) {
        if ( BFCMAP_MDIO_READ(mdio_addr, BFCMAP_PORT_UC_DATA_REG19(mpc), &fval16) != BLMI_E_NONE) {
            rc = BFCMAP_E_FAIL;
            goto exit;
        }
        pCfg->fw_on_active_8g = (fval16 == 0)? BFCMAP_8G_FW_ON_ACTIVE_ARBFF: BFCMAP_8G_FW_ON_ACTIVE_IDLE;
    }
exit:    
    BFCMAP_UNLOCK_PORT(mpc);
    return rc;
}



/*
 * Function:
 *      bfcmap84756_port_init
 * Purpose:
 *      Initializes a bfcmap port.
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      pCfg        - Pointer to port configuration
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */
STATIC int 
bfcmap84756_port_init(bfcmap_port_ctrl_t *mpc, bfcmap_port_config_t *pCfg)
{
    int                  rv = BFCMAP_E_NONE;
    buint32_t      fval;
    /* buint16_t      cksum; */  /* Disabling checksum checks */
    buint16_t      ver;
    
    bcm84756_a0_FCM_FCMAC_PCFGr_t  reg_pcfg;
    bcm84756_a0_FCM_FCMAC_PCFG1r_t  reg_pcfg1;
    bcm84756_a0_FCM_FCMON_RX_CONTROLr_t  reg_rxc;
    bcm84756_a0_FCM_LINK_RATEr_t  reg_link;
    bcm84756_a0_FCM_FCMAC_MINFRMLEN_CONFIGr_t  reg_minf;
    bcm84756_a0_FCM_FCMAC_MAXFRMLEN_CONFIGr_t  reg_maxf;
    bcm84756_a0_FCM_FCMAC_BADRXCHAR_SUBSTVALr_t reg_badc;
    bcm84756_a0_FCM_FCMAC_RTTRr_t  reg_rttr;
    bcm84756_a0_FCM_FCMON_RX_R_T_TOVr_t reg_rxtov;
    bcm84756_a0_FCM_PCS_CONFIGr_t   reg_pcs;
    bcm84756_a0_FCM_FCMAC_FCFGr_t  reg_fcfg;
    bcm84756_a0_FCM_FCMAC_FRXB0r_t  reg_frxb0;
    bcm84756_a0_FCM_FCMAC_BBCr_t    reg_bbc;
    bcm84756_a0_FCM_FCGEN_TX_CONTROLr_t  reg_txc;
#if 0
    bcm84756_a0_FCM_TX_DISABLEr_t reg_txdisable;
#endif
    bcm84756_a0_MPORT_SW_XPORT_RSV_MASKr_t reg_rsv_mask; 
#if 0
    bcm84756_a0_MPORT_LN_XPORT_RSV_MASKr_t reg_ln_rsv_mask; 
    bcm84756_a0_SP_INGRESS_MTU0_1r_t reg_ingress_mtu0_1; 
    bcm84756_a0_SP_MTU0r_t reg_sp_mtu0; 
#endif
    bcm84756_a0_TX_BUFFER_CONFIGr_t reg_tx_buffer; 
    bcm84756_a0_RX_BUFFER_CONFIGr_t reg_rx_buffer; 
    bcm84756_a0_FCM_FCME_CONFIGr_t reg_fcme_config; 
    bcm84756_a0_FCM_FCMI_CONFIGr_t reg_fcmi_config; 
    bcm84756_a0_FCM_FCME_VFTHDR_PROC_MODEr_t     reg_evfm;
    bcm84756_a0_FCM_FCMI_VFTHDR_PROC_MODEr_t     reg_ivfm;
    bcm84756_a0_FCM_FCMI_VLANTAG_PROC_MODEr_t    reg_ivlm;
    bcm84756_a0_FCM_FCME_VLANTAG_PROC_MODEr_t    reg_evlm;
    bcm84756_a0_FCM_FCMI_MAC_SRCr_t              reg_imm;
    bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_HIr_t    reg_src_hi;
    bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_LOr_t    reg_src_lo;
    bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_HIr_t    reg_dst_hi;
    bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_LOr_t    reg_dst_lo;
    bcm84756_a0_FCM_DEFAULT_VLAN_TAGr_t reg_vlan_tag;
    bcm84756_a0_FCM_DEFAULT_VFTHDRr_t reg_vfthdr;
    bcm84756_c0_MPORT_EGRESS_DATA_TRUNCATION_THDr_t reg_mport_edtt;
    bcm84756_c0_MPORT_EGRESS_HDR_TRUNCATION_THDr_t  reg_mport_ehtt;
    bcm84756_c0_FCM_FCMAC_PCFG2r_t    reg_pcfg2;
    
    bfcmap_dev_addr_t mdio_addr = mpc->uc_dev_addr;
    

    BFCMAP_LOCK_PORT(mpc);

    /* Enable Tx and RX, and DISABLE L_PORT */
    bcm84756_a0_FCM_FCMAC_PCFGr_CLR(reg_pcfg);
    bcm84756_a0_FCM_FCMAC_PCFGr_PCFG_RXEf_SET(mpc,reg_pcfg, 1);
    bcm84756_a0_FCM_FCMAC_PCFGr_PCFG_LPDf_SET(mpc,reg_pcfg, 1);
    bcm84756_a0_FCM_FCMAC_PCFGr_PCFG_TXEf_SET(mpc,reg_pcfg, 1);
    bcm84756_a0_WRITE_FCM_FCMAC_PCFGr(mpc,reg_pcfg);

    /* PCFG1 = 0x0 */
    bcm84756_a0_FCM_FCMAC_PCFG1r_CLR(reg_pcfg1);
    bcm84756_a0_WRITE_FCM_FCMAC_PCFG1r(mpc,reg_pcfg1);

    /* Word Sync on K28.5; Comma detection Window 100us */
    bcm84756_a0_FCM_FCMON_RX_CONTROLr_CLR(reg_rxc);
    bcm84756_a0_FCM_FCMON_RX_CONTROLr_RX10B_BIT_ORDERf_SET(mpc,reg_rxc, 0);
    bcm84756_a0_WRITE_FCM_FCMON_RX_CONTROLr(mpc,reg_rxc);

    /* TX control TX speed=8G  */
    bcm84756_a0_FCM_FCGEN_TX_CONTROLr_CLR(reg_txc);
    bcm84756_a0_FCM_FCGEN_TX_CONTROLr_FORCE_SPEED_NOT_ACQUIREDf_SET(mpc,reg_txc, 1);
    bcm84756_a0_FCM_FCGEN_TX_CONTROLr_TRANSMIT_LSB_FIRSTf_SET(mpc,reg_txc, 1);
    bcm84756_a0_WRITE_FCM_FCGEN_TX_CONTROLr(mpc,reg_txc);

    /* 8G speed on TX and RX */
    bcm84756_a0_FCM_LINK_RATEr_CLR(reg_link);
    bcm84756_a0_FCM_LINK_RATEr_RX_RATEf_SET(mpc,reg_link, 2);
    bcm84756_a0_FCM_LINK_RATEr_TX_RATEf_SET(mpc,reg_link, 2);
    bcm84756_a0_WRITE_FCM_LINK_RATEr(mpc,reg_link);

    /* Turn on laser in uCode at the beginning of AN */ 
#if 0
    /* TX Disable = 0*/
    bcm84756_a0_FCM_TX_DISABLEr_CLR(reg_txdisable);
    bcm84756_a0_WRITE_FCM_TX_DISABLEr(mpc,reg_txdisable);
#endif

    /* MINFRMLEN_CONFIG = 9 words  */
    bcm84756_a0_FCM_FCMAC_MINFRMLEN_CONFIGr_CLR(reg_minf);
    bcm84756_a0_FCM_FCMAC_MINFRMLEN_CONFIGr_MINLENf_SET(mpc,reg_minf, 9);
    bcm84756_a0_WRITE_FCM_FCMAC_MINFRMLEN_CONFIGr(mpc,reg_minf);

    /* MAX frame config = 2156 bytes = ? words  */
    bcm84756_a0_FCM_FCMAC_MAXFRMLEN_CONFIGr_CLR(reg_maxf);
    bcm84756_a0_FCM_FCMAC_MAXFRMLEN_CONFIGr_MAXLENf_SET(mpc,reg_maxf, 0x21b);
    bcm84756_a0_WRITE_FCM_FCMAC_MAXFRMLEN_CONFIGr(mpc,reg_maxf);

    /* BAD Character Substitute value */
    bcm84756_a0_FCM_FCMAC_BADRXCHAR_SUBSTVALr_CLR(reg_badc);
    bcm84756_a0_FCM_FCMAC_BADRXCHAR_SUBSTVALr_CODE_8B_SUBSTf_SET(mpc,reg_badc, 0);
    bcm84756_a0_WRITE_FCM_FCMAC_BADRXCHAR_SUBSTVALr(mpc,reg_badc);

    /* R_T_TOV  */
    bcm84756_a0_FCM_FCMAC_RTTRr_CLR(reg_rttr);
    bcm84756_a0_FCM_FCMAC_RTTRr_RTTR_R_T_TOVf_SET(mpc,reg_rttr, 0x64);
    bcm84756_a0_WRITE_FCM_FCMAC_RTTRr(mpc,reg_rttr);

    /* RX R_T_TOV  */
    bcm84756_a0_FCM_FCMON_RX_R_T_TOVr_CLR(reg_rxtov);
    fval = 61035;
    bcm84756_a0_FCM_FCMON_RX_R_T_TOVr_R_T_TOVf_SET(mpc,reg_rxtov, fval);
    bcm84756_a0_WRITE_FCM_FCMON_RX_R_T_TOVr(mpc,reg_rxtov);

    /* PCS = 0x0 */
    bcm84756_a0_FCM_PCS_CONFIGr_CLR(reg_pcs);
    bcm84756_a0_WRITE_FCM_PCS_CONFIGr(mpc,reg_pcs);

    /*  force config : Force in OLDP */
    bcm84756_a0_FCM_FCMAC_FCFGr_CLR(reg_fcfg);
    bcm84756_a0_FCM_FCMAC_FCFGr_CFG_FORCE_OLDPf_SET(mpc,reg_fcfg, 1);
    bcm84756_a0_WRITE_FCM_FCMAC_FCFGr(mpc,reg_fcfg);

    bcm84756_a0_FCM_FCMAC_FCFGr_CLR(reg_fcfg);
    bcm84756_a0_FCM_FCMAC_FCFGr_CFG_FORCE_OLDPf_SET(mpc,reg_fcfg, 0);
    bcm84756_a0_WRITE_FCM_FCMAC_FCFGr(mpc,reg_fcfg);

    /* Configure R_RDY */
    bcm84756_a0_FCM_FCMAC_FRXB0r_CLR(reg_frxb0);
    bcm84756_a0_FCM_FCMAC_FRXB0r_FRXB_FREE_RXB_CNTf_SET(mpc, reg_frxb0, 15);
    bcm84756_a0_WRITE_FCM_FCMAC_FRXB0r(mpc,reg_frxb0);

    /*  force config : Force in OL1D */
    bcm84756_a0_FCM_FCMAC_FCFGr_CLR(reg_fcfg);
    bcm84756_a0_FCM_FCMAC_FCFGr_CFG_FORCE_OL1Df_SET(mpc,reg_fcfg, 1);
    bcm84756_a0_WRITE_FCM_FCMAC_FCFGr(mpc,reg_fcfg);

    bcm84756_a0_FCM_FCMAC_FCFGr_CLR(reg_fcfg);
    bcm84756_a0_FCM_FCMAC_FCFGr_CFG_FORCE_OLDPf_SET(mpc,reg_fcfg, 0);
    bcm84756_a0_WRITE_FCM_FCMAC_FCFGr(mpc,reg_fcfg);

    /* Config BBC      */
    bcm84756_a0_FCM_FCMAC_BBCr_CLR(reg_bbc);
    fval = 0x20;
    bcm84756_a0_FCM_FCMAC_BBCr_BBC_BB_CREDITf_SET(mpc,reg_bbc, fval);
    bcm84756_a0_WRITE_FCM_FCMAC_BBCr(mpc,reg_bbc);

    /* Set RSV Masks */
    bcm84756_a0_MPORT_SW_XPORT_RSV_MASKr_CLR(reg_rsv_mask); 
    bcm84756_a0_MPORT_SW_XPORT_RSV_MASKr_XPORT_RSV_MASKf_SET(mpc, reg_rsv_mask, 0x1004C); 
    bcm84756_a0_WRITE_MPORT_SW_XPORT_RSV_MASKr(mpc,reg_rsv_mask);

#if 0
    bcm84756_a0_MPORT_LN_XPORT_RSV_MASKr_CLR(reg_ln_rsv_mask); 
    bcm84756_a0_MPORT_LN_XPORT_RSV_MASKr_XPORT_RSV_MASKf_SET(mpc, reg_ln_rsv_mask, 0x30054); 
    bcm84756_a0_WRITE_MPORT_LN_XPORT_RSV_MASKr(mpc,reg_ln_rsv_mask);

    bcm84756_a0_SP_INGRESS_MTU0_1r_CLR(reg_ingress_mtu0_1); 
    bcm84756_a0_SP_INGRESS_MTU0_1r_MTU0f_SET(mpc, reg_ingress_mtu0_1, 0x1400); 
    bcm84756_a0_WRITE_SP_INGRESS_MTU0_1r(reg_ingress_mtu0_1); 

    bcm84756_a0_SP_MTU0r_CLR(reg_sp_mtu0); 
    bcm84756_a0_SP_MTU0r_MTUf_SET(mpc, reg_sp_mtu0, 0x1400); 
    bcm84756_a0_WRITE_SP_MTU0r_MTUf(mpc, reg_sp_mtu0); 
#endif

    bcm84756_a0_TX_BUFFER_CONFIGr_CLR(reg_tx_buffer); 
    bcm84756_a0_TX_BUFFER_CONFIGr_HEADROOMf_SET(mpc, reg_tx_buffer, 0); 
    bcm84756_a0_TX_BUFFER_CONFIGr_THRESHOLDf_SET(mpc, reg_tx_buffer, 2); 
    bcm84756_a0_WRITE_TX_BUFFER_CONFIGr(mpc, reg_tx_buffer); 

    bcm84756_a0_RX_BUFFER_CONFIGr_CLR(reg_rx_buffer); 
    bcm84756_a0_RX_BUFFER_CONFIGr_HEADROOMf_SET(mpc, reg_rx_buffer, 0); 
    bcm84756_a0_RX_BUFFER_CONFIGr_THRESHOLDf_SET(mpc, reg_rx_buffer, 44); 
    bcm84756_a0_WRITE_RX_BUFFER_CONFIGr(mpc, reg_rx_buffer); 

    bcm84756_a0_FCM_FCME_CONFIGr_CLR(reg_fcme_config); 
    bcm84756_a0_FCM_FCME_CONFIGr_FCS_REGEN_MODEf_SET(mpc, reg_fcme_config, 0); 
    bcm84756_a0_FCM_FCME_CONFIGr_FCS_EOF_MODEf_SET(mpc, reg_fcme_config, 0); 
    bcm84756_a0_FCM_FCME_CONFIGr_FCS_FIELD_PRESENTf_SET(mpc, reg_fcme_config, 1); 
    bcm84756_a0_FCM_FCME_CONFIGr_DBG_IGNORE_CRCf_SET(mpc, reg_fcme_config, 0); 
    bcm84756_a0_FCM_FCME_CONFIGr_MAP_TABLE_INPUTf_SET(mpc, reg_fcme_config, 0); 
    bcm84756_a0_FCM_FCME_CONFIGr_MAP_TABLE_DIRECTIONf_SET(mpc, reg_fcme_config, 0); 
    bcm84756_a0_FCM_FCME_CONFIGr_DECAP_FCOE2FC_ENABLEf_SET(mpc, reg_fcme_config, 1); 
    bcm84756_a0_WRITE_FCM_FCME_CONFIGr(mpc, reg_fcme_config); 

    bcm84756_a0_FCM_FCMI_CONFIGr_CLR(reg_fcmi_config); 
    bcm84756_a0_FCM_FCMI_CONFIGr_DBG_IGNORE_CRCf_SET(mpc, reg_fcmi_config, 0); 
    bcm84756_a0_FCM_FCMI_CONFIGr_FCS_EOF_MODEf_SET(mpc, reg_fcmi_config, 0); 
    bcm84756_a0_FCM_FCMI_CONFIGr_ENCAP_FC2FCOE_ENf_SET(mpc, reg_fcmi_config, 1); 
    bcm84756_a0_FCM_FCMI_CONFIGr_MAP_TABLE_INPUTf_SET(mpc, reg_fcmi_config, 0); 
    bcm84756_a0_FCM_FCMI_CONFIGr_FCS_FIELD_PRESENTf_SET(mpc, reg_fcmi_config, 1); 
    bcm84756_a0_WRITE_FCM_FCMI_CONFIGr(mpc, reg_fcmi_config); 

    /* Egress VFT header processing mode  */
    bcm84756_a0_FCM_FCME_VFTHDR_PROC_MODEr_CLR(reg_evfm);
    bcm84756_a0_WRITE_FCM_FCME_VFTHDR_PROC_MODEr(mpc, reg_evfm);

    /* Egress VLAN header processing mode */
    bcm84756_a0_FCM_FCME_VLANTAG_PROC_MODEr_CLR(reg_evlm);
    bcm84756_a0_WRITE_FCM_FCME_VLANTAG_PROC_MODEr(mpc, reg_evlm);

    /* Egress VFT header processing mode  */
    bcm84756_a0_FCM_FCMI_VFTHDR_PROC_MODEr_CLR(reg_ivfm);
    bcm84756_a0_WRITE_FCM_FCMI_VFTHDR_PROC_MODEr(mpc, reg_ivfm);

    /* Ingress VLAN header processing mode */
    bcm84756_a0_FCM_FCMI_VLANTAG_PROC_MODEr_CLR(reg_ivlm);
    fval = 0x1;
    bcm84756_a0_FCM_FCMI_VLANTAG_PROC_MODEr_VLANTAG_PRESENCEf_SET(mpc, reg_ivlm, fval);
    fval = 0x2;
    bcm84756_a0_FCM_FCMI_VLANTAG_PROC_MODEr_VLANTAG_MAPSRCf_SET(mpc, reg_ivlm, fval);
    bcm84756_a0_WRITE_FCM_FCMI_VLANTAG_PROC_MODEr(mpc, reg_ivlm);

    /* FPMA Processing Mode */
    bcm84756_a0_FCM_FCMI_MAC_SRCr_CLR(reg_imm);
    bcm84756_a0_FCM_FCMI_MAC_SRCr_FCOE_SA_MODEf_SET(mpc, reg_imm, 0);
    bcm84756_a0_FCM_FCMI_MAC_SRCr_FCOE_DA_MODEf_SET(mpc, reg_imm, 0);
    bcm84756_a0_WRITE_FCM_FCMI_MAC_SRCr(mpc, reg_imm);


    bcm84756_a0_FCM_DEFAULT_VFTHDRr_CLR(reg_vfthdr);
    bcm84756_a0_FCM_DEFAULT_VFTHDRr_HOPCNTf_SET(mpc, reg_vfthdr, 0xa);
    bcm84756_a0_WRITE_FCM_DEFAULT_VFTHDRr(mpc, reg_vfthdr);

    /* Source MAC */
    bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_HIr_CLR(reg_src_hi);
    bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_HIr_SRCMACf_SET(mpc, reg_src_hi, 0);
    bcm84756_a0_WRITE_FCM_DEFAULT_FCOE_SRCMAC_HIr(mpc,reg_src_hi);

    bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_LOr_CLR(reg_src_lo);
    bcm84756_a0_FCM_DEFAULT_FCOE_SRCMAC_LOr_SRCMACf_SET(mpc, reg_src_lo, 0xFCFCFC);
    bcm84756_a0_WRITE_FCM_DEFAULT_FCOE_SRCMAC_LOr(mpc,reg_src_lo);
        
    /* Destination MAC */
    bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_HIr_CLR(reg_dst_hi);
    bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_HIr_DSTMACf_SET(mpc, reg_dst_hi, 0x00);
    bcm84756_a0_WRITE_FCM_DEFAULT_FCOE_DSTMAC_HIr(mpc,reg_dst_hi);

    bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_LOr_CLR(reg_dst_lo);
    bcm84756_a0_FCM_DEFAULT_FCOE_DSTMAC_LOr_DSTMACf_SET(mpc, reg_dst_lo, 2);
    bcm84756_a0_WRITE_FCM_DEFAULT_FCOE_DSTMAC_LOr(mpc,reg_dst_lo);

    bcm84756_a0_FCM_DEFAULT_VLAN_TAGr_CLR(reg_vlan_tag);
    bcm84756_a0_FCM_DEFAULT_VLAN_TAGr_PCPf_SET(mpc, reg_vlan_tag, 3);
    bcm84756_a0_FCM_DEFAULT_VLAN_TAGr_CFIf_SET(mpc, reg_vlan_tag, 0);
    bcm84756_a0_FCM_DEFAULT_VLAN_TAGr_VIDf_SET(mpc, reg_vlan_tag, 2);
    bcm84756_a0_FCM_DEFAULT_VLAN_TAGr_TPIDf_SET(mpc, reg_vlan_tag, 0x8100);
    bcm84756_a0_WRITE_FCM_DEFAULT_VLAN_TAGr(mpc, reg_vlan_tag);

    if (IS_C0_CHIP(mpc))
    {
        /* set up MPORT_EGRESS_DATA_DATA_TRUNCATION_THD */
        bcm84756_c0_MPORT_EGRESS_DATA_TRUNCATION_THDr_CLR(reg_mport_edtt);
        bcm84756_c0_MPORT_EGRESS_DATA_TRUNCATION_THDr_SET(reg_mport_edtt, 0x07c0);
        bcm84756_c0_WRITE_MPORT_EGRESS_DATA_TRUNCATION_THDr(mpc, reg_mport_edtt);

        /* set up MPORT_EGRESS_HDR_HDR_TRUNCATION_THD */
        bcm84756_c0_MPORT_EGRESS_HDR_TRUNCATION_THDr_CLR(reg_mport_ehtt);
        bcm84756_c0_MPORT_EGRESS_HDR_TRUNCATION_THDr_SET(reg_mport_ehtt, 0x0180);
        bcm84756_c0_WRITE_MPORT_EGRESS_HDR_TRUNCATION_THDr(mpc, reg_mport_ehtt);

        /* set up PCFG2 for IDLE/ARBFF handling */
        bcm84756_c0_FCM_FCMAC_PCFG2r_CLR(reg_pcfg2);
        bcm84756_c0_FCM_FCMAC_PCFG2r_SET(reg_pcfg2,0x2);  /* accept both IDLE and ARBFF */
        bcm84756_c0_WRITE_FCM_FCMAC_PCFG2r(mpc, reg_pcfg2);
    }

    /* Enable Port interrupts */
    BFCMAP_LOCK_DEVICE(mpc->parent);
    BFCMAP_MDIO_WRITE(mdio_addr, BFCMAP_PORT_UC_INT_ENABLE_REG(mpc), BFCMAP84756_UC_INTR_EN);
    BFCMAP_UNLOCK_DEVICE(mpc->parent);

    /* Clear counters */
    bfcmap_int_stat_clear(mpc);

    /* Send to UC Firmware Download data */
    bfcmap84756_a0_firmware_download(mdio_addr, 0, 1);
    BFCMAP_SAL_USLEEP(100000);
    BFCMAP_MDIO_READ(mdio_addr, BFCMAP_PORT_UC_DATA_REG14(mpc), &ver);
    BFCMAP_SAL_PRINTF("BCM84756: dev_addr = 0x%x, FC firmware version:%04X \n", mdio_addr, ver);

    BFCMAP_MDIO_WRITE(mpc->uc_dev_addr, BFCMAP_PORT_UC_DATA_REG16(mpc), 0);  /* clear linkup */
    BFCMAP_MDIO_WRITE(mpc->uc_dev_addr, BFCMAP_PORT_UC_DATA_REG17(mpc), 0);  /* clear negotiation done */

    BFCMAP_MDIO_WRITE(mpc->uc_dev_addr, BFCMAP_PORT_UC_DATA_REG19(mpc), 0);  /* By default, use ARBFF as fillword for 8Gbps ACTIVE state */

    /* Send to UC Firmware Init MB message */
    rv = _bfcmap84756_send_lmi_uc_msg(mpc, BFCMAP_UC_FIRMWARE_INIT, 0);
    BFCMAP_SAL_USLEEP(1000);

/* error: */
    /* Port unlock */
    BFCMAP_UNLOCK_PORT(mpc);

    return rv;
}

/*
 * Function:
 *      bfcmap84756_port_uninit
 * Purpose:
 *      Initializes a bfcmap port.
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      pCfg        - Pointer to port configuration
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */
STATIC int 
bfcmap84756_port_uninit(bfcmap_port_ctrl_t *mpc)
{
    int                                     rv = BFCMAP_E_NONE;
    buint16_t sval;
    bfcmap_dev_addr_t mdio_addr = mpc->uc_dev_addr ;
    
    /* Disable Port interrupts */
    BFCMAP_LOCK_PORT(mpc);
    BFCMAP_LOCK_DEVICE(mpc->parent);

    sval = 0;
    BFCMAP_MDIO_WRITE(mdio_addr, BFCMAP_PORT_UC_INT_ENABLE_REG(mpc), sval);

    BFCMAP_UNLOCK_DEVICE(mpc->parent);
    BFCMAP_UNLOCK_PORT(mpc);

    return rv;
}


/*
 * Function:
 *      bfcmap84756_port_reset
 * Purpose:
 *      Issues a link reset on the port.
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */
STATIC int 
bfcmap84756_port_reset(bfcmap_port_ctrl_t *mpc)
{
    int rv = BFCMAP_E_NONE;

    /* Send to UC - link reset */
    rv = _bfcmap84756_send_lmi_uc_msg(mpc, BFCMAP_UC_LINK_RESET, 0);

    return rv;
}

/*
 * Function:
 *      bfcmap84756_port_shutdown
 * Purpose:
 *      Disables the port
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */
STATIC int 
bfcmap84756_port_shutdown(bfcmap_port_ctrl_t *mpc)
{
    int rv = BFCMAP_E_NONE;

    /* Send to UC - link shutdown */
    rv = _bfcmap84756_send_lmi_uc_msg(mpc, BFCMAP_UC_LINK_ENABLE, 0);

    return rv;
}

/*
 * Function:
 *      bfcmap84756_port_link_enable
 * Purpose:
 *      Enable the port
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */
STATIC int 
bfcmap84756_port_link_enable(bfcmap_port_ctrl_t *mpc)
{
    int rv = BFCMAP_E_NONE;

    /* Send to UC - link Enable */
    rv = _bfcmap84756_send_lmi_uc_msg(mpc, BFCMAP_UC_LINK_ENABLE, 1);

    return rv;
}


/*
 * Function:
 *      bfcmap84756_port_link_bounce
 * Purpose:
 *      Bounce the link
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 * Returns:
 *      BFCMAP_E_SUCCESS - Success
 *      BFCMAP_E_INTERNAL- Failure
 */
STATIC int 
bfcmap84756_port_link_bounce(bfcmap_port_ctrl_t *mpc)
{
    int rv = BFCMAP_E_NONE;

    /* Send to UC - link Bounce */
    rv = _bfcmap84756_send_lmi_uc_msg(mpc, BFCMAP_UC_LINK_BOUNCE, 0);

    return rv;
}


STATIC int
_bfcmap84756_handle_pending_events(bfcmap_port_ctrl_t *mpc)
{
    buint16_t                                       sval;
    buint16_t                                       fval;
    buint16_t                                       port;
    bfcmap_dev_addr_t mdio_addr = mpc->uc_dev_addr ;
    
    BFCMAP_MDIO_READ(mdio_addr, BFCMAP_PORT_UC_INT_STATUS_REG(mpc), &sval);

    /* Hardcode till the issue of interrupt is fixed */
    sval = BFCMAP84756_UC_INTR_STATUS;
    if (sval & BFCMAP84756_UC_INTR_STATUS) {

        BFCMAP_MDIO_READ(mdio_addr, BFCMAP_PORT_UC_LMI_MB(mpc), &port);
        if (port == BFCMAP_PORT(mpc)) {

#if 0
            /* make port invalid */
            port = 0xdead;
            BFCMAP_MDIO_WRITE(mdio_addr, BFCMAP_PORT_UC_DATA_REG16(mpc), port);
#endif

            BFCMAP_MDIO_READ(mdio_addr, BFCMAP_PORT_UC_DATA_REG16(mpc), &fval);

            switch (fval) {
                case BFCMAP_UC_LINK_UP:
                    fval = BFCMAP_EVENT_FC_LINK_INIT;
                    break;
                case BFCMAP_UC_LINK_FAILURE:
                default:
                    fval = BFCMAP_EVENT_FC_LINK_DOWN;
                    break;
            }

            bfcmap_event(mpc, fval, -1, -1);
        }
    }
    

    if (sval & 0xffff) {
        /* clear interrupts */
    }
    return 0;
}

STATIC int 
bfcmap84756_event_handler(bfcmap_dev_ctrl_t *mdc)
{
    int                     port;
    bfcmap_port_ctrl_t     *mpc;

    for (port = 0; port < BFCMAP84756_MAX_PORTS; port++) {
        mpc = BFCMAP_UNIT_PORT_CONTROL(mdc, port);
        if ((mpc->f & BFCMAP_CTRL_FLAG_ATTACHED) == 0) {
            continue;
        }
        _bfcmap84756_handle_pending_events(mpc);
    }
    return 0;
}


/*
 * Function:
 *      bfcmap84756_linkfault_trigger_rc_get
 * Purpose:
 *      Returns the current link fault trigger and reason code from the FC port.
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      tr        - Pointer to the to be returned trigger type
 *      rc       - Pointer to the to be returned reason code
 * Returns:
 *      BFCMAP_E_NONE   - Success
 *      BFCMAP_E_PARAM - parameter error
 */
STATIC int 
bfcmap84756_linkfault_trigger_rc_get(bfcmap_port_ctrl_t *mpc, 
                          bfcmap_lf_tr_t *tr, bfcmap_lf_rc_t *rc)
{
    buint16_t  sval;
    bfcmap_dev_addr_t mdio_addr = mpc->uc_dev_addr ;

    if (!tr || !rc) {
        return BFCMAP_E_PARAM;
    }
    BFCMAP_MDIO_READ(mdio_addr, BFCMAP_PORT_UC_DATA_REG21(mpc), &sval);
    *tr = sval >> 8;
    *rc = sval & 0xFF;
    return BFCMAP_E_NONE;
}


/*
 * Function:
 *      bfcmap84756_port_diag_get
 * Purpose:
 *      Returns the current diagnostic code from the FC port.
 * Parameters:
 *      mpc          - bfcmap port control strtucture
 *      diag         - Pointer to the to be returned diagnostic code 
 * Returns:
 *      BFCMAP_E_NONE   - Success
 *      BFCMAP_E_PARAM - parameter error
 */
STATIC int 
bfcmap84756_port_diag_get(bfcmap_port_ctrl_t *mpc, 
                          bfcmap_diag_code_t *diag)
{
    buint16_t  sval;
    bfcmap_dev_addr_t mdio_addr = mpc->uc_dev_addr ;
    
    if (!diag) {
        return BFCMAP_E_PARAM;
    }
    BFCMAP_MDIO_READ(mdio_addr, BFCMAP_PORT_UC_DATA_REG22(mpc), &sval);
    *diag = sval;
    return BFCMAP_E_NONE;
}


#if defined (BCM_WARM_BOOT_SUPPORT)  /*  */
int
bfcmap84756_vlan_recover(bfcmap_port_ctrl_t *mpc)
{
    int rv = BFCMAP_E_NONE;

    rv = bfcmap_int_vlanmap_get_from_hw(mpc, mpc->mp);

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/************************************************************
 * BFCMAP84756 API table
 */

bfcmap_drv_t bfcmap84756_a0_drv = {
    bfcmap84756_device_init,
    bfcmap84756_device_uninit,
    bfcmap84756_port_config_selective_set,
    bfcmap84756_port_config_selective_get,
    bfcmap84756_port_init,
    bfcmap84756_port_uninit,
    bfcmap84756_port_reset,
    bfcmap84756_port_shutdown,
    bfcmap84756_port_link_enable,
    bfcmap84756_port_speed_set,
    bfcmap84756_port_link_bounce,
    bfcmap84756_event_handler,
    bfcmap84756_port_diag_get,
    bfcmap84756_linkfault_trigger_rc_get,
#if defined (BCM_WARM_BOOT_SUPPORT)  /*  */
    bfcmap84756_vlan_recover,
#endif /* BCM_WARM_BOOT_SUPPORT */

    NULL, NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};
