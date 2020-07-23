/*! \file bcm56780_a0_tm_flexport.c
 *
 * File containing flexport sequence for bcm56780_a0.
 *
 * The code in this file is shared with DV team. This file will only be updated
 * by porting DV code changes. If any extra change needs to be made to the
 * shared code, please also inform the DV team.
 *
 * DV file: $DV/trident4/dv/ngsdk_interface/flexport/mmu_flexport.c
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_sleep.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmtm/chip/bcm56780_a0_tm.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include "bcm56780_a0_tm_mmu_flexport.h"
#include "bcm56780_a0_tm_mmu_tdm.h"


#define BSL_LOG_MODULE  BSL_LS_BCMTM_FLEXPORT

#ifndef BSL_LS_SOC_COMMON
#define BSL_LS_SOC_COMMON BSL_LOG_MODULE
#endif

/*! @fn int bcm56780_a0_tm_set_crb_port_mapping(int unit,
 *              port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the CRB's port mapping during port
 *         up and port down operations in flexport.
 */
static int
bcm56780_a0_tm_set_crb_port_mapping(int unit, int lport, int is_port_down)
{
    int inst, mmu_port;
    uint32_t ltmbuf;
    uint32_t mmu_port_new;

    bcmdrd_sid_t sid;
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    /* Port Mapping update */
    mmu_port = port_map->lport_map[lport].mport;
    inst = lport;

    if (is_port_down) {
        mmu_port_new = TD4_X9_INVALID_MMU_PORT;
        /* Invalid entries should be programmed to 0xFF. */
    } else {
        mmu_port_new = mmu_port;
    }

    sid = MMU_CRB_DEVICE_PORT_TO_MMU_PORT_MAPPINGr;
    ltmbuf = 0;
    BCMTM_PT_DYN_INFO(pt_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, MMU_PORTf, &ltmbuf, &mmu_port_new));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}


/*! @fn int bcm56780_a0_tm_set_rqe_port_mapping(int unit,
 *              port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the RQE's port mapping during port
 *         up and port down operations in flexport.
 */
static int
bcm56780_a0_tm_set_rqe_port_mapping(int unit, int lport, int is_port_down)
{
    int mem_indx;
    int mmu_port;
    uint32_t mmu_port_new;
    uint32_t mem_entry_data[BCMTM_MAX_ENTRY_WSIZE];

    bcmdrd_sid_t mem;
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    mmu_port = port_map->lport_map[lport].mport;

    if (is_port_down) {
        mmu_port_new = TD4_X9_INVALID_MMU_PORT; /* All 1's */
    } else {
        mmu_port_new = mmu_port;
    }

    mem = MMU_RQE_DEVICE_TO_MMU_PORT_MAPPINGm;
    sal_memset(mem_entry_data, 0, sizeof(mem_entry_data));

    mem_indx = lport;

    BCMTM_PT_DYN_INFO(pt_info, mem_indx, 0);
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit,
                                    mem,
                                    MMU_PORTf,
                                    mem_entry_data,
                                    &mmu_port_new));

    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                                           mem,
                                           ltid,
                                           &pt_info,
                                           mem_entry_data));
exit:
    SHR_FUNC_EXIT();
}


/*! @fn int bcm56780_a0_tm_set_ebcfp_port_mapping(int unit,
 *              port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the EBCFP's port mapping during port
 *         up and port down operations in flexport.
 */
static int
bcm56780_a0_tm_set_ebcfp_port_mapping(int unit, int lport, int is_port_down)
{
    uint32_t ltmbuf;
    int inst;
    uint32_t device_port_new;
    int mmu_port;

    bcmdrd_sid_t sid;
    bcmtm_port_map_info_t *port_map;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bcmtm_pt_info_t pt_info = {0};

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    mmu_port = port_map->lport_map[lport].mport;
    inst = mmu_port;

    if (is_port_down) {
        device_port_new = TD4_X9_INVALID_DEV_PORT; /*Resetval*/
    } else {
        device_port_new = lport;
    }

    sid = MMU_EBCFP_MMU_PORT_TO_DEVICE_PORT_MAPPINGr;
    ltmbuf = 0;
    BCMTM_PT_DYN_INFO(pt_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, DEVICE_PORTf, &ltmbuf, &device_port_new));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}


/*! @fn int bcm56780_a0_tm_set_intfi_port_mapping(int unit,
 *              port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the INTFI's port mapping during port
 *         up and port down operations in flexport.
 */
static int
bcm56780_a0_tm_set_intfi_port_mapping(int unit, int lport, int is_port_down)
{
    uint32_t pport_new;
    uint32_t lbuf;
    int inst;
    int pport, mmu_port, pipe;

    bcmdrd_sid_t sid;
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    pport = port_map->lport_map[lport].pport;
    mmu_port = port_map->lport_map[lport].mport;
    pipe = port_map->lport_map[lport].pipe;

    inst = mmu_port;

    if (is_port_down) {
        pport_new = 0x3f; /* Resetval */
    } else {
        if (pipe == 0) {
            pport_new = pport % TD4_X9_PHY_PORT_PIPE_OFFSET;
        } else {
            pport_new = (pport - 1) % TD4_X9_PHY_PORT_PIPE_OFFSET;
        }
    }

    lbuf = 0;
    sid = MMU_INTFI_MMU_PORT_TO_PHY_PORT_MAPPINGr;
    BCMTM_PT_DYN_INFO(pt_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, PHY_PORT_NUMf, &lbuf, &pport_new));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &lbuf));

exit:
    SHR_FUNC_EXIT();
}


/*! @fn int bcm56780_a0_tm_set_intfo_port_mapping(int unit,
 *              port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the INTFO's port mapping during port
 *         up and port down operations in flexport.
 */
static int
bcm56780_a0_tm_set_intfo_port_mapping(int unit, int lport, int is_port_down)
{
    int inst;
    int pport, mmu_port;
    uint32_t pport_new;
    uint32_t lbuf;

    bcmdrd_sid_t sid;
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    pport = port_map->lport_map[lport].pport;
    mmu_port = port_map->lport_map[lport].mport;
    inst = mmu_port;

    if (is_port_down) {
        pport_new = 0; /* Resetval */
    } else {
        pport_new = pport;
    }

    lbuf = 0;
    sid = MMU_INTFO_MMU_PORT_TO_PHY_PORT_MAPPINGr;

    BCMTM_PT_DYN_INFO(pt_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, PHY_PORT_NUMf, &lbuf, &pport_new));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &lbuf));
exit:
    SHR_FUNC_EXIT();
}


/*! @fn int bcm56780_a0_tm_set_thdo_port_mapping(int unit,
 *              port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the THDO's port mapping during port
 *         up and port down operations in flexport.
 */
