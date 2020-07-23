/*! \file bcmptm_ser_chip_internal.h
 *
 * Interface for SER APIs defined in bcmptm_common.c
 * Those APIs only can be used in SER sub-component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_SER_CHIP_INTERNAL_H
#define BCMPTM_SER_CHIP_INTERNAL_H

#include <bcmdrd/bcmdrd_types.h>
#include <sal/sal_types.h>
#include <sal/sal_time.h>
#include <bcmdrd/bcmdrd_ser.h>
#include <bcmltd/bcmltd_types.h>


/*!\brief  string name of single access types */
#define ACC_TYPE_SINGLE_STR                "SINGLE"

/*! \brief  string name of unique access types */
#define ACC_TYPE_UNIQUE_STR                "UNIQUE"

/*!\brief  string name of duplicate access types */
#define ACC_TYPE_DUPLICATE_STR             "DUPLICATE"

/*!\brief  string name of data-split access types */
#define ACC_TYPE_DATA_SPLIT_STR            "DATA_SPLIT"

/*!\brief  string name of addr-split access types */
#define ACC_TYPE_ADDR_SPLIT_STR            "ADDR_SPLIT"

/*!\brief  string name of addr-split-split access types */
#define ACC_TYPE_ADDR_SPLIT_SPLIT_STR      "ADDR_SPLIT_SPLIT"

/*!\brief  string name of duplicate 2 access types, write 0, 2, 4, 6 pipes */
#define ACC_TYPE_DUPLICATE_2_STR      "DUPLICATE_2"

/*!\brief  string name of duplicate 4 access types, write 0, 4 pipes */
#define ACC_TYPE_DUPLICATE_4_STR      "DUPLICATE_4"

/*!\brief SINGLE access type */
#define PT_IS_SINGLE_ACC_TYPE(str) \
    (sal_strcmp(ACC_TYPE_SINGLE_STR, str) == 0)

/*!\brief UNIQUE access type */
#define PT_IS_UNIQUE_ACC_TYPE(str) \
    (sal_strcmp(ACC_TYPE_UNIQUE_STR, str) == 0)

/*!\brief DATA-SPLIT access type */
#define PT_IS_DATA_SPLIT_ACC_TYPE(str) \
    (sal_strcmp(ACC_TYPE_DATA_SPLIT_STR, str) == 0)

/*!\brief DUPLICATE access type */
#define PT_IS_DUPLICATE_ACC_TYPE(str) \
    ((sal_strcmp(ACC_TYPE_DUPLICATE_STR, str) == 0) || \
     (sal_strcmp(ACC_TYPE_DUPLICATE_2_STR, str) == 0) || \
     (sal_strcmp(ACC_TYPE_DUPLICATE_4_STR, str) == 0) || \
      PT_IS_DATA_SPLIT_ACC_TYPE(str))

/*!\brief ADDR-SPLIT access type */
#define PT_IS_ADDR_SPLIT_ACC_TYPE(str) \
    (sal_strcmp(ACC_TYPE_ADDR_SPLIT_STR, str) == 0)

/*!\brief ADDR-SPLIT-SPLIT access type */
#define PT_IS_ADDR_SPLIT_SPLIT_ACC_TYPE(str) \
    (sal_strcmp(ACC_TYPE_ADDR_SPLIT_SPLIT_STR, str) == 0)

/*!\brief DUPLICATE_2 access type */
#define PT_IS_DUPLICATE_2_ACC_TYPE(str) \
    (sal_strcmp(ACC_TYPE_DUPLICATE_2_STR, str) == 0)

/*!\brief DUPLICATE_4 access type */
#define PT_IS_DUPLICATE_4_ACC_TYPE(str) \
    (sal_strcmp(ACC_TYPE_DUPLICATE_4_STR, str) == 0)

/*!\brief HW multi-pipe table */
#define BCMPTM_SER_FLAG_MULTI_PIPE     0x00400

/*!\brief Read table with XY format */
#define BCMPTM_SER_FLAG_XY_READ        0x00800

/*!\brief Memories shared in HW */
#define BCMPTM_SER_FLAG_OVERLAY        0x04000

/*!\brief HW multi-pipe table */
#define BCMPTM_SER_FLAG_NARROW_VIEW    0x08000

/*!\brief HW multi-pipe table */
#define BCMPTM_SER_FLAG_WIDE_VIEW      0x10000

/*!\brief Unique pipe acc type mode */
#define BCMPTM_SER_FLAG_ACC_TYPE_CHK   0x20000

/*!\brief Use 'addr_mask' as range disable bitmap */
#define BCMPTM_SER_FLAG_RANGE_DISABLE  0x40000


/*!\brief SBUS memory DMA read operation */
#define BCMPTM_SER_READ_MEMORY_CMD_MSG        1

/*!\brief SBUS register DMA read operation */
#define BCMPTM_SER_READ_REGISTER_CMD_MSG      2

/* SBUS Memory write operation */
#define BCMPTM_SER_WRITE_MEMORY_CMD_MSG       3

/* SBUS Register write operation */
#define BCMPTM_SER_WRITE_REGISTER_CMD_MSG     4


/* Using overlay block type to replace block type reported by H/W wrongly. */
#define BCMPTM_SER_OVERLAY_BLK_TYPE_NUM       2

/*! Code of access type, string name of access type */
typedef struct bcmptm_ser_acctype_info_s {
    /*! Code of access type, chip specific */
    uint32_t    acctype;
    /*! String name of access type */
    const char *acctype_name;
} bcmptm_ser_acctype_info_t;

/*!
 * \brief Defined for MMU memory tables which are needed to cleared before enabling SER parity
 */
typedef struct bcmptm_ser_mem_list_s {
    /*!< SID of mem */
    bcmdrd_sid_t  mem_sid;
    /*!< -1: clear all entries; >=0: clear the index'th entry*/
    int           index;
} bcmptm_ser_mem_clr_list_t;

/*!
 * \brief Field and its value used to initialize registers during SER initialization.
 */
typedef struct bcmptm_fld_info_s {
    /*!< FID of control register */
    bcmdrd_fid_t  ctrl_fld;
    /*!< Value of FID read from PT */
    uint32_t      rd_val[BCMDRD_CONFIG_MAX_UNITS];
    /*!< Value of FID written to PT */
    uint32_t      wr_val;
    /*! < field needed to read, 0: write, 1: read, 2: pop */
    uint32_t      rd;
} bcmptm_ser_fld_info_t;

/*!
 * \brief Used to initialize registers during SER initialization.
 */
