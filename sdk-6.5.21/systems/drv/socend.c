/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	socend.c
 * Purpose: 	Defines SOC device END driver for VxWorks.
 *
 *
 *     If VLAN tag in tx packet, extract PRI and map to COS
 *     Provide default COS for untagged packets
 *     Determine IP packet snooping really required; remove if not
 *     Determine reasonable "reason filter" for RX;
 *        and/or provide filter callback on RX.
 */
 

#include <shared/bsl.h>

#include <sys/types.h>
#include <sal/core/libc.h>
#include <sal/appl/sal.h>

#include <end.h>
#include <endLib.h>
#include <netLib.h>
#include <netBufLib.h>
#include <etherMultiLib.h>

#include <appl/diag/system.h>
#if defined(BROADCOM_DEBUG)
#   include <appl/diag/decode.h>
#endif /* BROADCOM_DEBUG */

#include <soc/enet.h>
#include <soc/util.h>

#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/l2.h>
#include <bcm/mcast.h>
#include <bcm/pkt.h>
#include <bcm/stack.h>

#include "socend.h"

/*
 * TX and RX packet snooping/filter functions.
 *
 *    For RX snooping, the function may return "handled" or "owned"
 *    in which case no further processing is done.  If RX snoop returns
 *    "not handled", then "enqueue" must be set to TRUE or FALSE to
 *    indicate if packet should be enqueued via netJobAdd.
 */

static socend_snoop_ip_tx_t snoop_ip_tx = NULL;
static socend_snoop_ip_rx_t snoop_ip_rx = NULL;

int socend_cos_default = SOCEND_COS_DEFAULT;
int socend_cos_map[BCM_COS_COUNT] = SOCEND_COS_MAP_DEFAULT;

/*
 * Define END_OBJ parameter type for each of the VxWorks platform. This method
 * provides the best type checking for Tornado II.
 */
#if VX_VERSION == 531
#   define _END_OBJ_PAR		void
#else
#   define _END_OBJ_PAR		END_OBJ
#endif

#define	SOC_END_HANDLER_PRIO_BASE			100
#define	SOC_END_HANDLER_PRIO_COUNT			100

/*
 * Define:	dv_end_mblk/dv_end_se
 * Purpose:	Defines the dv_t usable entires for a pointer
 *		to the mblk structure if one is associated with
 *		the DMA, and the "se" pointer.
 */
#define	dv_end_mblk	dv_public1.ptr
#define	dv_end_se	dv_public2.ptr

END_OBJ *socend_load(char *is, void* ap);

STATUS	socend_unload(_END_OBJ_PAR *);
STATUS	socend_ioctl(_END_OBJ_PAR *, int cmd, caddr_t data);
STATUS	socend_send(_END_OBJ_PAR *, M_BLK_ID mb);
STATUS	socend_start(_END_OBJ_PAR *p);
STATUS	socend_stop(_END_OBJ_PAR *p);
STATUS	socend_poll_send(_END_OBJ_PAR *p, M_BLK_ID mb);
STATUS	socend_poll_receive(_END_OBJ_PAR *p, M_BLK_ID mb);
STATUS	socend_mcast_add(_END_OBJ_PAR *p, char *a);
STATUS	socend_mcast_del(_END_OBJ_PAR *p, char *a);
STATUS	socend_mcast_get(_END_OBJ_PAR *p, MULTI_TABLE *mt);
M_BLK_ID socend_address_form(M_BLK_ID mb, M_BLK_ID sa, M_BLK_ID da);
STATUS	socend_data_get(M_BLK_ID mb, LL_HDR_INFO *h);
STATUS	socend_addr_get(M_BLK_ID mb, M_BLK_ID sa, M_BLK_ID da,
			M_BLK_ID esa, M_BLK_ID eda);
bcm_rx_t socend_receive(int unit, bcm_pkt_t *pkt, void *cookie);
int _socend_l2_lookup(int unit, bcm_mac_t mac, vlan_id_t vid,
                    bcm_pkt_t *pkt);

typedef STATUS (*_socend_fun_t) (void *p);
/*
 * Variable: 	socend_netfuncs
 * Purpose:	Provides function switch entires for SOC devices.
 */
static NET_FUNCS socend_netfuncs = {
    socend_start,		/* Start Function */
    socend_stop,		/* Stop Function */
    socend_unload,		/* Unload function */
    socend_ioctl,		/* IOCTL function */
    socend_send,		/* Send Function */
    socend_mcast_add,		/* Add mcast address function */
    socend_mcast_del,		/* Delete mcast address function */
    socend_mcast_get,		/* Get mcast address function */
    socend_poll_send,		/* Polling send function */
    socend_poll_receive,	/* Poling receive function */
    endEtherAddressForm,	/* Address formation function */
    endEtherPacketDataGet,	/* Packet data get function */
    endEtherPacketAddrGet	/* Packet address get function */
};

/*
 * Typedef: seu_t (SOC End Unit)
 * Purpose: Group together items specific to a device, not an interface
 */
typedef struct seu_s {
    uint32		seu_init;	/* Initialized */
    socend_t		*seu_devices;	/* Linked list of devices */
    M_CL_CONFIG		seu_blks;	/* mBlks/clBlks */
    CL_DESC		seu_cluster;	/* cluster memory */
    CL_POOL_ID		seu_clpool_id;	/* Cluster Pool ID */
    NET_POOL_ID		seu_netpool_id;	/* Net Pool ID  */
#if defined(__mips__)
    void                *pkt_align_buf; /* Tmp buffer to word align IP pkt */
#endif /* __mips__ */
} seu_t;

typedef struct socend_prio_map_s {
	int			valid;
	socend_t		*socend;
} socend_prio_map_t;

/*
 * Variable:	socend_seu
 * Purpose:	One entry for each soc unit to not allow ont unit to
 *		use up all memory, and keep reference counts.
 * Notes: 	Since the driver only uses one size of cluster, each
 *		element in the array for for a different SOC unit.
 */
static seu_t		socend_seu[BCM_MAX_NUM_UNITS];

static	sal_mac_addr_t	mac_zero = {0, 0, 0, 0, 0, 0};
static	sal_mac_addr_t	mac_ones = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static  int             set_target_broadcast = 0;

static	socend_prio_map_t	socend_prio_map[SOC_END_HANDLER_PRIO_COUNT];

#if defined(BROADCOM_DEBUG)

void
socend_snoop_ip_tx_register(socend_snoop_ip_tx_t snoop_func)
{
    snoop_ip_tx = snoop_func;
}

void
socend_snoop_ip_tx_unregister(void)
{
    snoop_ip_tx = NULL;
}

void
socend_snoop_ip_rx_register(socend_snoop_ip_rx_t snoop_func)
{
    snoop_ip_rx = snoop_func;
}

void
socend_snoop_ip_rx_unregister(void)
{
    snoop_ip_rx = NULL;
}

void
socend_set_dest_broadcast (int flag)
/*
 * Function: 	socend_set_dest_broadcast
 * Purpose:	Dump memory pools.
 * Parameters:	flag
 * Returns:	Nothing
 */
{
    if (flag)
	set_target_broadcast = 1;
    else
	set_target_broadcast = 0;
}