static int
bcm56780_a0_tm_set_thdo_port_mapping(int unit, int lport, int is_port_down)
{
    int inst;
    int chip_port_num;
    uint32_t device_port_new;
    uint32_t thdo_port_map_data[BCMTM_MAX_ENTRY_WSIZE];

    bcmdrd_sid_t mem;
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    /* THDO uses Chip Port Num. */
    chip_port_num = port_map->lport_map[lport].mchip_port;
    inst = chip_port_num;

    if (is_port_down) {
        device_port_new = TD4_X9_THDO_INVALID_DEV_PORT; /*Resetval*/
    } else {
        device_port_new = lport;
    }

    mem = MMU_THDO_DEVICE_PORT_MAPm;
    sal_memset(thdo_port_map_data, 0, sizeof(thdo_port_map_data));

    BCMTM_PT_DYN_INFO(pt_info, inst, 0);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit,
                                          mem,
                                          ltid,
                                          &pt_info,
                                          thdo_port_map_data));
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit,
                                    mem,
                                    DEVICE_PORTf,
                                    thdo_port_map_data,
                                    &device_port_new));

    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                                           mem,
                                           ltid,
                                           &pt_info,
                                            thdo_port_map_data));
exit:
    SHR_FUNC_EXIT();
}

/*! @fn int bcm56780_a0_tm_set_mmu_port_mappings(int unit,
 *              bcmtm_port_map_info_t *port_map,
 *              port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_map Port Schedule State Struct
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the MMU port mappings for various blocks
 *         during port up and port down operations in flexport.
 */
static int
bcm56780_a0_tm_set_mmu_port_mappings(int unit, int lport, int is_port_down)
{

    /*
     * When a physical port goes down, the port resource struct phy port number
     * becomes -1. At that time, we need to invalidate the phy port. When
     * a port is coming back up, the struct is populated correctly.
     */
    SHR_FUNC_ENTER(unit);

    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit, "before bcm56780_a0_tm_set_crb_port_mapping\n")));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_set_crb_port_mapping(unit, lport, is_port_down));
         if (bcmtm_lport_is_rdb(unit, lport)) {
             SHR_EXIT();
    }
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit, "before bcm56780_a0_tm_set_rqe_port_mapping\n")));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_set_rqe_port_mapping(unit, lport, is_port_down));

    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit, "before bcm56780_a0_tm_set_ebcfp_port_mapping\n")));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_set_ebcfp_port_mapping(unit, lport, is_port_down));

    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit, "before bcm56780_a0_tm_set_intfi_port_mapping\n")));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_set_intfi_port_mapping(unit, lport, is_port_down));

    if (bcmtm_lport_is_fp(unit,lport)) {
        LOG_DEBUG(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit, "before bcm56780_a0_tm_set_intfo_port_mapping\n")));
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_set_intfo_port_mapping(unit, lport, is_port_down));
    }

    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit, "before bcm56780_a0_tm_set_thdo_port_mapping\n")));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_set_thdo_port_mapping(unit, lport, is_port_down));

exit:
    SHR_FUNC_EXIT();
}

/*! @fn int bcm56780_a0_tm_qsch_port_flush(int unit,
 *      bcmtm_port_map_info_t *port_map, int pipe, uint32_t set_val)
 *  @param unit Device number
 *  @param *port_map Port Schedule_State struct
 *  @param set_val Value to be set to the QSCH port flush sidisters
 *  @brief API to Set the QSCH Port Flush registers
 */
static int
bcm56780_a0_tm_qsch_port_flush(int unit, int lport, int pipe, uint32_t set_val)
{
    uint32_t temp;
    uint32_t ltmbuf;
    uint32_t new_val;
    uint32_t enable_val;
    int inst;
    int mmu_port, lcl_mmu_port;

    bcmdrd_sid_t sid;
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);

    sid = MMU_QSCH_PORT_FLUSHr;
    inst = pipe;

    /* Read-modify-write. */
    ltmbuf = 0;
    enable_val = 0;

    LOG_DEBUG(BSL_LS_SOC_PORT,
            (BSL_META_U(unit,
                        "before MMU_QSCH_PORT_FLUSHr \n")));

    BCMTM_PT_DYN_INFO(pt_info, 0, inst);

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, ENABLEf, &ltmbuf, &enable_val));

    LOG_DEBUG(BSL_LS_SOC_PORT,
            (BSL_META_U(unit,
                        "after MMU_QSCH_PORT_FLUSHr \n")));

    mmu_port = port_map->lport_map[lport].mport;

    new_val = 1;
    lcl_mmu_port = mmu_port % TD4_X9_MMU_PORT_PIPE_OFFSET;
    temp = TD4_X9_MMU_FLUSH_ON;
    new_val <<= lcl_mmu_port;

    if (set_val == temp) {
        enable_val |= new_val;
    } else {
        new_val = ~new_val;
        enable_val &= new_val;
    }

    ltmbuf = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, ENABLEf, &ltmbuf, &enable_val));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}




/*! @fn int bcm56780_a0_tm_intfo_clr_port_en(int unit,
 *      bcmtm_port_map_info_t *port_map, int pipe)
 *  @param unit Device number
 *  @param *port_map Port Schedule_State struct
 *  @param pipe Pipe number
 *  @brief API to clear the oobport enable bits in INTFO for a disabled port
 */
static int
bcm56780_a0_tm_intfo_clr_port_en(int unit, int lport, int pipe)
{
    uint32_t entry_oobport_mapping_thdi[BCMTM_MAX_ENTRY_WSIZE];
    uint32_t entry_oobport_mapping_thdo[BCMTM_MAX_ENTRY_WSIZE];
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_sid_t sid, sid1;

    uint32_t ltmbuf[2] = {0}, offset = 0 ;
    int inst;
    uint32_t ing_oob_port;
    uint32_t eng_oob_port;
    bcmtm_port_map_info_t *port_map;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);


    inst = 0;

    /* Read-modify-write. */
    sal_memset(entry_oobport_mapping_thdi, 0, sizeof(entry_oobport_mapping_thdi));
    sal_memset(entry_oobport_mapping_thdo, 0, sizeof(entry_oobport_mapping_thdo));

    BCMTM_PT_DYN_INFO(pt_info, lport, 0);
    sid = MMU_INTFO_MMU_PORT_TO_OOB_PORT_MAPPING_THDIm;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, OOB_PORT_NUMf, ltmbuf, &ing_oob_port));

    sid = MMU_INTFO_MMU_PORT_TO_OOB_PORT_MAPPING_THDO0m;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, OOB_PORT_NUMf, ltmbuf, &eng_oob_port));

    if (ing_oob_port < 64) {
        sid = MMU_INTFO_OOBFC_ING_PORT_EN0_64r;
        sid1 = MMU_INTFO_OOBFC_CONGST_ST_EN0_64r;
    } else if (ing_oob_port < 128) {
        /* Reg1 is OOBPORT 64-127. */
        sid = MMU_INTFO_OOBFC_ING_PORT_EN1_64r;
        sid1 = MMU_INTFO_OOBFC_CONGST_ST_EN1_64r;
        offset = ing_oob_port - 64;
    } else {
        /* Reg2 is OOBPORT 128-154. */
        sid = MMU_INTFO_OOBFC_ING_PORT_EN2r;
        sid1 = MMU_INTFO_OOBFC_CONGST_ST_EN2r;
        offset = ing_oob_port - 128;
    }

    BCMTM_PT_DYN_INFO(pt_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));

    SHR_BITCLR(ltmbuf, offset);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid1, ltid, &pt_info, ltmbuf));
    SHR_BITCLR(ltmbuf, offset);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid1, ltid, &pt_info, ltmbuf));

    if (eng_oob_port < 64) {
        sid = MMU_INTFO_OOBFC_ENG_PORT_EN0_64r;
    } else if (eng_oob_port < 128) {
        /* Reg1 is OOBPORT 64-127. */
        sid = MMU_INTFO_OOBFC_ENG_PORT_EN1_64r;
        offset = eng_oob_port - 64;
    } else {
        /* Reg2 is OOBPORT 128-154. */
        sid = MMU_INTFO_OOBFC_ENG_PORT_EN2r;
        offset = eng_oob_port - 128;
    }

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));
    SHR_BITCLR(ltmbuf, offset);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}


