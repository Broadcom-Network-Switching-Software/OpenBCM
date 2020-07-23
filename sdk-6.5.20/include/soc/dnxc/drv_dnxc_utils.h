
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DRV_DNX_UTILS_H_INCLUDED

#define DRV_DNX_UTILS_H_INCLUDED
#include <bcm/types.h>
#include <bcm/switch.h>

#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/register.h>

typedef void (
    *SOC_TIME_CALLOUT_TYPE) (
    int);

typedef enum
{
    FIRST_SOC_IS_DEVICE_E = 0,

    JERICHO_DEVICE = FIRST_SOC_IS_DEVICE_E,

    DNX_JERICHO_AND_BELOW,

    QMX_DEVICE,

    JERICHO_B0_AND_ABOVE_DEVICE,

    DNX_JERICHO_PLUS_A0,

    J2C_DEVICE,

    J2C_TYPE,

    Q2A_DEVICE,

    Q2A_B0_DEVICE,

    Q2A_B1_DEVICE,

    Q2A_B_DEVICE,

    J2P_DEVICE,

    JERICHO2_ONLY_DEVICE,

    JERICHO2_A0_DEVICE,

    JERICHO2_B1_ONLY,

    DNX_DEVICE,

    DNX2_DEVICE,

    DNXF_DEVICE,

    DFE_DEVICE,

    ARDON_DEVICE,

    ARAD_DEVICE,

    DPP_DEVICE,

    DNX_LTSW,

    DNX_ARADPLUS_AND_BELOW,

    DNX_JERICHO_PLUS_ONLY,

    DNX_QUX,

    DNX_QAX,

    DNX_RAMON,

    DNX_INIT,
    NUM_SOC_IS_DEVICE_E
} soc_is_device_e;

typedef enum
{

    FIRST_SOC_CONTROL_E = 0,

    TIME_CALL_REF_COUNT = FIRST_SOC_CONTROL_E,

    SCACHE_DIRTY,
    SWITCH_EVENT_NOMINAL_STORM,

    INTERRUPTS_INFO,

    SOC_TIME_CALLOUT,

    SOC_FLAGS,

    MEM_SCAN_PID,

    MEM_SCAN_INTERVAL,

    COUNTER_INTERVAL,

    CONTROLLED_COUNTERS,

    COUNTER_PBMP,

    DMA_RPKT,

    DMA_RBYT,

    DMA_TPKT,

    DMA_TBYT,

    NUM_SOC_CONTROL_E
} soc_control_e;

typedef enum
{

    FIRST_SOC_PTYPE_E = 0,
    FE_PTYPE = FIRST_SOC_PTYPE_E,
    GE_PTYPE,

    CE_PTYPE,

    CDE_PTYPE,

    TPON_PTYPE,

    PON_PTYPE,

    LLID_PTYPE,
    XE_PTYPE,

    LE_PTYPE,

    CC_PTYPE,
    HG_PTYPE,

    IL_PTYPE,

    SCH_PTYPE,

    HG_SUBPORT_PTYPE,

    HL_PTYPE,

    ST_PTYPE,

    GX_PTYPE,

    XL_PTYPE,

    XLB0_PTYPE,

    QSGMII_PTYPE,

    MXQ_PTYPE,

    XG_PTYPE,

    XQ_PTYPE,

    XT_PTYPE,

    XW_PTYPE,

    CL_PTYPE,

    CLG2_PTYPE,

    CXX_PTYPE,

    CPRI_PTYPE,

    RSVD4_PTYPE,

    C_PTYPE,

    LBPORT_PTYPE,

    RDBPORT_PTYPE,

    FAEPORT_PTYPE,

    MACSECPORT_PTYPE,

    AXP_PTYPE,

    HYPLITE_PTYPE,

    MMU_PTYPE,

    SPI_PTYPE,

    SPI_SUBPORT_PTYPE,

    SCI_PTYPE,

    SFI_PTYPE,

    REQ_PTYPE,

    TDM_PTYPE,

    RCY_PTYPE,

    CD_PTYPE,

    PORT_PTYPE,

    ETHER_PTYPE,

    ALL_PTYPE,

    PP_PTYPE,

    LP_PTYPE,

    SUBTAG_PTYPE,

    INTP_PTYPE,

    LBGPORT_PTYPE,

    FLEXE_CLIENT_PTYPE,

    FLEXE_MAC_PTYPE,

    FLEXE_SAR_PTYPE,

    FLEXE_PHY_PTYPE,

    NUM_SOC_PTYPE_E
} soc_ptype_e;