void
socend_dump_pools(int u)
/*
 * Function: 	socend_dump_pools
 * Purpose:	Dump memory pools.
 * Parameters:	u - unit #
 * Returns:	Nothing
 */
{
    IMPORT	void netPoolShow(NET_POOL_ID);
    seu_t	*seu = &socend_seu[u];

    if ((u < BCM_MAX_NUM_UNITS) && seu->seu_init) {
	LOG_CLI((BSL_META_U(u,
                            "NET POOL: Unit %d\n"), u));
	netPoolShow(socend_seu[u].seu_netpool_id);
    } else {
	LOG_CLI((BSL_META_U(u,
                            "Invalid unit: %d\n"), u));
    }
}

#endif /* BROADCOM_DEBUG */

static	STATUS
socend_mempool_alloc(socend_t *se, int pksize, int clblks, int cl, int mblks)
/*
 * Function: 	socend_mempool_alloc
 * Purpose:	Initialize END memory pools for the specified SOC.
 * Parameters:	u - unit number to initialize.
 *		pksize - Packet size to use, MUST BE MTU size or greater.
 *		clblks - number of cluster block headers this unit
 *		cl - number of actual cluster buffers this unit.
 *		mblks - number of mblks this unit.
 *
 * Returns:	OK or error if failed.
 * Notes:	If already initialized, nothing is done.
 */
{
    seu_t	*seu = &socend_seu[se->se_unit];

    LOG_INFO(BSL_LS_SYS_END,
             (BSL_META("socend_mempool_alloc: Unit %d %sinitialized\n"),
              se->se_unit, seu->seu_init ? "" : "not "));
    /*
     * If already done, skip the allocation and just assign the required
     * values.
     */
    if (!seu->seu_init) {
	seu->seu_init = TRUE;
	seu->seu_netpool_id = (NET_POOL_ID)sal_alloc(sizeof(NET_POOL),
						     "net_pool");

	/* First Initialize the CL configuration */

	seu->seu_blks.mBlkNum = mblks;
	seu->seu_blks.clBlkNum= clblks;
	seu->seu_blks.memSize = (mblks * (M_BLK_SZ + sizeof(long)))
	    + (clblks * (CL_BLK_SZ + sizeof(long)));
	seu->seu_blks.memArea = sal_alloc(seu->seu_blks.memSize, "socend cl");

	/* Now initialize the actual cluster buffers that contain packets */

	seu->seu_cluster.clNum = cl;
	seu->seu_cluster.clSize = pksize;
	seu->seu_cluster.memSize = cl*(pksize + sizeof(long)) + sizeof(long);
       
        if (SOC_IS_ESW(se->se_unit)) {
           seu->seu_cluster.memArea = sal_dma_alloc(seu->seu_cluster.memSize,
						 "socend_mempool");
        } else {
           seu->seu_cluster.memArea = memalign(4096, 
                                           seu->seu_cluster.memSize);
        }
	/* Now set up the pool and GO! */

	if (!seu->seu_blks.memArea || !seu->seu_cluster.memArea ||
	    !seu->seu_netpool_id ||
	    (OK != netPoolInit(seu->seu_netpool_id, &seu->seu_blks,
			       &seu->seu_cluster, 1, NULL))){
	    LOG_CLI((BSL_META("socend_mempool_init: Failed for unit %d\n"), se->se_unit));
	    if (seu->seu_blks.memArea) {
		sal_free(seu->seu_blks.memArea);
	    }
	    if (seu->seu_cluster.memArea) {
		sal_dma_free(seu->seu_cluster.memArea);
	    }
	    if (seu->seu_netpool_id) {
		free(seu->seu_netpool_id);
	    }
	    bzero((void *)seu, sizeof(*seu));
	    return(ERROR);
	}
	seu->seu_clpool_id = netClPoolIdGet(seu->seu_netpool_id, pksize,
					    FALSE);
    }

    se->se_eo.pNetPool	= seu->seu_netpool_id;

    return(OK);
}

static int
socend_packet_alloc(int u, int pksize, uint32 flags, void** buf)
/*
 * Function: 	socend_packet_alloc
 * Purpose:	Allocate a packet buffer.
 * Parameters:	u - unit number to allocate packet for
 *		pksize - size of packet to allocate
 *		flags  - Dont know yet ....
 * Returns:	pointer to packet or NULL.
 */
{
    void 	*p;
    seu_t	*seu = &socend_seu[u];

    assert(pksize <= SOC_END_PK_SZ);

    p = netClusterGet(seu->seu_netpool_id, seu->seu_clpool_id);
    *buf = p;
    return(0);
}

static int
socend_packet_free(int u, void *p)
/*
 * Function: 	socend_packet_free
 * Purpose:	Free a packet buffer.
 * Parameters:	u - unit number to allocate packet for
 *		p - pointer to packet to free.
 *              cookie:  If != NULL, this is a pointer to the mBlk.  Free it.
 *                       Else, it's a new buffer we allocated. netClFree it.
 * Returns:	Nothing
 */
{
    seu_t	*seu = &socend_seu[u];
    netClFree(seu->seu_netpool_id, p);
    return 0;
}

