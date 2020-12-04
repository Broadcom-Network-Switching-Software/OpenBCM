/*! \file bcmcth_tnl_internal.h
 *
 * Internal Tunnel handler function header file
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TNL_INTERNAL_H
#define BCMCTH_TNL_INTERNAL_H

/*******************************************************************************
  Includes
 */
#include <shr/shr_bitop.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmimm/bcmimm_int_comp.h>

/*! Maximum number of MPLS labels that can be added. */
#define BCMCTH_TNL_MPLS_MAX_LABELS  (8)

/*! Structure to maintain SW state for MPLS tunnel entries. */
typedef struct tnl_mpls_encap_entry_s {
    /*! SW encap ID. */
    uint32_t    sw_encap_id;
    /*! HW index. */
    uint32_t    encap_id;
    /*! Maximum number of labels that may be added in this entry. */
    uint32_t    max_labels;
    /*! Number of labels currently added in this entry. */
    uint32_t    num_labels;
    /*! MPLS labels. */
    uint32_t    label[8];
    /*! MPLS label TTL. */
    uint8_t     ttl[8];
    /*! MPLS label exp mode. */
    uint8_t     exp_mode[8];
    /*! MPLS label PHB ID. */
    uint8_t     phb_id[8];
    /*! MPLS label exp value. */
    uint8_t     exp[8];
    /*! MPLS label new PRI. */
    uint8_t     pri[8];
    /*! MPLS label new CFI. */
    uint8_t     cfi[8];
    /*! MPLS label CNG to EXP ptr. */
    uint8_t     cng_to_exp[8];
    /*! MPLS label CNG to EXP ptr priority. */
    uint8_t     cng_to_exp_pri[8];
    /*! MPLS label IP-ECN to EXP ptr. */
    uint8_t     ecn_to_exp[8];
    /*! MPLS label IP-ECN to EXP ptr priority. */
    uint8_t     ecn_to_exp_pri[8];
} tnl_mpls_encap_entry_t;

/*! IPV4 tunnel HW entry populate */
typedef int (*tnl_ipv4_encap_lt_to_hw_entry_map_f)(int unit,
                                                          uint32_t hw_sid,
                                                          const bcmltd_field_t *data,
                                                          uint8_t *entry_buf);

/*! IPV6 tunnel HW entry populate */
typedef int (*tnl_ipv6_encap_lt_to_hw_entry_map_f)(int unit,
                                                          uint32_t hw_sid,
                                                          const bcmltd_field_t *data,
                                                          uint8_t *entry_buf);

/*! MPLS tunnel HW entry populate */
typedef int (*tnl_mpls_encap_lt_to_hw_entry_map_f)(int unit,
                                                          uint32_t hw_sid,
                                                          uint32_t sub_id,
                                                          tnl_mpls_encap_entry_t *entry,
                                                          uint8_t *entry_buf);
/*! MPLS tunnel HW entry clear */
typedef int (*tnl_mpls_encap_hw_entry_clear_f)(int unit,
                                                      uint32_t hw_sid,
                                                      uint32_t sub_id,
                                                      int max_labels,
                                                      bool remove,
                                                      uint8_t *entry_buf);

/*! IPV4 tunnel HW entry populate */
typedef int (*tnl_ipv4_encap_hw_to_lt_entry_map_f)(int unit,
                                                      uint32_t hw_sid,
                                                      uint8_t *entry_buf,
                                                      bcmltd_fields_t *data);

/*! IPV6 tunnel HW entry populate */
typedef int (*tnl_ipv6_encap_hw_to_lt_entry_map_f)(int unit,
                                                      uint32_t hw_sid,
                                                      uint8_t *entry_buf,
                                                      bcmltd_fields_t *data);

/*! MPLS tunnel HW entry populate */
typedef int (*tnl_mpls_encap_hw_to_lt_entry_map_f)(int unit,
                                                      uint32_t hw_sid,
                                                      uint32_t sub_id,
                                                      tnl_mpls_encap_entry_t *entry,
                                                      uint8_t *entry_buf,
                                                      bcmltd_fields_t *data);

