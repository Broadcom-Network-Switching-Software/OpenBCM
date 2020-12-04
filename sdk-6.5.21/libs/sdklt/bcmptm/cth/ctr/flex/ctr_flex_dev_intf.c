/*! \file ctr_flex_dev_intf.c
 *
 * Flex counter device interface and dispatch functions.
 *
 * These routines are common across devices.
 * They will call routines for the approprite attached device.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "ctr_flex_internal.h"
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_cci.h>

/*!
 * \brief Read Pkt selector register value
 *
 * \param [in] unit       Unit number.
 * \param [in] ingress    Ingress or Egress direction.
 * \param [in] pool       Pool number of table to read.
 * \param [out] pkt_selector_reg_value Read value.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Illegal pool number.
 */
int
read_pkt_selector_regvalue(int unit,
                           bool ingress,
                           uint32_t pool,
                           uint32_t *pkt_selector_reg_value)
{
    bcmdrd_sid_t *pkt_selector_reg_list = NULL;
    uint32_t num_selectors;
    const bcmptm_ctr_flex_data_driver_t *cb = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_data_driver_get(unit, &cb));
    if (cb == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (cb->pkt_selector_get_fn(unit,
                   ingress,
                   &pkt_selector_reg_list,
                   &num_selectors,
                   NULL, NULL, NULL, NULL));

    if (pool >= num_selectors) {
        return SHR_E_PARAM;
    }

    /* Get register value */
    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ireq_read(unit,
                                    ingress ? CTR_ING_FLEX_ENTRYt:
                                    CTR_EGR_FLEX_ENTRYt,
                                    pkt_selector_reg_list[pool],
                                    0,
                                    pkt_selector_reg_value));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update OFFSET_TABLE with new table map.
 *
 * \param [in] unit     Unit number.
 * \param [in] ingress  Ingress or Egress direction.
 * \param [in] pool    Pool number of offset table to update.
 * \param [in] offset_mode  The offset mode (0,1,2).
 * \param [in] offset_tabl_map  Contains the enable and offset values.
 * \param [in] entry_wsize Width in words of each offset table entry.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Illegal pool number.
 */
int
update_offset_table(int unit,
                    bool ingress,
                    uint32_t pool,
                    uint8_t offset_mode,
                    offset_table_entry_t* offset_table_map)
{
    bcmdrd_sid_t pt_id;
    uint32_t count_enable = 1;
    uint32_t offset_value = 0;
    uint32_t index;
    bcmdrd_sid_t *offset_table_list = NULL;
    uint32_t offset_table_num;
    uint32_t max_counters_per_mode;
    offset_table_fields_t *offset_table_fields = NULL;
    uint32_t *offset_table_entry=NULL;
    uint32_t entry_wsize;
    const bcmptm_ctr_flex_data_driver_t *cb = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_data_driver_get(unit, &cb));
    if (cb == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (cb->offset_table_get_fn(unit,
                    ingress,
                    &offset_table_list,
                    &offset_table_num,
                    &max_counters_per_mode,
                    &entry_wsize,
                    &offset_table_fields));

    if (pool >= offset_table_num) {
        return SHR_E_PARAM;
    }

    pt_id = offset_table_list[pool];

    SHR_ALLOC(offset_table_entry, entry_wsize * 4, "bcmptmCtrFlexOffsetTable");
    SHR_NULL_CHECK(offset_table_entry, SHR_E_MEMORY);

    /* Clear the offset table */
    sal_memset(offset_table_entry, 0 , entry_wsize * 4);

    for (index = 0; index < max_counters_per_mode; index++) {
        /* Set offset table for specified offset_mode */
        count_enable = offset_table_map[index].count_enable;
        offset_value = offset_table_map[index].offset;

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                pt_id,
                                offset_table_entry,
                                offset_table_fields->count_enable,
                                &count_enable));

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                pt_id,
                                offset_table_entry,
                                offset_table_fields->offset,
                                &offset_value));

        /* Write back modified offset table */
        SHR_IF_ERR_EXIT
            (bcmptm_ctr_flex_ireq_write(unit,
                                        ingress ? CTR_ING_FLEX_ENTRYt :
                                        CTR_EGR_FLEX_ENTRYt,
                                        pt_id,
                                        offset_mode * 256 + index,
                                        offset_table_entry));
    }