typedef struct bcmptm_ser_ctrl_reg_s {
    /*!< SID of control register or mem */
    bcmdrd_sid_t                 ctrl_reg;
    /*!< index of control register or mem */
    int                          ctrl_reg_index;
    /*!< fields and their value */
    bcmptm_ser_fld_info_t       *ctrl_flds_val;
    /*! < number of fields of SID */
    uint32_t                     num_fld;
    /*! < sleep for utime */
    uint32_t                     utime;
} bcmptm_ser_ctrl_reg_info_t;


/*!
 * \brief SER protection mode of cmic SER engine
 */
typedef enum {
    /*!< Parity 1bit protection mode */
    BCMPTM_SER_PARITY_1BIT,
    /*!< Parity 2bit protection mode */
    BCMPTM_SER_PARITY_2BITS,
    /*!< Parity 4bit protection mode */
    BCMPTM_SER_PARITY_4BITS,
    /*!< Parity 8bit protection mode */
    BCMPTM_SER_PARITY_8BITS
} bcmptm_ser_protection_mode_t;

/*!
 * \brief SER protection type of cmic SER engine
 */
typedef enum {
    /*!< Parity protection mode */
    BCMPTM_SER_TYPE_PARITY,
} bcmptm_ser_protection_type_t;

/*!
 * \brief interleave type of cmic SER engine
 */
typedef enum {
    /*!< None interleave */
    BCMPTM_SER_INTERLEAVE_NONE,
    /*!< 1bit interleave */
    BCMPTM_SER_INTERLEAVE_MOD2,
    /*!< 2bit interleave */
    BCMPTM_SER_INTERLEAVE_MOD4
} bcmptm_ser_interleave_type_t;

/*!
 * \brief SER info types (for all chips)
 */
typedef enum {
    BCMPTM_SER_INFO_TYPE_NONE,
    /*!< MMU */
    BCMPTM_SER_INFO_TYPE_MMU,
    /*!< IPIPE */
    BCMPTM_SER_INFO_TYPE_IPIPE,
    /*!< EPIPE */
    BCMPTM_SER_INFO_TYPE_EPIPE,
    /*!< IPIPE IDB */
    BCMPTM_SER_INFO_TYPE_IPIPE_IDB,
    /*!< SER engine for TCAM*/
    BCMPTM_SER_INFO_TYPE_SER_ENGINE,
    /*!< L2_MGMTm */
    BCMPTM_SER_INFO_TYPE_L2_MGMT,
    /*!< CLPORT */
    BCMPTM_SER_INFO_TYPE_CLPORT,
    /*!< XLPORT */
    BCMPTM_SER_INFO_TYPE_XLPORT,
    /*!< CDPORT */
    BCMPTM_SER_INFO_TYPE_CDPORT,
    /*!< IPIPE DLB ECMP */
    BCMPTM_SER_INFO_TYPE_IPIPE_DLB_ECMP,
    /*!< IPIPE POST */
    BCMPTM_SER_INFO_TYPE_IPIPE_POST,
    /*!< EPIPE 2 */
    BCMPTM_SER_INFO_TYPE_EPIPE_2,
    /*!< EPIPE EDB */
    BCMPTM_SER_INFO_TYPE_EPIPE_EDB,
    /*!< PORT_IF EDB */
    BCMPTM_SER_INFO_TYPE_PORT_IF_EDB,
    /*!< PORT_IF IDB */
    BCMPTM_SER_INFO_TYPE_PORT_IF_IDB,
    /*!< IPIPE DII */
    BCMPTM_SER_INFO_TYPE_IPIPE_DII,
    /*!< EPIPE DII */
    BCMPTM_SER_INFO_TYPE_EPIPE_DII,
    /*!< SEC */
    BCMPTM_SER_INFO_TYPE_SEC,
    /* Add new parity type here */
} bcmptm_ser_info_type_t;

/*!
 *\brief Specifies the start bit position and
 * end bit position for the subfield within the entry
 */
typedef struct bcmptm_ser_start_end_bits_s {
    /*!< start bit, not be used when its value is less than 0 */
    int start_bit;
    /*!< end bit, not be used when its value is less than 0 */
    int end_bit;
} bcmptm_ser_start_end_bits_t;

/*!\brief Max number of subfield per entry, for CMIC SW scan */
#define BCMPTM_NUM_PROT_SECTIONS  4

/*!
 *\brief  TCAM SER information used to configure CMIC SER engine.
 */
typedef struct bcmptm_ser_tcam_generic_info_s {
    /*!< parity data of mem */
    bcmdrd_sid_t                  mem;
    /*!< Block type */
    int                           blk_type;
    /*!< SER protection type */
    bcmptm_ser_protection_type_t  prot_type;
    /*!< SER protection mode */
    bcmptm_ser_protection_mode_t  prot_mode;
    /*!< SER interleave type */
    bcmptm_ser_interleave_type_t  intrlv_mode;
    /*!< the start and end bit position for the subfield within the entry */
    bcmptm_ser_start_end_bits_t   start_end_bits[BCMPTM_NUM_PROT_SECTIONS];
    /*!< Range disable bitmap to skip memory holes, valid only for BCMPTM_SER_FLAG_RANGE_DISABLE */
    uint32_t                      addr_mask;
    /*!< The start offset of SER_MEMORY for one SER range */
    uint32_t                      ser_section_start;
    /*!< The end offset of SER_MEMORY fo one SER range */
    uint32_t                      ser_section_end;
    /*! Parameters configured by the applications */
    uint32_t                      ser_flags;
    /*!< number of instance of mem, Valid only for BCMPTM_SER_FLAG_ACC_TYPE_CHK */
    uint8_t                       num_instances;
    /*!< Start bit position to skip memory holes, valid only for BCMPTM_SER_FLAG_RANGE_DISABLE */
    uint32_t                      addr_start_bit;
    /*!< Get data from this LT to know whether TCAM is used as unique mode */
    bcmltd_sid_t                  cfg_lt_sid;
    /*!< cfg_lt_fid of cfg_lt_sid, if value of this field is 1, TCAM is used as unique mode*/
    bcmltd_fid_t                  cfg_lt_fid;
} bcmptm_ser_tcam_generic_info_t;

/*!
 *\brief Controlling register and name of buffer or bus.
 */
typedef struct bcmptm_ser_buf_bus_ctrl_reg_s {
    /*!< Controlling register */
    bcmdrd_sid_t reg;
    /*!< Name of buffer or bus */
    char         *mem_str;
    /*!< Membase of buffer or bus */
    int          membase;
    /*!< Stage ID of buffer or bus */
    int          stage_id;
} bcmptm_ser_buf_bus_ctrl_reg_t;

/*!
 * \brief SER interrupt clear way.
 */
