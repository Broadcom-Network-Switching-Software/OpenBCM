/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_CM_H
#define _SOC_CM_H

#include <sal/types.h>
#include <sal/core/sync.h>
#include <soc/cmtypes.h>
#include <soc/cmdebug.h>
#include <soc/devids.h>
#include <soc/defs.h>		/* for SOC_MAX_NUM_DEVICES */
#ifdef INCLUDE_RCPU
#include <soc/rcpu.h>
#endif
/* 
 * Configuration Manager
 *
 * This interface is for the driver side only
 *
 * All driver access to the chip must be through this
 * interface and this interface ONLY. 
 */

#define SOC_CM_REVID_MASK 0xff
/* A flag to mark this entry is used for any device ID with (ID & 0xfff0) == (specified device ID) */
#define SOC_CM_DEVICE_ANY_LAST_DEVID_DIGIT  0x100
/* A flag to mark this entry is used for any revision with ID >= ((specified revID) && SOC_CM_REVID_MASK) && ID < 0xff */
#define SOC_CM_DEVICE_OR_LARGER_REVID 0x200
#define SOC_CM_DEVICE_LARGER_REVID_AND_ANY_LAST_DEVID_DIGIT (SOC_CM_DEVICE_ANY_LAST_DEVID_DIGIT | SOC_CM_DEVICE_OR_LARGER_REVID)

/* Register Initialization */

extern char     *soc_cm_config_var_get(int unit, const char *name);

/* Device information */

extern int      soc_cm_get_id(int unit, uint16 *dev_id, uint8 *rev_id);
extern int      soc_cm_get_id_driver(uint16 dev_id, uint8 rev_id,
                                     uint16 *dev_id_driver,
				     uint16 *rev_id_driver);
extern const char *soc_cm_get_name(int unit);
extern uint32 soc_cm_get_dev_type(int unit);
extern const char *soc_cm_get_device_name(uint16 dev_id, uint8 rev_id);
extern int      soc_cm_get_num_devices(void);
extern int      soc_cm_get_endian(int unit, int *pio, int *packet, int *other);
#if defined(SRC_SOC_COMMON_ACTIVATE_UNUSED)
extern sal_vaddr_t   soc_cm_get_base_address(int unit);
#endif
#if defined(SRC_SOC_COMMON_ACTIVATE_UNUSED) || defined(BCM_LTSW_SUPPORT) || defined(BCM_CTSW_SUPPORT)
extern int soc_cm_dev_num_get(int unit, int *dev_num);
#endif

#ifdef INCLUDE_RCPU
#if defined(SRC_SOC_COMMON_ACTIVATE_UNUSED) || defined (BCM_ESW_SUPPORT)
extern int  soc_cm_get_rcpu_cfg(int unit, soc_rcpu_cfg_t *rcpu_cfg);
extern int  soc_cm_set_rcpu_cfg(int unit, soc_rcpu_cfg_t *rcpu_cfg);
#endif
#if defined(SRC_SOC_COMMON_ACTIVATE_UNUSED) || defined(BCM_ESW_SUPPORT) || defined(INCLUDE_RCPU)
extern int  soc_cm_set_rcpu_trans_tpr(int unit, rcpu_trans_ptr_t *rcpu_tp);
#endif
#endif /* INCLUDE_RCPU */

/* Device interrupts */

extern int
soc_cm_interrupt_connect(int unit, soc_cm_isr_func_t f, void *data);

extern int      soc_cm_interrupt_enable(int unit);
extern void     soc_cm_interrupt_disable(int unit);
extern int      soc_cm_interrupt_disconnect(int unit);

/* Device Shared Memory Management */

extern void     *soc_cm_salloc(int unit, int size, const char *name);
extern void     soc_cm_sfree(int unit, void *ptr);
extern int      soc_cm_sflush(int unit, void *addr, int length);
extern int      soc_cm_sinval(int unit, void *addr, int length);

/* Device PCI config read/write */

#if defined(SRC_SOC_COMMON_ACTIVATE_UNUSED) || defined(BCM_ESW_SUPPORT)
extern uint32   soc_cm_pci_conf_read(int unit, uint32 addr);
#endif
#if defined(SRC_SOC_COMMON_ACTIVATE_UNUSED)
extern void     soc_cm_pci_conf_write(int unit, uint32 addr, uint32 data);
#endif

/* Device Address Translations */

extern sal_paddr_t   soc_cm_l2p(int unit, void *addr);
extern void     *soc_cm_p2l(int unit, sal_paddr_t addr);

/* iProc Access */