exit:
    if (offset_table_entry) {
        SHR_FREE(offset_table_entry);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update Pkt selector register
 *
 * \param [in] unit Unit number.
 * \param [in] ingress Ingress or Egress direction.
 * \param [in] pool Pool number of table to read.
 * \param [in] pkt_attr_total_bits Bit width of this attribute
 * \param [in] pkt_attr_bit_position Bit position on h/w attribute bus.
 * \param [in] current_bit_position Bit position on pkt_selector_reg.
 * \param [in] pkt_selector_reg_value Contents of pkt_selector_reg.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Illegal pool number.
 * \retval SHR_E_INTERNAL Exceeded max bits on pkt_selector_reg.
 */
int
update_pkt_attr_selector_bits(int unit,
                              bool ingress,
                              bool set_bit,
                              uint32_t key_id,
                              uint32_t pkt_attr_total_bits,
                              uint32_t pkt_attr_bit_position,
                              uint8_t  *current_bit_position,
                              uint32_t *pkt_selector_reg_value)
{
    int rv = SHR_E_NONE;
    uint32_t regVal;
    uint32_t index;
    bcmdrd_sid_t pkt_selector_reg;
    bcmdrd_sid_t *pkt_selector_reg_list;
    bcmdrd_fid_t *pkt_selector_for_bit_field_list;
    bcmdrd_fid_t *pkt_selector_bit_en_field_list;
    uint32_t num_selectors;
    uint32_t num_fields;
    const bcmptm_ctr_flex_data_driver_t *cb = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_data_driver_get(unit, &cb));
    if (cb == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (cb->pkt_selector_get_fn(unit,
                    ingress,
                    &pkt_selector_reg_list,
                    &num_selectors,
                    &pkt_selector_for_bit_field_list,
                    &pkt_selector_bit_en_field_list,
                    &num_fields,
                    NULL));

    if (key_id >= num_selectors) {
        return SHR_E_PARAM;
    }

    pkt_selector_reg = pkt_selector_reg_list[key_id];

    if (*current_bit_position + pkt_attr_total_bits > num_fields) {
        rv = SHR_E_INTERNAL;
        SHR_ERR_EXIT(rv);
    }

    for (index = 0; index < pkt_attr_total_bits; index++) {
        /* If set bit, set value to 1; If clear bit, set value to 0 */
        regVal = set_bit ? 1 : 0;
        bcmdrd_pt_field_set(unit,
                            pkt_selector_reg,
                            pkt_selector_reg_value,
                            pkt_selector_bit_en_field_list[*current_bit_position],
                            &regVal);

        regVal = set_bit ? (pkt_attr_bit_position + index) : 0;
        bcmdrd_pt_field_set(unit,
                            pkt_selector_reg,
                            pkt_selector_reg_value,
                            pkt_selector_for_bit_field_list[*current_bit_position],
                            &regVal);
        *current_bit_position +=1;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Write Pkt selector register
 *
 * \param [in] unit       Unit number.
 * \param [in] ingress    Ingress or Egress direction.
 * \param [in] pool       Pool number of table to read.
 * \param [in] pkt_selector_reg_value Write value.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Illegal pool number.
 */
int
write_pkt_attr_selectors(int unit,
                        bool ingress,
                        uint32_t pool,
                        uint32_t *pkt_selector_reg_value)
{
    uint32_t  zero_field = {0};
    bcmdrd_sid_t pkt_selector_reg;
    bcmdrd_sid_t *pkt_selector_reg_list;
    pkt_key_selector_udf_fields_t *pkt_selector_udf_field_list;
    uint32_t num_selectors;
    const bcmptm_ctr_flex_data_driver_t *cb = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_data_driver_get(unit, &cb));
    if (cb == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (cb->pkt_selector_get_fn(unit,
                    ingress,
                    &pkt_selector_reg_list,
                    &num_selectors,
                    NULL,
                    NULL,
                    NULL,
                    &pkt_selector_udf_field_list));

    if (pool >= num_selectors) {
        return SHR_E_PARAM;
    }

    pkt_selector_reg = pkt_selector_reg_list[pool];

    /* Get register value
    uint32_t entry_sz;
    entry_sz = bcmdrd_pt_entry_wsize(unit, pkt_selector_reg);
    entry_sz *= 4;
    uint32_t *pkt_selector_read_value = NULL;
    SHR_ALLOC(pkt_selector_read_value, entry_sz, "Packet Selector entry");
    SHR_NULL_CHECK(pkt_selector_read_value, SHR_E_MEMORY);
    sal_memset(pkt_selector_read_value, 0, entry_sz);

    bcmptm_ctr_flex_ireq_read(unit,
                              CTR_ING_FLEX_ENTRYt,
                              ingress_pkt_attr_selector_key_reg[pool],
                              0,
                              pkt_selector_read_value);
    */

    /* Setup required bits */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                            pkt_selector_reg,
                            pkt_selector_reg_value,
                            pkt_selector_udf_field_list->user_specified_udf_valid, &zero_field));

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit,
                            pkt_selector_reg,
                            pkt_selector_reg_value,
                            pkt_selector_udf_field_list->use_udf_key, &zero_field));

    if (pkt_selector_udf_field_list->use_compressed_pkt_key) {
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                pkt_selector_reg,
                                pkt_selector_reg_value,
                                pkt_selector_udf_field_list->use_compressed_pkt_key, &zero_field));
    }

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ireq_write(unit,
                                    ingress ? CTR_ING_FLEX_ENTRYt :
                                    CTR_EGR_FLEX_ENTRYt,
                                    pkt_selector_reg,
                                    0,
                                    pkt_selector_reg_value));