/*! @fn int bcm56780_a0_tm_ebqs_port_flush(int unit,
 *      bcmtm_port_map_info_t *port_map, int pipe, uint32_t set_val)
 *  @param unit Device number
 *  @param *port_map Port Schedule_State struct
 *  @param set_val Value to be set to the EBQS port flush registers
 *  @brief API to Set the EBQS Port Flush registers
 */
static int
bcm56780_a0_tm_ebqs_port_flush(int unit, int lport, int pipe, uint32_t set_val)
{
    int inst;
    int lcl_mmu_port;
    uint32_t temp;
    uint32_t ltmbuf;
    uint32_t new_val;
    uint32_t enable_val;
    bcmdrd_sid_t sid;
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    sid = MMU_EBQS_PORT_FLUSHr;
    inst = pipe;

    /* Read-modify-write. */
    ltmbuf = 0;

    BCMTM_PT_DYN_INFO(pt_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, ENABLEf, &ltmbuf, &enable_val));

    lcl_mmu_port = port_map->lport_map[lport].mlocal_port;
    new_val = 1;
    temp = TD4_X9_MMU_FLUSH_ON;
    new_val <<= lcl_mmu_port;

    if (set_val == temp) {
        enable_val |= new_val;
    } else {
        new_val = ~new_val;
        enable_val &= new_val;
    }

    ltmbuf = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, ENABLEf, &ltmbuf, &enable_val));

    BCMTM_PT_DYN_INFO(pt_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}



/*! @fn int bcm56780_a0_tm_wait_ebctm_empty(int unit,
 *              bcmtm_port_map_info_t *port_map, int pipe)
 *  @param unit Device number
 *  @param pipe Pipe number to check
 *  @param *port_map Port Schedule_State struct
 *  @brief API to poll MMU EBCTM Port Empty sidister
 */
static int
bcm56780_a0_tm_wait_ebctm_empty(int unit, int lport, int pipe)
{
    uint32_t ltmbuf;
    uint32_t empty_val;
    int inst, count = 0;
    int lcl_mmu_port;
    int port_empty;
    bcmdrd_sid_t sid;
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    sid = MMU_EBCTM_PORT_EMPTY_STSr;

    inst = pipe;

    while (1) {
        ltmbuf = 0;
        BCMTM_PT_DYN_INFO(pt_info, 0, inst);
        SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit,
                    sid,
                    ltid,
                    &pt_info,
                    &ltmbuf));

        SHR_IF_ERR_EXIT(bcmtm_field_get(unit,
                    sid,
                    DST_PORT_EMPTYf,
                    &ltmbuf,
                    &empty_val));


        lcl_mmu_port = port_map->lport_map[lport].mlocal_port;
        port_empty = ((empty_val & (1 << lcl_mmu_port)) != 0);
        if (port_empty == 1) {
            break;
        }
        sal_usleep(1 + (emul ? 1 : 0) * EMULATION_FACTOR);
        count++;
        if (count > 30000) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "WAIT_EBCTM_EMPTY: Ports not empty in 30000 "
                                "iterations\n")));
            return SHR_E_TIMEOUT;
        }
    }

exit:
    SHR_FUNC_EXIT();
}




/*! @fn int bcm56780_a0_tm_mtro_port_metering_config(int unit,
 *      bcmtm_port_map_info_t *port_map, int pipe, uint32_t set_val_mtro)
 *  @param unit Device number
 *  @param *port_map Port Schedule_State struct
 *  @param set_val_mtro Value to be set in the per-port DISABLE bitmap
 *  @brief This function is used to set the MTRO Port Entity Disable register to
 *         ignore shaper information on a port during flexport operation
 */
static int
bcm56780_a0_tm_mtro_port_metering_config(int unit,
                                  int lport,
                                  int pipe,
                                  uint32_t set_val_mtro)
{

    int inst;
    int lcl_mmu_port;
    uint32_t ltmbuf;
    uint32_t bitmap_val;
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_info = {0};

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    sid = MMU_MTRO_PORT_ENTITY_DISABLEr;
    inst = pipe;

    /* Read-modify-write.*/

    ltmbuf = 0;

    BCMTM_PT_DYN_INFO(pt_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, METERING_DISABLEf, &ltmbuf, &bitmap_val));

    lcl_mmu_port = port_map->lport_map[lport].mlocal_port;
    /* Now, Set or Clear disable bit depending on function input. */
    if (set_val_mtro == 1) { /* Disable refresh-on all UP/DOWN Ports. */
        /* Set the 'lcl_mmu_port' bit. */
        uint32_t temp;
        temp = 1;
        temp <<= lcl_mmu_port;
        bitmap_val |= temp;
    }
    else { /* Enable refresh */
        /* Clear the 'lcl_mmu_port' bit. */
        uint32_t temp;
        temp = 1;
        temp <<= lcl_mmu_port;
        temp = ~temp;
        bitmap_val &= temp;
    }

    ltmbuf = 0;
    BCMTM_PT_DYN_INFO(pt_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, METERING_DISABLEf, &ltmbuf, &bitmap_val));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}


/*** END SDK API COMMON CODE ***/
/*! @fn int bcm56780_a0_tm_flex_mmu_port_flush(int unit,
 *              bcmtm_port_map_info_t *port_map)
 *  @param unit Device number
 *  @param *port_map Port Schedule_State struct
 *  @brief API to flush MMU during flexport port down operation.
 */
