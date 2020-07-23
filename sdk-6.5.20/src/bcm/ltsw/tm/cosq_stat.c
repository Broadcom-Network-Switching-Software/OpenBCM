/* \file cosq_stat.c
 *
 * TM COSQ STAT Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/control.h>

#include <bcm_int/ltsw/mbcm/cosq_stat.h>
#include <bcm_int/ltsw/tm/cosq_stat.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/cosq_thd.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COSQ

#define LTSW_COMPILER_64_AND(dst, src)    ((dst) &= (src))
#define LTSW_COMPILER_64_ADD_64(dst, src)    ((dst) += (src))
#define LTSW_COMPILER_64_SUB_64(dst, src)    ((dst) -= (src))
#define LTSW_COMPILER_64_ZERO(dst)           ((dst) = 0)
#define LTSW_COMPILER_64_COPY(dst, src)      (dst = src)
#define LTSW_COMPILER_64_SHR(dst, bits)    ((dst) >>= (bits))
#define LTSW_COMPILER_64_SET(dst, src_hi, src_lo)                \
    ((dst) = (((uint64_t) ((uint32_t)(src_hi))) << 32) | ((uint64_t) ((uint32_t)(src_lo))))

#define NUM_RQE_QUEUES                  9
#define MMU_NUM_PG                      8
#define OBM_DROP                        (1 << 0)
#define OBM_FLOW_CTRL                   (1 << 1)
#define OBM_USAGE                       (1 << 2)
#define UC_QUEUE                        (1 << 3)
#define MC_QUEUE                        (1 << 4)

typedef struct cosq_stat_obm_map_s
{
    bcm_cosq_stat_t type;
    const char *lt_val;
    bcmi_lt_field_t field;
} cosq_stat_obm_map_t;

typedef struct cosq_stat_adt_enum_convert_s {
    bcm_cosq_stat_t                               stat;
    bcm_cosq_control_drop_limit_alpha_value_t     alpha;
} cosq_stat_adt_enum_convert_t;

/******************************************************************************
 * Private functions
 */

/*
 * \brief Check port type.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lport         PORT ID.
 *
 * \retval TRUE               Port is cpu port.
 * \retval FALSE              Port is non cpu port.
 */
static int
is_cpu_port(int unit, bcm_port_t lport)
{
    return bcmi_ltsw_port_is_type(unit, lport,  BCMI_LTSW_PORT_TYPE_CPU);
}

/*
 * \brief Get port num queue.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lport         PORT ID.
 *
 * \retval port num queue.
 */
static int
port_num_queue(int unit, bcm_port_t lport)
{
    bcmi_ltsw_cosq_device_info_t device_info = {0};

    (void)bcmi_ltsw_cosq_device_info_get(unit, &device_info);

    return is_cpu_port(unit, lport) ? device_info.num_cpu_queue :
                                      device_info.num_queue;
}

/*
 * \brief Convert cosq stat type to lt
 *
 * \param [in]  type         bcm_cosq_stat_t.
 * \param [in]  array        cosq_control_map_t.
 * \param [in]  nfields      nfields.
 *
 * \retval array              array.
 */
static cosq_stat_obm_map_t*
cosq_stat_type_to_field(bcm_cosq_stat_t stat, cosq_stat_obm_map_t *array, int nfields)
{
    int i;
    bool found = false;

    for (i = (nfields - 1); i >= 0; i--) {
      if (stat == array[i].type) {
          found = true;
          break;
      }
    }

    return found ? &array[i] : NULL;
}

/*
 * \brief Convert obm to lt field.
 */
static cosq_stat_obm_map_t cosq_stat_obm_to_fields[] =
{
    {bcmCosqStatOBMLossless0DroppedPackets,    CTR_TM_OBM_PORT_DROPs, {LOSSLESS0_PKTs}},
    {bcmCosqStatOBMLossless0DroppedBytes,      CTR_TM_OBM_PORT_DROPs, {LOSSLESS0_BYTEs}},
    {bcmCosqStatOBMLossless1DroppedPackets,    CTR_TM_OBM_PORT_DROPs, {LOSSLESS1_PKTs}},
    {bcmCosqStatOBMLossless1DroppedBytes,      CTR_TM_OBM_PORT_DROPs, {LOSSLESS1_BYTEs}},
    {bcmCosqStatHighPriDroppedPackets,         CTR_TM_OBM_PORT_DROPs, {LOSSY_HIGH_PKTs}},
    {bcmCosqStatOBMLossyHighPriDroppedPackets, CTR_TM_OBM_PORT_DROPs, {LOSSY_HIGH_PKTs}},
    {bcmCosqStatHighPriDroppedBytes,           CTR_TM_OBM_PORT_DROPs, {LOSSY_HIGH_BYTEs}},
    {bcmCosqStatOBMLossyHighPriDroppedBytes,   CTR_TM_OBM_PORT_DROPs, {LOSSY_HIGH_BYTEs}},
    {bcmCosqStatLowPriDroppedPackets,          CTR_TM_OBM_PORT_DROPs, {LOSSY_LOW_PKTs}},
    {bcmCosqStatOBMLossyLowPriDroppedPackets,  CTR_TM_OBM_PORT_DROPs, {LOSSY_LOW_PKTs}},
    {bcmCosqStatLowPriDroppedBytes,            CTR_TM_OBM_PORT_DROPs, {LOSSY_LOW_BYTEs}},
    {bcmCosqStatOBMLossyLowPriDroppedBytes,    CTR_TM_OBM_PORT_DROPs, {LOSSY_LOW_BYTEs}},
    {bcmCosqStatOBMFlowControlEvents,          CTR_TM_OBM_PORT_FLOW_CTRLs, {FLOW_CTRL_EVENTSs}},
    {bcmCosqStatOBMBufferBytes,                CTR_TM_OBM_PORT_USAGEs,{BYTEs}},
    {bcmCosqStatOBMLossless0BufferBytes,       CTR_TM_OBM_PORT_USAGEs,{LOSSLESS0_BYTEs}},
    {bcmCosqStatOBMLossless1BufferBytes,       CTR_TM_OBM_PORT_USAGEs,{LOSSLESS1_BYTEs}},
    {bcmCosqStatOBMLossyBufferBytes,           CTR_TM_OBM_PORT_USAGEs,{LOSSY_BYTEs}},
    {bcmCosqStatOBMHighWatermark,              CTR_TM_OBM_PORT_USAGEs,{MAX_USAGE_BYTEs}},
};

/*
 * \brief Convert obm to map
 *
 * \param [in]  stat     bcm_cosq_stat_t.
 *
 * \retval map           cosq_control_map_t.
 */
static cosq_stat_obm_map_t*
cosq_stat_obm_to_field(bcm_cosq_stat_t stat)
{
    int nfields;

    nfields = sizeof(cosq_stat_obm_to_fields) / sizeof(cosq_stat_obm_to_fields[0]);

    return cosq_stat_type_to_field(stat, cosq_stat_obm_to_fields, nfields);
}

/*
 * \brief Add an entry into LT.
 *
 * This function is used to add an entry into LT.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  lt_val               Logic table name.
 * \param [in]  local_port           Port id.
 * \param [in]  field                bcmi_lt_field_t.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_stat_obm_table_set(int unit, const char *lt_val, int local_port,
                        bcmi_lt_field_t *field)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_val, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_IDs, local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, field->fld_name, field->u.val));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Get an entry from LT.
 *
 * This function is used to get an entry from LT.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  lt_val               Logic table name.
 * \param [in]  local_port           Port id.
 * \param [out] field                bcmi_lt_field_t.
 * \param [in]  entry_attr           entry_attr.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_stat_obm_table_get(int unit, const char *lt_val, int local_port,
                        bcmi_lt_field_t *field, uint32_t entry_attr)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_val, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_IDs, local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_attrib_set(entry_hdl, entry_attr));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, field->fld_name,
                               &(field->u.val)));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
cosq_stat_obm_counter_trans_process(int unit, const char *table_name,
                                    uint32_t flags, bcmlt_opcode_t opcode)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    int port;
    bool valid_trans = false;
    bcm_pbmp_t pbmp, pbmp_mgmt;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_PORT, &pbmp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_MGMT, &pbmp_mgmt));
    BCM_PBMP_REMOVE(pbmp, pbmp_mgmt);

    BCM_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, PORT_IDs, port));

        if (opcode == BCMLT_OPCODE_UPDATE) {
            if (flags == OBM_DROP) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, LOSSLESS0_PKTs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, LOSSLESS0_BYTEs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, LOSSLESS1_PKTs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, LOSSLESS1_BYTEs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, LOSSY_HIGH_PKTs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, LOSSY_HIGH_BYTEs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, LOSSY_LOW_PKTs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, LOSSY_LOW_BYTEs, 0));
            } else if (flags == OBM_FLOW_CTRL) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, FLOW_CTRL_EVENTSs, 0));
            } else if (flags == OBM_USAGE) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, BYTEs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, LOSSLESS0_BYTEs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, LOSSLESS1_BYTEs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, LOSSY_BYTEs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, MAX_USAGE_BYTEs, 0));
            }
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_transaction_entry_add(trans_hdl, opcode, entry_hdl));
        entry_hdl = BCMLT_INVALID_HDL;
        valid_trans = true;
    }

    if (valid_trans) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_transaction_commit(unit, trans_hdl, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
cosq_stat_rqe_drop_counter_trans_process(int unit, bcmlt_opcode_t opcode)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    uint32_t queue_num;
    int itm;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    bool valid_trans = false;
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    for (itm = 0; itm < device_info.num_itm; itm++) {
        for (queue_num = 0; queue_num < NUM_RQE_QUEUES; queue_num++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_allocate(dunit, CTR_TM_REPL_Q_DROPs, &entry_hdl));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, BUFFER_POOLs, itm));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, REPL_Q_NUMs, queue_num));
            if (opcode == BCMLT_OPCODE_UPDATE) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, PKTs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, BYTESs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, RED_PKTs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, YELLOW_PKTs, 0));
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_transaction_entry_add(trans_hdl, opcode, entry_hdl));
            entry_hdl = BCMLT_INVALID_HDL;
            valid_trans = true;
        }
    }

    if (valid_trans) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_transaction_commit(unit, trans_hdl, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
cosq_stat_port_queue_counter_clear(int unit, int port, int queue, bool is_uc)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = is_uc ? CTR_EGR_UC_Qs : CTR_EGR_MC_Qs;
    fld_name = is_uc ? UC_Qs : MC_Qs;

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, tbl_name, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_IDs, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fld_name, queue));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PKTs, 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, BYTESs, 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
cosq_stat_port_queue_counter_trans_process(int unit, const char *table_name,
                                           const char *fld_name, uint32_t flags,
                                           bcmlt_opcode_t opcode)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    uint32_t queue_num, queue_max;
    int port;
    int num_ucq, num_mcq, rv;
    bool valid_trans = false;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_num_queue_get(unit, &num_ucq, &num_mcq));

    for (port = 0; port < bcmi_ltsw_dev_logic_port_num(unit); port++) {
        if (flags == MC_QUEUE) {
            queue_max = is_cpu_port(unit, port) ? port_num_queue(unit, port) :
                                                  num_mcq;
        } else {
            if (is_cpu_port(unit, port)) {
                continue;
            }
            queue_max = num_ucq;
        }

        for (queue_num = 0; queue_num < queue_max; queue_num++) {
            rv = bcmlt_entry_allocate(dunit, table_name, &entry_hdl);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_RESOURCE);
            if (rv == SHR_E_RESOURCE) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_transaction_commit(unit, trans_hdl,
                                                BCMLT_PRIORITY_NORMAL));

                if (trans_hdl != BCMLT_INVALID_HDL) {
                    bcmlt_transaction_free(trans_hdl);
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH,
                                                &trans_hdl));

                valid_trans = false;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, PORT_IDs, port));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, fld_name, queue_num));
            if (opcode == BCMLT_OPCODE_UPDATE) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, PKTs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, BYTESs, 0));
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_transaction_entry_add(trans_hdl, opcode, entry_hdl));
            entry_hdl = BCMLT_INVALID_HDL;
            valid_trans = true;
        }
    }

    if (valid_trans) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_transaction_commit(unit, trans_hdl, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
cosq_stat_port_queue_wred_drop_get(int unit, int local_port, uint64_t *value, int sync)
{
    int itm, ci;
    uint64_t sum;
    bcmi_ltsw_cosq_port_map_info_t port_info;
    const char *table_name;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {BUFFER_POOLs,              BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {TM_UC_Q_IDs,               BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 3 */ {WRED_PKTs,                 BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmi_ltsw_cosq_device_info_t device_info;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_port_map_info_get(unit, local_port, &port_info));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }

    fields[0].u.val = local_port;
    table_name = CTR_TM_UC_Q_DROPs;
    LTSW_COMPILER_64_ZERO(sum);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    for (itm = 0; itm < device_info.num_itm; itm++) {
        fields[1].u.val = itm;
        for (ci = 0; ci < port_info.num_uc_q; ci++) {
            fields[2].u.val = ci;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));

            LTSW_COMPILER_64_ADD_64(sum, fields[3].u.val);
        }
    }

    LTSW_COMPILER_64_COPY(*value, sum);

exit:
    SHR_FUNC_EXIT();
}