/*!
 * Tunnel encap driver structure.
 */
typedef struct bcmcth_tnl_encap_drv_s {
    /*! Number of ipv4 tunnels supported. */
    uint32_t    num_ipv4_tnl;
    /*! Number of ipv6 tunnels supported. */
    uint32_t    num_ipv6_tnl;
    /*! Number of mpls tunnels supported. */
    uint32_t    num_mpls_tnl;
    /*! Maximum number of mpls labels per tunnel entry. */
    uint32_t    max_labels;
    /*! Width of one IPV4 tunnel entry relative to the base tunnel(IPV4). */
    uint8_t     ipv4_tnl_width;
    /*! Width of one IPV6 tunnel entry relative to the base tunnel(IPV4). */
    uint8_t     ipv6_tnl_width;
    /*! Width of one MPLS tunnel entry relative to the base tunnel(IPV4). */
    uint8_t     mpls_tnl_width;
    /*! First hardware index valid for IPv4. */
    uint8_t     ipv4_start_idx;
    /*! First hardware index valid for IPv6. */
    uint8_t     ipv6_start_idx;
    /*! First hardware index valid for MPLS. */
    uint8_t     mpls_start_idx;
    /*! Maximum number of key fields in the TNL LT. */
    uint16_t    num_key_flds;
    /*! Maximum number of data fields in the TNL LT. */
    uint16_t    num_data_flds;
    /*! Hardware table that is mapped to TNL_IPV4_ENCAP LT. */
    uint32_t    ipv4_hw_sid;
    /*! Hardware table that is mapped to TNL_IPV6_ENCAP LT. */
    uint32_t    ipv6_hw_sid;
    /*! Hardware table that is mapped to TNL_MPLS_ENCAP LT. */
    uint32_t    mpls_hw_sid;
    /*! Function to convert LT entry to HW entry. */
    tnl_ipv4_encap_lt_to_hw_entry_map_f ipv4_lt_to_hw_map;
    /*! Function to convert LT entry to HW entry. */
    tnl_ipv6_encap_lt_to_hw_entry_map_f ipv6_lt_to_hw_map;
    /*! Function to convert LT entry to HW entry. */
    tnl_mpls_encap_lt_to_hw_entry_map_f mpls_lt_to_hw_map;
    /*! Function to clear MPLS HW entry. */
    tnl_mpls_encap_hw_entry_clear_f     mpls_hw_entry_clear;
    /*! Function to convert HW entry to LT entry. */
    tnl_ipv4_encap_hw_to_lt_entry_map_f ipv4_hw_to_lt_map;
    /*! Function to convert HW entry to LT entry. */
    tnl_ipv6_encap_hw_to_lt_entry_map_f ipv6_hw_to_lt_map;
    /*! Function to convert HW entry to LT entry. */
    tnl_mpls_encap_hw_to_lt_entry_map_f mpls_hw_to_lt_map;
} bcmcth_tnl_encap_drv_t;

/*! No tunnel present. */
#define TNL_TYPE_NONE                       (0)
/*! IPV4 TUNNEL. */
#define TNL_TYPE_IPV4                       (1)
/*! IPV6 TUNNEL. */
#define TNL_TYPE_IPV6                       (2)
/*! MPLS TUNNEL. */
#define TNL_TYPE_MPLS                       (3)

/*! Dont push the label. */
#define BCMCTH_TNL_MPLS_PUSH_NONE               (0)
/*! Push this label. */
#define BCMCTH_TNL_MPLS_PUSH_ONE                (1)
/*! Push this label and the next label. */
#define BCMCTH_TNL_MPLS_PUSH_TWO                (2)

/*! Tunnel encap driver for this unit. */
#define BCMCTH_TNL_ENCAP_DRV(unit)              tnl_encap_drv[unit]