exit:
    /*
    if (pkt_selector_read_value) {
      SHR_FREE(pkt_selector_read_value);
    }
    */
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable counter pool
 *
 * \param [in] unit    Unit number.
 * \param [in] ingress Ingress or Egress direction.
 * \param [in] all_pools Apply to all pools.
 * \param [in] pool_num  Pool number when all_pools is false.
 * \param [in] enable  TRUE=enable, FALSE=disable
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Illegal pool number.
 */
int
enable_counter_pool (int unit,
                     bool ingress,
                     bool all_pools,
                     uint32_t pool_num,
                     bool enable)
{
    bcmdrd_sid_t counter_table_sid;
    bcmdrd_sid_t *counter_table_list;
    bcmdrd_sid_t *update_control_list;
    uint32_t num_counter_table;
    uint32_t num_update_control;
    uint32_t counter_pool_enable_fid;
    int i, pool_start, pool_end;
    const bcmptm_ctr_flex_data_driver_t *cb = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_data_driver_get(unit, &cb));
    if (cb == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Get counter table SID and update control SID */
    SHR_IF_ERR_EXIT
        (cb->counter_table_get_fn(unit,
                    ingress,
                    &counter_table_list,
                    &num_counter_table));

    SHR_IF_ERR_EXIT
        (cb->update_control_get_fn(unit,
                    ingress,
                    &update_control_list,
                    &num_update_control,
                    &counter_pool_enable_fid));


    if (all_pools) {
        pool_start = 0;
        pool_end = num_counter_table;
    }
    else {
        if ((pool_num >= num_counter_table) || (pool_num >= num_update_control)) {
            return SHR_E_PARAM;
        }
        pool_start = pool_num;
        pool_end = pool_num + 1;
    }

    for (i=pool_start; i<pool_end; i++) {
        bcmptm_cci_pool_state_t state;
        state = (enable) ? ENABLE : DISABLE;
        counter_table_sid = counter_table_list[i];

        /* Enable/Disable CCI counter eviction */
        SHR_IF_ERR_EXIT(
            bcmptm_cci_flex_counter_pool_set_state(unit, counter_table_sid, state));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get counter pool SID
 *
 * \param [in] unit    Unit number.
 * \param [in] ingress Ingress or Egress direction.
 * \param [in] pool    Pool number of table.
 * \param [out] table_sid Counter table SID for this pool.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Illegal pool number.
 */
int
get_counter_sid (int unit,
                 bool ingress,
                 uint32_t pool,
                 bcmdrd_sid_t *table_sid)
{
    bcmdrd_sid_t *counter_table_list;
    uint32_t num_counter_table;
    const bcmptm_ctr_flex_data_driver_t *cb = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_data_driver_get(unit, &cb));
    if (cb == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (cb->counter_table_get_fn(unit,
                    ingress,
                    &counter_table_list,
                    &num_counter_table));

    if (pool >= num_counter_table) {
        return SHR_E_PARAM;
    }

    *table_sid = counter_table_list[pool];

exit:
    SHR_FUNC_EXIT();
}