static int
bcm56780_a0_tm_flex_mmu_port_flush(int unit, int lport)
{
    uint32_t val;
    int pipe;
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    LOG_DEBUG(BSL_LS_SOC_PORT,
            (BSL_META_U(unit, "setting FLUSH_ON\n")));
    val = TD4_X9_MMU_FLUSH_ON;

    pipe = port_map->lport_map[lport].pipe;

    LOG_DEBUG(BSL_LS_SOC_PORT,
            (BSL_META_U(unit, "before mmu_qsch_port_flush\n")));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_qsch_port_flush(unit, lport, pipe, val));

    LOG_DEBUG(BSL_LS_SOC_PORT,
            (BSL_META_U(unit, "before mmu_ebqs_port_flush\n")));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_ebqs_port_flush(unit, lport, pipe, val));

    LOG_DEBUG(BSL_LS_SOC_PORT,
            (BSL_META_U(unit, "before mtro_port_metering_config\n")));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_mtro_port_metering_config(unit, lport, pipe, val));

    LOG_DEBUG(BSL_LS_SOC_PORT,
            (BSL_META_U(unit, "before mmu_intfo_clr_port_en\n")));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_intfo_clr_port_en(unit, lport, pipe));

    LOG_DEBUG(BSL_LS_SOC_PORT,
            (BSL_META_U(unit, "before mmu_rqe_port_flush\n")));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_rqe_port_flush(unit));
    LOG_DEBUG(BSL_LS_SOC_PORT,
            (BSL_META_U(unit, "after FLUSH_ON set\n")));

    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_wait_ebctm_empty(unit, lport, pipe));

    val = TD4_X9_MMU_FLUSH_OFF;

    LOG_DEBUG(BSL_LS_SOC_PORT,
            (BSL_META_U(unit, "setting FLUSH_OFF\n")));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_mtro_port_metering_config(unit, lport, pipe, val));

    LOG_DEBUG(BSL_LS_SOC_PORT,
            (BSL_META_U(unit, "before mmu_qsch_port_flush\n")));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_qsch_port_flush(unit, lport, pipe, val));

    LOG_DEBUG(BSL_LS_SOC_PORT,
            (BSL_META_U(unit, "before mmu_ebqs_port_flush\n")));
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_ebqs_port_flush(unit, lport, pipe, val));

    LOG_DEBUG(BSL_LS_SOC_PORT,
            (BSL_META_U(unit, "after FLUSH_OFF set\n")));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_oqs_reconfig(int unit, int lport)
{
    uint32_t ltmbuf, port_speed_new;
    bcmdrd_sid_t sid = MMU_OQS_AGER_DST_PORT_MAPr;
    bcmdrd_fid_t fid = PORT_SPEEDf;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bcmtm_pt_info_t pt_info = {0};
    bcmtm_port_map_info_t *port_map;
    int mmu_port;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    mmu_port = port_map->lport_map[lport].mport;
    switch (port_map->lport_map[lport].max_speed) {
        case 10000:
        case 25000:
        case 40000:
        case 50000:
            port_speed_new = 0;
            break;
        case 100000:
            port_speed_new = 1;
            break;
        case 200000:
            port_speed_new = 2;
            break;
        case 400000:
            port_speed_new = 3;
            break;
        default:
            port_speed_new = 0;
            LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Incorrect speed specified %0d\n"),
                     port_map->lport_map[lport].max_speed));
            return SHR_E_FAIL;
    }
    BCMTM_PT_DYN_INFO(pt_info, 0, mmu_port);

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &port_speed_new));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}
static int
bcm56780_a0_tm_port_speed_encoding(int unit, int lport, uint32_t *encode)
{
    bcmtm_port_map_info_t *port_map;
    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);

    switch (port_map->lport_map[lport].max_speed){
        case 10000:
            *encode = 1;
            break;
        case 25000:
            *encode = 2;
            break;
        case 40000:
            *encode = 3;
            break;
        case 50000:
            *encode = 4;
            break;
        case 100000:
            *encode = 5;
            break;
        case 200000:
            *encode = 6;
            break;
        case 400000:
            *encode = 7;
            break;
        default:
            *encode = 0;
            LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Incorrect speed specified %0d\n"),
                     port_map->lport_map[lport].max_speed));
            SHR_ERR_EXIT(SHR_E_FAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_crb_scb_reconfig(int unit, int mport, uint32_t encode)
{
    bcmdrd_sid_t sid[] = {MMU_CRB_SRC_PORT_CFGr,
                          MMU_SCB_SOURCE_PORT_CFGr};
    bcmdrd_fid_t fid;
    uint32_t ltmbuf, idx;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);
    fid = PORT_SPEEDf;

    BCMTM_PT_DYN_INFO(pt_info, 0, mport);

    for (idx = 0; idx < COUNTOF(sid); idx++) {
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, sid[idx], ltid, &pt_info, &ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid[idx], fid, &ltmbuf, &encode));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit ,sid[idx], ltid, &pt_info, &ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_ebpcc_reconfig(int unit, int mport, uint32_t encode)
{
    bcmdrd_sid_t sid = MMU_EBPCC_EPORT_CFGr;
    bcmdrd_fid_t fid = DST_PORT_SPEEDf;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf;

    SHR_FUNC_ENTER(unit);

    BCMTM_PT_DYN_INFO(pt_info, 0, mport);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &encode));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit ,sid, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_eb_sch_reconfig(int unit, int lport)
{
    bcmdrd_sid_t sid = MMU_EBQS_PORT_CFGr;
    bcmdrd_fid_t fid = PORT_SPEEDf;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bcmtm_port_map_info_t *port_map;
    uint32_t ltmbuf, encode;
    int mport;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    mport = port_map->lport_map[lport].mport;
    switch (port_map->lport_map[lport].max_speed) {
        case 10000:
            encode = 0;
            break;
        case 25000:
            encode = 1;
            break;
        case 40000:
            encode = 2;
            break;
        case 50000:
            encode = 3;
            break;
        case 100000:
            encode = 4;
            break;
        case 200000:
            encode = 5;
            break;
        case 400000:
            encode = 6;
            break;
        default:
            encode = 7;
            LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Incorrect speed specified %0d\n"),
                     port_map->lport_map[lport].max_speed));
            return SHR_E_FAIL;
    }
    BCMTM_PT_DYN_INFO(pt_info, 0, mport);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &encode));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit ,sid, ltid, &pt_info, &ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_clear_mtro_bucket_mems(int unit, int lport)
{
    int pipe, lcl_mport, idx;
    bcmdrd_sid_t sid;
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf[3] = {0}, fval = 0, mem_idx;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    pipe = port_map->lport_map[lport].pipe;
    lcl_mport = port_map->lport_map[lport].mlocal_port;

    /* Port bucket memory. */
    sid = MMU_MTRO_EGRMETERINGBUCKETm;
    fval = 0;
    BCMTM_PT_DYN_INFO(pt_info, lcl_mport, pipe);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, BUCKETf, ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));

    /* L0 Bucket memory */
    sid = MMU_MTRO_BUCKET_L0m;
    for (idx = 0; idx < TD4_X9_MMU_NUM_COS; idx++) {
        fval = 0;
        mem_idx = (lcl_mport * TD4_X9_MMU_NUM_COS) + idx;
        BCMTM_PT_DYN_INFO(pt_info, mem_idx, pipe);

        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, MIN_BUCKETf, ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, MAX_BUCKETf, ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_clear_qsch_credit_memories(int unit, int lport)
{
    int pipe, lcl_mport, idx;
    bcmdrd_sid_t sid;
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    uint32_t ltmbuf = 0, mem_idx;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    lcl_mport = port_map->lport_map[lport].mlocal_port;
    pipe = port_map->lport_map[lport].pipe;

    for (idx = 0; idx < TD4_X9_MMU_NUM_COS; idx++) {
        mem_idx = (lcl_mport * TD4_X9_MMU_NUM_COS) + idx;
        BCMTM_PT_DYN_INFO(pt_info, mem_idx, pipe);

        sid = MMU_QSCH_L0_CREDIT_MEMm;
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));

        sid = MMU_QSCH_L0_ACCUM_COMP_MEMm;
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));
    }

    for (idx = 0; idx < TD4_X9_MMU_NUM_L0_NODES_PER_HSP_PORT; idx++) {
        mem_idx = (lcl_mport * TD4_X9_MMU_NUM_L0_NODES_PER_HSP_PORT) + idx;
        BCMTM_PT_DYN_INFO(pt_info, mem_idx, pipe);

        sid = MMU_QSCH_L1_CREDIT_MEMm;
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));

        sid = MMU_QSCH_L1_ACCUM_COMP_MEMm;
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));
    }

    for (idx = 0; idx < TD4_X9_MMU_NUM_L0_NODES_PER_HSP_PORT; idx++) {
        mem_idx = (lcl_mport * TD4_X9_MMU_NUM_L0_NODES_PER_HSP_PORT);
        BCMTM_PT_DYN_INFO(pt_info, mem_idx, pipe);

        sid = MMU_QSCH_L2_CREDIT_MEMm;
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));

        sid = MMU_QSCH_L2_ACCUM_COMP_MEMm;
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_wred_clr(int unit, int lport)
{
    int q, sp;
    int cosq_mem_index, portsp_mem_index;

    uint32_t mem_data;
    uint32_t wred_q_data[BCMTM_MAX_ENTRY_WSIZE];
    uint32_t wred_portsp_data[BCMTM_MAX_ENTRY_WSIZE];

    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bcmdrd_sid_t mmu_wred_avg_qsize, mmu_wred_avg_portsp_size;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);

    /*
     * Calculate base indexes for the set of UC queues and port SPs for the
     * given port.
     */

    sal_memset(wred_q_data, 0, sizeof(wred_q_data));
    sal_memset(wred_portsp_data, 0, sizeof(wred_portsp_data));

    mmu_wred_avg_portsp_size = MMU_WRED_AVG_PORTSP_SIZEm;
    mmu_wred_avg_qsize = MMU_WRED_AVG_QSIZEm;
    mem_data = 0;

    /* Per-UCQ loop */
    for (q = 0; q < TD4_X9_MMU_NUM_Q_PER_HSP_PORT; q++) {

        cosq_mem_index = (lport * TD4_X9_MMU_NUM_Q_PER_HSP_PORT) + q;

        BCMTM_PT_DYN_INFO(pt_info, cosq_mem_index, 0);
        SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit,
                                              mmu_wred_avg_qsize,
                                              ltid,
                                              &pt_info,
                                              wred_q_data));

        SHR_IF_ERR_EXIT(bcmtm_field_set(unit,
                                        mmu_wred_avg_qsize,
                                        AVG_QSIZE_FRACTIONf,
                                        wred_q_data,
                                        &mem_data));
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit,
                                        mmu_wred_avg_qsize,
                                        AVG_QSIZEf,
                                        wred_q_data,
                                        &mem_data));

        SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                                               mmu_wred_avg_qsize,
                                               ltid,
                                               &pt_info,
                                               wred_q_data));
    }

    /* Per-SP loop */
    for (sp = 0; sp < TD4_X9_MMU_NUM_SPS; sp++) {

        portsp_mem_index = (lport * TD4_X9_MMU_NUM_SPS) + sp;

        BCMTM_PT_DYN_INFO(pt_info, portsp_mem_index, 0);
        SHR_IF_ERR_EXIT(bcmtm_pt_indexed_read(unit,
                                              mmu_wred_avg_portsp_size,
                                              ltid,
                                              &pt_info,
                                              wred_portsp_data));

        SHR_IF_ERR_EXIT(bcmtm_field_set(unit,
                                        mmu_wred_avg_portsp_size,
                                        AVG_QSIZE_FRACTIONf,
                                        wred_portsp_data,
                                        &mem_data));
        SHR_IF_ERR_EXIT(bcmtm_field_set(unit,
                                        mmu_wred_avg_portsp_size,
                                        AVG_QSIZEf,
                                        wred_portsp_data,
                                        &mem_data));

        SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                                               mmu_wred_avg_portsp_size,
                                               ltid,
                                               &pt_info,
                                               wred_portsp_data));
    }