typedef enum {
    /* Read and clear */
    BCMPTM_SER_CLR_INTR_NO_OPER,
    /* 0->1 raise edge */
    BCMPTM_SER_CLR_INTR_RAISE_EDGE,
    /* Write 0 to clear */
    BCMPTM_SER_CLR_INTR_WRITE_ZERO,
    /* Write 1 to clear */
    BCMPTM_SER_CLR_INTR_WRITE_ONE,
    /* Read and clear */
    BCMPTM_SER_CLR_INTR_READ,
    /* Add new method here */
} bcmptm_ser_intr_clr_type_t;

/*
 *\brief Status register information format of BCMPTM_SER_INFO_TYPE_MAC.
 */
typedef struct bcmptm_ser_mac_status_fld_s {
    /*!< Field of SER intr status register  */
    bcmdrd_fid_t            intr_status_field;
    /*!< SER fields of intr clear register */
    bcmdrd_fid_t            intr_clr_field;
    /*!< 0: 2bit error, 1: 1bit error, 2: multi error, -1: don't care */
    int                     err_type;
    /*!<
     * 0: rising edge(0->1), write clear register.
     * 1: write 0 to clear, 2: write 1 to clear, 3: read-and-clear, write or read status register.
     */
    bcmptm_ser_intr_clr_type_t clr_method;
} bcmptm_ser_mac_status_fld_t;

/*
 *\brief Status register information format of BCMPTM_SER_INFO_TYPE_SEC.
 */
typedef struct bcmptm_ser_sec_status_fld_s {
    /*!< Register for additional SER intr status. */
    bcmdrd_sid_t            intr_status_reg;
    /*!< Field of additional SER intr status register  */
    bcmdrd_fid_t            intr_status_field;
    /*!< Value of field to be written. */
    int                     wr_val;
    /*!< Skip read. */
    int                     rd_skip;
} bcmptm_ser_sec_status_fld_t;

/*!
 *\brief Status register information format of BCMPTM_SER_INFO_TYPE_ECC.
 */
typedef struct bcmptm_ser_ecc_status_fld_s {
    /*!< ECC_ERRf  */
    bcmdrd_fid_t              ecc_err_field;
    /*!< Fields of SER intr status register except ECC_ERRf  */
    bcmdrd_fid_t              *intr_status_field_list;
} bcmptm_ser_ecc_status_fld_t;

/*!
 *\brief Enable ECC checking and interrupts which are not enabled by BD.
 */
typedef struct bcmptm_ser_ctrl_en_s {
    /*!< SID of enabling register */
    bcmdrd_sid_t         enable_reg;
    /*!< FID of enabling register */
    bcmdrd_fid_t         enable_field;
    /*!< FID list of enabling register */
    bcmdrd_fid_t         *enable_field_list;
} bcmptm_ser_ctrl_en_t;

/*!\brief information of buffer or bus */
typedef struct bcmptm_ser_buffer_bus_info_s {
    /*!< Name of buffer or bus from BD */
    char         *bd_name;
    /*!< Membase information from SER_STATUS_BUS format in regsfile */
    char         *membase_info;
    /*!< Membase of buffer or bus */
    int          membase;
    /*!< Stage ID of buffer or bus */
    int          stage_id;
    /*!< 1: buffer or memory, 0: bus */
    bool         mem;
} bcmptm_ser_buffer_bus_info_t;

/*!\brief handler for interrupts, its format is equal to bcmbd_intr_f */
typedef void (*bcmbd_ser_intr_f)(int unit, uint32_t intr_param);

/* Instance bit map << 16 | interrupt block flag << 8 | interrupt source number */
#define BCMPTM_SER_INTR_INST_SHIFT     16

#define BCMPTM_SER_INTR_FLG_SHIFT       8

/*!\brief Macro to retrieve the interrupt number. */
#define BCMPTM_SER_INTR_NUM_SET(_prm, _intr_num) \
    ((_prm) |= _intr_num)

/*!\brief Macro to retrieve the interrupt number. */
#define BCMPTM_SER_INTR_NUM_GET(_prm) \
    ((_prm) & ((1 << BCMPTM_SER_INTR_FLG_SHIFT) - 1))

/*!\brief Macro to set the instance number.  */
#define BCMPTM_SER_INTR_INST_SET(_prm, _inst_num) \
    ((_prm) |= ((_inst_num) << BCMPTM_SER_INTR_INST_SHIFT))

/*!\brief Macro to retrieve the instance number. */
#define BCMPTM_SER_INTR_INST_GET(_prm) \
    (((_prm) >> BCMPTM_SER_INTR_INST_SHIFT) & 0xffff)

/*!\brief Macro to set the interrupt flag.  */
#define BCMPTM_SER_INTR_FLG_SET(_prm, _intr_flg) \
    ((_prm) |= ((_intr_flg) << BCMPTM_SER_INTR_FLG_SHIFT))

/*!\brief Macro to get sub-level intr flag. */
#define BCMPTM_SER_INTR_FLG_GET(_prm) \
    (((_prm) >> BCMPTM_SER_INTR_FLG_SHIFT) & 0xff)


/*!\brief Non sub-level interrupt source */
#define BCMPTM_SER_NON_SUB_LVL_INT (0x80)

/*!
 *\brief interrupt number of one block can overlap interrupt number of other blocks.
 */

/* IPEP */
#define BCMPTM_SER_IPEP_INTR_FLG  (1)
/* MMU */
#define BCMPTM_SER_MMU_INTR_FLG   (2)
/* PORT */
#define BCMPTM_SER_PORT_INTR_FLG  (3)

/* EDB-IDB */
#define BCMPTM_SER_EDB_IDB_INTR_FLG  (4)

typedef struct bcmptm_serc_non_fifo_s {
    /*!< Interrupt status register */
    bcmdrd_sid_t              stat_reg;
    /*!< Field of interrupt status register */
    bcmdrd_fid_t              intr_stat_fld;
    /*!< 0: MAC, 1: ECC, 2: SEC */
    int                       ecc_mac_mode;
    /*!< SER intr status register  */
    bcmdrd_sid_t              intr_stat_reg;
    /*!< SER intr status register list only for ECC  */
    bcmptm_ser_buf_bus_ctrl_reg_t *intr_stat_reg_list;
    /*!< SER intr clear register, can be INVALIDf */
    bcmdrd_sid_t              intr_clr_reg;
    /*!< SER fields of intr status register of ECC */
    bcmptm_ser_ecc_status_fld_t *ecc_stat_fld_info;
    /*!< SER fields of intr status register of MAC */
    bcmptm_ser_mac_status_fld_t *mac_stat_fld_info;
    /*!< SER fields of intr status register of SEC */
    bcmptm_ser_sec_status_fld_t *sec_stat_fld_info;
    /*!< Block type defined by BD */
    int                          blk_type;
    /*!< Debug information */
    char                        *mem_str;
} bcmptm_serc_non_fifo_t;