extern uint32   soc_cm_iproc_read(int unit, uint32 addr);
extern void     soc_cm_iproc_write(int unit, uint32 addr, uint32 val);


extern void     soc_cm_display_known_devices(void);

extern uint32  soc_cm_get_bus_type(int unit);

/* device registers access by I2C, given an internal device address */

extern int soc_cm_i2c_device_read(int unit, uint32 addr, uint32 *value);
extern int soc_cm_i2c_device_write(int unit, uint32 addr, uint32 value);

/*
 * Function: soc_cm_get_composite_dev_info
 * Purpose: w=return the CM information for a composite device
 * Parameters: unit - the unit/device ID of the device
 * Returns: If the device is composite, returns its information structure,
 *          otherwise returns NULL.
 */
extern const soc_cm_composite_dev_t* soc_cm_get_composite_dev_info(int unit);

/*
 * Function: soc_cm_get_nof_sub_units
 * Purpose: return the number of sub units of the given unit
 * Parameters: unit - the unit of the device
 * Returns: If the given unit is composite, returns its number of sub-units
 *          otherwise returns zero.
 */
extern unsigned soc_cm_get_nof_sub_units(int unit);

/*
 * Function:    soc_cm_find_comp_dev_info_by_subdev
 * Purpose:     Find the composite device information
 *
 * Parameters:  sub_dev_id - the device id of the sub devices of the required composite device.
 *              comp_dev_id-  output:Compodsite Device ID as may be specified in the composite_units soc property
 *              nof_sub_devs- output: The number of sub devices
 *
 * Returns:     Zero if successful, negative if the composite device type was not found
 */
extern int
soc_cm_find_comp_dev_info_by_subdev(uint16 sub_dev_id, uint32 *comp_dev_id, unsigned *nof_sub_devs);

/* Convert between unit and BDE device IDs */

/* Return the unit=logical deviceID=CM handle given the bde device, or a negative error value */
extern int soc_cm_bde_dev_2_unit(int bde_dev_num);
/* Return thre BDE device ID given the unit=logical deviceID=CM handle, or a negative error value */
extern int soc_cm_unit_2_bde_dev(int unit);

/* Return the main unit=logical deviceID=CM handle give a sub-unit */
extern int soc_cm_get_main_unit(int unit);

/* Register Access */

typedef struct {
    soc_cm_dev_t		dev;
    soc_cm_device_vectors_t	vectors;
} cm_device_t;

extern cm_device_t              soc_cm_device[SOC_MAX_NUM_DEVICES];
extern int                      soc_cm_device_count;
extern soc_cm_composite_dev_t   cm_composite_devices[SOC_MAX_NOF_COMPOSITE_DEVS];

/* CM device information by unit=logical device number=CM device handle */
#define	CMDEV(unit)		soc_cm_device[unit]
#define	CMVEC(unit)		CMDEV(unit).vectors

/* maximum number of composite devices */
#define MAX_NOF_COMPOSITE_DEVICES (SOC_MAX_NUM_DEVICES / 3)

/* get the main device of a sub-device */
#define CM_SUB2MAIN_DEV(unit)                (CMDEV(unit).dev.composite_index)
/* get a sub-device of a main device */
#define CM_MAIN2SUB_DEV(unit, sub_unit_index) (cm_composite_devices[CMDEV(unit).dev.composite_index].sub_units[sub_unit_index])
/* get the main device of a sub-device, or -1 if the input is not a sub-device */
#define CM_SUB2MAIN_DEV_SAFE(unit) \
  ((unit) < SOC_MAX_NUM_DEVICES && (unit) >= 0 && CMDEV(unit).dev.info && \
     !(CMDEV(unit).dev.info->dev_type & SOC_COMPOSITE_DEV_TYPE) && \
     CMDEV(unit).dev.composite_index != SOC_CM_NONE_COMPOSITE_DEV && \
     CM_SUB2MAIN_DEV(unit) < SOC_MAX_NUM_DEVICES ? \
   CM_SUB2MAIN_DEV(unit) : -1)
/* get a sub-unit of a main unit, or -1 if the input is not a composite unit and a valid sub unit index */
#define CM_MAIN2SUB_DEV_SAFE(unit, sub_unit_index) \
  ((unit) < SOC_MAX_NUM_DEVICES && CMDEV(unit).dev.info && (CMDEV(unit).dev.info->dev_type & SOC_COMPOSITE_DEV_TYPE) \
  && CMDEV(unit).dev.composite_index < SOC_MAX_NOF_COMPOSITE_DEVS && \
  sub_unit_index < ((soc_cm_comp_device_info_t*)CMDEV(unit).dev.info)->nof_sub_devs ? \
  CM_MAIN2SUB_DEV(unit, (sub_unit_index)) : -1)