struct end_object *
socend_load(char *is, void* ap)
/*
 * Function: 	socend_load
 * Purpose:	Provides SENS load routine for SOC device.
 * Parameters:	is - initialization string.
 * Returns:	Pointer to VxWorks end_obj structure or NULL
 */
{
    struct end_object	*eo;
    socend_t		*se;		/* Sock End Structure */
    seu_t		*seu;
    char		dev_name[sizeof(eo->devObject.name)];
    char		mac[6];		/* MAC address parsing */
    int			net_unit;
    char	        mac_str[MACADDR_STR_LEN];
    int 		i, column_count, rt;

    if (NULL == is) {
	LOG_CLI((BSL_META("socend_load: unexpected NULL pointer for device name\n")));
	return(NULL);
    } else if ('\0' == *is) {		/* Copy in our device name */
	strcpy(is, "sc");
	return(NULL);
    }
    LOG_INFO(BSL_LS_SYS_END,
             (BSL_META("socend_load: %s\n"), is));

    /*
     * Setup END structure
     *
     * NOTE NOTE NOTE: 	This call MUST use calloc or malloc, since the object
     * 		   	is freed by VxWorks using free();
     */

    if (NULL == (se = malloc(sizeof(*se)))) {
      LOG_CLI((BSL_META("socend_load: failed to allocate memory for %s\n"), is));
      return(NULL);
    }

    bzero((void *)se, sizeof(*se));
    eo = &se->se_eo;

    net_unit = atoi(is);
    is = index(is, ':') + 1;

    /* Parse init string, format is "net_unit:bcm_unit:MAC_ADDR:vlan" */

    se->se_unit = atoi(is);
    seu = &socend_seu[se->se_unit];
    is = index(is, ':') + 1;		/* Onto net unit # */

    if ((se->se_unit >= BCM_MAX_NUM_UNITS) || !*is) {
	LOG_CLI((BSL_META("socend_load: Invalid soc device: %d\n"), se->se_unit));
	free(se);
	return(NULL);
    }

    /* Check if optional VLAN/MAC address */

    se->se_vlan = VLAN_ID_INVALID;
    if (is) {
	column_count = 0;
	i = 0;
	while (*is && i < MACADDR_STR_LEN - 1 && column_count <= 5) {
	    mac_str[i++] = *is;
	    if (*is++ == ':') {
		column_count++;
	    }
	}
	mac_str[i] = '\0';
	if (column_count == 6) {
	    mac_str[i-1] = '\0';
	}
	if (5 <= column_count) {
	    rt = parse_macaddr(mac_str, se->se_mac);
	} else {
	    rt = -1;
	}
	if (rt) {
	    LOG_CLI((BSL_META("socend_load: Invalid MAC address specified: %s\n"),
                     mac_str));
	    free(se);
	    return(NULL);
	}
	if (column_count == 6) {
	    se->se_vlan = atoi(is);
	}
    } else {
	ENET_COPY_MACADDR(mac_zero, se->se_mac);
    }

    if (se->se_vlan == VLAN_ID_INVALID) {
	bcm_vlan_t	vid;
	bcm_vlan_default_get(se->se_unit, &vid);
	se->se_vlan = vid;
    }

    if (ERROR == END_OBJ_INIT(eo, (DEV_OBJ *)se, "sc", net_unit,
			      &socend_netfuncs, SOC_END_STRING)) {
	LOG_CLI((BSL_META("socend_load: END_OBJ_INIT failed for %s\n"), dev_name));
    }
    if (ERROR == END_MIB_INIT(eo, M2_ifType_ethernet_csmacd, (UCHAR *)mac,
			      sizeof(mac), ETHERMTU, SOC_END_SPEED)) {
	LOG_CLI((BSL_META("socend_load: END_MIB_INIT failed for %s\n"), dev_name));
    }

    socend_mempool_alloc(se, SOC_END_PK_SZ, SOC_END_CLBLKS, SOC_END_CLBUFS,
			 SOC_END_MBLKS);

    /* Set Ready */

    END_OBJ_READY (eo, IFF_NOTRAILERS | IFF_MULTICAST | IFF_BROADCAST);

    /* Queue off of seu structure */

    se->se_next = seu->seu_devices;
    seu->seu_devices = se;
#if defined(__mips__)
    if(SOC_IS_ESW(se->se_unit)) {
        socend_packet_alloc(se->se_unit, SOC_END_PK_SZ, 0, 
                               &seu->pkt_align_buf);
    }
#endif /* __mips__ */
    return(eo);
}

STATUS
socend_unload(_END_OBJ_PAR *p)
/*
 * Function: 	socend_unload
 * Purpose:	SENS Unload function for SOC devices.
 * Parameters:	p - pointer to VxWorks end_obj structure.
 * Returns:	Status
 */
{
    struct end_object 	*eo = (struct end_object *)p;
    socend_t 		*se = (socend_t *)eo->devObject.pDevice;
    socend_t		*se_cur, *se_prev;
    seu_t		*seu= &socend_seu[se->se_unit];

    LOG_INFO(BSL_LS_SYS_END,
             (BSL_META("socend_unload: %s%d:\n"), eo->devObject.name,
              eo->devObject.unit));

    END_OBJECT_UNLOAD(eo);

    /* Remove from list of active devices */

    se_prev = NULL;
    for (se_cur = seu->seu_devices; se_cur != se; se_cur = se_cur->se_next) {
	if (se_cur == NULL) {
	    return(ERROR);
	}
	se_prev = se_cur;
    }

    if (se_prev) {			/* Not head */
	se_prev->se_next = se->se_next;
    } else {				/* Head */
	seu->seu_devices = se->se_next;
    }

    return(OK);
}