/*! Number of ipv4 tunnels supported for this unit. */
#define BCMCTH_TNL_NUM_IPV4(unit)               BCMCTH_TNL_ENCAP_DRV(unit)->num_ipv4_tnl
/*! Number of ipv6 tunnels supported for this unit. */
#define BCMCTH_TNL_NUM_IPV6(unit)               BCMCTH_TNL_ENCAP_DRV(unit)->num_ipv6_tnl
/*! Number of mpls tunnels supported for this unit. */
#define BCMCTH_TNL_NUM_MPLS(unit)               BCMCTH_TNL_ENCAP_DRV(unit)->num_mpls_tnl
/*! Maximum number of mpls labels per tunnel entry for this unit. */
#define BCMCTH_TNL_MAX_MPLS_LABELS(unit)        BCMCTH_TNL_ENCAP_DRV(unit)->max_labels
/*! Width of one IPV4 tunnel entry relative to the base tunnel(IPV4) for this unit. */
#define BCMCTH_TNL_IPV4_WIDTH(unit)             BCMCTH_TNL_ENCAP_DRV(unit)->ipv4_tnl_width
/*! Width of one IPV6 tunnel entry relative to the base tunnel(IPV4) for this unit. */
#define BCMCTH_TNL_IPV6_WIDTH(unit)             BCMCTH_TNL_ENCAP_DRV(unit)->ipv6_tnl_width
/*! Width of one MPLS tunnel entry relative to the base tunnel(IPV4) for this unit. */
#define BCMCTH_TNL_MPLS_WIDTH(unit)             BCMCTH_TNL_ENCAP_DRV(unit)->mpls_tnl_width
/*! First hardware index valid for IPv4 for this unit. */
#define BCMCTH_TNL_IPV4_START_IDX(unit)         BCMCTH_TNL_ENCAP_DRV(unit)->ipv4_start_idx
/*! First hardware index valid for IPv6 for this unit. */
#define BCMCTH_TNL_IPV6_START_IDX(unit)         BCMCTH_TNL_ENCAP_DRV(unit)->ipv6_start_idx
/*! First hardware index valid for MPLS for this unit. */
#define BCMCTH_TNL_MPLS_START_IDX(unit)         BCMCTH_TNL_ENCAP_DRV(unit)->mpls_start_idx
/*! Maximum number of key fields in the TNL LT. */
#define BCMCTH_TNL_NUM_KEY_FIELDS(unit)         BCMCTH_TNL_ENCAP_DRV(unit)->num_key_flds
/*! Maximum number of data fields in the TNL LT. */
#define BCMCTH_TNL_NUM_DATA_FIELDS(unit)        BCMCTH_TNL_ENCAP_DRV(unit)->num_data_flds
/*! Hardware table that is mapped to TNL_IPV4_ENCAP LT for this unit. */
#define BCMCTH_TNL_IPV4_HW_SID(unit)            BCMCTH_TNL_ENCAP_DRV(unit)->ipv4_hw_sid
/*! Hardware table that is mapped to TNL_IPV6_ENCAP LT for this unit. */
#define BCMCTH_TNL_IPV6_HW_SID(unit)            BCMCTH_TNL_ENCAP_DRV(unit)->ipv6_hw_sid
/*! Hardware table that is mapped to TNL_MPLS_ENCAP LT for this unit. */
#define BCMCTH_TNL_MPLS_HW_SID(unit)            BCMCTH_TNL_ENCAP_DRV(unit)->mpls_hw_sid
/*! Convert IPV4 HW entry to LT entry. */
#define BCMCTH_TNL_IPV4_HW_TO_LT_ENTRY(unit)    (BCMCTH_TNL_ENCAP_DRV(unit)->ipv4_hw_to_lt_map)
/*! Convert IPV6 HW entry to LT entry. */
#define BCMCTH_TNL_IPV6_HW_TO_LT_ENTRY(unit)    (BCMCTH_TNL_ENCAP_DRV(unit)->ipv6_hw_to_lt_map)
/*! Convert MPLS HW entry to LT entry. */
#define BCMCTH_TNL_MPLS_HW_TO_LT_ENTRY(unit)    (BCMCTH_TNL_ENCAP_DRV(unit)->mpls_hw_to_lt_map)
/*! Convert IPV4 LT entry to HW entry. */
#define BCMCTH_TNL_IPV4_LT_TO_HW_ENTRY(unit)    (BCMCTH_TNL_ENCAP_DRV(unit)->ipv4_lt_to_hw_map)
/*! Convert IPV6 LT entry to HW entry. */
#define BCMCTH_TNL_IPV6_LT_TO_HW_ENTRY(unit)    (BCMCTH_TNL_ENCAP_DRV(unit)->ipv6_lt_to_hw_map)
/*! Convert MPLS LT entry to HW entry. */
#define BCMCTH_TNL_MPLS_LT_TO_HW_ENTRY(unit)    (BCMCTH_TNL_ENCAP_DRV(unit)->mpls_lt_to_hw_map)
/*! Clear MPLS HW entry. */
#define BCMCTH_TNL_MPLS_HW_ENTRY_CLEAR(unit)    (BCMCTH_TNL_ENCAP_DRV(unit)->mpls_hw_entry_clear)