/* test if the device is composite */
#define CM_IS_COMPOSITE_DEVICE(unit) (CMDEV(unit).dev.info->dev_type & SOC_COMPOSITE_DEV_TYPE)
/* test if the device is a sub-device */
#define CM_IS_SUB_DEVICE(unit) (CMDEV(unit).dev.info->dev_type & SOC_SUB_DEV_TYPE)

typedef struct shared_block_s {
    uint32                start_sentinel;         /* value: 0xAABBCCDD */
    char                  *description;
    int                   size;
    int                   modified_size;
    struct shared_block_s *prev;
    struct shared_block_s *next;
    /* Variable user data; size S = (size + 3) / 4 words. */
    uint32                user_data[1];
    /* Then sentinel follows user data at user_data[S]; value: 0xDDCCBBAA */
} shared_block_t;

#ifdef BROADCOM_DEBUG
#define SHARED_GOOD_START(p) (p->start_sentinel == 0xaabbccdd)
#define SHARED_GOOD_END(p)   (p->user_data[(p->size + 3) / 4] == 0xddccbbaa)
#define SHARED_GOOD_FREE(p)  ((p->start_sentinel != 0xdeadbeef) || (p->user_data[(p->size + 3) / 4] != 0xdddddddd))
#ifdef BROADCOM_DEBUG_RISKY
#define SHARED_GOOD_END_DEBUG(p) SHARED_GOOD_END(p)
#else
#define SHARED_GOOD_END_DEBUG(p) 1
#endif
extern  int soc_cm_shared_good_range(int dev, shared_block_t *p);
extern  void soc_cm_dump_info(int unit);

#endif

/*
 * There are actually several different implementations that can be
 * configured for CMREAD and CMWRITE (which are effectively, pci read
 * and write).
 *
 * 1. If SOC_CM_MEMORY_BASE is defined, then that macro contains
 *    the constant base address of all pci devices.  The macro
 *    SOC_CM_MEMORY_OFFSET will be multiplied by the device and added
 *    to the base to create the actual pci address.  The cm vector
 *    base addresses will be checked against this.
 * 2. If SOC_CM_MEMORY is defined then the cm vector base_address
 *    will be used as a directo derefence to access pci space
 * 3. If SOC_CM_FUNCTION is defined then the cm vector read and
 *    write routines will be called.
 * 4. If none of the above, then a runtime check of both the cm vector
 *    base_address and the read/write routines will happen.
 */
#ifdef KEYSTONE
/*
 * Enforce PCIE transaction ordering. Commit the write transaction.
 */
#define _SSOC_CMREAD(_unit,_a) ({__asm__ __volatile__("sync"); SOC_E_NONE;})
#else
#define _SSOC_CMREAD(_unit,_a) SOC_E_NONE
#endif

#ifdef	SOC_CM_MEMORY_BASE
#ifdef  EXTERN_SOC_CM_MEMORY_BASE
extern uint32 EXTERN_SOC_CM_MEMORY_BASE;
#endif
#define SOC_CM_CMIC_BAR_START(_unit) ((VOL uint32 *)(SOC_CM_MEMORY_BASE+(SOC_CM_MEMORY_OFFSET*_unit)))
#define SOC_CM_CMIC_BAR_START64(_unit) ((VOL uint64 *)(SOC_CM_MEMORY_BASE+(SOC_CM_MEMORY_OFFSET*_unit)))
#define	CMREAD(_unit,_a)	\
	(SOC_CM_CMIC_BAR_START(_unit)[(_a)/4])
#define	CMWRITE(_unit,_a,_data)	\
	((CMREAD(_unit,_a) = _data), _SSOC_CMREAD(_unit,_a))
#define CMREAD64(unit, a) CMREAD(unit, a)
#define CMWRITE64(unit, a, data) CMWRITE(unit, a, data)
#else
#ifdef	SOC_CM_MEMORY
#define SOC_CM_CMIC_BAR_START(_unit) ((VOL uint32 *)CMVEC(_unit).base_address)
#define	CMREAD(_unit,_a)	\
	(((VOL uint32 *)CMVEC(_unit).base_address)[(_a)/4])
#define	CMWRITE(_unit,_a,_data)	\
	((CMREAD(_unit,_a) = _data), _SSOC_CMREAD(_unit,_a))
#define	CMREAD64(_unit,_a)	\
	(*(VOL uint64*)(((VOL uint8*)CMVEC(_unit).base_address)+(_a)))