STATUS
socend_ioctl(_END_OBJ_PAR *p, int cmd, caddr_t data)
/*
 * Function: 	socend_ioctl
 * Purpose:	SENS IOCTL function for SOC devices.
 * Parameters:	p - pointer to VxWorks end_obj structure for device
 *		cmd - command to execute
 *		data - data or address of data for CMD.
 * Returns:	Status
 */
{
    struct end_object 	*eo = (struct end_object *)p;
    socend_t 		*se = (socend_t *)eo->devObject.pDevice;
    STATUS 		error = OK;
    int unit = se->se_unit;

    LOG_INFO(BSL_LS_SYS_END,
             (BSL_META_U(unit,
                         "socend_ioctl: unit %d\n"), se->se_unit));

    switch((unsigned int)cmd) {
    case EIOCSFLAGS:
	LOG_INFO(BSL_LS_SYS_END,
                 (BSL_META_U(unit,
                             "socend_ioctl: %s%d: EIOCSFLAGS(0x%x)\n"),
                  eo->devObject.name, eo->devObject.unit, (unsigned int)data));
	if ((long)data < 0) {	/* Clearing flags? */
	    END_FLAGS_CLR(eo, -(long)data);
	} else {
	    END_FLAGS_SET(eo, (long)data);
	}
	break;
    case EIOCGFLAGS:
	LOG_INFO(BSL_LS_SYS_END,
                 (BSL_META_U(unit,
                             "socend_ioctl: %s%d: EIOCGFLAGS\n"),
                  eo->devObject.name, eo->devObject.unit));
	if (NULL == data) {
	    error = EINVAL;
	} else {
	    *(int *)data = eo->flags;
	}
	break;
    case EIOCSADDR:		/* Set interface address */
	LOG_INFO(BSL_LS_SYS_END,
                 (BSL_META_U(unit,
                             "socend_ioctl: %s%d: EIOCSADDR\n"),
                  eo->devObject.name, eo->devObject.unit));
	if (NULL == data) {
	    error = EINVAL;
	} else {
	    bcm_l2_addr_t	l2addr;
	    int                 my_modid;
	    if (ENET_CMP_MACADDR(se->se_mac, mac_zero)) {
		LOG_INFO(BSL_LS_SYS_END,
                         (BSL_META_U(unit,
                                     "socend_ioctl: Deleting old mac address\n")));
		(void)bcm_l2_addr_delete(se->se_unit, se->se_mac, se->se_vlan);
	    }
	    ENET_COPY_MACADDR(data, se->se_mac);
	    LOG_INFO(BSL_LS_SYS_END,
                     (BSL_META_U(unit,
                                 "socend_ioctl: Setting new mac address\n")));
	    bcm_l2_addr_t_init(&l2addr, se->se_mac, se->se_vlan);
	    l2addr.port = CMIC_PORT(unit);
	    l2addr.flags = BCM_L2_STATIC;
	    bcm_stk_my_modid_get(se->se_unit, &my_modid);
	    l2addr.modid = my_modid;
	    if (0 > bcm_l2_addr_add(se->se_unit, &l2addr)) {
		LOG_CLI((BSL_META_U(unit,
                                    "socend_ioctl: mac_configure_address failed: "
                                    "Network Device %s%d\n"),
                         eo->devObject.name, eo->devObject.unit));
		ENET_COPY_MACADDR(mac_zero, se->se_mac);
		error = EIO;
		break;
	    }
	    END_MIB_INIT(eo, M2_ifType_ethernet_csmacd, (uint8 *)data, 6,
			 ETHERMTU, SOC_END_SPEED);
	}
	break;
    case EIOCGADDR:		/* Get Interface address */
	LOG_INFO(BSL_LS_SYS_END,
                 (BSL_META_U(unit,
                             "socend_ioctl: %s%d: EIOCGADDR\n"),
                  eo->devObject.name, eo->devObject.unit));
	if (NULL == data) {
	    error = EINVAL;
	} else {
	    bcopy((char *)eo->mib2Tbl.ifPhysAddress.phyAddress,
		  (char *)data, eo->mib2Tbl.ifPhysAddress.addrLength);
	}
	break;
    case EIOCGFBUF:		/* Get min 1st buf for chain */
	LOG_INFO(BSL_LS_SYS_END,
                 (BSL_META_U(unit,
                             "socend_ioctl: %s%d: EIOCGFBUF\n"),
                  eo->devObject.name, eo->devObject.unit));
	if (data == NULL) {
	    error = EINVAL;
	} else {
	    *(int *)data = 32;
	}
	break;
    case EIOCGMWIDTH:		/* Get device memory witdh */
	LOG_INFO(BSL_LS_SYS_END,
                 (BSL_META_U(unit,
                             "socend_ioctl: %s%d: EIOCGMWIDTH\n"),
                  eo->devObject.name, eo->devObject.unit));
	if (NULL == data) {
	    error = EINVAL;
	} else {
	    *(int *)data = 32;
	}
	break;
    case EIOCMULTIADD:
    case EIOCMULTIDEL:
    case EIOCMULTIGET:
    case EIOCPOLLSTART:
    case EIOCPOLLSTOP:
	LOG_INFO(BSL_LS_SYS_END,
                 (BSL_META_U(unit,
                             "socend_ioctl: %s%d: MULTI/POLL\n"),
                  eo->devObject.name, eo->devObject.unit));
	error = ENOSYS;
	break;
    case EIOCGMIB2:		/* Get MIB2 Table */
	LOG_INFO(BSL_LS_SYS_END,
                 (BSL_META_U(unit,
                             "socend_ioctl: %s%d: EIOCGMIB2\n"),
                  eo->devObject.name, eo->devObject.unit));
	if (data == NULL) {
	    error = EINVAL;
	} else {
	    bcopy ((char *)&eo->mib2Tbl, (char *)data, sizeof(eo->mib2Tbl));
	}
	break;
    case EIOCGNAME:		/* Get device Name */
	LOG_INFO(BSL_LS_SYS_END,
                 (BSL_META_U(unit,
                             "socend_ioctl: %s%d: EIOCGNAME\n"),
                  eo->devObject.name, eo->devObject.unit));
	if (NULL == data) {
	    error = EINVAL;
	} else {
	    bcopy(eo->devObject.name, (char *)data,
		  sizeof(eo->devObject.name));
	}
	break;
    case EIOCGHDRLEN:
	if (NULL == data) {
	    error = EINVAL;
	} else {
	    *(int *)data = ENET_TAGGED_HDR_LEN;
	}
	break;
#if VX_VERSION == 62 || VX_VERSION == 64 || VX_VERSION == 66 || VX_VERSION == 68
        case EIOCGRCVJOBQ:
    LOG_INFO(BSL_LS_SYS_END,
             (BSL_META_U(unit,
                         "socend_ioctl: %s%d: EIOCGRCVJOBQ\n"),
              eo->devObject.name, eo->devObject.unit));
    if (NULL == data) {
	    error = EINVAL;
    } else {
        END_RCVJOBQ_INFO *qinfo = (END_RCVJOBQ_INFO *)data;
        LOG_INFO(BSL_LS_SYS_END,
                 (BSL_META_U(unit,
                             "socend_ioctl: %s%d: numRcvJobQs input#: %d\n"),
                  eo->devObject.name, eo->devObject.unit, qinfo->numRcvJobQs));
        if (qinfo->numRcvJobQs < 1) {
           error = ENOSPC;
        } else {
           qinfo->qIds[0] = netJobQueueId;
        }
        qinfo->numRcvJobQs = 1;
    }
            break;
#endif /* VX_VERSION */

    default:
	LOG_INFO(BSL_LS_SYS_END,
                 (BSL_META_U(unit,
                             "socend_ioctl: %s%d: Unknown IOCTL\n"),
                  eo->devObject.name, eo->devObject.unit));
	error = EINVAL;
	break;
    }
    LOG_INFO(BSL_LS_SYS_END,
             (BSL_META_U(unit,
                         "socend_ioctl: %s%d: cmd=0x%x Return(%d)\n"),
              eo->devObject.name, eo->devObject.unit, cmd, error));
    return(error);
}

void
socend_send_done(int unit, bcm_pkt_t *pkt, void *cookie)
/*
 * Function: 	socend_send_done
 * Purpose:	Process a completion of a send request
 * Parameters:	unit - unit #
 *		pkt - The tx packet structure
 *		cookie - pointer to mBlk structure to free
 *                       If null, assume we free the cluster directly.
 * Returns:	Nothing
 * Notes:
 * Assumption:  This function is called from a non-interrupt context.
 */
{
    LOG_INFO(BSL_LS_SYS_END,
             (BSL_META_U(unit,
                         "Socend send done, in. pkt %p. ck %p.  "), 
              (void *)pkt->_pkt_data.data, (void *)cookie));
    if (NULL == cookie) {
        if (SOC_IS_ESW(unit)) {
            socend_packet_free(unit, pkt->_pkt_data.data);
        } else {
            soc_cm_sfree(unit, pkt->_pkt_data.data);
        }
    } else {
        netMblkClChainFree(cookie);    /* Free MBLK */
    }
    sal_free(pkt);
    LOG_INFO(BSL_LS_SYS_END,
             (BSL_META_U(unit,
                         "out.\n")));
}