typedef struct bcmptm_serc_fifo_s {
    /*!< Interrupt status register */
    bcmdrd_sid_t              stat_reg;
    /*!< Field of interrupt status register */
    bcmdrd_fid_t              intr_stat_fld;
    /*!< SID of SER_FIFOm */
    bcmdrd_sid_t              fifo_mem;
    /*!< Field list of SER_FIFOm*/
    bcmdrd_fid_t              *fifo_mem_fids;
    /*!< SER event count register */
    bcmdrd_sid_t              ser_fail_ctr_reg;
    /*!< Block type defined by BD */
    int                       blk_type;
    /*!< Sub block name */
    char                      *blk_name;
} bcmptm_serc_fifo_t;

typedef struct bcmptm_ser_intr_mapping_s {
    /*!< Label where the interrupt come from */
    uint32_t                  sub_intr_flag;
    /*!< Sub-block interrupt number defined by BD */
    int                       sub_intr_num[16];
    /*!< CMIC-level interrupt number defined by BD */
    int                       cmic_intr_num_start;
    /*!< CMIC-level interrupt number defined by BD */
    int                       cmic_intr_num_end;
    /*!< Interrupt ID interval */
    uint32_t                  intr_interval;
    /*!< SER information type */
    bcmptm_ser_info_type_t    ser_route_type;
    /*!< Info used to report and clear SER error. */
    void                      *serc_info;
    /*!< Depth of info */
    int                       depth;
    /*!< Depth of info */
    int                       fifo;
} bcmptm_ser_intr_route_info_t;

/* Information gotten according to interrupt number. */
typedef struct bcmptm_pre_serc_info_s {
    /*!< Info of bcmptm_serc_fifo_t or bcmptm_serc_non_fifo_t */
    void *stat_info;
    /*!< Depth of stat_info */
    int   stat_depth;
    /*!< Block instance */
    int   blk_inst;
    /*!< Port number */
    int   port;
} bcmptm_pre_serc_info_t;

/*!
 *\brief Get adext for one chips according to the members of following structure.
 */
typedef struct bcmptm_cmic_adext_info_s {
    /*!< access type */
    int acc_type;
    /*!< block number */
    int blk_num;
    /* Add new member here */
} bcmptm_cmic_adext_info_t;

/*!\brief SER flags */
typedef enum {
    /*! Multi-bits SER error */
    BCMPTM_SER_FLAG_MULTIBIT  = 1 << 1,
    /*! SER type. 1: 1bit SER checking; 0: parity/2bit SER checking */
    BCMPTM_SER_FLAG_DOUBLEBIT = 1 << 2,
    /*! ECC SER error */
    BCMPTM_SER_FLAG_ECC = 1 << 3,
    /*! Parity SER error */
    BCMPTM_SER_FLAG_PARITY = 1 << 4,
    /*! SBUS can't be accessible */
    BCMPTM_SER_FLAG_NON_SBUS = 1 << 5,
    /*! SER caused packet drop */
    BCMPTM_SER_FLAG_DROP = 1 << 6,
    /*! EP/IP: SER error in SOP cell.  */
    BCMPTM_SER_FLAG_ERR_IN_SOP = 1 << 7,
    /*! EP/IP: SER error in MOP cell. */
    BCMPTM_SER_FLAG_ERR_IN_MOP = 1 << 8,
    /*! EP/IP: SER error in EOP cell. */
    BCMPTM_SER_FLAG_ERR_IN_EOP = 1 << 9,
    /*! EP/IP: SER error in sbus transaction. */
    BCMPTM_SER_FLAG_ERR_IN_SBUS = 1 << 10,
    /*! EP/IP: SER error in transaction - refresh, aging etc. */
    BCMPTM_SER_FLAG_ERR_IN_OTHER = 1 << 11,
    /*! MMU: SER error in MMU transaction. */
    BCMPTM_SER_FLAG_ERR_IN_MMU = 1 << 12,
    /*! PORT: SER error in PORT transaction. */
    BCMPTM_SER_FLAG_ERR_IN_PORT = 1 << 13,
    /*! TCAM SER error. */
    BCMPTM_SER_FLAG_TCAM = 1 << 14,
    /*! Internal memory */
    BCMPTM_SER_FLAG_NON_SBUS_MEM = 1 << 15,
    /*! Internal bus */
    BCMPTM_SER_FLAG_NON_SBUS_BUS = 1 << 16,
    /*! HW fault */
    BCMPTM_SER_FLAG_HF = 1 << 17,
    /*! High severity fault */
    BCMPTM_SER_FLAG_HSF = 1 << 18,
    /*! Error is recovered by HW */
    BCMPTM_SER_FLAG_HW_CORRECTED = 1 << 19,
    /*! SEC: SER error in SEC transaction. */
    BCMPTM_SER_FLAG_ERR_IN_SEC = 1 << 20,
    /*! SEC: SER error in shadow PT. */
    BCMPTM_SER_FLAG_ERR_IN_SHADOW = 1 << 21
} bcmptm_ser_log_flag_t;

/*!
 * \brief information can be used to correct SER error.
 */
typedef struct bcmptm_ser_correct_info_s {
    /*! flags of SER correction */
    uint32_t        flags;
    /*! SIDs of memory or register owning SER error, use it to correct SER error */
    bcmdrd_sid_t    sid[2];
    /*! SID of memory or register owning SER error, reporting view */
    bcmdrd_sid_t    reported_sid;
    /*! ID of table instance owning SER error */
    int             tbl_inst[2];
    /*! If tbl_copy is not 0, it is block instance */
    uint32_t        tbl_copy;
    /*! SBUS address of one entry owning SER error */
    uint32_t        addr[2];
    /*! Entry index owning SER error */
    uint32_t        index[2];
    /*! Block type */
    int             blk_type;
    /*! Recovery type */
    int             resp_type;
    /*! Detect_time */
    sal_usecs_t     detect_time;
    /*!
     * SID of status register for buffer or bus, such as: IDB_IS_TDM_CAL_ECC_STATUSr.
     * According to status register, SERC can know which memory needs to be corrected or cleared.
     */
    bcmdrd_sid_t    status_reg_sid;
} bcmptm_ser_correct_info_t;

/*!\brief In order to correct a SER error, SERC needs to write several PTs.  */
#define BCMPTM_SER_MAX_SRAMS              16

/*!\brief Max entries number per bucket.  */
#define BCMPTM_SER_MAX_ENTRIES_PER_BKT    8

