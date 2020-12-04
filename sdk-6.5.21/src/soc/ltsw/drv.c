/*! \file drv.c
 *
 * SOC driver
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#if defined(BCM_LTSW_SUPPORT)

#include <bsl/bsl.h>

#include <soc/drv.h>
#include <soc/ltsw/drv.h>
#include <bcmdrd/bcmdrd_dev.h>

#include <soc/cm.h>

/******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_SOC_COMMON
#define TO_STRINGS(_a) #_a

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1)\
            TO_STRINGS(_bd),
static char *base_drv_name[] = {
        NULL, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
};

/*! Driver info of SDKLT-based devices, indexed by DRD handle. */
static soc_driver_t soc_ltsw_driver[SOC_MAX_NUM_DEVICES] = {{0}};


/******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the soc_driver for a unit.
 *
 * Populate the soc_driver structure for a unit.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to soc_driver structure.
 */
static soc_driver_t *
soc_ltsw_chip_driver_find(int unit)
{
    soc_driver_t *sd;
    uint16 dev_id;
    uint8 rev_id;

    sd = &soc_ltsw_driver[unit];
    soc_cm_get_id(unit, &dev_id, &rev_id);

    sal_memset(sd,0,sizeof(soc_driver_t));
    sd->chip_string = (char *)soc_cm_get_device_name(dev_id, rev_id);
    sd->pci_vendor = BROADCOM_VENDOR_ID;
    sd->pci_device = dev_id;
    sd->pci_revision = rev_id;

    return sd;
}

/*!
 * \brief Initialize soc_info for a unit.
 *
 * Fill in soc_info structure for a newly attached unit.
 *
 * \param [in] unit Unit number.
 * \param [in] soc  Pointer to soc_control associated with this unit
 *
 * \retval SOC_E_NONE No errors.
 */
static int
soc_ltsw_info_config(int unit, soc_control_t *soc)
{
    soc_info_t *si;

    si = &soc->info;
    sal_memset((void *)si, 0, sizeof(soc_info_t));
    si->driver_type = SOC_CHIP_LTSW;
    si->driver_group = SOC_CHIP_LTSW_GRP;
    si->chip_type = SOC_INFO_CHIP_TYPE_LT_SWITCHES;

    return SOC_E_NONE;
}

/******************************************************************************
 * Public Functions
 */

int
soc_ltsw_detach(int unit)
{
    if (!SOC_UNIT_NUM_VALID(unit)) {
        return SOC_E_UNIT;
    }

    if (SOC_PERSIST(unit) != NULL) {
        sal_free(SOC_PERSIST(unit));
        SOC_PERSIST(unit) = NULL;
    }
    if (SOC_CONTROL(unit) != NULL) {
        sal_free(SOC_CONTROL(unit));
        SOC_CONTROL(unit) = NULL;
    }

    return SOC_E_NONE;
}

int
soc_ltsw_attach(int unit)
{
    int rv;
    soc_control_t       *soc;
    soc_persist_t       *sop;

    soc = SOC_CONTROL(unit);
    if (soc == NULL) {
        soc = sal_alloc(sizeof (soc_control_t), "soc_control");
        if (soc == NULL) {
            rv = SOC_E_MEMORY;
            goto exit;
        }
        SOC_CONTROL(unit) = soc;
    }
    sal_memset(soc, 0, sizeof (soc_control_t));

    sop = SOC_PERSIST(unit);
    if (sop == NULL) {
        sop = sal_alloc(sizeof (soc_persist_t), "soc_persist");
        if (sop == NULL) {
            rv = SOC_E_MEMORY;
            goto exit;
        }
        SOC_PERSIST(unit) = sop;
    }
    sal_memset(sop, 0, sizeof (soc_persist_t));

    sop->version = 1;

    soc->chip_driver = soc_ltsw_chip_driver_find(unit);
    if (soc->chip_driver == NULL) {
        rv = SOC_E_INIT;
        goto exit;
    }

    soc->soc_flags |= SOC_F_ATTACHED;

    rv = soc_ltsw_info_config(unit, soc);
    if (SOC_FAILURE(rv)) {
        goto exit;
    }

exit:
    if (SOC_FAILURE(rv)) {
        soc_ltsw_detach(unit);
    }
    return rv;
}

int
soc_ltsw_base_drv_name_get(int unit, const char **name) {
    bcmdrd_dev_type_t dev_type;
    int dunit = 0;
    int rv;
    int count = sizeof(base_drv_name) / sizeof(base_drv_name[0]);

    rv = soc_cm_dev_num_get(unit, &dunit);
    if (SOC_FAILURE(rv)) {
        return rv;
    }

    dev_type = bcmdrd_dev_type(dunit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SOC_E_INIT;
    }

    if (!((dev_type > BCMDRD_DEV_T_NONE) && (dev_type < count))) {
        return -1;
    }

   *name = base_drv_name[dev_type];

    return SOC_E_NONE;
}

#else
typedef int soc_ltsw_drv_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_LTSW_SUPPORT */