STATUS
socend_send(_END_OBJ_PAR *p, M_BLK_ID mb)
/*
 * Function: 	socend_send
 * Purpose:	SENS send packet interface to SOC device.
 * Parameters:	p - pointer to VxWorks end_obj structure for device
 *		mb - mBlk containing packet.
 * Returns:	OK/ERROR
 *
 * Notes: Since since we don't have scatter/gather capability right
 *	  now, we must copy up the data into one clustered buffer.
 *	  If Possible, we simply hand off the buffer.
 */
{
    int			rv;
    struct end_object 	*eo = (struct end_object *)p;
    socend_t 		*se = (socend_t *)eo->devObject.pDevice;
    char		*packet;
    int			l;
    enet_hdr_t		*eh;
    void *cookie;
    bcm_pkt_t *pkt;

    if (NULL == (pkt = sal_alloc(sizeof(bcm_pkt_t), "bcm pkt"))) {
        return(ENOMEM);
    }

    /*
     * If there is more than one cluster, we must allocate another
     * buffer and copy the data.
     *
     * Note: Requires entire ethernet header to be in first mblk.
     */

    if (mb->mBlkHdr.mNext|| !ENET_TAGGED((enet_hdr_t *)mb->mBlkHdr.mData)) {
        if (SOC_IS_ESW(se->se_unit)) {
            socend_packet_alloc(se->se_unit,SOC_END_PK_SZ, 0, (void*)&packet);
        } else {
            packet = soc_cm_salloc(se->se_unit, SOC_END_PK_SZ, 
                            "SOCEND_TX");
        }
        if (NULL == (packet)) {
            END_ERR_ADD(eo, MIB2_OUT_ERRS, +1);
            netMblkClChainFree(mb);
            return(ENOMEM);
        }

	cookie = NULL;
	/*
	 * If packet is not tagged, build a tag now.
	 */
	eh = (enet_hdr_t *)packet;
	if (!ENET_TAGGED((enet_hdr_t *)mb->mBlkHdr.mData)) {

	  
	  /* Set COS to default here */
	    l = ENET_TAG_SIZE +
		netMblkToBufCopy(mb, packet + ENET_TAG_SIZE, NULL);
	    /* Copy MAC addresses */
	    sal_memcpy(eh, mb->mBlkHdr.mData, sizeof(sal_mac_addr_t) * 2);

	    /* Set VLAN info */
	    eh->en_tag_tpid = htons(ENET_DEFAULT_TPID);
	    eh->en_tag_ctrl = htons(VLAN_CTRL(0,0,se->se_vlan));
	    eh->en_tag_len  =
		((enet_hdr_t *)mb->mBlkHdr.mData)->en_untagged_len;
	} else {
	    /* Just copy entire packet */
	  
	  /* Extract priority and map to COS here */
	    l = netMblkToBufCopy(mb, packet, NULL);
	}

	if (set_target_broadcast &&
	    sal_memcmp(eh->en_dhost, eh->en_shost, 6) == 0) {
	    sal_memcpy(eh->en_dhost, mac_ones, 6);
	}
	netMblkClChainFree(mb);		/* Free MBLK */
    } else {
	l = mb->mBlkHdr.mLen;
	packet = mb->mBlkHdr.mData;
	cookie = (void *)mb;
    }

#if defined(BROADCOM_DEBUG)
    if (LOG_CHECK(BSL_LS_SYS_END | BSL_INFO) &&
        LOG_CHECK(BSL_LS_SOC_PACKET | BSL_INFO)) {
	d_packet_format("socend TX", DECODE_ETHER, packet, l, NULL);
    }
#endif /* defined(BROADCOM_DEBUG) */

    if (snoop_ip_tx != NULL) {
	snoop_ip_tx(se->se_unit, packet, l);
    }

    sal_memset(pkt, 0, sizeof(bcm_pkt_t));
    pkt->pkt_data = &(pkt->_pkt_data);
    pkt->_pkt_data.len = l;
    pkt->_pkt_data.data = (uint8 *)packet;
    pkt->blk_count = 1;
    pkt->unit = se->se_unit;
    pkt->pkt_len = l;
    SOC_PBMP_CLEAR(pkt->tx_pbmp);
    SOC_PBMP_CLEAR(pkt->tx_upbmp);
    SOC_PBMP_CLEAR(pkt->tx_l3pbmp);

    /* get tx_pbmp, tx_upbmp */
    eh = (enet_hdr_t *)packet;
    rv = _socend_l2_lookup(se->se_unit,
                      eh->en_dhost,
                      VLAN_CTRL_ID(se->se_vlan),
                      pkt);
    if(rv == ERROR){
        LOG_CLI((BSL_META("socend_send : can't get egress port(s) by _socend_l2_lookup.\n")));
        return(ERROR);
        }

    pkt->cookie = cookie;
    pkt->call_back = socend_send_done;
    pkt->flags &= ~BCM_TX_CRC_FLD;
    pkt->flags |= BCM_TX_CRC_APPEND;

    if ((rv = bcm_tx(se->se_unit, pkt, cookie)) != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("bcm_tx failed: Unit %d: %s\n"),
                   se->se_unit, bcm_errmsg(rv)));
        return(ERROR);
    }

    return(OK);
}

STATUS
socend_start(_END_OBJ_PAR *p)
/*
 * Function: 	socend_start
 * Purpose:	SENS interface to activate a SOC interface.
 * Parameters:	p - pointer to VxWorks end_obj structure for device
 * Returns:	OK/ERROR
 *
 * Notes:	
 */
{
    int			rv;
    struct end_object 	*eo = (struct end_object *)p;
    socend_t 		*se = (socend_t *)eo->devObject.pDevice;
    bcm_l2_addr_t	l2addr;
    int i;
    int                 my_modid;

    LOG_INFO(BSL_LS_SYS_END,
             (BSL_META("socend_start: Device(%s), unit %d\n"),
              eo->devObject.name, se->se_unit));
    if (!ENET_CMP_MACADDR(se->se_mac, mac_zero)) {
	LOG_CLI((BSL_META("socend_start: No valid mac address assigned: "
                          "Network Device %s%d\n"),
                 eo->devObject.name, eo->devObject.unit));
    }

    bcopy((char *)se->se_mac, (char *)eo->mib2Tbl.ifPhysAddress.phyAddress,
	  eo->mib2Tbl.ifPhysAddress.addrLength);

    LOG_INFO(BSL_LS_SYS_END,
             (BSL_META("socend_start: Starting bcm_rx\n")));
    if (!bcm_rx_active(se->se_unit)) {

        bcm_rx_cfg_t rx_cfg;

        bcm_rx_cfg_get(se->se_unit, &rx_cfg);
        rx_cfg.pkt_size = SOC_END_PK_SZ;
        rx_cfg.rx_alloc = socend_packet_alloc;
        rx_cfg.rx_free = socend_packet_free; 

        if ((rv= bcm_rx_start(se->se_unit, &rx_cfg)) < 0) {
            LOG_CLI((BSL_META("rxmon: Error: Cannot start RX: %s.\n"), bcm_errmsg(rv)));
	    return(ERROR);
	}
    }else {
        LOG_CLI((BSL_META("RX Thread is already running\n")));
    }

    LOG_INFO(BSL_LS_SYS_END,
             (BSL_META("socend_start: adding l2 addr\n")));
    bcm_l2_addr_t_init(&l2addr, se->se_mac, se->se_vlan);
    l2addr.port = CMIC_PORT(se->se_unit);
    l2addr.flags = BCM_L2_STATIC;
    bcm_stk_my_modid_get(se->se_unit, &my_modid);
    l2addr.modid = my_modid;
    rv = bcm_l2_addr_add(se->se_unit, &l2addr);
    if (rv < 0) {
	LOG_CLI((BSL_META("socend_start: %s%d bcm_l2_addr_add failed: %s\n"),
                 eo->devObject.name, eo->devObject.unit, bcm_errmsg(rv)));
	return(ERROR);
    }

    LOG_INFO(BSL_LS_SYS_END,
             (BSL_META("socend_start: registering with bcm_rx\n")));
    for (i = 0; i < SOC_END_HANDLER_PRIO_COUNT; i++) {
        if (socend_prio_map[i].valid != TRUE){
            socend_prio_map[i].valid = TRUE;
            socend_prio_map[i].socend = se;
            break;
        }
    }

    if(i == SOC_END_HANDLER_PRIO_COUNT){
        LOG_CLI((BSL_META("socend_start: socend count is larger %d\n"), SOC_END_HANDLER_PRIO_COUNT));
	return(ERROR);
    }

    /* Register call back */
    if (SOC_IS_ESW(se->se_unit)) {
        rv = bcm_rx_register(se->se_unit, "SOCEND", socend_receive, 
                                (i + SOC_END_HANDLER_PRIO_BASE),
                                        (void *)se, BCM_RCO_F_ALL_COS);
    } else {
        rv = bcm_rx_register(se->se_unit, "SOCEND", socend_receive, 
                           (i + SOC_END_HANDLER_PRIO_BASE),
                           (void *)se, BCM_RCO_F_ALL_COS | BCM_RCO_F_INTR);
    }

    if (rv < 0) {
        LOG_CLI((BSL_META("Warning:  Could not register socend_receive from RX\n")));
	return(ERROR);
    }

    END_FLAGS_SET(eo, IFF_UP | IFF_RUNNING | IFF_BROADCAST);
    LOG_INFO(BSL_LS_SYS_END,
             (BSL_META("socend_start: Started Network Interface: %s%d\n"),
              eo->devObject.name, eo->devObject.unit));
    return(OK);
}