typedef enum
{
    FIRST_SOC_PBMP_E = 0,

    ROE_COMPRESSION = FIRST_SOC_PBMP_E,

    LMD_PBM,

    SPI_BITMAP,

    S_PBM,

    GMII_PBM,

    HG2_PBM,

    LB_PBM,

    MMU_PBM,

    EQ_PBM,

    MANAGEMENT_PBM,

    OVERSUB_PBM,

    PBM_EXT_MEM,

    XPIPE_PBM,

    YPIPE_PBM,

    PIPE_PBM,

    TDM_PBM,

    RCY_PBM,

    PHYSICAL_PBM,

    LINKPHY_PBM,

    SUBTAG_PBM,

    SUBTAG_ALLOWED_PBM,

    LINKPHY_ALLOWED_PBM,

    LINKPHY_PP_PORT_PBM,

    ENABLED_LINKPHY_PP_PORT_PBM,

    SUBTAG_PP_PORT_PBM,

    ENABLED_SUBTAG_PP_PORT_PBM,

    GENERAL_PP_PORT_PBM,

    ENABLED_GENERAL_PP_PORT_PBM,

    CUSTOM_PORT_PBMP,

    PBMP_IL_ALL_BITMAP,

    PBMP_XL_ALL_BITMAP,

    PBMP_CE_ALL_BITMAP,

    PBMP_XE_ALL_BITMAP,

    PBMP_HG_ALL_BITMAP,

    PBMP_SFI_ALL_BITMAP,

    PBMP_PORT_ALL_BITMAP,

    PBMP_CMIC_BITMAP,

    PBMP_RCY_ALL_BITMAP,

    PBMP_E_ALL_BITMAP,

    PBMP_ALL_BITMAP,

    PBMP_CDE_BITMAP,

    PBMP_LE_BITMAP,

    PBMP_CC_BITMAP,

    PBMP_FLEXE_CLIENT_BITMAP,

    PBMP_FLEXE_MAC_BITMAP,

    PBMP_FLEXE_SAR_BITMAP,

    PBMP_FLEXE_PHY_BITMAP,

    BLOCK_BITMAP,

    NUM_SOC_PBMP_E
} soc_pbmp_e;

typedef enum
{
    FIRST_UINT8_IN_SOC_INFO_E = 0,

    PORT_NAME_ALTER_VALID = FIRST_UINT8_IN_SOC_INFO_E,

    BLOCK_VALID,
    NUM_UINT8_IN_SOC_INFO_E
} uint8_in_soc_info_e;

typedef enum
{
    FIRST_CHAR2D_IN_SOC_INFO_E = 0,

    PORT_NAME = FIRST_CHAR2D_IN_SOC_INFO_E,
    PORT_NAME_ALTER,

    BLOCK_NAME,
    NUM_CHAR2D_IN_SOC_INFO_E
} char2d_in_soc_info_e;

typedef enum
{
    FIRST_SOC_BLOCK_INFO_E = 0,

    TYPE = FIRST_SOC_BLOCK_INFO_E,

    NUMBER,

    SCHAN,

    CMIC,
    NUM_SOC_BLOCK_INFO_E
} soc_block_info_e;

typedef enum
{
    FIRST_INT_IN_SOC_INFO_E = 0,

    BLOCK_PORT_INT = FIRST_INT_IN_SOC_INFO_E,

    PORT_TYPE_INT,

    PORT_L2P_MAPPING_INT,

    PORT_NUM_LANES_INT,

    PORT_P2L_MAPPING_INT,

    FABRIC_LOGICAL_PORT_BASE_INT,

    NUM_TIME_INTERFACE_INT,

    NUM_UCS_INT,
    HBC_BLOCK_INT,
    FSRD_BLOCK_INT,
    FMAC_BLOCK_INT,
    BRDC_HBC_BLOCK_INT,

    DCC_BLOCK_INT,

    FASIC_BLOCK_INT,
    FLEXEWP_BLOCK_INT,
    FSAR_BLOCK_INT,
    FSCL_BLOCK_INT,
    HAS_BLOCK_INT,
    NUM_INT_IN_SOC_INFO_E
} int_in_soc_info_e;

