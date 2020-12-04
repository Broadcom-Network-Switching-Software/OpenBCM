/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM Control
 */

#ifndef	_BCM_INT_CONTROL_H
#define	_BCM_INT_CONTROL_H

#include <bcm/types.h>
#include <bcm/init.h>
#include <bcm_int/dispatch.h>

#ifndef	BCM_CONTROL_MAX
#define	BCM_CONTROL_MAX	BCM_UNITS_MAX
#endif

/*
 * BCM Supported Dispatch Type Enumeration. 
 */
#define BCM_DLIST_ENTRY(_dtype) \
bcmDtype_##_dtype,

typedef enum bcm_dtype_e {
#include <bcm_int/bcm_dlist.h>
    bcmTypeCount,
    bcmTypeNone = -1
} bcm_dtype_t; 
/* Enums which describe the state of bcm_control_t structure */
typedef enum _bcm_control_state_e {
    _bcmControlStateNone = 0,       /* No modules are attached */

    _bcmControlStateTxRxInit,       /* To Initialize tx and rx modules only */
    _bcmControlStateTxRxInited,     /* Only Tx and Rx modules are initialized */
    _bcmControlStateInitedAll,      /* All modules are initialized */

    _bcmControlStateDeinitLateTxRx, /* Detach all modules except Tx and Rx */
    _bcmControlStateDeinitAll,      /* Only Tx and Rx need to be deinited */

    _bcmControlStateCount
} _bcm_control_state_t;

typedef struct {
    int			unit;		/* driver unit */
    void		*drv_control;	/* driver control structure */
    bcm_dtype_t         dtype;          /* driver dispatch type */
    const char*         name;           /* driver dispatch name */
    char		*subtype;	/* attached subtype (or NULL) */

    uint32		chip_vendor;	/* switch chip ident (pci id) */
    uint32		chip_device;
    uint32		chip_revision;
    uint32		capability;	/* chip capabilities */
#define		BCM_CAPA_SWITCH		BCM_INFO_SWITCH	/* net switch chip */
#define		BCM_CAPA_FABRIC		BCM_INFO_FABRIC	/* fabric chip */
#define		BCM_CAPA_L3		BCM_INFO_L3	/* chip has layer 3 */
#define		BCM_CAPA_IPMC		BCM_INFO_IPMC	/* chip has IP mcast */
#define		BCM_CAPA_LOCAL		0x1000	/* local (direct) dispatch */
#define		BCM_CAPA_REMOTE		0x2000	/* remotely dispatch */
#define		BCM_CAPA_COMPOSITE	0x4000	/* composite dispatch */
#define		BCM_CAPA_CALLBACK	0x8000	/* attach callback complete */

   /* Used with early_attach and late_detach sequences. 
    * Will be one of _bcm_control_state_t enums             */
    int         attach_state;      

    /* per module storage */
    void		*data_auth;
    void		*data_i2c;
    void		*data_cosq;
    void		*data_diffserv;
    void		*data_dmux;
    void		*data_filter;
    void		*data_init;
    void		*data_ipmc;
    void		*data_l2;
    void		*data_l3;
    void		*data_link;
    void		*data_mcast;
    void		*data_meter;
    void		*data_mirror;
    void		*data_pkt;
    void		*data_port;
    void		*data_rate;
    void		*data_rx;
    void		*data_stack;
    void		*data_stat;
    void		*data_stg;
    void		*data_switch;
    void		*data_trunk;
    void		*data_tx;
    void		*data_vlan;
    void		*data_misc1;
    void		*data_misc2;
    void		*data_misc3;
    void		*data_misc4;

    /* system_init() return value */
    int         system_initialized_rv;
} bcm_control_t;

extern bcm_control_t	*bcm_control[BCM_CONTROL_MAX];

#define		BCM_CONTROL(_unit)	bcm_control[_unit]
#define		BCM_DISPATCH(_unit)	BCM_CONTROL(_unit)->dispatch

/* BCM control unit in legal range? */
#define         BCM_CONTROL_UNIT_LEGAL(_unit) \
    (((_unit) >= 0) && ((_unit) < BCM_CONTROL_MAX))

/* use below dispatch layer or during init/deinit */
#define		BCM_CONTROL_UNIT_VALID(_unit) \
    ((BCM_CONTROL_UNIT_LEGAL(_unit)) && (BCM_CONTROL(_unit) != NULL))

/* use at dispatch layer and above */
#ifdef BCM_CONTROL_API_TRACKING

extern uint32   bcm_control_unit_valid[BCM_CONTROL_MAX];
extern int      bcm_unit_refcount(int unit, int refcount);


#define		BCM_UNIT_VALID(_unit)	(BCM_CONTROL_UNIT_LEGAL(_unit) && \
					 bcm_control_unit_valid[_unit] != 0)

/* BCM_UNIT_CHECK/BCM_UNIT_IDLE and
   BCM_UNIT_BUSY/BCM_UNIT_IDLE must always be used in pairs.
   BCM_UNIT_CHECK() returns a unit valid flag and is used as a predicate.
   BCM_UNIT_BUSY() does not return a flag and is used standalone.
   BCM_UNIT_IDLE() is always used standalone.
*/
#define         BCM_UNIT_CHECK(unit) bcm_unit_refcount((unit), 1)
#define         BCM_UNIT_BUSY(unit) ((void)BCM_UNIT_CHECK(unit))
#define         BCM_UNIT_IDLE(unit) ((void)bcm_unit_refcount((unit), -1))

#else /* !BCM_CONTROL_API_TRACKING */

#define		BCM_UNIT_VALID(_unit) BCM_CONTROL_UNIT_VALID(_unit)
#define         BCM_UNIT_CHECK(unit) BCM_UNIT_VALID(unit) 
#define         BCM_UNIT_BUSY(unit) 
#define         BCM_UNIT_IDLE(unit)

#endif /* BCM_CONTROL_API_TRACKING */

#define		BCM_CAPABILITY(_unit)	BCM_CONTROL(_unit)->capability
#define		BCM_IS_LOCAL(_unit)	(BCM_CAPABILITY(_unit) & \
					 BCM_CAPA_LOCAL)
#define		BCM_IS_REMOTE(_unit)	(BCM_CAPABILITY(_unit) & \
					 BCM_CAPA_REMOTE)
#define		BCM_IS_SWITCH(_unit)	(BCM_CAPABILITY(_unit) & \
					 BCM_CAPA_SWITCH)
#define		BCM_IS_FABRIC(_unit)	(BCM_CAPABILITY(_unit) & \
					 BCM_CAPA_FABRIC)
#define         BCM_DTYPE(_unit)        BCM_CONTROL(_unit)->dtype
#define         BCM_TYPE_NAME(_unit)     BCM_CONTROL(_unit)->name

extern int bcmi_warmboot_get(int unit);

#endif	/* _BCM_INT_CONTROL_H */