STATUS
socend_stop(_END_OBJ_PAR *p)
/*
 * Function: 	socend_stop
 * Purpose:	SENS interface to de-activate a SOC interface.
 * Parameters:	p - pointer to VxWorks end_obj structure for device
 * Returns:	OK/ERROR
 */
{
    struct end_object 	*eo = (struct end_object *)p;
    socend_t 		*se = (socend_t *)eo->devObject.pDevice;
    ETHER_MULTI	* pMCastNode;

    int i, rv;

    /* change to use bcm_rx */
    for (i = 0; i < SOC_END_HANDLER_PRIO_COUNT; i++) {
        if ((uint32)socend_prio_map[i].socend == (uint32)se){
            socend_prio_map[i].valid = FALSE;
            rv = bcm_rx_unregister(se->se_unit, socend_receive, i+SOC_END_HANDLER_PRIO_BASE);
            if (rv < 0) {
                LOG_CLI((BSL_META("Warning:  Could not unregister socend_receive from RX\n")));
                return(ERROR);
            }  
            break;
        }
    }

    if(i == SOC_END_HANDLER_PRIO_COUNT){
        LOG_CLI((BSL_META("socend_stop: can't find the socend from socend_prio_map table\n")));
        return(ERROR);
    }
    (void)bcm_l2_addr_delete(se->se_unit, se->se_mac, se->se_vlan);
    while ((pMCastNode = END_MULTI_LST_FIRST (eo)) != NULL) {
	socend_mcast_del(p, pMCastNode->addr);
    }
    END_FLAGS_CLR(eo, IFF_UP | IFF_RUNNING);
    LOG_INFO(BSL_LS_SYS_END,
             (BSL_META("socend_stop: Device(%s)\n"), eo->devObject.name));
    return(OK);
}

STATUS
socend_poll_send(_END_OBJ_PAR *p, M_BLK_ID mb)
/*
 * Function: 	socend_poll_send
 * Purpose:	SENS interface to perform a polling send operation.
 * Parameters:	p - pointer to VxWorks end_obj structure for device
 *		mb - block data copied from.
 * Returns:	OK/ERROR
 */
{
    struct end_object 	*eo = (struct end_object *)p;
    socend_t 		*se = (socend_t *)eo->devObject.pDevice;

    LOG_CLI((BSL_META("socend_poll_send 0x%x 0x%x\n"), (int)se, (int)eo));
    return(ENOSYS);
}

STATUS
socend_poll_receive(_END_OBJ_PAR *p, M_BLK_ID mb)
/*
 * Function: 	socend_poll_receive
 * Purpose:	SENS interface to perform a polling receive operation.
 * Parameters:	p - pointer to VxWorks end_obj structure for device
 *		mb - block data copied into.
 * Returns:	OK/ERROR
 */
{
    struct end_object 	*eo = (struct end_object *)p;
    socend_t 		*se = (socend_t *)eo->devObject.pDevice;

    LOG_CLI((BSL_META("socend_poll_receive 0x%x 0x%x\n"), (int)se, (int)eo));
    return(ERROR);
}

STATUS
socend_mcast_add(_END_OBJ_PAR *p, char *a)
/*
 * Function: 	socend_mcast_add
 * Purpose:	SENS interface to add a multicast address for the interface.
 * Parameters:	p - pointer to VxWorks end_obj structure for device
 *		a - pointer to address to add
 * Returns:	OK/ERROR
 */
{
    struct end_object 	*eo = (struct end_object *)p;
    socend_t 		*se = (socend_t *)eo->devObject.pDevice;
    int ret;
    pbmp_t pbmp, ubmp;
    bcm_mcast_addr_t mcaddr;
    char mac_str[MACADDR_STR_LEN];
    int unit = se->se_unit;

    if (LOG_CHECK(BSL_LS_SYS_END | BSL_INFO)) {
	format_macaddr(mac_str, (uint8 *)a);
	LOG_INFO(BSL_LS_SYS_END,
                 (BSL_META_U(unit,
                             "socend_mcast_add: %s%d %s\n"),
                  eo->devObject.name, eo->devObject.unit, mac_str));
    }
    /*soc_ioctl(ADD_MULTICAST)*/
    if (ENETRESET == etherMultiAdd(&eo->multiList, a)) {
	BCM_PBMP_CLEAR(pbmp);
	BCM_PBMP_PORT_ADD(pbmp, CMIC_PORT(unit));
#if 0   
	BCM_PBMP_PORT_ADD(pbmp, eo->devObject.unit);
#endif
	BCM_PBMP_ASSIGN(ubmp, pbmp);
	bcm_mcast_addr_t_init(&mcaddr, (uint8 *)a, se->se_vlan);
	mcaddr.cos_dst = 0;
	BCM_PBMP_ASSIGN(mcaddr.pbmp, pbmp);
	BCM_PBMP_ASSIGN(mcaddr.ubmp, ubmp);
	ret = bcm_mcast_addr_add(se->se_unit, &mcaddr);
	if (ret >= BCM_E_NONE) {
#if 0	
	    ret = bcm_amux_register(se->se_unit, a, se->se_vlan,
				    socend_receive, (void *)se);
#endif
	    if (ret < 0) {
		LOG_CLI((BSL_META_U(unit,
                                    "socend_mcast_add: %s%d bcm_amux_register failed: %s\n"),
                         eo->devObject.name, eo->devObject.unit,
                         bcm_errmsg(ret)));
		 bcm_mcast_addr_remove(se->se_unit,
				       (uint8 *)a, se->se_vlan);
	    }
	    return (OK);
	} else {
	    LOG_CLI((BSL_META_U(unit,
                                "bcm_mcast_addr_add failed ERROR : %s\n"),
                     bcm_errmsg(ret)));
	    LOG_INFO(BSL_LS_SYS_END,
                     (BSL_META_U(unit,
                                 "socend_mcast_add: Unit %d Device %s%d "
                                 "Software RESET\n"),
                      se->se_unit, eo->devObject.name, eo->devObject.unit));
	    return (ERROR);
	}
    }
    return(ERROR);
}