/*!\brief Used to correct SER in XOR or LP PTs */
typedef struct bcmptm_ser_sram_info_s {
    /*! RAM number */
    uint32_t      ram_count;
    /*! PT ID */
    bcmdrd_sid_t  pt_view[BCMPTM_SER_MAX_SRAMS];
    /*! Number of indexes */
    uint32_t      index_count[BCMPTM_SER_MAX_SRAMS];
    /*! Indexes */
    uint32_t      indexes[BCMPTM_SER_MAX_SRAMS][BCMPTM_SER_MAX_ENTRIES_PER_BKT];
    /*! H/W recovery for LP tables */
    bool          hw_recovery;
} bcmptm_ser_sram_info_t;

/*!\brief number of extra controlling register */
#define BCMPTM_SER_EXTRA_REG_NUM     2

/*!\brief controlling register info for XOR PTs */
typedef struct bcmptm_ser_control_reg_info_s {
    /*! SID: Disables PRE_MEMWR parity check. */
    bcmdrd_sid_t  wr_check_reg;
    /*! FID: Disables PRE_MEMWR parity check. */
    bcmdrd_fid_t  wr_check_field;
    /*! SID: force XOR generation per table bank for reads */
    bcmdrd_sid_t  force_reg;
    /*! SID: force XOR generation per table bank for reads */
    bcmdrd_fid_t  force_field;
    /*! SID: Disables XOR bank write. */
    bcmdrd_sid_t  xor_wr_reg;
    /*! FID: Disables XOR bank write. */
    bcmdrd_fid_t  xor_wr_field;
    /*! SID: can be used to Freeze L2. */
    bcmdrd_sid_t  extra_reg[BCMPTM_SER_EXTRA_REG_NUM];
    /*! FID: can be used to Freeze L2. */
    bcmdrd_fid_t  extra_field[BCMPTM_SER_EXTRA_REG_NUM];
    /*! FVAL: can be used to Freeze L2. */
    uint32_t      extra_field_val[BCMPTM_SER_EXTRA_REG_NUM];
} bcmptm_ser_control_reg_info_t;

/*!\brief Block type mapping between BD and SER */
typedef struct bcmptm_ser_blk_map_s {
    /*! Block type */
    int  blk_type;
    /*! SER block type */
    int  ser_blk_type;
    /*! Use overlay block type to replace block type reported by H/W wrongly. */
    int  blk_type_overlay[BCMPTM_SER_OVERLAY_BLK_TYPE_NUM];
} bcmptm_ser_blk_map_t;


/*!
 * \brief Get MMU memory tables which should be cleared before enabling SER parity.
 *
 * \param [in] unit       Unit number.
 * \param [out] mem_list  List of PTs which should be cleared
 * \param [out] num       Number of PTs
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_mem_clr_list_get(int unit,
                            bcmptm_ser_mem_clr_list_t **mem_list,
                            int *entry_num);

/*!
 * \brief Get control registers' info.
 *
 * These control registers can be used to clear memory tables which
 * should be cleared after enabling SER checking.
 *
 * \param [in] unit           Unit number.
 * \param [out] reg_ctrl_lst  Data list of controlling registers
 * \param [out] reg_num       Number of item in data list
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_clear_mem_after_ser_enable(int unit,
                                      bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                      int *reg_num);

/*!
 * \brief Get control registers' info.
 *
 * These registers can be used to clear memory tables which
 * should be cleared before enabling SER checking.
 *
 * \param [in] unit          Unit number.
 * \param [out] reg_ctrl_lst Data list of controlling registers used to clear memory tables
 * \param [out] reg_num      Number of item in data list
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_clear_mem_before_ser_enable(int unit,
                                       bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                       int *reg_num);

/*!
 * \brief Get control registers' info used to start IDB SER H/W scan engine for TCAMs.
 *
 * \param [in] unit          Unit number.
 * \param [in] enable        1/0: Controlling information can be used to enable/disable H/W scan.
 * \param [out] reg_ctrl_lst Data list of controlling registers
 * \param [out] reg_num      Number of item in data list
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_tcam_hw_scan_reg_info_get(int unit, int enable,
                                     bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                     int *reg_num);

/*!
 * \brief Get ser_acc_type_map info. The info can be used to configure SER_ACC_TYPE_MAPm.
 *
 * \param [in] unit               Unit number.
 * \param [out] ser_acc_type_map  Data list of SER_ACC_TYPE_MAPm
 * \param [out] reg_num           Number of item in data list
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_acc_type_map_info_get(int unit,
                                 bcmptm_ser_ctrl_reg_info_t **ser_acc_type_map,
                                 int *reg_num);

/*!
 * \brief Get TCAM SER information.
 *
 * According to this SER information, TCAM tables can be protected by SER CMIC engine.
 * And TCAM S/W scan thread can know how to scan the TCAM tables.
 *
 * \param [in] unit             Unit number.
 * \param [out] tcam_ser_info   Data list of TCAM SER scan info
 * \param [out] tcam_info_num   Number of item in data list
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_tcam_info_get(int unit,
                         bcmptm_ser_tcam_generic_info_t **tcam_ser_info,
                         int *tcam_info_num);

/*!
 * \brief Get SER mute memory tables or buffers or buses.
 *
 * Some tables or buffers or buses SER checking should not be enabled.
 *
 * \param [in] unit                Unit number.
 * \param [out] mem_ser_mute_list  Memory table list.
 * \param [out] mem_ser_mute_num   Number of memory tables.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_mute_mem_list_get(int unit,
                             bcmdrd_sid_t **mem_ser_mute_list,
                             int *mem_ser_mute_num);

/*!
 * \brief Can't enable DROP_PKT for some PTs.
 *
 * Take EGR_METADATA_BITP_PROFILEm for example,
 * its control register is EGR_METADATA_BITP_PROFILE_SER_CONTROLr, we do not need to
 * enable field SER_PKT_DROP_EN of this control register for EGR_METADATA_BITP_PROFILEm.
 *
 * \param [in] unit                Unit number.
 * \param [out] ctrl_reg_mute_list control register list.
 * \param [out] ctrl_reg_mute_num Number of control register.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_mute_drop_pkt_reg_list_get(int unit,
                                      bcmdrd_sid_t **ctrl_reg_mute_list,
                                      int *ctrl_reg_mute_num);

/*!
 * \brief Get SER route information.
 *
 * The routine information is helpful to enable SER checking and interrupt (blocks).
 * Most of SER control registers are controlled by BD.
 *
 * \param [in] unit              Unit number.
 * \param [out] ser_blks_ctrl_regs Data list used to enable SER checking and interrupt.
 * \param [out] route_num        Number of item in data list
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_blk_ctrl_regs_get(int unit,
                             bcmptm_ser_ctrl_en_t **ser_blks_ctrl_regs,
                             int *ctrl_item_num);

/*!
 * \brief Get SER interrupt mapping.
 *
 * Get SER correction information accorrding SER interrupt number.
 *
 * \param [in] unit            Unit number.
 * \param [out] mac_inst   Mac instance number per PM.
 * \param [out] intr_map       List of low level interrupt information.
 * \param [out] intr_map_num   Number of item of data list.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_intr_map_get(int unit,
                        int *mac_inst,
                        bcmptm_ser_intr_route_info_t **intr_map,
                        int *intr_map_num);

/*!
 * \brief Get SER logical block types according to block types(per chip).
 *
 * For some PTs, H/W can reports wrong block type, so when SER logic find
 * block type reported by H/W is wrong, this routine will tell SER logic which
 * block type the PTs belonged to.
 *
 * \param [in] unit               Unit number.
 * \param [in] blk_type           Block type (per chip) defined by BD.
 * \param [out] blk_type_overlay  Block type used to replace block type reported by H/W.
 * \param [out] ser_blk_type      Block type defined by SER LT.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_blk_type_map(int unit,
                        int blk_type, int *blk_type_overlay,
                        int *ser_blk_type);

/*!
 * \brief Enable or disable SER interrupts.
 *
 * \param [in] unit         Unit number.
 * \param [in] sync         1: sync mode, 0: async mode.
 * \param [in] intr_num     Interrupt number of CMIC level.
 * \param [in] enable       1: enable, 0: disable.
 *
 * \return NONE.
 */
