/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
#define SOC_CM_DEVICE_LARGET_REVID_AND_ANY_LAST_DEVID_DIGIT (SOC_CM_DEVICE_ANY_LAST_DEVID_DIGIT | SOC_CM_DEVICE_OR_LARGER_REVID)

/* Register Initialization */

extern char     *soc_cm_config_var_get(int dev, const char *name);

/* Device information */

extern int      soc_cm_get_id(int dev, uint16 *dev_id, uint8 *rev_id);
extern int      soc_cm_get_id_driver(uint16 dev_id, uint8 rev_id,
                                     uint16 *dev_id_driver,
				     uint16 *rev_id_driver);
extern const char *soc_cm_get_name(int dev);
extern uint32 soc_cm_get_dev_type(int dev);
extern const char *soc_cm_get_device_name(uint16 dev_id, uint8 rev_id);
extern int      soc_cm_get_num_devices(void);
extern int      soc_cm_get_endian(int dev, int *pio, int *packet, int *other);
extern sal_vaddr_t   soc_cm_get_base_address(int unit);
extern int soc_cm_dev_num_get(int dev, int *dev_num);

#ifdef INCLUDE_RCPU
extern int  soc_cm_get_rcpu_cfg(int dev, soc_rcpu_cfg_t *rcpu_cfg);
extern int  soc_cm_set_rcpu_cfg(int dev, soc_rcpu_cfg_t *rcpu_cfg);
extern int  soc_cm_set_rcpu_trans_tpr(int dev, rcpu_trans_ptr_t *rcpu_tp);
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

extern uint32   soc_cm_pci_conf_read(int dev, uint32 addr);
extern void     soc_cm_pci_conf_write(int dev, uint32 addr, uint32 data);

/* Device Address Translations */

extern sal_paddr_t   soc_cm_l2p(int unit, void *addr);
extern void     *soc_cm_p2l(int unit, sal_paddr_t addr);

/* iProc Access */

extern uint32   soc_cm_iproc_read(int dev, uint32 addr);
extern void     soc_cm_iproc_write(int dev, uint32 addr, uint32 val);


extern void     soc_cm_display_known_devices(void);

extern uint32  soc_cm_get_bus_type(int dev);

/* device registers access by I2C, given an internal device address */

extern int soc_cm_i2c_device_read(int dev, uint32 addr, uint32 *value);
extern int soc_cm_i2c_device_write(int dev, uint32 addr, uint32 value);

/* Register Access */

typedef struct {
    soc_cm_dev_t		dev;
    soc_cm_device_vectors_t	vectors;
} cm_device_t;

extern cm_device_t              soc_cm_device[SOC_MAX_NUM_DEVICES];
extern int                      soc_cm_device_count;

#define	CMDEV(dev)		soc_cm_device[dev]
#define	CMVEC(dev)		CMDEV(dev).vectors

/* maximum number of composite devices */
#define MAX_NOF_COMPOSITE_DEVICES (SOC_MAX_NUM_DEVICES / 3)

/* get the main device of a sub-device */
#define CM_SUB2MAIN_DEV(d)                (CMDEV(d).dev.composite_index)
/* get a sub-device of a main device */
#define CM_MAIN2SUB_DEV(d, sub_dev_index) (cm_composite_devices[CMDEV(d).dev.composite_index].sub_devs[sub_dev_index])
/* get the main device of a sub-device, or -1 if the input is not a sub-device */
#define CM_SUB2MAIN_DEV_SAFE(d) \
  ((d) < SOC_MAX_NUM_DEVICES && CMDEV(d).dev.info && \
     !(CMDEV(d).dev.info->dev_type & SOC_COMPOSITE_DEV_TYPE) && \
     CMDEV(d).dev.composite_index != SOC_CM_NONE_COMPOSITE_DEV && \
     CM_SUB2MAIN_DEV(d) < SOC_MAX_NUM_DEVICES ? \
   CM_SUB2MAIN_DEV(d) : -1)
/* get a sub-device of a main device, or -1 if the input is not a sub-device */
#define CM_MAIN2SUB_DEV_SAFE(d, sub_dev_index) \
  ((d) < SOC_MAX_NUM_DEVICES && CMDEV(d).dev.info && (CMDEV(d).dev.info->dev_type & SOC_COMPOSITE_DEV_TYPE) \
  && CMDEV(d).dev.composite_index < SOC_MAX_NOF_COMPOSITE_DEVS && \
  sub_dev_index < ((soc_cm_comp_device_info_t*)CMDEV(d).dev.info)->nof_sub_devs ? \
  CM_MAIN2SUB_DEV(d, (sub_dev_index)) : -1)