STATUS
socend_mcast_del(_END_OBJ_PAR *p, char *a)
/*
 * Function: 	socend_mcast_del
 * Purpose:	SENS interface to add a multicast address for the interface.
 * Parameters:	p - pointer to VxWorks end_obj structure for device
 *		a - pointer to address to delete
 * Returns:	OK/ERROR
 */
{
    struct end_object *eo = (struct end_object *)p;
    socend_t *se = (socend_t *)eo->devObject.pDevice;
    int ret;

    if (ENETRESET == etherMultiDel(&eo->multiList, a)) {
#if 0	
	bcm_amux_unregister(se->se_unit, (uint8 *)a, se->se_vlan);
#endif
	ret = bcm_mcast_addr_remove(se->se_unit, (uint8 *)a, se->se_vlan);
	if (ret == BCM_E_NONE) {
	    return (OK);
	} else {
	    LOG_CLI((BSL_META("bcm_mcast_addr_remove failed ERROR : %s\n"),
                     bcm_errmsg(ret)));
	    LOG_INFO(BSL_LS_SYS_END,
                     (BSL_META("socend_mcast_del: Unit %d Device %s%d Software RESET\n"),
                      se->se_unit, eo->devObject.name, eo->devObject.unit));
	}
    }
    return(ENOSYS);
}

STATUS
socend_mcast_get(_END_OBJ_PAR *p, MULTI_TABLE *mt)
/*
 * Function: 	socend_mcast_get
 * Purpose:	SENS interface to get a multicast address for the interface.
 * Parameters:	p - pointer to VxWorks end_obj structure for device
 *		mt - pointer to multi table to fill in on return.
 * Returns:	OK/ERROR
 */
{
    struct end_object *eo = (struct end_object *)p;

    LOG_INFO(BSL_LS_SYS_END,
             (BSL_META("socend_mcast_get: %s%d\n"), eo->devObject.name,
              eo->devObject.unit));

    return(etherMultiGet(&eo->multiList, mt) == OK ? OK : ERROR);
}

static	void
socend_untag(M_BLK_ID mb)
/*
 * Function: 	socend_untag
 * Purpose:	Untag an ethernet packet (if tagged)
 * Parameters:	mb - MBLK containing packet.
 * Returns:	Nothing
 * Notes:	Assumes packet contiguous in one MBLK/CBLK.
 */
{
    enet_hdr_t	*th, *uh;		/* Tagged/Untagged header pointers */
    const uint16 dec_length = sizeof(th->en_tag_ctrl)+sizeof(th->en_tag_len);

    th = (enet_hdr_t *)mb->mBlkHdr.mData;
    if (ENET_TAGGED(th)) {		/* If tagged - strip */
	uh = (enet_hdr_t *)((char *)th + dec_length);
	sal_memcpy(uh->en_shost, th->en_shost, /* ORDER --- FIRST */
		   sizeof(th->en_shost));
	sal_memcpy(uh->en_dhost, th->en_dhost, /* ORDER --- SECOND */
		   sizeof(th->en_dhost));
	if (ENET_LEN(uh)) {
	    uh->en_untagged_len = th->en_tag_len;
	}
	mb->mBlkHdr.mData += dec_length; /* Update SENS pointer */
	mb->mBlkPktHdr.len =
	mb->mBlkHdr.mLen  -= dec_length; /* Update SENS length */
    }
}

void
socend_receive_netjob(int pi, int pckti, int pckt_size)
/*
 * Function: 	socend_receive_netjob
 * Purpose:	Called from netjob after packet arrives.
 * Parameters:	p - pointer to (se), cookied passed on registration
 *		pckpi - pointer to packet data (int form).
 *		pckt_size - received length of packet.
 * Returns:	Nothing
 */
{
    socend_t		*se = (socend_t *)pi;
    struct end_object 	*eo = &se->se_eo;
    void		*pckt = (void *)pckti;
    bcm_vlan_t		vid;
    CL_BLK_ID		cb = NULL;
    M_BLK_ID		mb = NULL;

    /* Check the DCB for errors */
#if defined(BROADCOM_DEBUG)			/* Only compile in for debug */
    if (LOG_CHECK(BSL_LS_SYS_END | BSL_INFO) &&
        LOG_CHECK(BSL_LS_SOC_PACKET | BSL_INFO)) {
	d_packet_format("socend RX", DECODE_ETHER, pckt, pckt_size, NULL);
    }
#endif /* BROADCOM_DEBUG */
    /*
     * Build Cluster - then attach to mblks, 1 for each interface that
     * should get the packet. Broadcast/Multicast packets go up all the stacks.
     *
     * For multicast, it should only go up stacks that have registered for that
     * address, but for now, this will allow multiple interfaces to work.
     */

    if ((NULL == (cb = netClBlkGet(eo->pNetPool, M_DONTWAIT)))) {
	if (mb) netMblkFree(eo->pNetPool, mb);
	netClFree(eo->pNetPool, pckt);	/* Free packet */
	END_ERR_ADD(eo, MIB2_IN_ERRS, +1);
	return;
    }

#if defined(__mips__)
    if (SOC_IS_ESW(se->se_unit)) 
    {
        seu_t *seu = &socend_seu[se->se_unit];
        void  *tmp;
	
        sal_memcpy((UINT8 *)seu->pkt_align_buf + 2, (UINT8 *)pckt, pckt_size);
	
        tmp = seu->pkt_align_buf;
        seu->pkt_align_buf = pckt;
        pckt = tmp;
    }
#endif /* __mips__ */

    /* 1 MBLK for each interface */

    if ((mb = mBlkGet(eo->pNetPool, M_DONTWAIT, MT_DATA))) {
	netClBlkJoin(cb, pckt, SOC_END_PK_SZ, NULL, 0, 0, 0);
	netMblkClJoin(mb, cb);
	mb->mBlkPktHdr.len = mb->mBlkHdr.mLen = pckt_size;
	mb->mBlkHdr.mFlags |= M_PKTHDR;
#if defined(__mips__)
        if (SOC_IS_ESW(se->se_unit)) {
            mb->mBlkHdr.mData +=2;
        }
#endif /* __mips__ */
	vid = VLAN_CTRL_ID(soc_ntohs(((enet_hdr_t *)mb->mBlkHdr.mData)->en_tag_ctrl));
        if (SOC_IS_ESW(se->se_unit)) {
            socend_untag(mb);		/* Untag packet for now */
        } 
    } else {
	netClBlkFree(eo->pNetPool, cb);
	netClFree(eo->pNetPool, pckt);	/* Free packet */
	return;
    }

    {
	M_BLK_ID	tmb, fmb = mb;
	seu_t		*seu = &socend_seu[se->se_unit];
	/*
	 * Duplicate MBLK, and point to same cluster.
	 */
	for (se = seu->seu_devices; se != NULL; se = se->se_next) {

	    if (vid != se->se_vlan) {
		continue;
	    }

	    eo = &se->se_eo;

	    if (se->se_next != NULL) {
		tmb = mBlkGet(eo->pNetPool, M_DONTWAIT, MT_DATA);
		netMblkDup(mb, tmb);
	    } else {
		fmb = NULL;
		tmb = mb;
	    }

	    END_RCV_RTN_CALL(eo, tmb);
	}

	if (fmb) {
	    netMblkClFree(fmb);
	}
    }
}