void dnx_drv_soc_mem_force_read_through_set(
    int unit,
    int enable);

uint8 dnx_drv_soc_mem_force_read_through(
    int unit);

const char *dnx_drv_soc_dev_name(
    int unit);

void *dnx_drv_soc_interrupt_db_ptr(
    int unit);

int dnx_drv_max_num_of_cores(
    void);

int dnx_drv_soc_block_in_list(
    int unit,
    int *blk,
    int type);

shr_error_e dnx_drv_soc_is_valid_block_instance(
    int unit,
    soc_block_types_t block_types,
    int block_instance,
    int *is_valid);

int dnx_drv_soc_block_is_type(
    int unit,
    int blk_idx,
    int *list);

shr_error_e dnx_drv_soc_event_register(
    int unit,
    bcm_switch_event_cb_t cb,
    void *userdata);

int dnx_drv_soc_reg_block_in_list(
    int unit,
    int reg,
    int port_type);

int dnx_drv_soc_reg_numels(
    int unit,
    int reg);

int dnx_drv_soc_reg_first_array_index(
    int unit,
    int reg);

int dnx_drv_soc_reg_array(
    int unit,
    int reg);

int dnx_drv_soc_reg_array2(
    int unit,
    int reg);

shr_error_e dnx_drv_soc_reg_rst_val_get(
    int unit,
    int reg,
    uint64 *rval64_p);

shr_error_e dnx_drv_soc_reg_rst_msk_get(
    int unit,
    int reg,
    uint64 *rmsk64_p);

void *dnx_drv_reg_info_ptr_get(
    int unit,
    int reg);

shr_error_e dnx_drv_soc_event_unregister(
    int unit,
    bcm_switch_event_cb_t cb,
    void *userdata);

shr_error_e dnx_drv_soc_dnxc_avs_value_get(
    int unit,
    uint32 *rov);

void dnx_drv_soc_control_unlock(
    int unit);

void dnx_drv_soc_control_lock(
    int unit);

int dnx_drv_is_e_port(
    int unit,
    bcm_port_t port);

int dnx_drv_is_hg_port(
    int unit,
    bcm_port_t port);

int dnx_drv_is_spi_subport_port(
    int unit,
    bcm_port_t port);

int dnx_drv_soc_controlled_counter_use(
    int unit,
    bcm_port_t port);

void dnx_drv_soc_detach(
    int unit,
    int do_detach);

int dnx_drv_soc_property_name_max(
    void);

int dnx_drv_ihp_block(
    int unit,
    int instance);

int dnx_drv_ile_block(
    int unit,
    int instance);

char *dnx_drv_soc_block_name_lookup_ext(
    soc_block_t has_block,
    int unit);

int dnx_drv_soc_reg_is_write_only(
    int unit,
    int reg);

int dnx_drv_soc_reg_is_above_32(
    int unit,
    int reg);

int dnx_drv_soc_reg_is_above_64(
    int unit,
    int reg);

int dnx_drv_soc_reg_is_array(
    int unit,
    int reg);

int dnx_drv_soc_reg_is_64(
    int unit,
    int reg);

void *dnx_drv_soc_reg_above_64_info_p(
    int unit,
    int reg);

int dnx_drv_soc_reg_above_64_is_zero(
    soc_reg_above_64_val_t fldval);

int dnx_drv_soc_reg_words(
    int unit,
    soc_reg_t reg);

int dnx_drv_is_sfi_port(
    int unit,
    bcm_port_t port);

int dnx_drv_is_il_port(
    int unit,
    bcm_port_t port);

int dnx_drv_soc_warm_boot(
    int unit);

int dnx_drv_clport_block(
    int unit,
    int instance);

int dnx_drv_cdport_block(
    int unit,
    int instance);

int dnx_drv_soc_core_default(
    void);

soc_custom_reg_access_t *dnx_drv_soc_info_custom_reg_access_get(
    int unit);

shr_error_e soc_info_int_address_get(
    int unit,
    int_in_soc_info_e int_id,
    int **int_p_p);

shr_error_e soc_info_int_count_get(
    int unit,
    int_in_soc_info_e int_id,
    int *count_p);

shr_error_e soc_info_pbmp_address_get(
    int unit,
    soc_pbmp_e pbmp,
    pbmp_t ** pbmp_p_p);