/*! Get the minimum value of an LT field. */
#define BCMCTH_TNL_FIELD_MIN(fdef)   ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U8) ?      \
                                      (fdef.min.u8) :                               \
                                      ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U16) ?    \
                                       (fdef.min.u16) :                             \
                                       ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U32) ?   \
                                        (fdef.min.u32) :                            \
                                        ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U64) ?  \
                                         (fdef.min.u64) :                           \
                                         (fdef.min.is_true)))))


/*! Get the default value of an LT field. */
#define BCMCTH_TNL_FIELD_DEF(fdef)   ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U8) ?      \
                                      (fdef.def.u8) :                               \
                                      ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U16) ?    \
                                       (fdef.def.u16) :                             \
                                       ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U32) ?   \
                                        (fdef.def.u32) :                            \
                                        ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U64) ?  \
                                         (fdef.def.u64) :                           \
                                         (fdef.def.is_true)))))

/*! Get the maximum value of an LT field. */
#define BCMCTH_TNL_FIELD_MAX(fdef)   ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U8) ?      \
                                      (fdef.max.u8) :                               \
                                      ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U16) ?    \
                                       (fdef.max.u16) :                             \
                                       ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U32) ?   \
                                        (fdef.max.u32) :                            \
                                        ((fdef.dtag == BCMLT_FIELD_DATA_TAG_U64) ?  \
                                         (fdef.max.u64) :                           \
                                         (fdef.max.is_true)))))

/*! Convert uint32 array into one uint64 variable. */
#define BCMCTH_TNL_ENCAP_UINT32_TO_UINT64(val32, val64) \
    do {                                                \
        val64 = val32[1];                               \
        val64 = ((val64 << 32) | val32[0]);             \
    } while(0)

/*! Convert uint64 variable into uint32 array. */
#define BCMCTH_TNL_ENCAP_UINT64_TO_UINT32(val64, val32) \
    do {                                                \
        val32[0] = (uint32_t)val64;                     \
        val32[1] = (uint32_t)(val64 >> 32);             \
    } while(0)

/*! SW state for each index entry in tunnel table */
typedef struct bcmcth_tnl_encap_sw_state_s {
    /*! IPv4/v6 tunnel bitmap*/
    SHR_BITDCL *bcmcth_tnl_base_tnl_bitmap;
    /*! MPLS tunnel bitmap */
    SHR_BITDCL *bcmcth_tnl_mpls_tnl_bitmap;
    /*! Tunnel type. */
    uint8_t    *bcmcth_tnl_base_tnl_type;
    /*! Tunnel hardware index. */
    uint16_t   *bcmcth_tnl_hw_idx;
} bcmcth_tnl_encap_sw_state_t;

/*! Tunnel encap software state. */
#define BCMCTH_TNL_ENCAP_SW_STATE(unit)     bcmcth_tnl_encap_sw_state[unit]
/*! Tunnel encap committed software state. */
#define BCMCTH_TNL_ENCAP_SW_STATE_HA(unit)  bcmcth_tnl_encap_sw_state_ha[unit]