extern void
bcmptm_ser_lp_intr_enable(int unit, int sync, int intr_num, int enable);

/*!
 * \brief Register SER interrupt CB routine.
 *
 * \param [in] unit             Unit number.
 * \param [in] intr_num         Number of interrupt source.
 * \param [in] intr_func        CB routine for one interrupt source
 * \param [in] intr_param     Input parameter value of CB routine
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_lp_intr_func_set(int unit, unsigned int intr_num,
                         bcmbd_ser_intr_f intr_func, uint32_t intr_param);

/*!
 * \brief Get registers to configure CMIC SER engine, in order to
 * provide SER protection for TCAMs.
 *
 * Such as: SER_RANGE_CONFIGr, SER_RANGE_STARTr, SER_RANGE_ENDr,
 * SER_RANGE_RESULTr, SER_RANGE_ADDR_BITSr, SER_RANGE_DISABLEr and
 * SER_RANGE_x_PROT_WORDr
 * 'x= 0, 1, 2....31'
 * *
 * \param [in] unit                   Unit number.
 * \param [out] range_cfg_list        SER_RANGE_CONFIGr.
 * \param [out] range_start_list      SER_RANGE_STARTr.
 * \param [out] range_end_list        SER_RANGE_ENDr.
 * \param [out] range_result_list     SER_RANGE_RESULTr.
 * \param [out] prot_list             List of SER_RANGE_x_PROT_WORDr.
 * \param [out] range_addr_bits_list  SER_RANGE_ADDR_BITSr
 * \param [out] range_disable_list    SER_RANGE_DISABLEr.
 * \param [out] group_num             Number of item in above lists.
 * \param [out] range_cfg_fid_list    Field list of SER_RANGE_CONFIGr.
 * \param [out] prot_fid_list         Field list of SER_RANGE_x_PROT_WORDr.
 * \param [out] ser_memory_sid        SER_MEMORYm
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_cmic_ser_engine_regs_get(int unit,
                                    bcmdrd_sid_t *range_cfg_list,
                                    bcmdrd_sid_t *range_start_list,
                                    bcmdrd_sid_t *range_end_list,
                                    bcmdrd_sid_t *range_result_list,
                                    bcmdrd_sid_t **prot_list,
                                    bcmdrd_sid_t *range_addr_bits_list,
                                    bcmdrd_sid_t *range_disable_list,
                                    int *group_num,
                                    bcmdrd_fid_t  **range_cfg_fid_list,
                                    bcmdrd_fid_t  **prot_fid_list,
                                    bcmdrd_sid_t  *ser_memory_sid);

/*!
 * \brief Get registers to enanbe or disable CMIC SER engine.
 *
 * Such as: SER_RING_ERR_CTRLr/SER_RANGE_ENABLEr/SER_CONFIG_REGr.
 *
 * \param [in] unit           Unit number.
 * \param [in] ser_range_enable Value of SER_RANGE_ENABLEr.SER_RANGE_ENABLEf
 * \param [out] reg_ctrl_lst  Data list of controlling registers.
 * \param [out] reg_num       Number of item in data list.
 * \param [out] intr_num      Interrupt number of CMIC SER engine.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_cmic_ser_engine_enable(int unit, uint32_t ser_range_enable,
                                  bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                  int *reg_num, int *intr_num);

/*!
 * \brief Get registers which can tell SER logic which TCAM table has SER error.
 *
 * Such as:  SER_ERROR_0r/SER_ERROR_1r/SER_RESULT_0m/SER_RESULT_1m.
 *
 * \param [in] unit            Unit number.
 * \param [out] ser_error_lst  Data list of SER_ERROR_0r and SER_ERROR_1r.
 * \param [out] ser_result_0   Data list of SER_RESULT_0m.
 * \param [out] ser_result_1   Data list of SER_RESULT_1m.
 * \param [out] err_num        Number of item in 'ser_error_lst'.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_cmic_ser_result_get(int unit,
                               bcmptm_ser_ctrl_reg_info_t **ser_error_lst,
                               bcmptm_ser_ctrl_reg_info_t **ser_result_0,
                               bcmptm_ser_ctrl_reg_info_t **ser_result_1,
                               int *err_num);

/*!
 * \brief Get adext according to adext_info.
 *
 * \param [in] unit        Unit number.
 * \param [in] adext_info  Acc_type and block number.
 * \param [out] adext      Magical data from acc_type and block number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_cmic_adext_get(int unit, bcmptm_cmic_adext_info_t adext_info,
                          uint32_t *adext);

/*!
 * \brief Get access type according to sid.
 *
 * \param [in] unit         Unit number.
 * \param [in] sid          PT ID.
 * \param [out] acc_type    ID of access type (per chip).
 * \param [out] acctype_str String name of access type (all chips).
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_pt_acctype_get(int unit, bcmdrd_sid_t sid, uint32_t *acc_type,
                          const char **acctype_str);

/*!
 * \brief Map a non-existing instance to a existing instance.
 *
 * \param [in] unit          Unit number.
 * \param [in] sid           PT ID.
 * \param [in/out] tbl_inst  Table instance number.
 * \param [in/out] tbl_copy  Table copy number.
 *
 * \return FALSE on instance doesn't exist, True exist.
 */