exit:
    SHR_FUNC_EXIT();
}

/*! @fn int bcm56780_a0_tm_get_thdi_bst_mode(int unit, int *thdi_bst_mode)
 *  @param unit Device number
 *  @param *thdi_bst_mode Return value thdi bst mode
 *  @brief API to return the THDI bst mode programmed
 */
static int
bcm56780_a0_tm_get_thdi_bst_mode(int unit, uint32_t *thdi_bst_mode)
{
    int inst = 0;
    uint32_t ltmbuf = 0;
    uint32_t fval;

    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_sid_t sid = MMU_THDI_BSTCONFIGr;
    bcmdrd_fid_t fid = TRACKING_MODEf;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    BCMTM_PT_DYN_INFO(pt_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, &ltmbuf, &fval));
    *thdi_bst_mode = fval;
exit:
    SHR_FUNC_EXIT();
}

/*! @fn int bcm56780_a0_tm_get_thdo_bst_mode(int unit, int *thdo_bst_mode)
 *  @param unit Device number
 *  @param *thdo_bst_mode Return value thdo bst mode
 *  @brief API to return the thdo bst mode programmed
 */
static int
bcm56780_a0_tm_get_thdo_bst_mode(int unit, uint32_t *thdo_bst_mode)
{
    int inst = 0;
    uint32_t ltmbuf = 0;
    uint32_t fval;

    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_sid_t sid = MMU_THDO_BST_CONFIGr;
    bcmdrd_fid_t fid = TRACKING_MODEf;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    BCMTM_PT_DYN_INFO(pt_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, &ltmbuf, &fval));
    *thdo_bst_mode = fval;
exit:
    SHR_FUNC_EXIT();
}

/*! @fn int bcm56780_a0_tm_thdi_bst_clr(int unit,
 *              port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to clear port/PG hdrm and shared BST counters for a given MMU
 *         source port
 */
static int
bcm56780_a0_tm_thdi_bst_clr(int unit, int lport)
{
    int pipe_number;
    int local_mmu_port;
    uint32_t data[BCMTM_MAX_ENTRY_WSIZE];
    bcmtm_port_map_info_t *port_map;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bcmdrd_sid_t mmu_thdi_port_pg_bst;
    bcmdrd_sid_t mmu_thdi_port_sp_bst;
    bcmdrd_sid_t mmu_thdi_port_pg_hdrm_bst;

    /*Set up pipe & port information*/

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    local_mmu_port = port_map->lport_map[lport].mlocal_port;
    pipe_number = port_map->lport_map[lport].pipe;

    sal_memset(data, 0, sizeof(data));

    mmu_thdi_port_pg_bst = MMU_THDI_PORT_PG_SHARED_BSTm;
    mmu_thdi_port_sp_bst = MMU_THDI_PORTSP_BSTm;
    mmu_thdi_port_pg_hdrm_bst = MMU_THDI_PORT_PG_HDRM_BSTm;

    /* Writing 0 to all fields. */
    BCMTM_PT_DYN_INFO(pt_info, local_mmu_port, pipe_number);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                mmu_thdi_port_pg_bst,
                ltid,
                &pt_info,
                data));
    BCMTM_PT_DYN_INFO(pt_info, local_mmu_port, pipe_number);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                mmu_thdi_port_pg_hdrm_bst,
                ltid,
                &pt_info,
                data));
    BCMTM_PT_DYN_INFO(pt_info, local_mmu_port, pipe_number);
    SHR_IF_ERR_EXIT(bcmtm_pt_indexed_write(unit,
                mmu_thdi_port_sp_bst,
                ltid,
                &pt_info,
                data));