shr_error_e dnx_drv_soc_port_min(
    int unit,
    soc_ptype_e ptype,
    int *port_min);

shr_error_e dnx_drv_soc_port_max(
    int unit,
    soc_ptype_e ptype,
    int *port_max);

shr_error_e dnx_algo_port_soc_info_port_add(
    int unit,
    soc_ptype_e ptype,
    bcm_port_t logical_port);

shr_error_e dnx_algo_port_soc_info_port_remove(
    int unit,
    soc_ptype_e ptype,
    bcm_port_t logical_port);

soc_block_t dnx_drv_soc_port_block(
    int unit,
    uint32 phy_port);

int dnx_drv_soc_block_port(
    int unit,
    int block);

int dnx_drv_soc_port_idx_info_blk(
    int unit,
    int port,
    int idx);

shr_error_e dnx_drv_soc_block_info_port_block_address_get(
    int unit,
    soc_block_info_e soc_block_info,
    int port_block,
    int **soc_block_info_p_p);

shr_error_e dnx_drv_soc_block_info_block_address_get(
    int unit,
    int port_block,
    void **soc_block_info_p_p);

shr_error_e soc_info_char_2dimensions_address_size_get(
    int unit,
    char2d_in_soc_info_e char_2d_id,
    int first_index,
    char **char_p_p,
    int *sizeof_second_dimension);

shr_error_e soc_info_uint8_address_get(
    int unit,
    uint8_in_soc_info_e uint8_id,
    uint8 **uint8_p_p);

uint32 dnx_drv_soc_property_get(
    int unit,
    const char *name,
    uint32 defl);

uint32 dnx_drv_soc_property_suffix_num_get(
    int unit,
    int num,
    const char *name,
    const char *suffix,
    uint32 defl);

char *dnx_drv_soc_property_suffix_num_str_get(
    int unit,
    int num,
    const char *name,
    const char *suffix);

char *dnx_drv_soc_property_get_str(
    int unit,
    const char *name);

uint32 dnx_drv_soc_property_port_get(
    int unit,
    soc_port_t port,
    const char *name,
    uint32 defl);

uint32 dnx_drv_soc_property_suffix_num_get_only_suffix(
    int unit,
    int num,
    const char *name,
    const char *suffix,
    uint32 defl);

char *dnx_drv_soc_property_suffix_num_only_suffix_str_get(
    int unit,
    int num,
    const char *name,
    const char *suffix);

char *dnx_drv_soc_property_port_get_str(
    int unit,
    soc_port_t port,
    const char *name);

int dnx_drv_soc_reg_is_valid(
    int unit,
    int reg);

int dnx_drv_soc_reg_is_counter(
    int unit,
    int reg);

int time_interface_id_max(
    int unit);

int soc_is(
    int unit,
    soc_is_device_e soc_device);

shr_error_e soc_control_element_address_get(
    int unit,
    soc_control_e soc_control_element,
    void **soc_control_element_p_p);

int dnx_drv_cmic_schan_words(
    int unit);

int dnx_drv_soc_feature(
    int unit,
    soc_feature_t soc_feature);

uint32 dnx_drv_soc_feature_get(
    int unit,
    soc_feature_t soc_feature);

void dnx_drv_soc_feature_clear(
    int unit,
    soc_feature_t soc_feature);

void dnx_drv_soc_feature_set(
    int unit,
    soc_feature_t soc_feature);

uint32 dnx_drv_soc_intr_enable(
    int unit,
    uint32 mask);

uint32 dnx_drv_soc_intr_disable(
    int unit,
    uint32 mask);

int dnx_drv_soc_port_valid(
    int unit,
    bcm_gport_t gport);