extern bool
bcmptm_ser_pt_inst_remap(int unit, bcmdrd_sid_t sid,
                         int *tbl_inst, int *tbl_copy);

/*!
 * \brief Get schan operation_code for DMA access.
 *
 * \param [in] unit         Unit number.
 * \param [in] cmd          Command defined by SER, e.g: BCMPTM_SER_READ_MEMORY_CMD_MSG.
 * \param [in] dst_blk      Destination block number.
 * \param [in] blk_types    Block types.
 * \param [in] tbl_inst     Table instance ID.
 * \param [in] tbl_copy     Table copy ID, like parameter ovrr_info of bcmbd_pt_write.
 * \param [in] data_len     Length of data.
 * \param [in] dma
 * \param [in] ignore_mask
 * \param [out] opcode      DMA opcode.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_cmic_schan_opcode_get(int unit, int cmd, int dst_blk,
                                 int blk_types, int tbl_inst, int tbl_copy,
                                 int data_len, int dma, uint32_t ignore_mask,
                                 uint32_t *opcode);

/*!
 * \brief Get HW memory base information of an internal bus or buffer.
 *
 * \param [in] unit         Unit number.
 * \param [in] membase      Membase of buffer or bus.
 * \param [in] stage_id     Stage id of buffer or bus.
 * \param [out] bd_bb_nm    Name of internal bus or buffer defined by BD.
 * \param [out] base_info   String of base information  e.g: "OBM_QUEUE_FIFO - In Idb" etc.
 * \param [out] mem         1: internal memory, 0: internal bus.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_hwmem_base_info_get(int unit,
                               int membase, int stage_id,
                               char **bd_bb_nm, char **base_info, bool *mem);

/*!
 * \brief Remap SER raw information reported by SER_FIFO of IP/EP.
 *
 * For some PTs, their SER information is reported by mistake,
 * this routine will correct the wrong SER information.
 *
 * \param [in] unit          Unit number.
 * \param [in] address       Address of PT.
 * \param [in] blk_type      Block type.
 * \param [in] membase    Mem HW base.
 * \param [in] instruction   Instruction ID.
 * \param [out] add_mapped   New remapped address.
 * \param [out] non_sbus   0: PT tables, 1: internal buffers or buses.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_ipep_hw_info_remap(int unit, uint32_t address, int blk_type,
                              uint32_t membase, uint32_t instruction,
                              uint32_t *address_mapped, int *non_sbus);

/*!
 * \brief Remap index or SID and address of MMU memory tables.
 *
 * Due to H/W fault, MMU reports wrong SID or address,
 * this routine is used to correct wrong information.
 *
 * \param [in] unit           Unit number.
 * \param [in/out] spci       Old/new SID, address, and other information.
 * \param [in] adext          Adext.
 * \param [in] blk_inst       Block instance.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_mmu_mem_remap(int unit, bcmptm_ser_correct_info_t *spci,
                         uint32_t adext, int blk_inst);

/*!
 * \brief  Remap table A of IP/EP memory tables which has no PTcache to
 * to B which has PTcache. A and B share space of the same physical table.
 *
 * \param [in] unit        Unit number.
 * \param [in] spci        SID without PTcache.
 * \param [out] remap_sid  SID with PTcache.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_ipep_mem_remap(int unit, bcmdrd_sid_t sid,
                          bcmdrd_sid_t *remap_sid);

/*!
 * \brief  Get test field ID to inject SER error for DATA-SPLIT memory table.
 *
 * Some FID only exists in one instance for DATA-SPLIT memory table.
 *
 * \param [in] unit          Unit number.
 * \param [out] sid          Data-split memory table id.
 * \param [out] acc_type     Pipe number.
 * \param [out] test_fid     Field id existed only in one pipe which id is equal to 'acc_type'.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_data_split_mem_test_fid_get(int unit, bcmdrd_sid_t sid,
                                       int acc_type, bcmdrd_fid_t *test_fid);

/*!
 * \brief  Remap functional SID to ECC view SID, and remap functional index to ECC view index.
 *
 * \param [in] unit             Unit number.
 * \param [in] sid_fv           Functional PT ID.
 * \param [in] index_fv         Functional PT index.
 * \param [out] sid             ECC view PT ID, can be NULL
 * \param [out] index           ECC view PT index, can be NULL
 * \param [out] ecc_checking    1: Functional PT has ECC view, cannot be NULL
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_mem_ecc_info_get(int unit, bcmdrd_sid_t sid_fv, int index_fv,
                            bcmdrd_sid_t *sid, int *index, int *ecc_checking);

/*!
 * \brief Map TCAM-ONLY or AGGR view to the view protected by TCAM SER engine.
 *
 * \param [in] unit                     Unit number.
 * \param [in] tcam_sid                 PT ID.
 * \param [out] remap_sid               Functional or TCAM-ONLY PT ID which
 *                                      is protected by SER engine or H/W engine.
 * \param [out] inject_err_to_key_fld   1: Inject SER error to TCAM fields.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_tcam_remap(int unit, bcmdrd_sid_t tcam_sid,
                      bcmdrd_sid_t *remap_sid,
                      int *inject_err_to_key_fld);

/*!
 * \brief Get field SER_STICKY_BIT_CLEAR. For all TCAMs if DROP_EN=1,
 * then SW has to clear it using STICKY_BIT_CLEAR(0->1, 1->0),
 * else all traffic hitting that TCAM will be dropped.
 *
 * \param [in] unit           Unit number.
 * \param [in] tcam_sid       PT ID.
 * \param [out] ctrl_sid      Ctrl register SID.
 * \param [out] ctrl_fid      FID of SER_STICKY_BIT_CLEAR.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_tcam_sticky_bit_clr_get(int unit, bcmdrd_sid_t tcam_sid,
                                   bcmdrd_sid_t *ctrl_sid,
                                   bcmdrd_fid_t *ctrl_fid);

/*!
* \brief Get refresh register information.
*
* Such as: REFRESH_ENf of MMU_GCFG_MISCCONFIGr.
* During SER injection and test, H/W refresh should be disabled for a while.
* After then, re-eanble H/W refresh.
*
* \param [in] unit                 Unit number.
* \param [out] mem_refresh_info    Data list of refresh registers.
* \param [out] info_num            Number of item of data list.
*
* \return SHR_E_NONE on success, error code otherwise.
*/
extern int
bcmptm_ser_refresh_regs_info_get(int unit,
                                 bcmptm_ser_ctrl_reg_info_t **mem_refresh_info,
                                 uint32_t *info_num);