exit:
    SHR_FUNC_EXIT();
}

/*! @fn int bcm56780_a0_tm_thdo_bst_clr_queue(int unit,
 *                                   int pipe, int global_mmu_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe in which port exists
 *  @param int global_mmu_port global mmu port
 *  @brief Clears thdo bst total queue counters of the port
 */
static int
bcm56780_a0_tm_thdo_bst_clr_queue(int unit, int pipe, int lport)
{
    int curr_idx;
    int num_queues;
    int mem_idx;
    int chip_q_num_base_for_port;
    uint32_t data[BCMTM_MAX_ENTRY_WSIZE];
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bcmdrd_sid_t thdo_total_queue_bst_mem;
    bcmtm_pt_info_t pt_info = {0};
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);

    sal_memset(data, 0, sizeof(data));
    bcmtm_port_map_get(unit, &port_map);

    thdo_total_queue_bst_mem = MMU_THDO_BST_TOTAL_QUEUEm;
    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit, "pipe = %0d, lport = %0d\n"), pipe, lport));

    num_queues = port_map->lport_map[lport].num_ucq +
                 port_map->lport_map[lport].num_mcq;
    chip_q_num_base_for_port = port_map->lport_map[lport].base_ucq;
    mem_idx = chip_q_num_base_for_port;

    for (curr_idx = 0; curr_idx < num_queues; curr_idx++) {
        LOG_DEBUG(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "curr_idx = %0d, mem_idx = %0d\n"),
                   curr_idx, mem_idx));

        BCMTM_PT_DYN_INFO(pt_info, mem_idx, 0);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit,
                                   thdo_total_queue_bst_mem,
                                   ltid,
                                   &pt_info,
                                   data));
        mem_idx++;
    }

exit:
    SHR_FUNC_EXIT();
}

/*! @fn int bcm56780_a0_tm_thdo_bst_clr_port(int unit,
 *                                   int pipe, int global_device_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe in which port exists
 *  @param int global_device_port global device port
 *  @brief Clears thdo bst port counters of the port
 */
static int
bcm56780_a0_tm_thdo_bst_clr_port(int unit, int pipe, int global_device_port)
{
    uint32_t data[BCMTM_MAX_ENTRY_WSIZE];
    bcmdrd_sid_t mem;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    sal_memset(data, 0, sizeof(data));

    mem = MMU_THDO_BST_SHARED_PORTm;

    BCMTM_PT_DYN_INFO(pt_info, global_device_port, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, mem, ltid, &pt_info, data));
exit:
    SHR_FUNC_EXIT();
}


/*! @fn int bcm56780_a0_tm_thdo_bst_clr_portsp(int unit,
 *                                   int pipe, int global_device_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe in which port exists
 *  @param int global_device_port global device port
 *  @brief Clears thdo bst port/sp counters of the port
 */
static int
bcm56780_a0_tm_thdo_bst_clr_portsp(int unit, int pipe, int global_device_port)
{
    int sp;
    int mem_idx;
    uint32_t data[BCMTM_MAX_ENTRY_WSIZE];

    bcmdrd_sid_t mem;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    sal_memset(data, 0, sizeof(data));

    mem = MMU_THDO_BST_SHARED_PORTSP_MCm;

    for (sp = 0; sp < TD4_X9_MMU_NUM_SPS; sp++) {
        mem_idx = (global_device_port * 4) + sp;
        BCMTM_PT_DYN_INFO(pt_info, mem_idx, 0);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, mem, ltid, &pt_info, data));
    }

exit:
    SHR_FUNC_EXIT();
}

/*! @fn int bcm56780_a0_tm_thdo_bst_clr(int unit,
 *              port_resource_t *port_resource_t)
 *  @param int unit Chip unit number
 *  @param port_resource_t *port_resource_t Port resource struct containing
           epipe and port info
 *  @brief Clears thdo queue/port bst counters of an egress port
 */
static int
bcm56780_a0_tm_thdo_bst_clr(int unit, int lport)
{
    int pipe;
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    pipe = port_map->lport_map[lport].pipe;

    /* Clear per-queue memories. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_thdo_bst_clr_queue(unit, pipe, lport));
    /* Clear per-port memories. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_thdo_bst_clr_port(unit, pipe, lport));
    /* Clear per-port-per-spid memories. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_thdo_bst_clr_portsp(unit, pipe, lport));

exit:
    SHR_FUNC_EXIT();
}

/*! @fn int bcm56780_a0_tm_thdo_pktstat_clr_queue(int unit, int pipe,
 *              int global_mmu_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe
 *  @param int global_mmu_port Global mmu port
 *  @brief Internal function. Clears thdo stats counters of all queues
 *         in the egress port
 */
static int
bcm56780_a0_tm_thdo_pktstat_clr_queue(int unit, int pipe, int lport)
{
    int curr_idx;
    int num_queues;
    int mem_idx;
    int chip_q_num_base_for_port;
    uint32_t data[BCMTM_MAX_ENTRY_WSIZE];
    bcmtm_port_map_info_t *port_map;
    bcmdrd_sid_t mem;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    sal_memset(data, 0, sizeof(data));


    mem = MMU_THDO_QUEUE_DROP_COUNTm;

    num_queues = port_map->lport_map[lport].num_ucq +
                 port_map->lport_map[lport].num_mcq;

    LOG_DEBUG(BSL_LS_SOC_PORT,
              (BSL_META_U(unit,
                          "pipe = %0d, lport = %0d\n"), pipe, lport));
    chip_q_num_base_for_port = port_map->lport_map[lport].base_ucq;
    mem_idx = chip_q_num_base_for_port;

    for (curr_idx = 0; curr_idx < num_queues; curr_idx++) {
        LOG_DEBUG(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "curr_idx = %0d, mem_idx = %0d\n"),
                   curr_idx, mem_idx));

        BCMTM_PT_DYN_INFO(pt_info, mem_idx, 0);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, mem, ltid, &pt_info, data));
        mem_idx++;
    }

exit:
    SHR_FUNC_EXIT();
}

/*! @fn int bcm56780_a0_tm_thdo_pktstat_clr_port(int unit, int pipe,
 *              int global_device_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe
 *  @param int global_device_port Global device port
 *  @brief Internal function. Clears thdo stats counters of
 *         the egress ports
 */