#define	CMWRITE64(_unit,_a,_data)	\
	((CMREAD64(_unit,_a) = _data), _SSOC_CMREAD(_unit,_a))
#else
#ifdef	SOC_CM_FUNCTION
#define	CMREAD(_unit,_a)	\
	(CMVEC(_unit).read(&CMDEV(_unit).dev, _a))
#define	CMWRITE(_unit,_a,_data)	\
	((CMVEC(_unit).write(&CMDEV(_unit).dev, _a, _data)), _SSOC_CMREAD(_unit,_a))
#define	CMREAD64(_unit,_a)	\
	(CMVEC(_unit).read64(&CMDEV(_unit).dev, _a))
#define	CMWRITE64(_unit,_a,_data)	\
	((CMVEC(_unit).write64(&CMDEV(_unit).dev, _a, _data)), _SSOC_CMREAD(_unit,_a))
#else
#define SOC_CM_CMIC_BAR_START(_unit) ((VOL uint32 *)CMVEC(_unit).base_address)
#define SOC_CM_CMIC_BAR_START64(_unit) ((VOL uint64 *)CMVEC(_unit).base_address)
#define	CMREAD(_unit,_a)	\
	(CMVEC(_unit).base_address ? \
	    SOC_CM_CMIC_BAR_START(_unit)[(_a)/4] : \
	    CMVEC(_unit).read(&CMDEV(_unit).dev, _a))
#define	CMWRITE(_unit,_a,_data)	\
	(CMVEC(_unit).base_address ? \
	    ((void)(((VOL uint32 *)CMVEC(_unit).base_address)[(_a)/4] = _data), _SSOC_CMREAD(_unit,_a)) : \
	    (CMVEC(_unit).write(&CMDEV(_unit).dev, _a, _data), _SSOC_CMREAD(_unit,_a)))
#define	CMREAD64(_unit,_a)	\
	(CMVEC(_unit).base_address ? \
	    (*(VOL uint64*)(((VOL uint8*)CMVEC(_unit).base_address)+(_a))) : \
	    CMVEC(_unit).read64(&CMDEV(_unit).dev, _a))
#define	CMWRITE64(_unit,_a,_data)	\
	(CMVEC(_unit).base_address ? \
	    ((void)(*(VOL uint64*)(((VOL uint8*)CMVEC(_unit).base_address)+(_a)) = _data), _SSOC_CMREAD(_unit,_a)) : \
	    (CMVEC(_unit).write64(&CMDEV(_unit).dev, _a, _data), _SSOC_CMREAD(_unit,_a)))
#endif	/* SOC_CM_FUNCTION */
#endif	/* SOC_CM_MEMORY */
#endif	/* SOC_CM_MEMORY_BASE */

/* Absolute Read/Write */
/* Bypasses any base_address, and R/W to the absolute address */
#ifdef	SOC_CM_MEMORY
#define	CMAREAD(_unit,_a)	\
	(((VOL uint32 *)0)[(_a)/4])
#define	CMAWRITE(_unit,_a,_data)	\
	((CMAREAD(_unit,_a) = _data), _SSOC_CMREAD(_unit,_a))
#else
#ifdef	SOC_CM_FUNCTION
#define	CMAREAD(_unit,_a)	\
	(CMVEC(_unit).read(&CMDEV(_unit).dev, _a))
#define	CMAWRITE(_unit,_a,_data)	\
	((CMVEC(_unit).write(&CMDEV(_unit).dev, _a, _data)), _SSOC_CMREAD(_unit,_a))
#else
#define	CMAREAD(_unit,_a)	\
	(CMVEC(_unit).base_address ? \
	    ((VOL uint32 *)0)[(_a)/4] : \
	    CMVEC(_unit).read(&CMDEV(_unit).dev, _a))
#define	CMAWRITE(_unit,_a,_data)	\
	(CMVEC(_unit).base_address ? \
	    ((void)(((VOL uint32 *)0)[(_a)/4] = _data), _SSOC_CMREAD(_unit,_a)) : \
	    (CMVEC(_unit).write(&CMDEV(_unit).dev, _a, _data), _SSOC_CMREAD(_unit,_a)))
#endif	/* SOC_CM_FUNCTION */
#endif	/* SOC_CM_MEMORY */

#define	CMCONFREAD(_unit,_a)	\
	(CMVEC(_unit).pci_conf_read(&CMDEV(_unit).dev, _a))
#define	CMCONFWRITE(_unit,_a,_data)	\
	(CMVEC(_unit).pci_conf_write(&CMDEV(_unit).dev, _a, _data))

#endif  /* !_SOC_CM_H */