/* test if the device is composite */
#define CM_IS_COMPOSITE_DEVICE(d) (CMDEV(d).dev.info->dev_type & SOC_COMPOSITE_DEV_TYPE)

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
#define _SSOC_CMREAD(_d,_a) ({__asm__ __volatile__("sync"); SOC_E_NONE;})
#else
#define _SSOC_CMREAD(_d,_a) SOC_E_NONE
#endif

#ifdef	SOC_CM_MEMORY_BASE
#ifdef  EXTERN_SOC_CM_MEMORY_BASE
extern uint32 EXTERN_SOC_CM_MEMORY_BASE;
#endif
#define SOC_CM_CMIC_BAR_START(_d) ((VOL uint32 *)(SOC_CM_MEMORY_BASE+(SOC_CM_MEMORY_OFFSET*_d)))
#define	CMREAD(_d,_a)	\
	(SOC_CM_CMIC_BAR_START(_d)[(_a)/4])
#define	CMWRITE(_d,_a,_data)	\
	((CMREAD(_d,_a) = _data), _SSOC_CMREAD(_d,_a))
#else
#ifdef	SOC_CM_MEMORY
#define SOC_CM_CMIC_BAR_START(_d) ((VOL uint32 *)CMVEC(_d).base_address)
#define	CMREAD(_d,_a)	\
	(((VOL uint32 *)CMVEC(_d).base_address)[(_a)/4])
#define	CMWRITE(_d,_a,_data)	\
	((CMREAD(_d,_a) = _data), _SSOC_CMREAD(_d,_a))
#else
#ifdef	SOC_CM_FUNCTION
#define	CMREAD(_d,_a)	\
	(CMVEC(_d).read(&CMDEV(_d).dev, _a))
#define	CMWRITE(_d,_a,_data)	\
	((CMVEC(_d).write(&CMDEV(_d).dev, _a, _data)), _SSOC_CMREAD(_d,_a))
#else
#define SOC_CM_CMIC_BAR_START(_d) ((VOL uint32 *)CMVEC(_d).base_address)
#define	CMREAD(_d,_a)	\
	(CMVEC(_d).base_address ? \
	    SOC_CM_CMIC_BAR_START(_d)[(_a)/4] : \
	    CMVEC(_d).read(&CMDEV(_d).dev, _a))
#define	CMWRITE(_d,_a,_data)	\
	(CMVEC(_d).base_address ? \
	    ((void)(((VOL uint32 *)CMVEC(_d).base_address)[(_a)/4] = _data), _SSOC_CMREAD(_d,_a)) : \
	    (CMVEC(_d).write(&CMDEV(_d).dev, _a, _data), _SSOC_CMREAD(_d,_a)))
#endif	/* SOC_CM_FUNCTION */
#endif	/* SOC_CM_MEMORY */
#endif	/* SOC_CM_MEMORY_BASE */

/* Absolute Read/Write */
/* Bypasses any base_address, and R/W to the absolute address */
#ifdef	SOC_CM_MEMORY
#define	CMAREAD(_d,_a)	\
	(((VOL uint32 *)0)[(_a)/4])
#define	CMAWRITE(_d,_a,_data)	\
	((CMAREAD(_d,_a) = _data), _SSOC_CMREAD(_d,_a))
#else
#ifdef	SOC_CM_FUNCTION
#define	CMAREAD(_d,_a)	\
	(CMVEC(_d).read(&CMDEV(_d).dev, _a))
#define	CMAWRITE(_d,_a,_data)	\
	((CMVEC(_d).write(&CMDEV(_d).dev, _a, _data)), _SSOC_CMREAD(_d,_a))
#else
#define	CMAREAD(_d,_a)	\
	(CMVEC(_d).base_address ? \
	    ((VOL uint32 *)0)[(_a)/4] : \
	    CMVEC(_d).read(&CMDEV(_d).dev, _a))
#define	CMAWRITE(_d,_a,_data)	\
	(CMVEC(_d).base_address ? \
	    ((void)(((VOL uint32 *)0)[(_a)/4] = _data), _SSOC_CMREAD(_d,_a)) : \
	    (CMVEC(_d).write(&CMDEV(_d).dev, _a, _data), _SSOC_CMREAD(_d,_a)))
#endif	/* SOC_CM_FUNCTION */
#endif	/* SOC_CM_MEMORY */

#define	CMCONFREAD(_d,_a)	\
	(CMVEC(_d).pci_conf_read(&CMDEV(_d).dev, _a))
#define	CMCONFWRITE(_d,_a,_data)	\
	(CMVEC(_d).pci_conf_write(&CMDEV(_d).dev, _a, _data))

#endif  /* !_SOC_CM_H */