static int
bcm56780_a0_tm_thdo_pktstat_clr_port(int unit, int pipe, int global_device_port)
{
    int local_device_port;
    uint32_t data[BCMTM_MAX_ENTRY_WSIZE];
    bcmdrd_sid_t mem;
    bcmdrd_sid_t mc_mem;
    bcmdrd_sid_t uc_mem;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */

    SHR_FUNC_ENTER(unit);

    sal_memset(data, 0, sizeof(data));

    mem = MMU_THDO_SRC_PORT_DROP_COUNTm;
    mc_mem = MMU_THDO_PORT_DROP_COUNT_MCm;
    uc_mem = MMU_THDO_PORT_DROP_COUNT_UCm;

    local_device_port = global_device_port % TD4_X9_MMU_PORTS_PER_PIPE;
    BCMTM_PT_DYN_INFO(pt_info, local_device_port, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, mem, ltid, &pt_info, data));

    BCMTM_PT_DYN_INFO(pt_info, global_device_port, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, mc_mem, ltid, &pt_info, data));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, uc_mem, ltid, &pt_info, data));

exit:
    SHR_FUNC_EXIT();
}

/*! @fn int bcm56780_a0_tm_thdo_pktstat_clr(int unit,
 *              port_resource_t *port_resource_t)
 *  @param int unit Chip unit number
 *  @param port_resource_t *port_resource_t Port resource struct containing
 *         epipe and port info
 *  @brief Clears thdo stats counters of all queues/ports in the egress port
 */
static int
bcm56780_a0_tm_thdo_pktstat_clr(int unit, int lport)
{
    int pipe;
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    pipe = port_map->lport_map[lport].pipe;

    /* Clear per-queue memories. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_thdo_pktstat_clr_queue(unit, pipe, lport));
    /* Clear per-port memories. */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_thdo_pktstat_clr_port(unit, pipe, lport));
exit:
    SHR_FUNC_EXIT();
}

/*! @fn int bcm56780_a0_tm_get_max_src_ct_speed(int unit, int dst_speed,
 *              int ct_mode,
 *              uint32_t *max_speed)
 *  @param unit Device number
 *  @param dst_speed Dst port speed value
 *  @param *max_speed max src speed
 *  @brief API to return the Maximum src port speed allowed to
 *         cut-through to a destination port.
 */
static int
bcm56780_a0_tm_get_max_src_ct_speed(int unit,
                         int dst_speed,
                         int ct_mode,
                         uint32_t *max_speed)
{
    uint32_t max_speed_val = 0;

    if (ct_mode == MODE_SAF) {
        max_speed_val = 0;
    }
    else if (ct_mode == MODE_SAME_SPEED) {
        switch (dst_speed) {
            case 10000  : max_speed_val = 40000; break;
            case 25000  : max_speed_val = 100000; break;
            case 40000  : max_speed_val = 100000; break;
            case 50000  : max_speed_val = 200000; break;
            case 100000 : max_speed_val = 400000; break;
            case 200000 : max_speed_val = 400000; break;
            case 400000 : max_speed_val = 400000; break;
            default     : max_speed_val = 0; break;
        }
    }
    else {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Unsupported MODE\n")));
        return SHR_E_PARAM;
    }

    *max_speed = max_speed_val;

    return SHR_E_NONE;
}

/*! @fn void td4_mmu_get_speed_decode(int unit, int speed,
 *              int *speed_decode)
 *  @param unit Device number
 *  @param speed Actual speed value
 *  @param *speed_decode Decoded speed ID value
 *  @brief API to return the decoded speed value, used to program
 *         EBCTM registers
 */
static void
bcm56780_a0_tm_get_speed_decode(int unit, int speed, uint32_t *speed_decode)
{
    switch (speed) {
        case 10000  : *speed_decode = 1; break;
        case 25000  : *speed_decode = 2; break;
        case 40000  : *speed_decode = 3; break;
        case 50000  : *speed_decode = 4; break;
        case 100000 : *speed_decode = 5; break;
        case 200000 : *speed_decode = 6; break;
        case 400000 : *speed_decode = 7; break;
        default     : *speed_decode = 0; break;
    }

}

/*! @fn int bcm56780_a0_tm_ebctm_reconfig(int unit,
 *              port_resource_t *port_resource_t,
 *              asf_mode_e ct_mode, int mmu_port)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @param ct_mode CT Mode
 *  @param mmu_port The mmu_port for this api call
 *  @brief API to update the EBCTM config during
 *         port up and port down operations in flexport.
 */