/*! Tunnel sub component ID for base tunnel bitmap s/w state. */
#define BCMCTH_TNL_BASE_TNL_BMAP_SUB_COMP_ID    1
/*! Tunnel sub component ID for mpls tunnel bitmaps/w state. */
#define BCMCTH_TNL_MPLS_TNL_BMAP_SUB_COMP_ID    2
/*! Tunnel sub component ID for base tunnel type s/w state. */
#define BCMCTH_TNL_BASE_TNL_TYPE_SUB_COMP_ID    3
/*! Tunnel sub component ID for mpls tunnel entry s/w state. */
#define BCMCTH_TNL_MPLS_ENTRY_STATE_SUB_COMP_ID 4
/*! Tunnel sub component ID for L3 interface s/w state. */
#define BCMCTH_TNL_L3_INTF_SUB_COMP_ID          5
/*! Tunnel sub component ID for tunnel h/w index. */
#define BCMCTH_TNL_HW_IDX_SUB_COMP_ID           6

/*!
 * \brief Insert a new entry in the Tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table ID.
 * \param [in]  op_arg          The operation arguments.
 * \param [in]  key             Input key fields list.
 * \param [in]  data            Input data fields list.
 * \param [out] out             Output parameter list.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmcth_tnl_encap_entry_insert(int unit,
                              bcmltd_sid_t sid,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data,
                              bcmltd_fields_t *out);

/*!
 * \brief Update an entry in the Tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table ID.
 * \param [in]  op_arg          The operation arguments.
 * \param [in]  key             Input key fields list.
 * \param [in]  data            Input data fields list.
 * \param [out] out             Output parameter list.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmcth_tnl_encap_entry_update(int unit,
                              bcmltd_sid_t sid,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data,
                              bcmltd_fields_t *out);

/*!
 * \brief Delete an entry from the Tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table ID.
 * \param [in]  op_arg          The operation arguments.
 * \param [in]  key             Input key fields list.
 * \param [in]  data            Input data fields list.
 * \param [out] out             Output parameter list.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmcth_tnl_encap_entry_delete(int unit,
                              bcmltd_sid_t sid,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data,
                              bcmltd_fields_t *out);

/*!
 * \brief Lookup an entry from the Tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table ID.
 * \param [in]  op_arg          The operation arguments.
 * \param [in]  context         Pointer to retrieve some context.
 * \param [in]  lkup_type       Lookup type.
 * \param [in]  in              Input key fields list.
 * \param [in/out] out          Output parameter list.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmcth_tnl_encap_entry_lookup(int unit,
                              bcmltd_sid_t sid,
                              const bcmltd_op_arg_t *op_arg,
                              void *context,
                              bcmimm_lookup_type_t lkup_type,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out);


/*!
 * \brief Initialise Tunnel SW structures.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  cold            Cold/Warm boot.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

extern int
bcmcth_tnl_sw_state_init(int unit, bool cold);

/*!
 * \brief Free allocated Tunnel SW structures on exit.
 *
 * \param [in]  unit            Unit number.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */

extern int
bcmcth_tnl_sw_state_cleanup(int unit);

/*!
 * \brief bcmcth_tnl_transaction_commit
 *
 * \brief Commit SW states for the tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  lt_sid          Logical table ID.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  context         Context pointer.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmcth_tnl_transaction_commit(int unit,
                              bcmltd_sid_t lt_sid,
                              uint32_t trans_id,
                              void *context);
/*!
 * \brief bcmcth_tnl_transaction_abort
 *
 * \brief Abort SW states for the tunnel Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  lt_sid          Logical table ID.
 * \param [in]  trans_id        Transaction ID.
 * \param [in]  context         Context pointer.
 *
 * \retval SHR_E_NONE  OK
 */
extern void
bcmcth_tnl_transaction_abort(int unit,
                             bcmltd_sid_t lt_sid,
                             uint32_t trans_id,
                             void *context);

#endif /* BCMCTH_TNL_INTERNAL_H */