static int
cosq_stat_port_queue_color_drop_clear(int unit, int port, int buffer_id)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, CTR_EGR_TM_PORT_DROPs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_IDs, port));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, BUFFER_POOLs, buffer_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, UC_RED_PKTs, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, UC_YELLOW_PKTs, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, MC_RED_PKTs, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, MC_YELLOW_PKTs, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, WRED_RED_PKTs, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, WRED_YELLOW_PKTs, 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
cosq_stat_port_queue_color_drop_trans_process(int unit, const char *table_name,
                                              bcmlt_opcode_t opcode)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    int port, itm, rv;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    bool valid_trans = false;
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    for (port = 0; port < bcmi_ltsw_dev_logic_port_num(unit); port++) {
        for (itm = 0; itm < device_info.num_itm; itm++) {
            rv = bcmlt_entry_allocate(dunit, table_name, &entry_hdl);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_RESOURCE);
            if (rv == SHR_E_RESOURCE) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_transaction_commit(unit, trans_hdl,
                                                BCMLT_PRIORITY_NORMAL));

                if (trans_hdl != BCMLT_INVALID_HDL) {
                    bcmlt_transaction_free(trans_hdl);
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH,
                                                &trans_hdl));

                valid_trans = false;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, PORT_IDs, port));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, BUFFER_POOLs, itm));
            if (opcode == BCMLT_OPCODE_UPDATE) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, UC_RED_PKTs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, UC_YELLOW_PKTs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, MC_RED_PKTs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, MC_YELLOW_PKTs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, WRED_RED_PKTs, 0));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, WRED_YELLOW_PKTs, 0));
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_transaction_entry_add(trans_hdl, opcode, entry_hdl));
            entry_hdl = BCMLT_INVALID_HDL;
            valid_trans = true;
        }
    }

    if (valid_trans) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_transaction_commit(unit, trans_hdl, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
cosq_stat_port_queue_drop_counter_clear(int unit, int port, int itm,
                                        int queue, bool is_uc)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    const char *tbl_name;
    const char *fld_name;
    SHR_FUNC_ENTER(unit);

    tbl_name = is_uc ? CTR_TM_UC_Q_DROPs : CTR_TM_MC_Q_DROPs;
    fld_name = is_uc ? TM_UC_Q_IDs : TM_MC_Q_IDs;

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, tbl_name, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_IDs, port));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, BUFFER_POOLs, itm));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, fld_name, queue));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PKTs, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, BYTEs, 0));
    if (is_uc) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, WRED_PKTs, 0));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