static int
bcm56780_a0_tm_ebctm_reconfig(int unit, int lport, int mmu_port, int is_port_down)
{
    int inst, phy_port;
    int dst_port_speed;
    uint32_t enable, offset, ct_mode;
    uint32_t ltmbuf;
    uint32_t max_src_speed;
    uint32_t dst_speed_id, max_src_speed_id;

    bcmdrd_fid_t fid;
    bcmdrd_sid_t sid;
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bcmtm_pt_info_t pt_info = {0};
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);

    phy_port = port_map->lport_map[lport].pport;

    inst = mmu_port;
    sid = MMU_EBCTM_EPORT_CT_CFGr;

    /* CT is disabled by default. */
    ct_mode = MODE_SAF;
    enable = 0;
    offset = 0;
    dst_port_speed = 0;

    LOG_DEBUG(BSL_LS_SOC_PORT,
            (BSL_META_U(unit,
                        "physical_port=%d log_port=%3d mmu_port=%3d \n"),
             phy_port, lport, mmu_port));

    /* CT Config */

    ltmbuf = 0;
    BCMTM_PT_DYN_INFO(pt_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));

    max_src_speed = 0;
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_get_max_src_ct_speed(unit, dst_port_speed,
                                            ct_mode, &max_src_speed));

    bcm56780_a0_tm_get_speed_decode(unit, dst_port_speed, &dst_speed_id);
    bcm56780_a0_tm_get_speed_decode(unit, max_src_speed, &max_src_speed_id);

    fid = CT_ENABLEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &enable));

    fid = DST_PORT_SPEEDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &dst_speed_id));

    fid = MAX_SRC_PORT_SPEEDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &max_src_speed_id));

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));

    /* TCT Config */

    ltmbuf = 0;
    sid = MMU_EBCTM_EPORT_TCT_CFGr;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));

    fid = TCT_ENABLEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &enable));
    fid = TCT_ON_TO_CT_OFFSETf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, &ltmbuf, &offset));

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_flex_mmu_reconfigure_phase2(int unit, int lport)
{
    int pport, mport;
    int pipe;
    uint32_t val;
    bcmtm_port_map_info_t *port_map;
    uint32_t encode;
    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);

    pport = port_map->lport_map[lport].pport;
    mport = port_map->pport_map[pport].mport;
    pipe = port_map->lport_map[lport].pipe;
    if (!bcmtm_lport_is_rdb(unit, lport)) {
       SHR_IF_ERR_EXIT
           (bcm56780_a0_tm_port_speed_encoding(unit, lport, &encode));
       val = TD4_X9_MMU_FLUSH_ON;
       SHR_IF_ERR_EXIT
             (bcm56780_a0_tm_mtro_port_metering_config(unit, mport, pipe, val));

       LOG_DEBUG(BSL_LS_SOC_PORT,
                (BSL_META_U(unit, "before td4_mmu_crb_scb_reconfig\n")));
       SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_crb_scb_reconfig(unit, mport, encode));

       LOG_DEBUG(BSL_LS_SOC_PORT,
                (BSL_META_U(unit, "before td4_mmu_oqs_reconfig\n")));
       SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_oqs_reconfig(unit, lport));
       LOG_DEBUG(BSL_LS_SOC_PORT,
                (BSL_META_U(unit, "before td4_mmu_ebpcc_reconfig\n")));
       SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_ebpcc_reconfig(unit, mport, encode));

       LOG_DEBUG(BSL_LS_SOC_PORT,
                (BSL_META_U(unit, "before bcm56780_a0_tm_mmu_eb_sch_reconfig\n")));
       SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_eb_sch_reconfig(unit, lport));

       /* Clear MTRO bucket memories. */
       LOG_DEBUG(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                        "before bcm56780_a0_tm_mmu_clear_mtro_bucket_mems\n")));
       SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_clear_mtro_bucket_mems(unit, lport));


       /* Clear QSCH credit memories. */
       LOG_DEBUG(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                        "bcm56780_a0_tm_mmu_clear_qsch_credit_memories\n")));
       SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_clear_qsch_credit_memories(unit, lport));

       /* Clear WRED Avg_Qsize instead of waiting for background process. */
       LOG_DEBUG(BSL_LS_SOC_PORT,
                (BSL_META_U(unit, "before bcm56780_a0_tm_mmu_wred_clr\n")));
       SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_wred_clr(unit, lport));

       bcm56780_a0_tm_get_thdi_bst_mode(unit, &val);
       if (val == TD4_X9_MMU_BST_WMARK_MODE) {
           /* Clear THDI BST in watermark mode. */
           LOG_DEBUG(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit, "before bcm56780_a0_tm_mmu_thdi_bst_clr\n")));
           SHR_IF_ERR_EXIT
               (bcm56780_a0_tm_thdi_bst_clr(unit, lport));
       }
       bcm56780_a0_tm_get_thdo_bst_mode(unit, &val);
       if (val == TD4_X9_MMU_BST_WMARK_MODE) {
           /* Clear THDO BST counters in watermark mode. */
           LOG_DEBUG(BSL_LS_SOC_PORT,
                   (BSL_META_U(unit, "before bcm56780_a0_tm_mmu_thdo_bst_clr\n")));
           SHR_IF_ERR_EXIT
               (bcm56780_a0_tm_thdo_bst_clr(unit, lport));

           /* Clear PktStat counters in THDO for Queues. */
           SHR_IF_ERR_EXIT
               (bcm56780_a0_tm_thdo_pktstat_clr(unit, lport));
       }

       if (bcmtm_lport_is_fp(unit,lport)) {
           SHR_IF_ERR_EXIT
                (bcm56780_a0_tm_ebctm_reconfig(unit, lport, mport, 0));
       }
    }
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_set_mmu_port_mappings(unit, lport, 0));

   if (!bcmtm_lport_is_rdb(unit, lport)) {
       val = TD4_X9_MMU_FLUSH_OFF;
       SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_mtro_port_metering_config(unit, mport, pipe, val));
   }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_tm_port_mmu_up(int unit, int lport)
{
    SHR_FUNC_ENTER(unit);

    /* Add UP port */
    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_tdm_port_up(unit, lport));

    SHR_IF_ERR_EXIT
        (bcm56780_a0_tm_flex_mmu_reconfigure_phase2(unit, lport));
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * BCMTM INTERNAL PUBLIC FUNCTIONS
 */
int
bcm56780_a0_tm_port_mmu_down(int unit, bcmtm_pport_t pport)
{
    bcmtm_port_map_info_t *port_map;
    int lport;
    bool sim = bcmdrd_feature_is_sim(unit);

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);

    lport = port_map->pport_map[pport].lport;
    if (lport != -1) {
        if (!sim) {
            SHR_IF_ERR_EXIT
                (bcm56780_a0_tm_flex_mmu_port_flush(unit, lport));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcm56780_a0_tm_port_mmu_delete(int unit, bcmtm_pport_t pport)
{
    bcmtm_port_map_info_t *port_map;
    int lport;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);

    lport = port_map->pport_map[pport].lport;
    if (lport != -1) {
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_tdm_port_down(unit, lport));
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_set_mmu_port_mappings(unit, lport, 1));
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcm56780_a0_tm_port_mmu_add(int unit, bcmtm_pport_t pport)
{
    bcmtm_port_map_info_t *port_map;
    int lport;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    lport = port_map->pport_map[pport].lport;
    if (lport != -1) {
        SHR_IF_ERR_EXIT
            (bcm56780_a0_tm_port_mmu_up(unit, lport));
    }

exit:
    SHR_FUNC_EXIT();
}

/*! @fn int bcm56780_a0_tm_rqe_port_flush(int unit)
 *  @param unit Device number
 *  @param itm ITM number to flush
 *  @brief API to Set the RQE Snapshot sidister to Flush out packets in the
 *         RQE Replication FIFO
 */
int
bcm56780_a0_tm_rqe_port_flush(int unit)
{
    int count = 0;
    uint32_t lbuf;
    uint32_t wr_val;
    uint32_t rd_val;
    bcmdrd_sid_t sid;
    bcmtm_pt_info_t pt_info = {0};
    bcmltd_sid_t ltid = -1; /* Requesting LT ID not available */
    bool emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    SHR_FUNC_ENTER(unit);

    /* Set MMU_RQE_QUEUE_FLUSH_EN.VALUE to 1. */
    lbuf = 0;
    wr_val = 1;
    sid = MMU_RQE_QUEUE_SNAPSHOT_ENr;
    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, VALUEf, &lbuf, &wr_val));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &lbuf));

    /* Wait for MMU_RQE_INT_STAT.FLUSH_DONE interrupt to be set. */

    sid = MMU_RQE_INT_STATr;
    while (1) {
        BCMTM_PT_DYN_INFO(pt_info, 0, 0);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info,&lbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, sid, FLUSH_DONEf, &lbuf, &rd_val));

        if (rd_val == 1) {
            break;
        }
        sal_usleep(1 + (emul ? 1 : 0) * EMULATION_FACTOR);
        count++;
        if (count > 30000) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "RQE Flush: FLUSH_DONE isn't reset even after"
                                  " 30000 iterations\n")));
            return SHR_E_TIMEOUT;
        }
    }
    /* Reset MMU_RQE_QUEUE_FLUSH_EN.VALUE to 0. */
    lbuf = 0;
    wr_val = 0;
    sid = MMU_RQE_QUEUE_SNAPSHOT_ENr;
    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, VALUEf, &lbuf, &wr_val));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &lbuf));

    /* Clear MMU_RQE_INT_STAT.FLUSH_DONE interrupt. */

    wr_val = 1; /* w1tc */
    lbuf = 0;
    sid = MMU_RQE_INT_STATr;
    BCMTM_PT_DYN_INFO(pt_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, FLUSH_DONEf, &lbuf, &wr_val));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &lbuf));

exit:
    SHR_FUNC_EXIT();
}