int dnx_drv_soc_mem_block_min(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_block_max(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_block_any(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_block_valid(
    int unit,
    soc_mem_t mem,
    int blk);

void dnx_drv_mem_unlock(
    int unit,
    soc_mem_t mem);

void dnx_drv_mem_lock(
    int unit,
    soc_mem_t mem);

soc_mem_info_t *dnx_drv_soc_mem_info_ptr(
    int unit,
    soc_mem_t mem);

uint32 dnx_drv_soc_mem_info_blocks(
    int unit,
    soc_mem_t mem);

uint32 dnx_drv_soc_mem_info_blocks_hi(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_index_min(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_index_max(
    int unit,
    soc_mem_t mem);

int dnx_drv_copyno_all(
    void);

int dnx_drv_soc_mem_is_array(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_numels(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_first_array_index(
    int unit,
    soc_mem_t mem);

void dnx_drv_soc_reg_above_64_rst_val_get(
    int unit,
    int reg,
    soc_reg_above_64_val_t rval);

void dnx_drv_soc_reg_above_64_rst_msk_get(
    int unit,
    int reg,
    soc_reg_above_64_val_t rmsk);

char *dnx_drv_soc_unit_name(
    int unit);

int dnx_drv_soc_attached(
    int unit);

int dnx_drv_soc_ndev_idx2dev(
    int dev_idx);

int dnx_drv_soc_ndev_get(
    void);

int dnx_drv_soc_mem_entry_words(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_is_writeonly(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_is_readonly(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_is_debug(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_is_sorted(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_is_hashed(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_is_cam(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_is_cbp(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_is_bistepic(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_is_bistffp(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_is_bistcbp(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_is_cachable(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_index_count(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_is_signal(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_is_valid(
    int unit,
    soc_mem_t mem);

int dnx_drv_a_soc_mem_is_valid(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_unit_valid(
    int unit);

uint16 dnx_drv_soc_pci_vendor(
    int unit);

uint16 dnx_drv_soc_pci_device(
    int unit);

uint8 dnx_drv_soc_pci_revision(
    int unit);

int dnx_drv_soc_mem_flags(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_f_all_modules_inited(
    void);

int dnx_drv_soc_f_inited(
    void);

int dnx_drv_mem_block_all(
    void);

int dnx_drv_mem_block_any(
    void);

soc_mem_array_info_t *dnx_drv_soc_mem_array_infop(
    int unit,
    soc_mem_t mem);

soc_reg_array_info_t *dnx_drv_soc_reg_array_info(
    int unit,
    int reg);

int dnx_drv_soc_mem_words(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_bytes(
    int unit,
    soc_mem_t mem);

int dnx_drv_soc_mem_table_bytes(
    int unit,
    soc_mem_t mem);

#ifdef BCM_DNX_SUPPORT

shr_error_e dnx_drv_soc_dnx_hard_reset(
    int unit);

shr_error_e dnx_drv_soc_dnx_rcpu_init(
    int unit);

int dnx_drv_soc_dnx_info_config(
    int unit);

int dnx_drv_soc_dnx_info_config_deinit(
    int unit);

shr_error_e dnx_drv_soc_dnx_schan_config_and_init(
    int unit);

shr_error_e dnx_drv_soc_dnx_endian_config(
    int unit);

shr_error_e dnx_drv_soc_dnx_polling_init(
    int unit);

shr_error_e dnx_drv_soc_dnx_ring_config(
    int unit);

int dnx_drv_soc_dnx_init_reset_cmic_regs(
    int unit);

shr_error_e dnx_drv_soc_dnx_iproc_config(
    int unit);

shr_error_e dnx_drv_soc_dnx_soft_reset(
    int unit);

shr_error_e dnx_drv_soc_dnx_dma_init(
    int unit);

shr_error_e dnx_drv_soc_dnx_dma_deinit(
    int unit);

shr_error_e dnx_drv_soc_dnx_mutexes_init(
    int unit);

shr_error_e dnx_drv_soc_dnx_mutexes_deinit(
    int unit);

shr_error_e dnx_drv_soc_dnx_sbus_polled_interrupts(
    int unit);

shr_error_e dnx_drv_soc_dnx_perform_bist_tests(
    int unit);

shr_error_e dnx_drv_soc_dnx_unmask_mem_writes(
    int unit);

int dnx_drv_soc_dnx_drv_sbus_broadcast_config(
    int unit);

shr_error_e dnx_drv_soc_dnx_mark_not_inited(
    int unit);

shr_error_e dnx_drv_soc_dnx_soft_init(
    int unit);

shr_error_e dnx_drv_soc_dnx_soft_init_no_fabric(
    int unit);

int dnx_drv_soc_dnx_read_hbm_temp(
    const int unit,
    const unsigned hbm_number,
    uint32 *out_temp);

shr_error_e dnx_drv_soc_block_enable_set(
    int unit);

#endif

#endif