cosq_stat_port_queue_drop_counter_trans_process(int unit, const char *table_name,
                                                const char *fld_name, uint32_t flags,
                                                bcmlt_opcode_t opcode)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    uint32_t queue_num, queue_max;
    int port, itm;
    int num_ucq, num_mcq, rv;
    bool valid_trans = false;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    bcmi_ltsw_cosq_device_info_t device_info;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_num_queue_get(unit, &num_ucq, &num_mcq));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    for (port = 0; port < bcmi_ltsw_dev_logic_port_num(unit); port++) {
        if (flags == MC_QUEUE) {
            queue_max = is_cpu_port(unit, port) ? port_num_queue(unit, port) :
                                                  num_mcq;
        } else {
            if (is_cpu_port(unit, port)) {
                continue;
            }
            queue_max = num_ucq;
        }

        for (itm = 0; itm < device_info.num_itm; itm++) {
            for (queue_num = 0; queue_num < queue_max; queue_num++) {
                rv = bcmlt_entry_allocate(dunit, table_name, &entry_hdl);
                SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_RESOURCE);
                if (rv == SHR_E_RESOURCE) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_lt_transaction_commit(unit, trans_hdl,
                                                    BCMLT_PRIORITY_NORMAL));

                    if (trans_hdl != BCMLT_INVALID_HDL) {
                        bcmlt_transaction_free(trans_hdl);
                    }

                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH,
                                                    &trans_hdl));

                    valid_trans = false;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, PORT_IDs, port));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, BUFFER_POOLs, itm));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, fld_name, queue_num));
                if (opcode == BCMLT_OPCODE_UPDATE) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add(entry_hdl, PKTs, 0));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add(entry_hdl, BYTEs, 0));
                    if (flags == UC_QUEUE) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmlt_entry_field_add(entry_hdl, WRED_PKTs, 0));
                    }
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_transaction_entry_add(trans_hdl, opcode, entry_hdl));
                entry_hdl = BCMLT_INVALID_HDL;
                valid_trans = true;
            }
        }
    }

    if (valid_trans) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_transaction_commit(unit, trans_hdl, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
cosq_stat_hp_drop_counter_trans_process(int unit, const char *table_name,
                                        bcmlt_opcode_t opcode)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    uint32_t queue_num;
    int itm;
    bcmlt_field_def_t field_def;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    bool valid_trans = false;
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, table_name, TM_HEADROOM_POOL_IDs, &field_def));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    for (itm = 0; itm < device_info.num_itm; itm++) {
        for (queue_num = 0; queue_num <= field_def.max; queue_num++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, BUFFER_POOLs, itm));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, TM_HEADROOM_POOL_IDs, queue_num));
            if (opcode == BCMLT_OPCODE_UPDATE) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, PKTs, 0));
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_transaction_entry_add(trans_hdl, opcode, entry_hdl));
            entry_hdl = BCMLT_INVALID_HDL;
            valid_trans = true;
        }
    }

    if (valid_trans) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_transaction_commit(unit, trans_hdl, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
cosq_stat_ing_port_drop_counter_clear(int unit, int port)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, CTR_ING_TM_PORT_UC_DROPs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_IDs, port));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PKTs, 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
cosq_stat_ing_port_drop_counter_trans_process(int unit, const char *table_name,
                                              bcmlt_opcode_t opcode)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    int port, rv;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    bool valid_trans = false;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    for (port = 0; port < bcmi_ltsw_dev_logic_port_num(unit); port++) {
        if (is_cpu_port(unit, port)) {
            continue;
        }
        rv = bcmlt_entry_allocate(dunit, table_name, &entry_hdl);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_RESOURCE);
        if (rv == SHR_E_RESOURCE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_transaction_commit(unit, trans_hdl,
                                            BCMLT_PRIORITY_NORMAL));

            if (trans_hdl != BCMLT_INVALID_HDL) {
                bcmlt_transaction_free(trans_hdl);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH,
                                            &trans_hdl));

            valid_trans = false;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, PORT_IDs, port));
        if (opcode == BCMLT_OPCODE_UPDATE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, PKTs, 0));
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_transaction_entry_add(trans_hdl, opcode, entry_hdl));
        entry_hdl = BCMLT_INVALID_HDL;
        valid_trans = true;
    }

    if (valid_trans) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_transaction_commit(unit, trans_hdl, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Convert ADT counter enum to alpha value.
 */
static cosq_stat_adt_enum_convert_t cosq_adt_counter_to_alpha[] =
{
    {bcmCosqStatAdtAlpha0AdmittedLowPriPackets,  bcmCosqControlDropLimitAlpha_1_128},
    {bcmCosqStatAdtAlpha1AdmittedLowPriPackets,  bcmCosqControlDropLimitAlpha_1_64},
    {bcmCosqStatAdtAlpha2AdmittedLowPriPackets,  bcmCosqControlDropLimitAlpha_1_32},
    {bcmCosqStatAdtAlpha3AdmittedLowPriPackets,  bcmCosqControlDropLimitAlpha_1_16},
    {bcmCosqStatAdtAlpha4AdmittedLowPriPackets,  bcmCosqControlDropLimitAlpha_1_8},
    {bcmCosqStatAdtAlpha5AdmittedLowPriPackets,  bcmCosqControlDropLimitAlpha_1_4},
    {bcmCosqStatAdtAlpha6AdmittedLowPriPackets,  bcmCosqControlDropLimitAlpha_1_2},
    {bcmCosqStatAdtAlpha7AdmittedLowPriPackets,  bcmCosqControlDropLimitAlpha_1},
    {bcmCosqStatAdtAlpha8AdmittedLowPriPackets,  bcmCosqControlDropLimitAlpha_2},
    {bcmCosqStatAdtAlpha9AdmittedLowPriPackets,  bcmCosqControlDropLimitAlpha_4},
    {bcmCosqStatAdtAlpha10AdmittedLowPriPackets, bcmCosqControlDropLimitAlpha_8},
};

static int
cosq_stat_adt_low_pri_pkt_trans_process(int unit, const char *table_name,
                                                 bcmlt_opcode_t opcode)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    int itm, alpha_index;
    int rv;
    bool valid_trans = false;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    const char *alpha;
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    for (alpha_index = 0; alpha_index < bcmCosqControlDropLimitAlphaCount; alpha_index++) {
         for (itm = 0; itm < device_info.num_itm; itm++) {
             rv = bcmlt_entry_allocate(dunit, table_name, &entry_hdl);
             SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_RESOURCE);
             if (rv == SHR_E_RESOURCE) {
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmi_lt_transaction_commit(unit, trans_hdl,
                                                 BCMLT_PRIORITY_NORMAL));

                 if (trans_hdl != BCMLT_INVALID_HDL) {
                     bcmlt_transaction_free(trans_hdl);
                 }

                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH,
                                                 &trans_hdl));

                 valid_trans = false;
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));
             }

             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmlt_entry_field_add(entry_hdl, BUFFER_POOLs, itm));

             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmi_ltsw_cosq_thd_alpha_bcm_to_lt_convert(unit, alpha_index,
                                                             &alpha));
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmlt_entry_field_symbol_add(entry_hdl, LOW_PRI_DYNAMICs,
                                               alpha));

             if (opcode == BCMLT_OPCODE_UPDATE) {
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmlt_entry_field_add(entry_hdl, PKTs, 0));
             }
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmlt_transaction_entry_add(trans_hdl, opcode, entry_hdl));
             entry_hdl = BCMLT_INVALID_HDL;
             valid_trans = true;
        }
    }

    if (valid_trans) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_transaction_commit(unit, trans_hdl, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
cosq_stat_adt_high_pri_pkt_trans_process(int unit, const char *table_name,
                                                  bcmlt_opcode_t opcode)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    int itm, pool;
    int rv;
    bool valid_trans = false;
    bcmlt_field_def_t field_def;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, table_name,
                               TM_EGR_SERVICE_POOL_IDs,
                               &field_def));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    for (pool = 0; pool <= field_def.max; pool++) {
         for (itm = 0; itm < device_info.num_itm; itm++) {
             rv = bcmlt_entry_allocate(dunit, table_name, &entry_hdl);
             SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_RESOURCE);
             if (rv == SHR_E_RESOURCE) {
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmi_lt_transaction_commit(unit, trans_hdl,
                                                 BCMLT_PRIORITY_NORMAL));

                 if (trans_hdl != BCMLT_INVALID_HDL) {
                     bcmlt_transaction_free(trans_hdl);
                 }

                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH,
                                                 &trans_hdl));

                 valid_trans = false;
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));
             }

             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmlt_entry_field_add(entry_hdl, BUFFER_POOLs, itm));
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmlt_entry_field_add(entry_hdl, TM_EGR_SERVICE_POOL_IDs,
                                        pool));
             if (opcode == BCMLT_OPCODE_UPDATE) {
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmlt_entry_field_add(entry_hdl, CELLSs, 0));
             }
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmlt_transaction_entry_add(trans_hdl, opcode, entry_hdl));
             entry_hdl = BCMLT_INVALID_HDL;
             valid_trans = true;
        }
    }

    if (valid_trans) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_transaction_commit(unit, trans_hdl, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
cosq_stat_debug_stat_saf_trans_process(int unit, const char *table_name,
                                                 bcmlt_opcode_t opcode)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    int itm, counter_id;
    bcm_cosq_stat_t stat;
    int rv;
    bool valid_trans = false;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    for (stat = bcmCosqStatDebugSaf0Packets; stat <= bcmCosqStatDebugSaf7Packets;
         stat++) {
         counter_id = stat - bcmCosqStatDebugSaf0Packets;
         for (itm = 0; itm < device_info.num_itm; itm++) {
             rv = bcmlt_entry_allocate(dunit, table_name, &entry_hdl);
             SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_RESOURCE);
             if (rv == SHR_E_RESOURCE) {
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmi_lt_transaction_commit(unit, trans_hdl,
                                                 BCMLT_PRIORITY_NORMAL));

                 if (trans_hdl != BCMLT_INVALID_HDL) {
                     bcmlt_transaction_free(trans_hdl);
                 }

                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH,
                                                 &trans_hdl));

                 valid_trans = false;
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));
             }

             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmlt_entry_field_add(entry_hdl, BUFFER_POOLs, itm));

             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmlt_entry_field_add(entry_hdl, CTR_TM_STORE_AND_FORWARD_IDs,
                                        counter_id));

             if (opcode == BCMLT_OPCODE_UPDATE) {
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmlt_entry_field_add(entry_hdl, PKTs, 0));
             }
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmlt_transaction_entry_add(trans_hdl, opcode, entry_hdl));
             entry_hdl = BCMLT_INVALID_HDL;
             valid_trans = true;
        }
    }

    if (valid_trans) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_transaction_commit(unit, trans_hdl, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
cosq_stat_debug_stat_ct_trans_process(int unit, const char *table_name,
                                                 bcmlt_opcode_t opcode)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    int itm, counter_id;
    bcm_cosq_stat_t stat;
    int rv;
    bool valid_trans = false;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    for (stat = bcmCosqStatDebugCt0Packets; stat <= bcmCosqStatDebugCt7Packets;
         stat++) {
         counter_id = stat - bcmCosqStatDebugCt0Packets;
         for (itm = 0; itm < device_info.num_itm; itm++) {
             rv = bcmlt_entry_allocate(dunit, table_name, &entry_hdl);
             SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_RESOURCE);
             if (rv == SHR_E_RESOURCE) {
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmi_lt_transaction_commit(unit, trans_hdl,
                                                 BCMLT_PRIORITY_NORMAL));

                 if (trans_hdl != BCMLT_INVALID_HDL) {
                     bcmlt_transaction_free(trans_hdl);
                 }

                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH,
                                                 &trans_hdl));

                 valid_trans = false;
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));
             }

             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmlt_entry_field_add(entry_hdl, BUFFER_POOLs, itm));

             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmlt_entry_field_add(entry_hdl, CTR_TM_CUT_THROUGH_IDs,
                                        counter_id));

             if (opcode == BCMLT_OPCODE_UPDATE) {
                 SHR_IF_ERR_VERBOSE_EXIT
                     (bcmlt_entry_field_add(entry_hdl, PKTs, 0));
             }
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmlt_transaction_entry_add(trans_hdl, opcode, entry_hdl));
             entry_hdl = BCMLT_INVALID_HDL;
             valid_trans = true;
        }
    }

    if (valid_trans) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_transaction_commit(unit, trans_hdl, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
cosq_stat_debug_stat_match_get(
    int unit,
    const char *table_name,
    const char *index_field,
    int itm,
    int counter_id,
    bcm_cosq_debug_stat_match_t *match)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    int rv = SHR_E_NONE;
    bcmi_ltsw_cosq_port_map_info_t info;
    uint64_t match_valid = FALSE;
    const char *q_type;
    union {
        /* scalar data. */
        uint64_t    val;
        /* symbol data. */
        const char  *sym_val;
    } value;

    SHR_FUNC_ENTER(unit);

    match->match_flags = 0;

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, index_field, counter_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, BUFFER_POOLs, itm));

    rv = bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL);

    if (rv == SHR_E_NOT_FOUND) {
        match->src_port = 0;
        match->dst_port = 0;
        match->cosq = 0;
        match->priority_group_id = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, ING_PORT_ID_MATCHs, &(value.val)));
    match_valid = value.val;
    if (match_valid) {
        match->match_flags |= BCM_COSQ_DEBUG_STAT_MATCH_SRC_PORT;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, ING_TM_PIPE_ID_MATCHs, &(value.val)));
    match_valid = value.val;
    if (match_valid) {
        match->match_flags |= BCM_COSQ_DEBUG_STAT_MATCH_SRC_PIPE;
    }

    if ((match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_SRC_PORT) ||
        (match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_SRC_PIPE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, ING_PORT_IDs, &(value.val)));
        match->src_port = value.val;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, EGR_PORT_ID_MATCHs, &(value.val)));
    match_valid = value.val;
    if (match_valid) {
        match->match_flags |= BCM_COSQ_DEBUG_STAT_MATCH_DST_PORT;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, EGR_TM_PIPE_ID_MATCHs, &(value.val)));
    match_valid = value.val;
    if (match_valid) {
        match->match_flags |= BCM_COSQ_DEBUG_STAT_MATCH_DST_PIPE;
    }

    if ((match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_DST_PORT) ||
        (match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_DST_PIPE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, EGR_PORT_IDs, &(value.val)));
        match->dst_port = value.val;

        /* Queue ID match will be valid only if dst port is provisioned */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, TM_Q_ID_MATCHs, &(value.val)));
        match_valid = value.val;
        if (match_valid) {
            /* Get UC/MC queue number. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_port_map_info_get(unit, match->dst_port, &info));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_get(entry_hdl, Q_TYPEs, &(value.sym_val)));
            q_type = value.sym_val;
            if (!sal_strcasecmp(q_type, UC_Qs)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(entry_hdl, TM_UC_Q_IDs, &(value.val)));
                match->cosq = value.val;
            } else if (!sal_strcasecmp(q_type, MC_Qs)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(entry_hdl, TM_MC_Q_IDs, &(value.val)));
                match->cosq = value.val + info.num_uc_q;
            } else {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
            match->match_flags |= BCM_COSQ_DEBUG_STAT_MATCH_QUEUE;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, PRI_GRP_ID_MATCHs, &(value.val)));
    match_valid = value.val;
    if (match_valid) {
        match->match_flags |= BCM_COSQ_DEBUG_STAT_MATCH_PRIORITY_GROUP;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, TM_PRI_GRP_IDs,
                                   &(value.val)));
        match->priority_group_id = value.val;
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
cosq_stat_debug_stat_match_delete(
    int unit,
    const char *table_name,
    const char *index_field,
    int itm,
    int counter_id)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, index_field, counter_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, BUFFER_POOLs, itm));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
cosq_stat_debug_stat_match_set(
    int unit,
    const char *table_name,
    const char *index_field,
    int itm,
    int counter_id,
    bcm_cosq_debug_stat_match_t *match)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    bcmi_ltsw_cosq_port_map_info_t info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (cosq_stat_debug_stat_match_delete(unit, table_name, index_field,
                                           itm, counter_id), SHR_E_NOT_FOUND);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, index_field, counter_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, BUFFER_POOLs, itm));

    if (match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_SRC_PORT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, ING_PORT_ID_MATCHs, TRUE));
    }

    if (match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_SRC_PIPE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, ING_TM_PIPE_ID_MATCHs, TRUE));
    }

    if ((match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_SRC_PORT) ||
        (match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_SRC_PIPE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, ING_PORT_IDs, match->src_port));
    }

    if (match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_DST_PORT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, EGR_PORT_ID_MATCHs, TRUE));
    }

    if (match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_DST_PIPE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, EGR_TM_PIPE_ID_MATCHs, TRUE));
    }

    if ((match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_DST_PORT) ||
        (match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_DST_PIPE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, EGR_PORT_IDs, match->dst_port));

        /* Queue ID match will be valid only if dst port is provisioned */
        if (match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_QUEUE) {
            /* Get UC/MC queue number. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_port_map_info_get(unit, match->dst_port, &info));

            if ((match->cosq >= 0) && (match->cosq < info.num_uc_q)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_symbol_add(entry_hdl, Q_TYPEs, UC_Qs));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, TM_UC_Q_IDs, match->cosq));
                /* Per SDKLT requirement, need to set to 0 when confgiure to UC Q type. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, TM_MC_Q_IDs, 0));
            } else if ((match->cosq < (info.num_uc_q + info.num_mc_q))){
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_symbol_add(entry_hdl, Q_TYPEs, MC_Qs));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, TM_MC_Q_IDs,
                                           match->cosq - info.num_uc_q));
                /* Per SDKLT requirement, need to set to 0 when confgiure to MC Q type. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(entry_hdl, TM_UC_Q_IDs, 0));

            } else {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, TM_Q_ID_MATCHs, TRUE));
        }
    }else if (match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_QUEUE) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_PRIORITY_GROUP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, PRI_GRP_ID_MATCHs, TRUE));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, TM_PRI_GRP_IDs,
                                   match->priority_group_id));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
cosq_stat_debug_stat_pkt_get(
    int unit,
    const char *table_name,
    const char *index_field,
    int itm,
    int counter_id,
    uint64_t *value,
    int sync)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, index_field, counter_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, BUFFER_POOLs, itm));

    if (sync == 1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_attrib_set(entry_hdl, BCMLT_ENT_ATTR_GET_FROM_HW));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, PKTs, value));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
cosq_stat_debug_stat_pkt_set(
    int unit,
    const char *table_name,
    const char *index_field,
    int itm,
    int counter_id,
    uint64_t value)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, index_field, counter_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, BUFFER_POOLs, itm));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PKTs, value));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Internal HSDK functions
 */

/*
 * \brief Obm stat setting
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
tm_ltsw_cosq_stat_obm_set(
    int unit,
    int gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t value)
{
    int local_port;
    cosq_stat_obm_map_t *map;
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (cosq != BCM_COS_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    map = cosq_stat_obm_to_field(stat);

    if (map == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    LTSW_COMPILER_64_COPY(map->field.u.val, value);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_obm_table_set(unit, map->lt_val, local_port, &map->field));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Obm stat get
 *
 * \param [in] unit Unit Number.
 * \param [in] gport GPORT ID.
 * \param [in] cosq COS queue.
 * \param [in] stat Feature.
 * \param [in] sync_mode sync mode.
 * \param [out] value Feature value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
tm_ltsw_cosq_stat_obm_get(
    int unit,
    int gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    int sync_mode,
    uint64_t *value)
{
    int local_port;
    cosq_stat_obm_map_t *map;
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (cosq != BCM_COS_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (value == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    map = cosq_stat_obm_to_field(stat);

    if (map == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_obm_table_get(unit, map->lt_val, local_port, &map->field,
                                 (sync_mode == 1 ? BCMLT_ENT_ATTR_GET_FROM_HW : 0)));

   LTSW_COMPILER_64_COPY(*value, map->field.u.val);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_obm_counter_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_obm_counter_trans_process(unit, CTR_TM_OBM_PORT_DROPs,
                                             OBM_DROP, BCMLT_OPCODE_INSERT));
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_obm_counter_trans_process(unit, CTR_TM_OBM_PORT_FLOW_CTRLs,
                                             OBM_FLOW_CTRL, BCMLT_OPCODE_INSERT));
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_obm_counter_trans_process(unit, CTR_TM_OBM_PORT_USAGEs,
                                             OBM_USAGE, BCMLT_OPCODE_INSERT));
exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_obm_counter_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_CONT
        (bcmi_lt_clear(unit, CTR_TM_OBM_PORT_DROPs));

    SHR_IF_ERR_CONT
        (bcmi_lt_clear(unit, CTR_TM_OBM_PORT_FLOW_CTRLs));

    SHR_IF_ERR_CONT
        (bcmi_lt_clear(unit, CTR_TM_OBM_PORT_USAGEs));

    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_rqe_drop_counter_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_rqe_drop_counter_trans_process(unit, BCMLT_OPCODE_INSERT));
exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_rqe_drop_counter_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_CONT
        (bcmi_lt_clear(unit, CTR_TM_REPL_Q_DROPs));

    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_rqe_drop_counter_get(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                                       bcm_cosq_stat_t stat, uint64_t *value, int sync)
{
    int itm;
    uint64_t val, byte_val, red_val, yellow_val;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {BUFFER_POOLs,           BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {REPL_Q_NUMs,            BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {PKTs,                   BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 3 */ {BYTEs,                  BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 4 */ {RED_PKTs,               BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 5 */ {YELLOW_PKTs,            BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmi_ltsw_cosq_device_info_t device_info;
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
        ((cosq < 0) || (cosq >= NUM_RQE_QUEUES))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }

    fields[1].u.val = cosq;
    LTSW_COMPILER_64_ZERO(val);
    LTSW_COMPILER_64_ZERO(byte_val);
    LTSW_COMPILER_64_ZERO(red_val);
    LTSW_COMPILER_64_ZERO(yellow_val);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    for (itm = 0; itm < device_info.num_itm; itm++) {
        fields[0].u.val = itm;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get(unit, CTR_TM_REPL_Q_DROPs, &lt_entry, NULL, NULL));

        LTSW_COMPILER_64_ADD_64(val, fields[2].u.val);
        LTSW_COMPILER_64_ADD_64(byte_val, fields[3].u.val);
        LTSW_COMPILER_64_ADD_64(red_val, fields[4].u.val);
        LTSW_COMPILER_64_ADD_64(yellow_val, fields[5].u.val);
    }

    switch (stat) {
        case bcmCosqStatRQETotalDroppedPackets:
            LTSW_COMPILER_64_COPY(*value, val);
            break;
        case bcmCosqStatRQETotalDroppedBytes:
            LTSW_COMPILER_64_COPY(*value, byte_val);
            break;
        case bcmCosqStatRQEGreenDroppedPackets:
            LTSW_COMPILER_64_SUB_64(val, red_val);
            LTSW_COMPILER_64_SUB_64(val, yellow_val);
            LTSW_COMPILER_64_COPY(*value, val);
            break;
        case bcmCosqStatRQEYellowDroppedPackets:
            LTSW_COMPILER_64_COPY(*value, yellow_val);
            break;
        case bcmCosqStatRQERedDroppedPackets:
            LTSW_COMPILER_64_COPY(*value, red_val);
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_rqe_drop_counter_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                                       bcm_cosq_stat_t stat, uint64_t value)
{
    int itm;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {BUFFER_POOLs,           BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {REPL_Q_NUMs,            BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {NULL,                   BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_ltsw_cosq_device_info_t device_info;
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
        ((cosq < 0) || (cosq >= NUM_RQE_QUEUES))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (stat == bcmCosqStatRQEGreenDroppedPackets) {
        /*Combnation of other stats*/
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[1].u.val = cosq;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    switch (stat) {
        case bcmCosqStatRQETotalDroppedPackets:
            fields[2].fld_name = PKTs;
            break;
        case bcmCosqStatRQETotalDroppedBytes:
            fields[2].fld_name = BYTEs;
            break;
        case bcmCosqStatRQEYellowDroppedPackets:
            fields[2].fld_name = YELLOW_PKTs;
            break;
        case bcmCosqStatRQERedDroppedPackets:
            fields[2].fld_name = RED_PKTs;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    for (itm = 0; itm < device_info.num_itm; itm++) {
        fields[0].u.val = itm;
        fields[2].u.val = value;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set(unit, CTR_TM_REPL_Q_DROPs, &lt_entry, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_rqe_current_usage_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync)
{
    int itm;
    uint64_t shared_val, min_val, total_val;
    bcmi_lt_entry_t lt_entry;
    const char *tbl_name = CTR_TM_REPL_Qs;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {BUFFER_POOLs,           BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {REPL_Q_NUMs,            BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {SHARED_USAGE_CELLSs,    BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 3 */ {MIN_USAGE_CELLSs,       BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 4 */ {TOTAL_USAGE_CELLSs,     BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmi_ltsw_cosq_device_info_t device_info;
    SHR_FUNC_ENTER(unit);

    if (gport != BCM_PORT_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if ((cosq < 0) || (cosq >= NUM_RQE_QUEUES)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }

    fields[1].u.val = cosq;
    LTSW_COMPILER_64_ZERO(shared_val);
    LTSW_COMPILER_64_ZERO(min_val);
    LTSW_COMPILER_64_ZERO(total_val);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    for (itm = 0; itm < device_info.num_itm; itm++) {
        fields[0].u.val = itm;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get(unit, tbl_name, &lt_entry, NULL, NULL));

        LTSW_COMPILER_64_ADD_64(shared_val, fields[2].u.val);
        LTSW_COMPILER_64_ADD_64(min_val, fields[3].u.val);
        LTSW_COMPILER_64_ADD_64(total_val, fields[4].u.val);
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "lt %s lookup %s=%d %s=%d %s=%d %s=%d %s=%d\n"),
                tbl_name,
                fields[0].fld_name, (int)fields[0].u.val,
                fields[1].fld_name, (int)fields[1].u.val,
                fields[2].fld_name, (int)fields[2].u.val,
                fields[3].fld_name, (int)fields[3].u.val,
                fields[4].fld_name, (int)fields[4].u.val));
    }

    switch (stat) {
        case bcmCosqStatRQEQueueSharedBytesCurrent:
            LTSW_COMPILER_64_COPY(*value, shared_val);
            break;
        case bcmCosqStatRQEQueueMinBytesCurrent:
            LTSW_COMPILER_64_COPY(*value, min_val);
            break;
        case bcmCosqStatRQEQueueTotalBytesCurrent:
            LTSW_COMPILER_64_COPY(*value, total_val);
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_rqe_pool_current_usage_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync)
{
    int itm;
    uint64_t shared_val;
    bcmi_lt_entry_t lt_entry;
    const char *tbl_name = CTR_TM_REPL_Q_SERVICE_POOLs;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {BUFFER_POOLs,           BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {SHARED_USAGE_CELLSs,    BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmi_ltsw_cosq_device_info_t device_info;
    SHR_FUNC_ENTER(unit);

    if (gport != BCM_PORT_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (cosq != BCM_COS_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }

    LTSW_COMPILER_64_ZERO(shared_val);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    for (itm = 0; itm < device_info.num_itm; itm++) {
        fields[0].u.val = itm;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get(unit, tbl_name, &lt_entry, NULL, NULL));

        LTSW_COMPILER_64_ADD_64(shared_val, fields[1].u.val);
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "lt %s lookup %s=%d %s=%d\n"),
                tbl_name,
                fields[0].fld_name, (int)fields[0].u.val,
                fields[1].fld_name, (int)fields[1].u.val));
    }

    LTSW_COMPILER_64_COPY(*value, shared_val);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_egr_sp_shared_current_usage_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync)
{
    int local_port, itm = -1;
    int pool = -1;
    uint64_t uc_val, mc_val;
    bcmi_lt_entry_t lt_entry;
    const char *tbl_name = CTR_EGR_TM_SERVICE_POOLs;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {BUFFER_POOLs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_EGR_SERVICE_POOL_IDs,       BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {TOTAL_USAGE_CELLSs,            BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 3 */ {TOTAL_MC_USAGE_CELLSs,         BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmi_ltsw_cosq_device_info_t device_info;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    switch (stat) {
        case bcmCosqStatEgressPoolSharedUCBytesCurrent:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_thd_egress_pool_get(unit, local_port, cosq,
                                                    bcmCosqControlUCEgressPool,
                                                    &pool));
            break;
        case bcmCosqStatEgressPoolSharedMCBytesCurrent:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_thd_egress_pool_get(unit, local_port, cosq,
                                                    bcmCosqControlMCEgressPool,
                                                    &pool));
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (pool == -1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }

    fields[1].u.val = pool;
    LTSW_COMPILER_64_ZERO(uc_val);
    LTSW_COMPILER_64_ZERO(mc_val);

    for (itm = 0; itm < device_info.num_itm; itm++) {
        fields[0].u.val = itm;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get(unit, tbl_name, &lt_entry, NULL, NULL));

        LTSW_COMPILER_64_ADD_64(uc_val, fields[2].u.val);
        LTSW_COMPILER_64_ADD_64(mc_val, fields[3].u.val);
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "lt %s lookup %s=%d %s=%d %s=%d %s=%d\n"),
                tbl_name,
                fields[0].fld_name, (int)fields[0].u.val,
                fields[1].fld_name, (int)fields[1].u.val,
                fields[2].fld_name, (int)fields[2].u.val,
                fields[3].fld_name, (int)fields[3].u.val));
    }

    switch (stat) {
        case bcmCosqStatEgressPoolSharedUCBytesCurrent:
            LTSW_COMPILER_64_COPY(*value, uc_val);
            break;
        case bcmCosqStatEgressPoolSharedMCBytesCurrent:
            LTSW_COMPILER_64_COPY(*value, mc_val);
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_port_total_current_usage_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync)
{
    int local_port;
    int itm;
    uint64_t uc_val, mc_val;
    bcmi_lt_entry_t lt_entry;
    const char *tbl_name = CTR_EGR_TM_PORTs;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {BUFFER_POOLs,              BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {UC_PKTs,                   BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 3 */ {MC_PKTs,                   BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmi_ltsw_cosq_device_info_t device_info;
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (cosq != BCM_COS_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }

    fields[0].u.val = local_port;
    LTSW_COMPILER_64_ZERO(uc_val);
    LTSW_COMPILER_64_ZERO(mc_val);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    for (itm = 0; itm < device_info.num_itm; itm++) {
        fields[1].u.val = itm;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get(unit, tbl_name, &lt_entry, NULL, NULL));

        LTSW_COMPILER_64_ADD_64(uc_val, fields[2].u.val);
        LTSW_COMPILER_64_ADD_64(mc_val, fields[3].u.val);
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "lt %s lookup %s=%d %s=%d %s=%d %s=%d\n"),
                tbl_name,
                fields[0].fld_name, (int)fields[0].u.val,
                fields[1].fld_name, (int)fields[1].u.val,
                fields[2].fld_name, (int)fields[2].u.val,
                fields[3].fld_name, (int)fields[3].u.val));
    }

    switch (stat) {
        case bcmCosqStatEgressPortUCPacketsCurrent:
            LTSW_COMPILER_64_COPY(*value, uc_val);
            break;
        case bcmCosqStatEgressPortMCPacketsCurrent:
            LTSW_COMPILER_64_COPY(*value, mc_val);
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_port_queue_counter_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_port_queue_counter_trans_process(unit, CTR_EGR_UC_Qs, UC_Qs,
                                                    UC_QUEUE, BCMLT_OPCODE_INSERT));
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_port_queue_counter_trans_process(unit, CTR_EGR_MC_Qs, MC_Qs,
                                                    MC_QUEUE, BCMLT_OPCODE_INSERT));
exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_port_queue_counter_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_CONT
        (bcmi_lt_clear(unit, CTR_EGR_UC_Qs));

    SHR_IF_ERR_CONT
        (bcmi_lt_clear(unit, CTR_EGR_MC_Qs));

    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_port_queue_counter_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                                         bcm_cosq_stat_t stat, uint64_t *value, int sync)
{
    int local_port;
    int numq, startq, loop_idx, ci;
    uint64_t sum, byte_sum;
    int num_uc = -1, num_mc = -1;
    int uc_q[2], mc_q[1];
    bcmi_ltsw_cosq_port_map_info_t port_info;
    const char *table_name;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {NULL,                      BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {PKTs,                      BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 3 */ {BYTESs,                    BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 4 */ {OPERATIONAL_STATEs,        BCMI_LT_FIELD_F_GET |
                                             BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    numq = port_num_queue(unit, local_port);

    if ((cosq < -1) || (cosq >= numq)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }
    fields[0].u.val = local_port;

    LTSW_COMPILER_64_ZERO(sum);
    LTSW_COMPILER_64_ZERO(byte_sum);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_gport_port_resolve(unit, gport, &local_port, &startq));
        fields[1].fld_name = UC_Qs;
        fields[1].u.val = startq;
        table_name = CTR_EGR_UC_Qs;
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "lt %s lookup %s=%d %s=%d %s=%d %s=%d %s=%s\n"),
                table_name,
                fields[0].fld_name, (int)fields[0].u.val,
                fields[1].fld_name, (int)fields[1].u.val,
                fields[2].fld_name, (int)fields[2].u.val,
                fields[3].fld_name, (int)fields[3].u.val,
                fields[4].fld_name, fields[4].u.sym_val));

        LTSW_COMPILER_64_ADD_64(sum, fields[2].u.val);
        LTSW_COMPILER_64_ADD_64(byte_sum, fields[3].u.val);
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_gport_port_resolve(unit, gport, &local_port, &startq));
        fields[1].fld_name = MC_Qs;
        fields[1].u.val = startq;
        table_name = CTR_EGR_MC_Qs;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "lt %s lookup %s=%d %s=%d %s=%d %s=%d %s=%s\n"),
                table_name,
                fields[0].fld_name, (int)fields[0].u.val,
                fields[1].fld_name, (int)fields[1].u.val,
                fields[2].fld_name, (int)fields[2].u.val,
                fields[3].fld_name, (int)fields[3].u.val,
                fields[4].fld_name, fields[4].u.sym_val));

        LTSW_COMPILER_64_ADD_64(sum, fields[2].u.val);
        LTSW_COMPILER_64_ADD_64(byte_sum, fields[3].u.val);
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_sched_node_child_get(unit, gport, &num_uc, uc_q,
                                                 &num_mc, mc_q));
        if (num_uc > 2 || num_mc > 1) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
        if (num_uc > 0) {
            for (loop_idx = 0; loop_idx < num_uc; loop_idx++) {
                fields[1].fld_name = UC_Qs;
                fields[1].u.val = uc_q[loop_idx];
                table_name = CTR_EGR_UC_Qs;
                SHR_IF_ERR_VERBOSE_EXIT
                   (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));
                LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                        "lt %s lookup %s=%d %s=%d %s=%d %s=%d %s=%s\n"),
                        table_name,
                        fields[0].fld_name, (int)fields[0].u.val,
                        fields[1].fld_name, (int)fields[1].u.val,
                        fields[2].fld_name, (int)fields[2].u.val,
                        fields[3].fld_name, (int)fields[3].u.val,
                        fields[4].fld_name, fields[4].u.sym_val));

                LTSW_COMPILER_64_ADD_64(sum, fields[2].u.val);
                LTSW_COMPILER_64_ADD_64(byte_sum, fields[3].u.val);
            }
        }
        if (num_mc > 0) {
            for (loop_idx = 0; loop_idx < num_mc; loop_idx++) {
                fields[1].fld_name = MC_Qs;
                fields[1].u.val = mc_q[loop_idx];
                table_name = CTR_EGR_MC_Qs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));
                LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                        "lt %s lookup %s=%d %s=%d %s=%d %s=%d %s=%s\n"),
                        table_name,
                        fields[0].fld_name, (int)fields[0].u.val,
                        fields[1].fld_name, (int)fields[1].u.val,
                        fields[2].fld_name, (int)fields[2].u.val,
                        fields[3].fld_name, (int)fields[3].u.val,
                        fields[4].fld_name, fields[4].u.sym_val));

                LTSW_COMPILER_64_ADD_64(sum, fields[2].u.val);
                LTSW_COMPILER_64_ADD_64(byte_sum, fields[3].u.val);
            }
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_port_map_info_get(unit, local_port, &port_info));

        if (cosq == -1) {
            /* Get UC values for NON CPU port */
            if (!is_cpu_port(unit, local_port)) {
                for (ci = 0; ci < port_info.num_uc_q; ci++) {
                    fields[1].fld_name = UC_Qs;
                    fields[1].u.val = ci;
                    table_name = CTR_EGR_UC_Qs;
                    SHR_IF_ERR_VERBOSE_EXIT
                       (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));
                    LOG_INFO(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                            "lt %s lookup %s=%d %s=%d %s=%d %s=%d %s=%s\n"),
                            table_name,
                            fields[0].fld_name, (int)fields[0].u.val,
                            fields[1].fld_name, (int)fields[1].u.val,
                            fields[2].fld_name, (int)fields[2].u.val,
                            fields[3].fld_name, (int)fields[3].u.val,
                            fields[4].fld_name, fields[4].u.sym_val));

                    LTSW_COMPILER_64_ADD_64(sum, fields[2].u.val);
                    LTSW_COMPILER_64_ADD_64(byte_sum, fields[3].u.val);
                }
            }
            /* Get sum of MC values for any port*/
            for (ci = 0; ci < port_info.num_mc_q; ci++) {
                fields[1].fld_name = MC_Qs;
                fields[1].u.val = ci;
                table_name = CTR_EGR_MC_Qs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));
                LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                        "lt %s lookup %s=%d %s=%d %s=%d %s=%d %s=%s\n"),
                        table_name,
                        fields[0].fld_name, (int)fields[0].u.val,
                        fields[1].fld_name, (int)fields[1].u.val,
                        fields[2].fld_name, (int)fields[2].u.val,
                        fields[3].fld_name, (int)fields[3].u.val,
                        fields[4].fld_name, fields[4].u.sym_val));

                LTSW_COMPILER_64_ADD_64(sum, fields[2].u.val);
                LTSW_COMPILER_64_ADD_64(byte_sum, fields[3].u.val);
            }
        } else {
            if (cosq < port_info.num_uc_q) {
                /* If queue is unicast */
                if ((stat == bcmCosqStatMCOutBytes) ||
                    (stat == bcmCosqStatMCOutPackets)) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
                fields[1].fld_name = UC_Qs;
                fields[1].u.val = cosq;
                table_name = CTR_EGR_UC_Qs;
                SHR_IF_ERR_VERBOSE_EXIT
                   (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));
                LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                        "lt %s lookup %s=%d %s=%d %s=%d %s=%d %s=%s\n"),
                        table_name,
                        fields[0].fld_name, (int)fields[0].u.val,
                        fields[1].fld_name, (int)fields[1].u.val,
                        fields[2].fld_name, (int)fields[2].u.val,
                        fields[3].fld_name, (int)fields[3].u.val,
                        fields[4].fld_name, fields[4].u.sym_val));

                LTSW_COMPILER_64_ADD_64(sum, fields[2].u.val);
                LTSW_COMPILER_64_ADD_64(byte_sum, fields[3].u.val);
            } else {
                /* Multicast queue */
                if ((stat == bcmCosqStatUCOutBytes) ||
                    (stat == bcmCosqStatUCOutPackets)) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
                ci = cosq - port_info.num_uc_q;
                fields[1].fld_name = MC_Qs;
                fields[1].u.val = ci;
                table_name = CTR_EGR_MC_Qs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));
                LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                        "lt %s lookup %s=%d %s=%d %s=%d %s=%d %s=%s\n"),
                        table_name,
                        fields[0].fld_name, (int)fields[0].u.val,
                        fields[1].fld_name, (int)fields[1].u.val,
                        fields[2].fld_name, (int)fields[2].u.val,
                        fields[3].fld_name, (int)fields[3].u.val,
                        fields[4].fld_name, fields[4].u.sym_val));

                LTSW_COMPILER_64_ADD_64(sum, fields[2].u.val);
                LTSW_COMPILER_64_ADD_64(byte_sum, fields[3].u.val);
            }
        }
    }

    switch (stat) {
        case bcmCosqStatOutPackets:
        case bcmCosqStatUCOutPackets:
        case bcmCosqStatMCOutPackets:
            LTSW_COMPILER_64_COPY(*value, sum);
            break;
        case bcmCosqStatOutBytes:
        case bcmCosqStatUCOutBytes:
        case bcmCosqStatMCOutBytes:
            LTSW_COMPILER_64_COPY(*value, byte_sum);
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "gport %d cosq %d sync %d stat %d arg %d\n"),
            gport, cosq, sync, stat, (int)*value));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_port_queue_counter_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                                         bcm_cosq_stat_t stat, uint64_t value)
{
    int local_port;
    int numq, startq, loop_idx, ci;
    int num_uc = -1, num_mc = -1;
    int uc_q[2], mc_q[1];
    bcmi_ltsw_cosq_port_map_info_t port_info;
    const char *table_name;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {NULL,                      BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {NULL,                      BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_port_map_info_get(unit, local_port, &port_info));

    numq = port_num_queue(unit, local_port);

    if ((cosq < -1) || (cosq >= numq)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    fields[0].u.val = local_port;

    switch (stat) {
        case bcmCosqStatOutPackets:
        case bcmCosqStatUCOutPackets:
        case bcmCosqStatMCOutPackets:
            fields[2].fld_name = PKTs;
            LTSW_COMPILER_64_COPY(fields[2].u.val, value);
            break;
        case bcmCosqStatOutBytes:
        case bcmCosqStatUCOutBytes:
        case bcmCosqStatMCOutBytes:
            fields[2].fld_name = BYTESs;
            LTSW_COMPILER_64_COPY(fields[2].u.val, value);
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_gport_port_resolve(unit, gport, &local_port, &startq));
        fields[1].fld_name = UC_Qs;
        fields[1].u.val = startq;
        table_name = CTR_EGR_UC_Qs;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set(unit, table_name, &lt_entry, NULL));
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_gport_port_resolve(unit, gport, &local_port, &startq));
        fields[1].fld_name = MC_Qs;
        fields[1].u.val = startq;
        table_name = CTR_EGR_MC_Qs;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set(unit, table_name, &lt_entry, NULL));
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_sched_node_child_get(unit, gport, &num_uc, uc_q,
                                                 &num_mc, mc_q));
        if (num_uc > 2 || num_mc > 1) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
        if (num_uc > 0) {
            for (loop_idx = 0; loop_idx < num_uc; loop_idx++) {
                fields[1].fld_name = UC_Qs;
                fields[1].u.val = uc_q[loop_idx];
                table_name = CTR_EGR_UC_Qs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_entry_set(unit, table_name, &lt_entry, NULL));
            }
        }
        if (num_mc > 0) {
            for (loop_idx = 0; loop_idx < num_mc; loop_idx++) {
                fields[1].fld_name = MC_Qs;
                fields[1].u.val = mc_q[loop_idx];
                table_name = CTR_EGR_MC_Qs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_entry_set(unit, table_name, &lt_entry, NULL));
            }
        }
    } else {
        if (cosq == -1) {
            /* Get UC values for NON CPU port */
            if (!is_cpu_port(unit, local_port)) {
                for (ci = 0; ci < port_info.num_uc_q; ci++) {
                    fields[1].fld_name = UC_Qs;
                    fields[1].u.val = ci;
                    table_name = CTR_EGR_UC_Qs;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_lt_entry_set(unit, table_name, &lt_entry, NULL));
                }
            }
            /* Get sum of MC values for any port*/
            for (ci = 0; ci < port_info.num_mc_q; ci++) {
                fields[1].fld_name = MC_Qs;
                fields[1].u.val = ci;
                table_name = CTR_EGR_MC_Qs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_entry_set(unit, table_name, &lt_entry, NULL));
            }
        } else {
            if (cosq < port_info.num_uc_q) {
                /* If queue is unicast */
                if ((stat == bcmCosqStatMCOutBytes) ||
                    (stat == bcmCosqStatMCOutPackets)) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
                fields[1].fld_name = UC_Qs;
                fields[1].u.val = cosq;
                table_name = CTR_EGR_UC_Qs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_entry_set(unit, table_name, &lt_entry, NULL));
            } else {
                /* Multicast queue */
                if ((stat == bcmCosqStatUCOutBytes) ||
                    (stat == bcmCosqStatUCOutPackets)) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
                ci = cosq - port_info.num_uc_q;
                fields[1].fld_name = MC_Qs;
                fields[1].u.val = ci;
                table_name = CTR_EGR_MC_Qs;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_entry_set(unit, table_name, &lt_entry, NULL));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_port_queue_color_drop_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_port_queue_color_drop_trans_process(unit, CTR_EGR_TM_PORT_DROPs,
                                                       BCMLT_OPCODE_INSERT));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_port_queue_color_drop_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_CONT
        (bcmi_lt_clear(unit, CTR_EGR_TM_PORT_DROPs));

    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_port_queue_color_drop_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                                            bcm_cosq_stat_t stat, uint64_t *value, int sync)
{
    int local_port;
    int itm;
    uint64_t sum, uc_red_val, uc_yellow_val;
    uint64_t mc_red_val, mc_yellow_val;
    uint64_t wred_red_val, wred_yellow_val;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {BUFFER_POOLs,              BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {UC_RED_PKTs,               BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 3 */ {UC_YELLOW_PKTs,            BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 4 */ {MC_RED_PKTs,               BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 5 */ {MC_YELLOW_PKTs,            BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 6 */ {WRED_RED_PKTs,             BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 7 */ {WRED_YELLOW_PKTs,          BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmi_ltsw_cosq_device_info_t device_info;
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (cosq != BCM_COS_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }

    fields[0].u.val = local_port;
    LTSW_COMPILER_64_ZERO(uc_red_val);
    LTSW_COMPILER_64_ZERO(uc_yellow_val);
    LTSW_COMPILER_64_ZERO(mc_red_val);
    LTSW_COMPILER_64_ZERO(mc_yellow_val);
    LTSW_COMPILER_64_ZERO(wred_red_val);
    LTSW_COMPILER_64_ZERO(wred_yellow_val);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    for (itm = 0; itm < device_info.num_itm; itm++) {
        fields[1].u.val = itm;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get(unit, CTR_EGR_TM_PORT_DROPs,
                               &lt_entry, NULL, NULL));

        LTSW_COMPILER_64_ADD_64(uc_red_val, fields[2].u.val);
        LTSW_COMPILER_64_ADD_64(uc_yellow_val, fields[3].u.val);
        LTSW_COMPILER_64_ADD_64(mc_red_val, fields[4].u.val);
        LTSW_COMPILER_64_ADD_64(mc_yellow_val, fields[5].u.val);
        LTSW_COMPILER_64_ADD_64(wred_red_val, fields[6].u.val);
        LTSW_COMPILER_64_ADD_64(wred_yellow_val, fields[7].u.val);
    }

    LTSW_COMPILER_64_ZERO(sum);
    switch (stat) {
        case bcmCosqStatYellowCongestionDroppedPackets:
            LTSW_COMPILER_64_ADD_64(sum, uc_yellow_val);
            LTSW_COMPILER_64_ADD_64(sum, mc_yellow_val);
            break;
        case bcmCosqStatRedCongestionDroppedPackets:
            LTSW_COMPILER_64_ADD_64(sum, uc_red_val);
            LTSW_COMPILER_64_ADD_64(sum, mc_red_val);
            break;
        case bcmCosqStatGreenDiscardDroppedPackets:
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_stat_port_queue_wred_drop_get(unit, local_port, &sum, sync));
            LTSW_COMPILER_64_SUB_64(sum, wred_yellow_val);
            LTSW_COMPILER_64_SUB_64(sum, wred_red_val);
            break;
        case bcmCosqStatYellowDiscardDroppedPackets:
            LTSW_COMPILER_64_ADD_64(sum, wred_yellow_val);
            break;
        case bcmCosqStatRedDiscardDroppedPackets:
            LTSW_COMPILER_64_ADD_64(sum, wred_red_val);
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    LTSW_COMPILER_64_COPY(*value, sum);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_port_queue_color_drop_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                                            bcm_cosq_stat_t stat, uint64_t value)
{
    int local_port;
    int itm;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {BUFFER_POOLs,              BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {NULL,                      0, 0, {0}},
         /* 3 */ {NULL,                      0, 0, {0}},
    };
    bcmi_ltsw_cosq_device_info_t device_info;
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (cosq != BCM_COS_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = local_port;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    switch (stat) {
        case bcmCosqStatYellowCongestionDroppedPackets:
            fields[2].flags = BCMI_LT_FIELD_F_SET;
            fields[2].fld_name = UC_YELLOW_PKTs;
            fields[3].flags = BCMI_LT_FIELD_F_SET;
            fields[3].fld_name = MC_YELLOW_PKTs;
            break;
        case bcmCosqStatRedCongestionDroppedPackets:
            fields[2].fld_name = UC_RED_PKTs;
            fields[2].flags = BCMI_LT_FIELD_F_SET;
            fields[3].flags = BCMI_LT_FIELD_F_SET;
            fields[3].fld_name = MC_RED_PKTs;
            break;
        case bcmCosqStatGreenDiscardDroppedPackets:
            /*
             * Green Discard drop is combination of complete drops of red/yellow
             * discard. All the elements can be set using other stat Enums
             */
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        case bcmCosqStatYellowDiscardDroppedPackets:
            fields[2].flags = BCMI_LT_FIELD_F_SET;
            fields[2].fld_name = WRED_YELLOW_PKTs;
            break;
        case bcmCosqStatRedDiscardDroppedPackets:
            fields[2].flags = BCMI_LT_FIELD_F_SET;
            fields[2].fld_name = WRED_RED_PKTs;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    for (itm = 0; itm < device_info.num_itm; itm++) {
        fields[1].u.val = itm;

        if (fields[2].flags == BCMI_LT_FIELD_F_SET) {
            fields[2].u.val = value;
        }

        if (fields[3].flags == BCMI_LT_FIELD_F_SET) {
            fields[3].u.val = value;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set(unit, CTR_EGR_TM_PORT_DROPs, &lt_entry, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_port_queue_drop_counter_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_port_queue_drop_counter_trans_process(unit, CTR_TM_UC_Q_DROPs, TM_UC_Q_IDs,
                                                         UC_QUEUE, BCMLT_OPCODE_INSERT));
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_port_queue_drop_counter_trans_process(unit, CTR_TM_MC_Q_DROPs, TM_MC_Q_IDs,
                                                         MC_QUEUE, BCMLT_OPCODE_INSERT));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_port_queue_drop_counter_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_CONT
        (bcmi_lt_clear(unit, CTR_TM_UC_Q_DROPs));

    SHR_IF_ERR_CONT
        (bcmi_lt_clear(unit, CTR_TM_MC_Q_DROPs));

    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_port_queue_drop_counter_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                                              bcm_cosq_stat_t stat, uint64_t *value, int sync)
{
    int local_port, itm;
    int numq, startq, loop_idx, ci;
    uint64_t sum, byte_sum, wred_sum;
    int num_uc = -1, num_mc = -1;
    int uc_q[2], mc_q[1];
    bcmi_ltsw_cosq_port_map_info_t port_info;
    const char *table_name;
    bcmi_lt_entry_t lt_entry;
    bcmi_ltsw_cosq_device_info_t device_info;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {BUFFER_POOLs,              BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {TM_UC_Q_IDs,               BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 3 */ {PKTs,                      BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 4 */ {BYTEs,                     BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 5 */ {WRED_PKTs,                 BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    if (is_cpu_port(unit, local_port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        if (stat == bcmCosqStatTotalDiscardDroppedPackets) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PORT);
        }
    }

    numq = port_num_queue(unit, local_port);

    if ((cosq < -1) || (cosq >= numq)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }
    fields[0].u.val = local_port;
    LTSW_COMPILER_64_ZERO(sum);
    LTSW_COMPILER_64_ZERO(byte_sum);
    LTSW_COMPILER_64_ZERO(wred_sum);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_gport_port_resolve(unit, gport, &local_port, &startq));
        fields[2].u.val = startq;
        table_name = CTR_TM_UC_Q_DROPs;
        for (itm = 0; itm < device_info.num_itm; itm++) {
            fields[1].u.val = itm;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));

            LTSW_COMPILER_64_ADD_64(sum, fields[3].u.val);
            LTSW_COMPILER_64_ADD_64(byte_sum, fields[4].u.val);
            LTSW_COMPILER_64_ADD_64(wred_sum, fields[5].u.val);
        }
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_gport_port_resolve(unit, gport, &local_port, &startq));
        fields[5].flags = 0;
        fields[2].fld_name = TM_MC_Q_IDs;
        fields[2].u.val = startq;
        table_name = CTR_TM_MC_Q_DROPs;
        for (itm = 0; itm < device_info.num_itm; itm++) {
            fields[1].u.val = itm;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));

            LTSW_COMPILER_64_ADD_64(sum, fields[3].u.val);
            LTSW_COMPILER_64_ADD_64(byte_sum, fields[4].u.val);
        }
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_sched_node_child_get(unit, gport, &num_uc, uc_q,
                                                 &num_mc, mc_q));
        if (num_uc > 2 || num_mc > 1) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
        if (num_uc > 0) {
            for (loop_idx = 0; loop_idx < num_uc; loop_idx++) {
                fields[2].u.val = uc_q[loop_idx];
                table_name = CTR_TM_UC_Q_DROPs;
                for (itm = 0; itm < device_info.num_itm; itm++) {
                    fields[1].u.val = itm;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));
                    LTSW_COMPILER_64_ADD_64(sum, fields[3].u.val);
                    LTSW_COMPILER_64_ADD_64(byte_sum, fields[4].u.val);
                    LTSW_COMPILER_64_ADD_64(wred_sum, fields[5].u.val);
                }
            }
        }
        if (num_mc > 0) {
            for (loop_idx = 0; loop_idx < num_mc; loop_idx++) {
                fields[5].flags = 0;
                fields[2].fld_name = TM_MC_Q_IDs;
                fields[2].u.val = mc_q[loop_idx];
                table_name = CTR_TM_MC_Q_DROPs;
                for (itm = 0; itm < device_info.num_itm; itm++) {
                    fields[1].u.val = itm;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));

                    LTSW_COMPILER_64_ADD_64(sum, fields[3].u.val);
                    LTSW_COMPILER_64_ADD_64(byte_sum, fields[4].u.val);
                }
            }
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_port_map_info_get(unit, local_port, &port_info));

        if (cosq == -1) {
            /* Get UC values for NON CPU port */
            if (!is_cpu_port(unit, local_port)) {
                for (ci = 0; ci < port_info.num_uc_q; ci++) {
                    fields[2].u.val = ci;
                    table_name = CTR_TM_UC_Q_DROPs;
                    for (itm = 0; itm < device_info.num_itm; itm++) {
                        fields[1].u.val = itm;
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));

                        LTSW_COMPILER_64_ADD_64(sum, fields[3].u.val);
                        LTSW_COMPILER_64_ADD_64(byte_sum, fields[4].u.val);
                        LTSW_COMPILER_64_ADD_64(wred_sum, fields[5].u.val);
                    }
                }
            }
            /* Get sum of MC values for any port*/
            for (ci = 0; ci < port_info.num_mc_q; ci++) {
                fields[5].flags = 0;
                fields[2].fld_name = TM_MC_Q_IDs;
                fields[2].u.val = ci;
                table_name = CTR_TM_MC_Q_DROPs;
                for (itm = 0; itm < device_info.num_itm; itm++) {
                    fields[1].u.val = itm;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));

                    LTSW_COMPILER_64_ADD_64(sum, fields[3].u.val);
                    LTSW_COMPILER_64_ADD_64(byte_sum, fields[4].u.val);
                }
            }
        } else {
            if (cosq < port_info.num_uc_q) {
                /* If queue is unicast */
                fields[2].u.val = cosq;
                table_name = CTR_TM_UC_Q_DROPs;
                for (itm = 0; itm < device_info.num_itm; itm++) {
                    fields[1].u.val = itm;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));

                    LTSW_COMPILER_64_ADD_64(sum, fields[3].u.val);
                    LTSW_COMPILER_64_ADD_64(byte_sum, fields[4].u.val);
                    LTSW_COMPILER_64_ADD_64(wred_sum, fields[5].u.val);
                }
            } else {
                /* Multicast queue */
                if (stat == bcmCosqStatTotalDiscardDroppedPackets) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
                ci = cosq - port_info.num_uc_q;
                fields[5].flags = 0;
                fields[2].fld_name = TM_MC_Q_IDs;
                fields[2].u.val = ci;
                table_name = CTR_TM_MC_Q_DROPs;
                for (itm = 0; itm < device_info.num_itm; itm++) {
                    fields[1].u.val = itm;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));

                    LTSW_COMPILER_64_ADD_64(sum, fields[3].u.val);
                    LTSW_COMPILER_64_ADD_64(byte_sum, fields[4].u.val);
                }
            }
        }
    }

    switch (stat) {
        case bcmCosqStatDroppedPackets:
            LTSW_COMPILER_64_COPY(*value, sum);
            LTSW_COMPILER_64_ADD_64(*value, wred_sum);
            break;
        case bcmCosqStatEgressCongestionDroppedPackets:
            LTSW_COMPILER_64_COPY(*value, sum);
            break;
        case bcmCosqStatDroppedBytes:
            LTSW_COMPILER_64_COPY(*value, byte_sum);
            break;
        case bcmCosqStatTotalDiscardDroppedPackets:
            LTSW_COMPILER_64_ADD_64(*value, wred_sum);
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_port_queue_drop_counter_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                                              bcm_cosq_stat_t stat, uint64_t value)
{
    int local_port, itm;
    int numq, startq, loop_idx, ci;
    int num_uc = -1, num_mc = -1;
    int uc_q[2], mc_q[1];
    bcmi_ltsw_cosq_port_map_info_t port_info;
    const char *table_name;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {BUFFER_POOLs,              BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {TM_UC_Q_IDs,               BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 3 */ {NULL,                      0, 0, {0}},
         /* 4 */ {NULL,                      0, 0, {0}},
    };
    bcmi_ltsw_cosq_device_info_t device_info;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    if (is_cpu_port(unit, local_port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        if (stat == bcmCosqStatTotalDiscardDroppedPackets) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PORT);
        }
    }

    numq = port_num_queue(unit, local_port);

    if ((cosq < -1) || (cosq >= numq)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    switch (stat) {
        case bcmCosqStatDroppedPackets:
            fields[3].fld_name = PKTs;
            fields[3].flags = BCMI_LT_FIELD_F_SET;
            fields[4].fld_name = WRED_PKTs;
            fields[4].flags = BCMI_LT_FIELD_F_SET;
            break;
        case bcmCosqStatEgressCongestionDroppedPackets:
            fields[3].fld_name = PKTs;
            fields[3].flags = BCMI_LT_FIELD_F_SET;
            break;
        case bcmCosqStatDroppedBytes:
            fields[3].fld_name = BYTEs;
            fields[3].flags = BCMI_LT_FIELD_F_SET;
            break;
        case bcmCosqStatTotalDiscardDroppedPackets:
            fields[4].fld_name = WRED_PKTs;
            fields[4].flags = BCMI_LT_FIELD_F_SET;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    fields[0].u.val = local_port;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_gport_port_resolve(unit, gport, &local_port, &startq));
        fields[2].u.val = startq;
        table_name = CTR_TM_UC_Q_DROPs;
        for (itm = 0; itm < device_info.num_itm; itm++) {
            fields[1].u.val = itm;

            if (fields[3].flags == BCMI_LT_FIELD_F_SET) {
                fields[3].u.val = value;
            }

            if (fields[4].flags == BCMI_LT_FIELD_F_SET) {
                fields[4].u.val = value;
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_set(unit, table_name, &lt_entry, NULL));
        }
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_gport_port_resolve(unit, gport, &local_port, &startq));
        fields[4].flags = 0;
        fields[2].fld_name = TM_MC_Q_IDs;
        fields[2].u.val = startq;
        table_name = CTR_TM_MC_Q_DROPs;
        for (itm = 0; itm < device_info.num_itm; itm++) {
            fields[1].u.val = itm;

            if (fields[3].flags == BCMI_LT_FIELD_F_SET) {
                fields[3].u.val = value;
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_set(unit, table_name, &lt_entry, NULL));
        }
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_sched_node_child_get(unit, gport, &num_uc, uc_q,
                                                 &num_mc, mc_q));
        if (num_uc > 2 || num_mc > 1) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
        if (num_uc > 0) {
            for (loop_idx = 0; loop_idx < num_uc; loop_idx++) {
                fields[2].u.val = uc_q[loop_idx];
                table_name = CTR_TM_UC_Q_DROPs;
                for (itm = 0; itm < device_info.num_itm; itm++) {
                    fields[1].u.val = itm;
                    if (fields[3].flags == BCMI_LT_FIELD_F_SET) {
                        fields[3].u.val = value;
                    }

                    if (fields[4].flags == BCMI_LT_FIELD_F_SET) {
                        fields[4].u.val = value;
                    }

                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_lt_entry_set(unit, table_name, &lt_entry, NULL));
                }
            }
        }
        if (num_mc > 0) {
            for (loop_idx = 0; loop_idx < num_mc; loop_idx++) {
                fields[4].flags = 0;
                fields[2].fld_name = TM_MC_Q_IDs;
                fields[2].u.val = mc_q[loop_idx];
                table_name = CTR_TM_MC_Q_DROPs;
                for (itm = 0; itm < device_info.num_itm; itm++) {
                    fields[1].u.val = itm;
                    if (fields[3].flags == BCMI_LT_FIELD_F_SET) {
                        fields[3].u.val = value;
                    }

                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_lt_entry_set(unit, table_name, &lt_entry, NULL));
                }
            }
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_port_map_info_get(unit, local_port, &port_info));

        if (cosq == -1) {
            /* Get UC values for NON CPU port */
            if (!is_cpu_port(unit, local_port)) {
                for (ci = 0; ci < port_info.num_uc_q; ci++) {
                    fields[2].u.val = ci;
                    table_name = CTR_TM_UC_Q_DROPs;
                    for (itm = 0; itm < device_info.num_itm; itm++) {
                        fields[1].u.val = itm;
                        if (fields[3].flags == BCMI_LT_FIELD_F_SET) {
                            fields[3].u.val = value;
                        }

                        if (fields[4].flags == BCMI_LT_FIELD_F_SET) {
                            fields[4].u.val = value;
                        }

                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmi_lt_entry_set(unit, table_name, &lt_entry, NULL));
                    }
                }
            }
            /* Get sum of MC values for any port*/
            for (ci = 0; ci < port_info.num_mc_q; ci++) {
                fields[4].flags = 0;
                fields[2].fld_name = TM_MC_Q_IDs;
                fields[2].u.val = ci;
                table_name = CTR_TM_MC_Q_DROPs;
                for (itm = 0; itm < device_info.num_itm; itm++) {
                    fields[1].u.val = itm;
                    if (fields[3].flags == BCMI_LT_FIELD_F_SET) {
                        fields[3].u.val = value;
                    }

                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_lt_entry_set(unit, table_name, &lt_entry, NULL));
                }
            }
        } else {
            if (cosq < port_info.num_uc_q) {
                /* If queue is unicast */
                fields[2].u.val = cosq;
                table_name = CTR_TM_UC_Q_DROPs;
                for (itm = 0; itm < device_info.num_itm; itm++) {
                    fields[1].u.val = itm;
                    if (fields[3].flags == BCMI_LT_FIELD_F_SET) {
                        fields[3].u.val = value;
                    }

                    if (fields[4].flags == BCMI_LT_FIELD_F_SET) {
                        fields[4].u.val = value;
                    }

                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_lt_entry_set(unit, table_name, &lt_entry, NULL));
                }
            } else {
                /* Multicast queue */
                if (stat == bcmCosqStatTotalDiscardDroppedPackets) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
                ci = cosq - port_info.num_uc_q;
                fields[4].flags = 0;
                fields[2].fld_name = TM_MC_Q_IDs;
                fields[2].u.val = ci;
                table_name = CTR_TM_MC_Q_DROPs;
                for (itm = 0; itm < device_info.num_itm; itm++) {
                    fields[1].u.val = itm;
                    if (fields[3].flags == BCMI_LT_FIELD_F_SET) {
                        fields[3].u.val = value;
                    }

                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_lt_entry_set(unit, table_name, &lt_entry, NULL));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_current_usage_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                                    bcm_cosq_stat_t stat, uint64_t *value, int sync)
{
    int local_port, itm;
    int numq, startq;
    uint64_t min_val, shared_val;
    bcmi_ltsw_cosq_port_map_info_t port_info;
    const char *table_name;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {BUFFER_POOLs,              BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {TM_UC_Q_IDs,               BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 3 */ {SHARED_USAGE_CELLSs,       BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 4 */ {MIN_USAGE_CELLSs,          BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmi_ltsw_cosq_device_info_t device_info;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_port_map_info_get(unit, local_port, &port_info));

    numq = port_num_queue(unit, local_port);

    if ((cosq < 0) || (cosq >= numq)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }
    fields[0].u.val = local_port;

    switch (stat) {
        case bcmCosqStatEgressUCQueueMinBytesCurrent:
        case bcmCosqStatEgressUCQueueBytesCurrent:
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
                BCM_GPORT_IS_SCHEDULER(gport)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_cosq_gport_port_resolve(unit, gport,
                                                       &local_port,
                                                       &startq));
            } else {
                if (cosq >= port_info.num_uc_q) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                } else {
                    startq = cosq;
                }
            }
            table_name = CTR_TM_THD_UC_Qs;
            break;
        case bcmCosqStatEgressMCQueueMinBytesCurrent:
        case bcmCosqStatEgressMCQueueBytesCurrent:
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
                BCM_GPORT_IS_SCHEDULER(gport)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_cosq_gport_port_resolve(unit, gport,
                                                       &local_port,
                                                       &startq));
            } else {
                if (cosq < port_info.num_uc_q) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                } else {
                    if (is_cpu_port(unit, local_port)) {
                        startq = cosq;
                    } else {
                        startq = (cosq - port_info.num_uc_q);
                    }
                }
            }
            fields[2].fld_name = TM_MC_Q_IDs;
            table_name = CTR_TM_THD_MC_Qs;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    fields[2].u.val = startq;
    LTSW_COMPILER_64_ZERO(shared_val);
    LTSW_COMPILER_64_ZERO(min_val);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    for (itm = 0; itm < device_info.num_itm; itm++) {
        fields[1].u.val = itm;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));

        LTSW_COMPILER_64_ADD_64(shared_val, fields[3].u.val);
        LTSW_COMPILER_64_ADD_64(min_val, fields[4].u.val);
    }

    switch (stat) {
        case bcmCosqStatEgressUCQueueMinBytesCurrent:
        case bcmCosqStatEgressMCQueueMinBytesCurrent:
            LTSW_COMPILER_64_COPY(*value, min_val);
            break;
        case bcmCosqStatEgressUCQueueBytesCurrent:
        case bcmCosqStatEgressMCQueueBytesCurrent:
            LTSW_COMPILER_64_COPY(*value, shared_val);
            break;
        /*
         * Coverity
         * This is defensive statement.
         */
        /* coverity[dead_error_begin] */
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_port_sp_usage_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                                    bcm_cosq_stat_t stat, uint64_t *value, int sync)
{
    int local_port, itm, pool;
    int numq, startq;
    int start_pool, end_pool;
    uint64_t uc_shared_val, mc_shared_val;
    bcmi_ltsw_cosq_port_map_info_t port_info;
    bcmi_ltsw_cosq_device_info_t device_info = {0};
    bcm_cosq_control_t type;
    const char *table_name = CTR_EGR_TM_PORT_SERVICE_POOLs;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {BUFFER_POOLs,              BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {TM_EGR_SERVICE_POOL_IDs,   BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 3 */ {MC_SHARED_USAGE_CELLSs,    BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 4 */ {UC_SHARED_USAGE_CELLSs,    BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_port_map_info_get(unit, local_port, &port_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    numq = port_num_queue(unit, local_port);
    if ((cosq < BCM_COS_INVALID) || (cosq >= numq)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_thd_egress_pool_get(unit, gport, cosq,
                                       bcmCosqControlMCEgressPool,
                                       &start_pool));
        end_pool = start_pool;
    } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_thd_egress_pool_get(unit, gport, cosq,
                                       bcmCosqControlUCEgressPool,
                                       &start_pool));
        end_pool = start_pool;
    } else {
        if (cosq == BCM_COS_INVALID) {
            start_pool = 0;
            end_pool = device_info.num_service_pool - 1;
        } else {
            if (is_cpu_port(unit, local_port)) {
                startq = cosq;
                type = bcmCosqControlMCEgressPool;
            } else {
                if (cosq >= port_info.num_uc_q) {
                    startq = cosq - port_info.num_uc_q;
                    type = bcmCosqControlMCEgressPool;
                } else {
                    startq = cosq;
                    type = bcmCosqControlUCEgressPool;
                }
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_thd_egress_pool_get(unit, local_port, startq,
                                                    type, &start_pool));
            end_pool = start_pool;
        }
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }
    fields[0].u.val = local_port;
    LTSW_COMPILER_64_ZERO(uc_shared_val);
    LTSW_COMPILER_64_ZERO(mc_shared_val);

    for (itm = 0; itm < device_info.num_itm; itm++) {
        for (pool = start_pool; pool <= end_pool; pool++) {
            fields[1].u.val = itm;
            fields[2].u.val = pool;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));

            LTSW_COMPILER_64_ADD_64(mc_shared_val, fields[3].u.val);
            LTSW_COMPILER_64_ADD_64(uc_shared_val, fields[4].u.val);
        }
    }

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        LTSW_COMPILER_64_COPY(*value, mc_shared_val);
    } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        LTSW_COMPILER_64_COPY(*value, uc_shared_val);
    } else {
        if (cosq == BCM_COS_INVALID) {
            LTSW_COMPILER_64_ADD_64(uc_shared_val, mc_shared_val);
            LTSW_COMPILER_64_COPY(*value, uc_shared_val);
        } else {
            if (is_cpu_port(unit, local_port)) {
                LTSW_COMPILER_64_COPY(*value, mc_shared_val);
            } else {
                if (cosq >= port_info.num_uc_q) {
                    LTSW_COMPILER_64_COPY(*value, mc_shared_val);
                } else {
                    LTSW_COMPILER_64_COPY(*value, uc_shared_val);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_qgroup_min_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                                 bcm_cosq_stat_t stat, uint64_t *value, int sync)
{
    int local_port, itm;
    int numq;
    uint64_t uc_min_val, mc_min_val;
    bcmi_ltsw_cosq_port_map_info_t port_info;
    const char *table_name = CTR_TM_THD_Q_GRPs;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {BUFFER_POOLs,              BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {UC_MIN_USAGE_CELLSs,       BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 3 */ {MC_MIN_USAGE_CELLSs,       BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmi_ltsw_cosq_device_info_t device_info;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_port_map_info_get(unit, local_port, &port_info));

    numq = port_num_queue(unit, local_port);
    if ((cosq < 0) || (cosq >= numq)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }
    fields[0].u.val = local_port;
    LTSW_COMPILER_64_ZERO(uc_min_val);
    LTSW_COMPILER_64_ZERO(mc_min_val);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    for (itm = 0; itm < device_info.num_itm; itm++) {
        fields[1].u.val = itm;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));

        LTSW_COMPILER_64_ADD_64(uc_min_val, fields[2].u.val);
        LTSW_COMPILER_64_ADD_64(mc_min_val, fields[3].u.val);
    }

    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        LTSW_COMPILER_64_COPY(*value, mc_min_val);
    } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        LTSW_COMPILER_64_COPY(*value, uc_min_val);
    } else {
        if (cosq == BCM_COS_INVALID) {
            LTSW_COMPILER_64_ADD_64(uc_min_val, mc_min_val);
            LTSW_COMPILER_64_COPY(*value, uc_min_val);
        } else {
            if (is_cpu_port(unit, local_port)) {
                LTSW_COMPILER_64_COPY(*value, mc_min_val);
            } else {
                if (cosq >= port_info.num_uc_q) {
                    LTSW_COMPILER_64_COPY(*value, mc_min_val);
                } else {
                    LTSW_COMPILER_64_COPY(*value, uc_min_val);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_pg_current_usage_get(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                                       bcm_cosq_stat_t stat, uint64_t *value, int sync)
{
    int local_port;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_PRI_GRP_IDs,            BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {MIN_USAGE_CELLSs,          BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 3 */ {SHARED_USAGE_CELLSs,       BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 4 */ {HEADROOM_USAGE_CELLSs,     BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
        ((cosq < 0) || (cosq >= MMU_NUM_PG))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, port, &local_port));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }

    fields[0].u.val = local_port;
    fields[1].u.val = cosq;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, CTR_ING_TM_PORT_PRI_GRPs,
                           &lt_entry, NULL, NULL));

    switch (stat) {
        case bcmCosqStatIngressPortPGMinBytesCurrent:
            LTSW_COMPILER_64_COPY(*value, fields[2].u.val);
            break;
        case bcmCosqStatIngressPortPGSharedBytesCurrent:
            LTSW_COMPILER_64_COPY(*value, fields[3].u.val);
            break;
        case bcmCosqStatIngressPortPGHeadroomBytesCurrent:
            LTSW_COMPILER_64_COPY(*value, fields[4].u.val);
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_sp_current_usage_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                                       uint64_t *value, int sync)
{
    int local_port;
    int pool = -1;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_ING_SERVICE_POOL_IDs,   BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {SHARED_USAGE_CELLSs,       BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        ((cosq < 0) || (cosq >= MMU_NUM_PG))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_thd_ing_pg_pool_get(unit, gport, cosq,
                                            bcmCosqControlIngressPool,
                                            &pool));
    if (pool == -1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }

    fields[0].u.val = local_port;
    fields[1].u.val = pool;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, CTR_ING_TM_THD_PORT_SERVICE_POOLs,
                           &lt_entry, NULL, NULL));

    LTSW_COMPILER_64_COPY(*value, fields[2].u.val);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_hp_drop_counter_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_hp_drop_counter_trans_process(unit, CTR_ING_TM_THD_HEADROOM_POOL_LOSSLESS_UC_DROPs,
                                                 BCMLT_OPCODE_INSERT));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_hp_drop_counter_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_CONT
        (bcmi_lt_clear(unit, CTR_ING_TM_THD_HEADROOM_POOL_LOSSLESS_UC_DROPs));

    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_hp_drop_counter_get(int unit, bcm_gport_t gport,
                                      bcm_cos_queue_t cosq,
                                      uint64_t *value, int sync)
{
    int local_port, itm = -1;
    int pool = -1;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {BUFFER_POOLs,           BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_HEADROOM_POOL_IDs,   BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {PKTs,                   BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        ((cosq < 0) || (cosq >= MMU_NUM_PG))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_thd_ing_pg_pool_get(unit, gport, cosq,
                                            bcmCosqControlIngressHeadroomPool,
                                            &pool));
    if (pool == -1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }

    fields[1].u.val = pool;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_port_itm_validate(unit, local_port, &itm));

    fields[0].u.val = itm;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, CTR_ING_TM_THD_HEADROOM_POOL_LOSSLESS_UC_DROPs,
                           &lt_entry, NULL, NULL));

    LTSW_COMPILER_64_COPY(*value, fields[2].u.val);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_hp_drop_counter_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                                      uint64_t value)
{
    int local_port, itm = -1;
    int pool = -1;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {BUFFER_POOLs,           BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_HEADROOM_POOL_IDs,   BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {PKTs,                   BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        ((cosq < 0) || (cosq >= MMU_NUM_PG))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_thd_ing_pg_pool_get(unit, gport, cosq,
                                            bcmCosqControlIngressHeadroomPool,
                                            &pool));
    if (pool == -1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_port_itm_validate(unit, local_port, &itm));
    fields[0].u.val = itm;
    fields[1].u.val = pool;
    fields[2].u.val = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, CTR_ING_TM_THD_HEADROOM_POOL_LOSSLESS_UC_DROPs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_hp_current_usage_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync)
{
    int local_port, itm = -1;
    int pool = -1;
    bcmi_lt_entry_t lt_entry;
    const char *tbl_name = CTR_ING_TM_HEADROOM_POOLs;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {BUFFER_POOLs,           BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_HEADROOM_POOL_IDs,   BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {TOTAL_USAGE_CELLSs,     BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        ((cosq < 0) || (cosq >= MMU_NUM_PG))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_thd_ing_pg_pool_get(unit, gport, cosq,
                                            bcmCosqControlIngressHeadroomPool,
                                            &pool));
    if (pool == -1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_port_itm_validate(unit, local_port, &itm));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }

    fields[0].u.val = itm;
    fields[1].u.val = pool;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, tbl_name, &lt_entry, NULL, NULL));

    LTSW_COMPILER_64_COPY(*value, fields[2].u.val);

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "lt %s lookup %s=%d %s=%d %s=%d\n"),
            tbl_name,
            fields[0].fld_name, (int)fields[0].u.val,
            fields[1].fld_name, (int)fields[1].u.val,
            fields[2].fld_name, (int)fields[2].u.val));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_ing_port_drop_counter_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_ing_port_drop_counter_trans_process(unit, CTR_ING_TM_PORT_UC_DROPs,
                                                       BCMLT_OPCODE_INSERT));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_ing_port_drop_counter_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_CONT
        (bcmi_lt_clear(unit, CTR_ING_TM_PORT_UC_DROPs));

    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_ing_port_drop_counter_set(int unit, bcm_gport_t gport,
                                            bcm_cos_queue_t cosq,
                                            uint64_t value)
{
    int local_port;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,           BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {PKTs,               BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (cosq != BCM_COS_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = local_port;
    fields[1].u.val = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, CTR_ING_TM_PORT_UC_DROPs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_ing_port_drop_counter_get(int unit, bcm_gport_t gport,
                                            bcm_cos_queue_t cosq,
                                            uint64_t *value, int sync)
{
    int local_port;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,           BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {PKTs,               BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (cosq != BCM_COS_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }
    fields[0].u.val = local_port;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, CTR_ING_TM_PORT_UC_DROPs,
                           &lt_entry, NULL, NULL));

    LTSW_COMPILER_64_COPY(*value, fields[1].u.val);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_ing_sp_current_usage_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync)
{
    int local_port, itm = -1;
    int pool = -1;
    bcmi_lt_entry_t lt_entry;
    const char *tbl_name = CTR_ING_TM_SERVICE_POOLs;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {BUFFER_POOLs,            BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_ING_SERVICE_POOL_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {TOTAL_USAGE_CELLSs,      BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        ((cosq < 0) || (cosq >= MMU_NUM_PG))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_thd_ing_pg_pool_get(unit, gport, cosq,
                                            bcmCosqControlIngressPool,
                                            &pool));
    if (pool == -1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_port_itm_validate(unit, local_port, &itm));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }

    fields[0].u.val = itm;
    fields[1].u.val = pool;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, tbl_name, &lt_entry, NULL, NULL));

    LTSW_COMPILER_64_COPY(*value, fields[2].u.val);

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "lt %s lookup %s=%d %s=%d %s=%d\n"),
            tbl_name,
            fields[0].fld_name, (int)fields[0].u.val,
            fields[1].fld_name, (int)fields[1].u.val,
            fields[2].fld_name, (int)fields[2].u.val));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_adt_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_adt_low_pri_pkt_trans_process(unit, CTR_TM_THD_DYNAMIC_LOWs,
                                                  BCMLT_OPCODE_INSERT));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_adt_high_pri_pkt_trans_process(unit, CTR_TM_THD_DYNAMIC_HIGHs,
                                                  BCMLT_OPCODE_INSERT));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_adt_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_CONT
        (bcmi_lt_clear(unit, CTR_TM_THD_DYNAMIC_LOWs));

    SHR_IF_ERR_CONT
        (bcmi_lt_clear(unit, CTR_TM_THD_DYNAMIC_HIGHs));

    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_adt_low_pri_pkt_set(int unit, bcm_gport_t gport,
                                        bcm_cos_queue_t cosq,
                                        bcm_cosq_stat_t stat,
                                        uint64_t value)
{
    int itm = -1;
    int i, alpha_index;
    const char *alpha;
    const char *table_name = CTR_TM_THD_DYNAMIC_LOWs;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {BUFFER_POOLs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {LOW_PRI_DYNAMICs,              BCMI_LT_FIELD_F_SET
                 | BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
         /* 2 */ {PKTs,                          BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_ltsw_cosq_device_info_t device_info;
    SHR_FUNC_ENTER(unit);

    if ((cosq != BCM_COS_INVALID) || (gport!= BCM_GPORT_INVALID)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    i = stat - bcmCosqStatAdtAlpha0AdmittedLowPriPackets;
    alpha_index = cosq_adt_counter_to_alpha[i].alpha;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_thd_alpha_bcm_to_lt_convert(unit, alpha_index,
                                                    &alpha));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[1].u.sym_val = alpha;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    for (itm = 0; itm < device_info.num_itm; itm++) {
        fields[0].u.val = itm;
        fields[2].u.val = value;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set(unit, table_name, &lt_entry, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_adt_low_pri_pkt_get(int unit, bcm_gport_t gport,
                                        bcm_cos_queue_t cosq,
                                        bcm_cosq_stat_t stat,
                                        uint64_t *value,
                                        int sync)
{
    int itm = -1;
    int i, alpha_index;
    const char *alpha;
    uint64_t sum;
    const char *table_name = CTR_TM_THD_DYNAMIC_LOWs;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {BUFFER_POOLs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {LOW_PRI_DYNAMICs,              BCMI_LT_FIELD_F_SET
                 | BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
         /* 2 */ {PKTs,                          BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmi_ltsw_cosq_device_info_t device_info;
    SHR_FUNC_ENTER(unit);

    if ((cosq != BCM_COS_INVALID) || (gport!= BCM_GPORT_INVALID)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    i = stat - bcmCosqStatAdtAlpha0AdmittedLowPriPackets;
    alpha_index = cosq_adt_counter_to_alpha[i].alpha;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_thd_alpha_bcm_to_lt_convert(unit, alpha_index,
                                                    &alpha));

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }

    fields[1].u.sym_val = alpha;
    LTSW_COMPILER_64_ZERO(sum);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    for (itm = 0; itm < device_info.num_itm; itm++) {
        fields[0].u.val = itm;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));

        LTSW_COMPILER_64_ADD_64(sum, fields[2].u.val);
    }

    LTSW_COMPILER_64_COPY(*value, sum);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_adt_high_pri_pkt_set(int unit, bcm_gport_t gport,
                                        bcm_cos_queue_t cosq,
                                        bcm_cosq_stat_t stat,
                                        uint64_t value)
{
    int local_port, itm = -1;
    int pool = -1;
    const char *table_name = CTR_TM_THD_DYNAMIC_HIGHs;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {BUFFER_POOLs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_EGR_SERVICE_POOL_IDs,       BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {CELLSs,                         BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_ltsw_cosq_device_info_t device_info;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_thd_egress_pool_get(unit, local_port, cosq,
                                            bcmCosqControlUCEgressPool,
                                            &pool));

    if (pool == -1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[1].u.val = pool;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    for (itm = 0; itm < device_info.num_itm; itm++) {
        fields[0].u.val = itm;
        fields[2].u.val = value;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set(unit, table_name, &lt_entry, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_adt_high_pri_pkt_get(int unit, bcm_gport_t gport,
                                        bcm_cos_queue_t cosq,
                                        bcm_cosq_stat_t stat,
                                        uint64_t *value,
                                        int sync)
{
    int local_port, itm = -1;
    int pool = -1;
    uint64_t sum;
    const char *table_name = CTR_TM_THD_DYNAMIC_HIGHs;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {BUFFER_POOLs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_EGR_SERVICE_POOL_IDs,       BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {CELLSs,                         BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    bcmi_ltsw_cosq_device_info_t device_info;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, gport, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_thd_egress_pool_get(unit, local_port, cosq,
                                            bcmCosqControlUCEgressPool,
                                            &pool));
    if (pool == -1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    if (sync == 1) {
        lt_entry.attr = BCMLT_ENT_ATTR_GET_FROM_HW;
    }

    fields[1].u.val = pool;
    LTSW_COMPILER_64_ZERO(sum);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));
    for (itm = 0; itm < device_info.num_itm; itm++) {
        fields[0].u.val = itm;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get(unit, table_name, &lt_entry, NULL, NULL));

        LTSW_COMPILER_64_ADD_64(sum, fields[2].u.val);
    }

    LTSW_COMPILER_64_COPY(*value, sum);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_debug_stat_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_debug_stat_saf_trans_process(unit, CTR_TM_STORE_AND_FORWARDs,
                                                BCMLT_OPCODE_INSERT));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_debug_stat_ct_trans_process(unit, CTR_TM_CUT_THROUGHs,
                                               BCMLT_OPCODE_INSERT));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_debug_stat_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_CONT
        (bcmi_lt_clear(unit, CTR_TM_STORE_AND_FORWARDs));

    SHR_IF_ERR_CONT
        (bcmi_lt_clear(unit, CTR_TM_CUT_THROUGHs));

    SHR_IF_ERR_CONT
        (bcmi_lt_clear(unit, CTR_TM_STORE_AND_FORWARD_CONTROLs));

    SHR_IF_ERR_CONT
        (bcmi_lt_clear(unit, CTR_TM_CUT_THROUGH_CONTROLs));

    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_debug_stat_match_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_stat_t stat,
    bcm_cosq_debug_stat_match_t *match)
{
    bcm_port_t local_port;
    int itm = -1;
    int counter_id;
    const char *table_name;
    const char *index_field;

    SHR_FUNC_ENTER(unit);

    if (match == NULL){
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Only accept local port */
    if (!BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &local_port));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_port_itm_validate(unit, local_port, &itm));

    if ((stat >= bcmCosqStatDebugSaf0Packets) &&
        (stat <= bcmCosqStatDebugSaf7Packets)) {
        counter_id = stat - bcmCosqStatDebugSaf0Packets;
        table_name = CTR_TM_STORE_AND_FORWARD_CONTROLs;
        index_field = CTR_TM_STORE_AND_FORWARD_IDs;
    } else if ((stat >= bcmCosqStatDebugCt0Packets) &&
               (stat <= bcmCosqStatDebugCt7Packets)) {
        counter_id = stat - bcmCosqStatDebugCt0Packets;
        table_name = CTR_TM_CUT_THROUGH_CONTROLs;
        index_field = CTR_TM_CUT_THROUGH_IDs;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_debug_stat_match_get(unit, table_name, index_field, itm,
                                        counter_id, match));

exit:
    SHR_FUNC_EXIT();
}


int
tm_ltsw_cosq_stat_debug_stat_match_set(
    int unit,
    bcm_port_t port,
    bcm_cosq_stat_t stat,
    bcm_cosq_debug_stat_match_t *match)
{
    bcm_port_t local_port;
    int itm = -1;
    int src_itm = -1;
    int counter_id;
    const char *table_name;
    const char *index_field;

    SHR_FUNC_ENTER(unit);

    if (match == NULL){
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Only accept local port */
    if (!BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &local_port));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_port_itm_validate(unit, local_port, &itm));

    if ((match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_SRC_PORT) ||
        (match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_SRC_PIPE)) {
        /* Only accept local port */
        if (!BCM_GPORT_IS_SET(match->src_port)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_validate(unit, match->src_port,
                                               &local_port));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_port_itm_validate(unit, local_port, &src_itm));
            if (itm != src_itm) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    if ((match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_DST_PORT) ||
        (match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_DST_PIPE)) {
        /* Only accept local port */
        if (!BCM_GPORT_IS_SET(match->dst_port)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_validate(unit, match->dst_port,
                                               &local_port));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    if (match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_PRIORITY_GROUP) {
        if ((match->priority_group_id < 0)
            || (match->priority_group_id  >= MMU_NUM_PG)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    if ((stat >= bcmCosqStatDebugSaf0Packets) &&
        (stat <= bcmCosqStatDebugSaf7Packets)) {
        counter_id = stat - bcmCosqStatDebugSaf0Packets;
        table_name = CTR_TM_STORE_AND_FORWARD_CONTROLs;
        index_field = CTR_TM_STORE_AND_FORWARD_IDs;
    } else if ((stat >= bcmCosqStatDebugCt0Packets) &&
               (stat <= bcmCosqStatDebugCt7Packets)) {
        /* CPU port can't be dst port for CT traffic */
        if ((match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_DST_PORT) ||
            (match->match_flags & BCM_COSQ_DEBUG_STAT_MATCH_DST_PIPE)) {
            if (bcmi_ltsw_port_is_type(unit, match->dst_port,
                                       BCMI_LTSW_PORT_TYPE_CPU)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        }

        counter_id = stat - bcmCosqStatDebugCt0Packets;
        table_name = CTR_TM_CUT_THROUGH_CONTROLs;
        index_field = CTR_TM_CUT_THROUGH_IDs;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_debug_stat_match_set(unit, table_name, index_field, itm,
                                        counter_id, match));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_debug_stat_pkt_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t *value,
    int sync)
{
    bcm_port_t local_port;
    int itm = -1;
    int counter_id;
    const char *table_name;
    const char *index_field;

    SHR_FUNC_ENTER(unit);

    if (cosq != -1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Only accept local port */
    if (!BCM_GPORT_IS_SET(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, gport, &local_port));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_port_itm_validate(unit, local_port, &itm));

    if ((stat >= bcmCosqStatDebugSaf0Packets) &&
        (stat <= bcmCosqStatDebugSaf7Packets)) {
        counter_id = stat - bcmCosqStatDebugSaf0Packets;
        table_name = CTR_TM_STORE_AND_FORWARDs;
        index_field = CTR_TM_STORE_AND_FORWARD_IDs;
    } else if ((stat >= bcmCosqStatDebugCt0Packets) &&
               (stat <= bcmCosqStatDebugCt7Packets)) {
        counter_id = stat - bcmCosqStatDebugCt0Packets;
        table_name = CTR_TM_CUT_THROUGHs;
        index_field = CTR_TM_CUT_THROUGH_IDs;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_debug_stat_pkt_get(unit, table_name, index_field, itm,
                                      counter_id, value, sync));

exit:
    SHR_FUNC_EXIT();
}


int
tm_ltsw_cosq_stat_debug_stat_pkt_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_stat_t stat,
    uint64_t value)
{
    bcm_port_t local_port;
    int itm = -1;
    int counter_id;
    const char *table_name;
    const char *index_field;

    SHR_FUNC_ENTER(unit);

    if (cosq != -1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Only accept local port */
    if (!BCM_GPORT_IS_SET(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, gport, &local_port));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_port_itm_validate(unit, local_port, &itm));

    if ((stat >= bcmCosqStatDebugSaf0Packets) &&
        (stat <= bcmCosqStatDebugSaf7Packets)) {
        counter_id = stat - bcmCosqStatDebugSaf0Packets;
        table_name = CTR_TM_STORE_AND_FORWARDs;
        index_field = CTR_TM_STORE_AND_FORWARD_IDs;
    } else if ((stat >= bcmCosqStatDebugCt0Packets) &&
               (stat <= bcmCosqStatDebugCt7Packets)) {
        counter_id = stat - bcmCosqStatDebugCt0Packets;
        table_name = CTR_TM_CUT_THROUGHs;
        index_field = CTR_TM_CUT_THROUGH_IDs;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_debug_stat_pkt_set(unit, table_name, index_field, itm,
                                      counter_id, value));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_port_queue_counter_clear(int unit, int port, int queue, bool is_uc)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_port_queue_counter_clear(unit, port, queue, is_uc));
exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_port_queue_color_drop_clear(int unit, int port, int buffer_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_port_queue_color_drop_clear(unit, port, buffer_id));
exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_port_queue_drop_counter_clear(
    int unit,
    int port,
    int buffer_id,
    int queue,
    bool is_uc)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_port_queue_drop_counter_clear(unit,
                                                 port,
                                                 buffer_id,
                                                 queue,
                                                 is_uc));
exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_stat_ing_port_drop_counter_clear(int unit, int port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_stat_ing_port_drop_counter_clear(unit, port));
exit:
    SHR_FUNC_EXIT();
}