/*!
* \brief Get SER correction SID of ECC status register.
*
* Sometimes, SER logical just knows ECC status register, but don't know which SID
* has SER error. This routine can get such SID according to ECC status register.
* For example, if SER event is reported by IDB_IS_TDM_CAL_ECC_STATUSr,
* SER logic needs to correct IS_TDM_CALENDAR1m or IS_TDM_CALENDAR0m.
*
* \param [in] unit             Unit number.
* \param [in] spci             ECC status register SID, index etc. used to correct SER error.
* \param [out] corrected_sids  SIDs used to correct SER event.
* \param [out] index           Index owning SER error.
* \param [out] sid_num         Number of SIDs.
*
* \return SHR_E_NONE on success, error code otherwise.
*/
extern int
bcmptm_ser_status_reg_map(int unit,
                          bcmptm_ser_correct_info_t *spci,
                          const bcmdrd_sid_t **corrected_sids,
                          uint32_t *index,
                          uint32_t *sid_num);

/*!
* \brief Get control registers for XOR PTs.
*
* For some XOR PTs, there are control registers which can be used to
* disable PRE_MEMWR parity check, disable data generation of XOR bank, etc.
*
* \param [in] unit                 Unit number.
* \param [in] sid                  XOR PT ID.
* \param [in] index                Index of PT.
* \param [out] sram_ctrl_reg_info  Controlling registers' information.
*
* \return SHR_E_NONE on success, error code otherwise.
*/
extern int
bcmptm_ser_sram_ctrl_reg_get(int unit, bcmdrd_sid_t sid, uint32_t index,
                             bcmptm_ser_control_reg_info_t *sram_ctrl_reg_info);

/*!
* \brief Get SER correction data for XOR PTs.
*
* For some XOR PTs, SER logic needs to correct SER error by
* writing corresponding indexes on every RAM.
* If LP memory tables have SER error, SER logic needs to correct SER by HW mode,
* overlay SER error data on one pipe by right data on another pipe.
*
* \param [in] unit       Unit number.
* \param [in] sid        XOR or LP PT ID.
* \param [in] index      Index of PT.
* \param [out] sram_info Information including SIDs, indexes of functional view.
*
* \return SHR_E_NONE on success, error code otherwise.
*/
extern int
bcmptm_ser_xor_lp_mem_info_get(int unit, bcmdrd_sid_t sid, uint32_t index,
                               bcmptm_ser_sram_info_t *sram_info);

/*!
* \brief Some tables do not need to be scanned.
*
* \param [in] unit     Unit number.
* \param [in] sid      PT ID
* \param [out] skip    1: 'sid' does not need to be scanned.
*
* \return SHR_E_NONE on success, error code otherwise.
*/
extern int
bcmptm_ser_sram_scan_mem_skip(int unit, bcmdrd_sid_t sid, bool *skip);

/*!
* \brief Enable copy to cpu.
*
* \param [in] unit     Unit number.
* \param [in] sid      PT ID
* \param [in] enable  'sid' does not need to be scanned.
*
* \return SHR_E_NONE on success, error code otherwise.
*/
extern int
bcmptm_ser_copy_to_cpu_regs_info_get(int unit,
                                     bcmptm_ser_ctrl_reg_info_t **reg_ctrl_lst,
                                     int *reg_num);

/*!
* \brief Enable copy to cpu.
*
* \param [in] unit     Unit number.
* \param [in] sid      PT ID
* \param [in] fid      field ID.
* \param [out] value  expected field value.
*
* \return SHR_E_NONE on success, error code otherwise.
*/
extern int
bcmptm_ser_expected_value_get(int unit, bcmdrd_sid_t sid,
                              bcmdrd_fid_t fid, uint32_t *value);

/*!
* \brief Get force error value.
*
* \param [in] unit     Unit number.
* \param [in] sid      PT ID
* \param [in] fid      field ID.
* \param [out] value  expected field value.
*
* \return SHR_E_NONE on success, error code otherwise.
*/
extern int
bcmptm_ser_force_error_value_get(int unit, bcmdrd_sid_t sid,
                                 bcmdrd_fid_t fid, uint32_t *value);

/*!
* \brief Get tcams ser control PTs.
*
* \param [in] unit           Unit number.
* \param [out] mem_list      TCAM control PT list
* \param [out] entry_num     Number of TCAM controls
* \param [out] fid           Config field
*
* \return SHR_E_NONE on success, error code otherwise.
*/
extern int
bcmptm_ser_tcam_ctrl_reg_list_get(int unit, bcmdrd_sid_t **mem_list,
                                  int *entry_num, bcmdrd_fid_t *fid);

/*!
* \brief SER chip specific routines are populated.
*
* \param [in] unit     Unit number.
*
* \return TRUE: populated, FALSE: not populated.
*/
extern bool
bcmptm_ser_drv_populated(int unit);

/*!
* \brief Some fields should not be used to inject error.
*
* \param [in] unit     Unit number.
* \param [in] sid      PT ID
* \param [in] fid      PT Field ID
* \param [out] skip    1: 'sid, fid' should be skipped
* \brief Get force error value.
*
* \param [in] unit     Unit number.
* \param [in] sid      PT ID
* \param [in] fid      field ID.
* \param [out] value  expected field value.
*
* \return SHR_E_NONE on success, error code otherwise.
*/
extern int
bcmptm_ser_test_fid_skip(int unit, bcmdrd_sid_t sid,
                         bcmdrd_fid_t fid, bool *skip);

/*!
* \brief Get the number of bits interleaved.
*        Certain memories are bit interleaved across
*        different banks. In such cases, a 2 bit error
*        can only be triggered by writing
*        to the same bank.
*
* \param [in] unit     Unit number.
* \param [in] sid      Memory ID.
* \param [out] num_interleave      Number of bits iterleaved.
*
* \return TRUE: populated, FALSE: not populated.
*/
extern int
bcmptm_ser_test_num_interleave_get(int unit, bcmdrd_sid_t sid,
                         bcmdrd_fid_t fid, uint32_t *num_interleave);

/*!
* \brief Parse the SER STATUS field based on chip information.
*
* \param [in] unit     Unit number.
* \param [in] sid      PT ID
* \param [in] num_fid  Number of fields
* \param [in/out] fval: Field values
*
* \return SHR_E_NONE on success, error code otherwise.
* \return SHR_E_PARAM on success, error code otherwise.
*/
extern int
bcmptm_ser_status_field_parse(int unit, bcmdrd_sid_t sid,
                              int fid_count, int *fval);

#endif /* BCMPTM_SER_CHIP_INTERNAL_H */