/*
 * Function: 	socend_receive
 * Purpose:	Called Address mux when packet matches MAC address.
 * Parameters:	unit - StrataSwitch unit #
 *		port - StrataSwitch port #
 *		cookie
 *		reason -
 *		packet -
 *		length
 * Returns:
 *	BCM_RX_HANDLED - packet received for unknown reason
 *	BCM_RX_HANDLED_OWNED - packet owned.
 */

bcm_rx_t
socend_receive(int unit, bcm_pkt_t *pkt, void *cookie)
{
#define OK_REASON ((uint32)(-1))
    
    /*
     * Was: 
     *      (DMAS1_OP_IPSW | DMAS1_OP_IPINF | DMAS1_OP_IPMC |       \
     *       DMAS1_OP_TTL | BCM_PMUX_PR_DEFIP | BCM_PMUX_PR_COSMK | \
     *       DMAS1_OP_BCA)
     */

    int pkt_rv = BCM_RX_NOT_HANDLED;
    int rv;
    int enqueue = TRUE;

    if (pkt->rx_reason  & ~OK_REASON) {
        return BCM_RX_NOT_HANDLED;
    }

    /* Check VLAN for match; Check dest MAC for match; check for bcast */

    if (snoop_ip_rx != NULL) {
        pkt_rv = snoop_ip_rx(unit, pkt, &enqueue);
    }

    if (pkt_rv == BCM_RX_NOT_HANDLED && enqueue) {
        if (SOC_IS_ESW(unit)) {
            rv = netJobAdd((FUNCPTR)socend_receive_netjob,
                       (int)cookie,
                       (int)pkt->pkt_data[0].data,
                       (int)pkt->pkt_len,
                       4,
                       5);
            pkt_rv = ((rv == OK) ? BCM_RX_HANDLED_OWNED : BCM_RX_NOT_HANDLED);
        } else {
            char *packet;

            socend_packet_alloc(unit, SOC_END_PK_SZ, 0, (void *)&packet);
            if (NULL == packet){
                LOG_INFO(BSL_LS_SYS_END,
                         (BSL_META_U(unit,
                                     "failed to allocate buffer with socend_packet_alloc\n")));
                return BCM_RX_NOT_HANDLED;
            } 
            sal_memset(packet, 0x0, SOC_END_PK_SZ);
            sal_memcpy(packet, (unsigned char *)pkt->pkt_data[0].data, 
                         pkt->pkt_len);
            rv = netJobAdd((FUNCPTR)socend_receive_netjob,
                       (int)cookie,
                       (int)packet,
                       (int)pkt->pkt_len,
                       4,
                       5);
            LOG_INFO(BSL_LS_SYS_END,
                     (BSL_META_U(unit,
                                 "netJobAdd rv=0x%x OK=%x\n"), rv, OK));
            pkt_rv = BCM_RX_HANDLED;
        }
    }

    return pkt_rv;
}

/*
 * Function:
 *	_socend_l2_lookup
 * Purpose:
 *	Determine packets egress port(s) based on MAC and VLAN ID
 * Parameters:
 *	unit - StrataSwitch Unit #.
 *	mac - mac address of destination.
 *	vid - VLAN ID of destination.
 *	pkt - Where the port bitmaps are stored
 * Returns:
 *	OK/ERROR
 * Notes:
 */

int
_socend_l2_lookup(int unit, bcm_mac_t mac, vlan_id_t vid,
                    bcm_pkt_t *pkt)
{
    int                 rv = OK;
    bcm_port_t          port = 0;
    pbmp_t              raw_pbm;
    pbmp_t              l_pbm;
    pbmp_t              l_upbm;
    pbmp_t              l_l3pbm;     /* To be implemented */
    bcm_mcast_addr_t    mcaddr;
    bcm_module_t        modid = 0;
    int                 my_modid;

    BCM_PBMP_CLEAR(raw_pbm);
    BCM_PBMP_CLEAR(l_pbm);
    BCM_PBMP_CLEAR(l_upbm);
    BCM_PBMP_CLEAR(l_l3pbm);

    bcm_stk_my_modid_get(unit, &my_modid);
    if (ENET_MACADDR_UNICAST(mac)) {
        bcm_l2_addr_t l2addr;

        pkt->opcode = BCM_HG_OPCODE_UC;	
        bcm_l2_addr_t_init(&l2addr, mac, vid);
        rv = bcm_l2_addr_get(unit, mac, vid, &l2addr);
        modid = l2addr.modid;
        port = (modid == my_modid) ? l2addr.port : IPIC_PORT(unit);

        BCM_PBMP_ASSIGN(pkt->dest_mod, modid);
        BCM_PBMP_ASSIGN(pkt->dest_port, l2addr.port);

        if (rv >= 0) { /* Determine if it's going out untagged */
            rv = bcm_vlan_port_get(unit, vid, &l_pbm, &l_upbm);
	    BCM_PBMP_CLEAR(l_pbm);
	    BCM_PBMP_PORT_ADD(l_pbm, port);
        } else {  /* combine all error return values */
            rv = ERROR;
        }
    } else if (ENET_MACADDR_MULTICAST(mac)) {
        pkt->opcode = BCM_HG_OPCODE_MC;	
        rv = bcm_mcast_port_get(unit, mac, vid, &mcaddr);
        if (rv >= 0) {
        } else {  /* combine all error return values */
            rv = ERROR;
        }
	BCM_PBMP_ASSIGN(raw_pbm, mcaddr.pbmp);
	BCM_PBMP_ASSIGN(l_pbm, mcaddr.pbmp);
	BCM_PBMP_ASSIGN(l_upbm, mcaddr.ubmp);
    } else if (ENET_MACADDR_BROADCAST(mac)) {
        pkt->opcode = BCM_HG_OPCODE_BC;	    
        rv = ERROR;
    } else {
        rv = ERROR;
    }

    switch(rv) {
    case ERROR:
        /*
         * If no match was found, then spray out all ports in vlan.
         */
        rv = bcm_vlan_port_get(unit, vid, &l_pbm, &l_upbm);
        if (rv >= 0) {
            rv = OK;
        } else {  /* combine all error return values */
            rv = ERROR;
        }
        break;
    default:
        break;
    }

    BCM_PBMP_PORT_REMOVE(l_pbm, CMIC_PORT(unit));	/* Never back to us */

    pkt->flags |= BCM_TX_SRC_MOD;
    BCM_PBMP_ASSIGN(pkt->src_mod, my_modid);
    pkt->flags |= BCM_TX_SRC_PORT;
    BCM_PBMP_ASSIGN(pkt->src_port, CMIC_PORT(unit));
    BCM_PBMP_ASSIGN(pkt->tx_pbmp, l_pbm);
    BCM_PBMP_ASSIGN(pkt->tx_upbmp, l_upbm);
    BCM_PBMP_ASSIGN(pkt->tx_l3pbmp, l_l3pbm);	/* Always 0 for now */
    
    return(rv);
}

