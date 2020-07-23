/** \file kbp_rop.c
 *
 * Functions for handling Jericho2 KBP ROP.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_ELKDNX

/*
 * {
 */

/*************
 * INCLUDES  *
 */
/*
 * {
 */

#include <shared/shrextend/shrextend_debug.h>

#include <soc/util.h>

#include <shared/bsl.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

#if defined(INCLUDE_KBP)

#include <bcm_int/dnx/kbp/kbp_connectivity.h>
#include <bcm_int/dnx/kbp/kbp_rop.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <bcm_int/dnx/kbp/kbp_recover.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#define DNX_KBP_ROP_TEST_DATA  {0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef, 0x12, 0x35}
#define DNX_KBP_SCRATCH_PAD_REG_ADDR      (0xe)
#define DNX_KBP_SCRATCH_PAD_REG_DATA_LEN  (0x8)

#define DNX_KBP_PIO_WRITE_DATA_BYTE_LEN   (0xa)
#define DNX_KBP_PIO_READ_DATA_BYTE_LEN    (0xa)
#define DNX_KBP_PIO_READ_STATUS_BYTE_LEN  (0x1)

#define DNX_KBP_ROP_GET_SUPPORTED_FLAGS   (DNX_KBP_ROP_GET_DNX_SIDE | DNX_KBP_ROP_GET_KBP_SIDE)

#define DNX_KBP_OP_DEV0_LAST_REQUEST_PORT0_REG_ADDR   0x90aa
#define DNX_KBP_OP_DEV0_LAST_REQUEST_PORT1_REG_ADDR   0x912a
#define DNX_KBP_OP_DEV0_LAST_REQUEST_REG_DEVAD        0x1

#define DNX_KBP_OP_LAST_RESPONSE_PORT0_REG_ADDR       0x2f16d
#define DNX_KBP_OP_LAST_RESPONSE_PORT1_REG_ADDR       0x2f1f2
#define DNX_KBP_OP_LAST_RESPONSE_REG_DEVAD            0x2

#define DNX_KBP_OP_LAST_REQUEST_REG_COUNT_PER_PORT    0x80
#define DNX_KBP_OP_LAST_RESPONSE_REG_COUNT_PER_PORT   0x84

#define DNX_KBP_RECOVER_ENABLE(unit)      (dnx_data_elk.recovery.enable_get(unit))
#define DNX_KBP_IS_ENABLE(unit)           (dnx_data_elk.general.device_type_get(unit) != DNX_KBP_DEV_TYPE_NONE)
#define DNX_KBP_ROP_RETRANSMISSION(unit)  (dnx_data_elk.recovery.iteration_get(unit))

#define DNX_KBP_APP_DATA_CHECK(_unit_) \
    if (Kbp_app_data[_unit_] == NULL) \
    { \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " KBP was not initialized for the device.\n"); \
    }

extern generic_kbp_app_data_t *Kbp_app_data[SOC_MAX_NUM_DEVICES];

/*
 * }
 */

/*************
 * FUNCTIONS *
 */
/*
 * {
 */
#ifdef BE_HOST
void
dnx_kbp_buffer_endians_fix(
    int unit,
    soc_reg_above_64_val_t buffer)
{
    uint32 i;
    for (i = 0; i < sizeof(soc_reg_above_64_val_t) / 4; ++i)
    {
        buffer[i] = _shr_swap32(buffer[i]);
    }
}
#endif /* BE_HOST */

/**
 * \brief
 *   Perform ROP test. First send ROP data to the
 *   KBP scratch register and then read the data.
 *   Compare write and read data.
 *
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_rop_scratch_write_read(
    int unit,
    bcm_core_t core)
{
    uint32 nbo_addr, addr = DNX_KBP_SCRATCH_PAD_REG_ADDR;
    uint8 data[NLM_DATA_WIDTH_BYTES] = DNX_KBP_ROP_TEST_DATA;
    dnx_kbp_rop_write_t wr_data;
    dnx_kbp_rop_read_t rd_data;

    SHR_FUNC_INIT_VARS(unit);

    nbo_addr = soc_htonl(addr);
    sal_memcpy(wr_data.addr, &nbo_addr, sizeof(uint32));
    sal_memset(wr_data.mask, 0x0, NLM_DATA_WIDTH_BYTES);
    sal_memset(wr_data.addr_short, 0x0, NLMDEV_REG_ADDR_LEN_IN_BYTES);
    wr_data.writeMode = NLM_DNX_WRITE_MODE_DATABASE_DM;

    /*
     * Set wr_data
     */
    DNX_KBP_ROP_REVERSE_DATA(data, wr_data.data, 10);

    /*
     * Set rd_data
     */
    sal_memcpy(rd_data.addr, &nbo_addr, sizeof(uint32));
    rd_data.dataType = NLM_DNX_READ_MODE_DATA_X;

    /*
     * ROP write and ROP read
     */
    SHR_IF_ERR_EXIT(dnx_kbp_rop_write(unit, core, &wr_data));
    SHR_IF_ERR_EXIT(dnx_kbp_rop_write(unit, core, &wr_data));
    SHR_IF_ERR_EXIT(dnx_kbp_rop_read(unit, core, &rd_data));

    /*
     * Comparing Read data with Written data, Scratch PD is only 8 bytes in Optimus Prime
     */
    if (sal_memcmp
        (rd_data.data +
         (DNX_KBP_PIO_READ_DATA_BYTE_LEN - DNX_KBP_SCRATCH_PAD_REG_DATA_LEN + DNX_KBP_PIO_READ_STATUS_BYTE_LEN),
         wr_data.data + (DNX_KBP_PIO_WRITE_DATA_BYTE_LEN - DNX_KBP_SCRATCH_PAD_REG_DATA_LEN),
         DNX_KBP_SCRATCH_PAD_REG_DATA_LEN))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "KBP ROP write-read failed. read data does no match expected data\n");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_kbp_init_rop_test(
    int unit)
{
    int iter, kbp_mdio_id;
    bcm_core_t core;
    uint32 recover_iter;
    SHR_FUNC_INIT_VARS(unit);

    if (Kbp_app_data[unit]->device_type == KBP_DEVICE_OP2)
    {
        /*
         * For OP2, we don't need ROP test.
         */
        SHR_EXIT();
    }

    recover_iter = DNX_KBP_ROP_RETRANSMISSION(unit);
    kbp_mdio_id = dnx_data_elk.connectivity.mdio_get(unit, 0)->mdio_id;

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_op_link_stability_check(unit, core, kbp_mdio_id, recover_iter));
        SHR_IF_ERR_CONT(dnx_kbp_rop_scratch_write_read(unit, core));

        if (SHR_FUNC_ERR())
        {
            if (DNX_KBP_RECOVER_ENABLE(unit))
            {
                for (iter = 0; iter < recover_iter; iter++)
                {
                    SHR_IF_ERR_EXIT(dnx_kbp_recover_run_recovery_sequence(unit, core, kbp_mdio_id));
                    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                    SHR_IF_ERR_CONT(dnx_kbp_rop_scratch_write_read(unit, core));

                    if (!SHR_FUNC_ERR())
                    {
                        /*
                         * KBP Recovery succeeded
                         */
                        LOG_INFO(BSL_LOG_MODULE,
                                 (BSL_META_U(unit, "KBP recovery succeeded within %d iterations.\n"), iter + 1));
                        break;
                    }
                    else
                    {
                        if (iter < recover_iter - 1)
                        {
                            LOG_INFO(BSL_LOG_MODULE,
                                     (BSL_META_U(unit, "KBP recovery iteration %d/%d failed, trying again.\n"),
                                      iter + 1, recover_iter));
                        }
                        else
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL, "KBP recovery failed.\n");
                        }
                    }
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Error happened during ROP test on core %d.\n", core);
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get KBP record counter.
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] counter_val - counter value.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_cpu_record_counter_polling(
    int unit,
    uint32 core,
    uint32 expect_val)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGW_RECORD_REPLY_COUNTER, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    SHR_IF_ERR_EXIT(dnxcmn_polling
                    (unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id, DBAL_FIELD_LOOKUP_REPLY_COUNTER,
                     expect_val));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Assemble the KBP CPU record.
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] opcode - KBP opcode number.
 * \param [in] msb_data - MSB data.
 * \param [in] lsb_data - LSB data.
 * \param [in] lsb_enable - enable the LSB data.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_cpu_record_set(
    int unit,
    uint32 core,
    uint32 opcode,
    soc_reg_above_64_val_t msb_data,
    soc_reg_above_64_val_t lsb_data,
    int lsb_enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGW_RECORD_REQUEST, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RECORD_OPCODE, INST_SINGLE, opcode);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_RECORD_DATA_MSB, INST_SINGLE, msb_data);

    if (lsb_enable)
    {
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_RECORD_DATA_LSB, INST_SINGLE, lsb_data);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Trigger the CPU to send the record to KBP.
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] trigger - rigger enable.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_cpu_record_trigger_set(
    int unit,
    uint32 core,
    dnx_kbp_record_trigger_t trigger)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGW_CPU_RECORD_CTRL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RECORD_DATA_LSB_TRIGGER, INST_SINGLE,
                                 trigger.lsb_trigger);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RECORD_DATA_MSB_TRIGGER, INST_SINGLE,
                                 trigger.msb_trigger);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RECORD_TRIGGER, INST_SINGLE, trigger.record_trigger);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Send the cpu record.
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] opcode - KBP opcode number.
 * \param [in] msb_data - MSB data.
 * \param [in] lsb_data - LSB data.
 * \param [in] lsb_enable - Enable the LSB data.
 * \param [out] read_data - Response data.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_cpu_record_send(
    int unit,
    uint32 core,
    uint32 opcode,
    soc_reg_above_64_val_t msb_data,
    soc_reg_above_64_val_t lsb_data,
    int lsb_enable,
    soc_reg_above_64_val_t read_data)
{
    dnx_kbp_record_trigger_t trigger;

#ifndef PLISIM
    int retransmission_num = DNX_KBP_ERROR_RETRANMISSION;
    dnx_kbp_record_reply_t record_reply;
#endif

    SHR_FUNC_INIT_VARS(unit);

#ifndef PLISIM
retransmit:
#endif
    /*
     * Clear the counter
     */
    SHR_IF_ERR_EXIT(dnx_kbp_cpu_record_counter_polling(unit, core, 0));

    /*
     * Record trigger
     */
    trigger.lsb_trigger = lsb_enable ? 1 : 0;
    trigger.msb_trigger = lsb_enable ? 0 : 1;
    trigger.record_trigger = 0;

    SHR_IF_ERR_EXIT(dnx_kbp_cpu_record_trigger_set(unit, core, trigger));
    /*
     * Assemble the cpu record
     */
    SHR_IF_ERR_EXIT(dnx_kbp_cpu_record_set(unit, core, opcode, msb_data, lsb_data, lsb_enable));

#ifndef PLISIM
    /*
     * Polling to validate reply is ready.
     */
    SHR_IF_ERR_EXIT(dnx_kbp_cpu_record_counter_polling(unit, core, 1));

    /*
     * reading the reply register
     */
    SHR_IF_ERR_EXIT(dnx_kbp_cpu_lookup_reply(unit, core, &record_reply));

    if ((record_reply.rop_error == 1) || (record_reply.rop_error == 1))
    {
        if (retransmission_num)
        {
            retransmission_num--;
            goto retransmit;
        }
        else
        {
            /*
             * ROP or ELK error
             */
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "ROP_ERROR=%d, ELK_ERROR=%d on core %d, expect ROP_ERROR & ELK_ERROR are both 0!\n",
                         record_reply.rop_error, record_reply.elk_error, core);
        }
    }

    /*
     * in this case, the rop is a request for data so we are reading the reply
     */
    if (read_data != NULL)
    {
        sal_memcpy(read_data, record_reply.reply_data, sizeof(soc_reg_above_64_val_t));
    }

    if (record_reply.reply_valid != 0x1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "LOOKUP_REPLY_VALIDf field not asserted correctly!\n");
    }

#endif

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Get the reply data.
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [out] record_reply - Reply data.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_cpu_lookup_reply(
    int unit,
    uint32 core,
    dnx_kbp_record_reply_t * reccord_reply)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGW_RECORD_REPLY, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_REPLY_VALID, INST_SINGLE, &reccord_reply->reply_valid);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_REPLY_RECORD, INST_SINGLE,
                                   reccord_reply->reply_data);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_REPLY_ROP_ERROR, INST_SINGLE,
                               &reccord_reply->rop_error);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_REPLY_ELK_ERROR, INST_SINGLE,
                               &reccord_reply->elk_error);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}
/**
 * \brief
 *   Write the LUT.
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] addr - LUT addr.
 * \param [in] lut_data - LUT data.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_lut_write(
    int unit,
    uint32 core,
    uint8 addr,
    dnx_kbp_lut_data_t * lut_data)
{
    uint32 ltr;
    uint16 first = 0, second = 0;
    uint16 dummy_val;

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_APP_DATA_CHECK(unit);
    SHR_NULL_CHECK(lut_data, _SHR_E_PARAM, "lut_data");

    /*
     * address validation
     */
    ltr = DNX_KBP_ROP_LUT_LTR_GET(lut_data->instr);

    /*
     * set addr bits
     */
    first = addr & DNX_KBP_LUT_DATA_ADDR_MASK;
    first |= ((lut_data->rec_size & DNX_KBP_LUT_REC_SIZE_MASK) << DNX_KBP_LUT_DATA_ADDR_BIT_LEN);
    first |= ((lut_data->rec_is_valid & DNX_KBP_LUT_REC_VALID_MASK) << DNX_KBP_LUTWR_REC_VALID_START_BIT);

    second |= ((ltr & DNX_KBP_LUT_LTR_MASK) << DNX_KBP_LUT_MODE_BIT_LEN);
    second |= (1 << DNX_KBP_LUTWR_CMP_INSTR_START_BIT);

    SHR_IF_ERR_EXIT(dnx_kbp_callback_mdio_write
                    (&Kbp_app_data[unit]->user_data[0], 0, DNX_KBP_LUT_PROG_REG_DEVAD_PORT0,
                     DNX_KBP_LUT_PROG0_REG_ADDR_PORT0, first));
    SHR_IF_ERR_EXIT(dnx_kbp_callback_mdio_write
                    (&Kbp_app_data[unit]->user_data[0], 0, DNX_KBP_LUT_PROG_REG_DEVAD_PORT0,
                     DNX_KBP_LUT_PROG1_REG_ADDR_PORT0, second));
    SHR_IF_ERR_EXIT(dnx_kbp_callback_mdio_write
                    (&Kbp_app_data[unit]->user_data[0], 0, DNX_KBP_LUT_PROG_REG_DEVAD_PORT1,
                     DNX_KBP_LUT_PROG0_REG_ADDR_PORT1, first));
    SHR_IF_ERR_EXIT(dnx_kbp_callback_mdio_write
                    (&Kbp_app_data[unit]->user_data[0], 0, DNX_KBP_LUT_PROG_REG_DEVAD_PORT1,
                     DNX_KBP_LUT_PROG1_REG_ADDR_PORT1, second));

    /** Dummy read to avoid MDIO lock up */
    SHR_IF_ERR_EXIT(dnx_kbp_callback_mdio_read
                    (&Kbp_app_data[unit]->user_data[0], 0, 1, DNX_KBP_LUT_PROG0_REG_ADDR_PORT0, &dummy_val));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Read the LUT.
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] addr - LUT addr.
 * \param [out] lut_data - LUT data.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_lut_read(
    int unit,
    uint32 core,
    uint8 addr,
    dnx_kbp_lut_data_t * lut_data)
{
    uint16 first = 0, second = 0;
    uint32 ltr;

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_APP_DATA_CHECK(unit);
    SHR_NULL_CHECK(lut_data, _SHR_E_PARAM, "lut_data");

    addr = addr & DNX_KBP_LUT_DATA_ADDR_MASK;
    SHR_IF_ERR_EXIT(dnx_kbp_callback_mdio_write
                    (&Kbp_app_data[unit]->user_data[0], 0, 1, DNX_KBP_LUT_RDADDR_REG_ADDR_PORT0, addr));

    SHR_IF_ERR_EXIT(dnx_kbp_callback_mdio_read
                    (&Kbp_app_data[unit]->user_data[0], 0, 1, DNX_KBP_LUT_RDATA0_REG_ADDR_PORT0, &first));
    SHR_IF_ERR_EXIT(dnx_kbp_callback_mdio_read
                    (&Kbp_app_data[unit]->user_data[0], 0, 1, DNX_KBP_LUT_RDATA1_REG_ADDR_PORT0, &second));

    lut_data->rec_size = (first >> DNX_KBP_LUTRD_REC_SIZE_START_BIT) & DNX_KBP_LUT_REC_SIZE_MASK;
    lut_data->rec_is_valid = (first >> DNX_KBP_LUTRD_REC_VALID_START_BIT) & DNX_KBP_LUT_REC_VALID_MASK;
    ltr = (first >> DNX_KBP_LUTRD_INSTR_START_BIT) & DNX_KBP_LUT_LTR_MASK;
    lut_data->instr = DNX_KBP_ROP_LUT_INSTR_GET(ltr);

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   ROP write function.
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] wr_data - ROP data.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_rop_write(
    int unit,
    uint32 core,
    dnx_kbp_rop_write_t * wr_data)
{
    soc_reg_above_64_val_t msb_data, lsb_data;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(msb_data);
    SOC_REG_ABOVE_64_CLEAR(lsb_data);

    /*
     * build the buffer addr[0:3] + data[4:13] + mask[14:23] and change the order for IHB
     */
    DNX_KBP_ROP_REVERSE_DATA(wr_data->addr,
                             (((uint8 *) msb_data) + DNX_KBP_CPU_DATA_REGISTER_BYTE_LEN - NLMDEV_REG_ADDR_LEN_IN_BYTES),
                             NLMDEV_REG_ADDR_LEN_IN_BYTES);
    DNX_KBP_ROP_REVERSE_DATA(wr_data->data,
                             (((uint8 *) msb_data) + DNX_KBP_CPU_DATA_REGISTER_BYTE_LEN - NLMDEV_REG_ADDR_LEN_IN_BYTES -
                              NLM_DATA_WIDTH_BYTES), NLM_DATA_WIDTH_BYTES);
    DNX_KBP_ROP_REVERSE_DATA(wr_data->mask,
                             (((uint8 *) msb_data) + DNX_KBP_CPU_DATA_REGISTER_BYTE_LEN - NLMDEV_REG_ADDR_LEN_IN_BYTES -
                              NLM_DATA_WIDTH_BYTES - NLM_DATA_WIDTH_BYTES), NLM_DATA_WIDTH_BYTES);

    /*
     * little endian buffer because of the ARAD_KBP_ROP_REVERSE_DATA
     */
#if defined(BE_HOST)
    dnx_kbp_buffer_endians_fix(unit, msb_data);
#endif

    SHR_IF_ERR_EXIT(dnx_kbp_cpu_record_send(unit, core, DNX_KBP_CPU_PIOWR_OPCODE, msb_data, lsb_data, 0, NULL));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   ROP read function.
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [out] rd_data - ROP data.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_rop_read(
    int unit,
    uint32 core,
    dnx_kbp_rop_read_t * rd_data)
{
    uint32 opcode, *addr;
    uint8 receive_data[DNX_KBP_CPU_DATA_REGISTER_BYTE_LEN] = { 0 };
    soc_reg_above_64_val_t msb_data, lsb_data, read_data;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(msb_data);
    SOC_REG_ABOVE_64_CLEAR(lsb_data);

    if (rd_data->dataType == NLM_DNX_READ_MODE_DATA_X)
    {
        opcode = DNX_KBP_CPU_PIORDX_OPCODE;
    }
    else if (rd_data->dataType == NLM_DNX_READ_MODE_DATA_Y)
    {
        opcode = DNX_KBP_CPU_PIORDY_OPCODE;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported data type %d \n", rd_data->dataType);
    }

    sal_memcpy(((uint8 *) msb_data) + DNX_KBP_CPU_DATA_REGISTER_BYTE_LEN - NLMDEV_REG_ADDR_LEN_IN_BYTES,
               rd_data->addr, NLMDEV_REG_ADDR_LEN_IN_BYTES);

    /*
     * Change the Addr back to HBO
     */
    addr = &msb_data[(DNX_KBP_CPU_DATA_REGISTER_BYTE_LEN - NLMDEV_REG_ADDR_LEN_IN_BYTES) / 4];
    *addr = soc_ntohl(*addr);

    SHR_IF_ERR_EXIT(dnx_kbp_cpu_record_send(unit, core, opcode, msb_data, lsb_data, 0, read_data));

    /*
     * reversed buffer
     */
#if defined(BE_HOST)
    dnx_kbp_buffer_endians_fix(unit, read_data);
#endif

    DNX_KBP_ROP_REVERSE_DATA(((uint8 *) read_data), receive_data, DNX_KBP_CPU_LOOKUP_REPLY_DATA_REGISTER_BYTE_LEN);
    sal_memcpy(rd_data->data, receive_data, NLM_DATA_WIDTH_BYTES + 1);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   ROP compare operation.
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [inout] cbw_cmp_data - compare data and result.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_rop_cbw_cmp(
    int unit,
    uint32 core,
    dnx_kbp_rop_cbw_cmp_t * cbw_cmp_data)
{
    uint32 cbw_data_bytes_left, cmp_resp_data_len;
    int lsb_enable;
    soc_reg_above_64_val_t msb_data, lsb_data, above_64_cmp_data;
    uint8 cmp_data[NLMDEV_MAX_RESP_LEN_IN_BYTES];

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(msb_data);
    SOC_REG_ABOVE_64_CLEAR(lsb_data);

    sal_memset(cmp_data, 0x0, sizeof(uint8) * NLMDEV_MAX_RESP_LEN_IN_BYTES);

    if (cbw_cmp_data->cbw_data.data_len <= DNX_KBP_CPU_DATA_REGISTER_BYTE_LEN)
    {
        sal_memcpy(((uint8 *) msb_data) + DNX_KBP_CPU_DATA_REGISTER_BYTE_LEN - cbw_cmp_data->cbw_data.data_len,
                   cbw_cmp_data->cbw_data.data, cbw_cmp_data->cbw_data.data_len);
        lsb_enable = 0;
    }
    else
    {
        cbw_data_bytes_left = cbw_cmp_data->cbw_data.data_len - DNX_KBP_CPU_DATA_REGISTER_BYTE_LEN;
        sal_memcpy(((uint8 *) msb_data), cbw_cmp_data->cbw_data.data + cbw_data_bytes_left,
                   DNX_KBP_CPU_DATA_REGISTER_BYTE_LEN);
        sal_memcpy(((uint8 *) lsb_data) + DNX_KBP_CPU_DATA_REGISTER_BYTE_LEN - cbw_data_bytes_left,
                   cbw_cmp_data->cbw_data.data, cbw_data_bytes_left);
    }
    /*
     * reversed buffer
     */
#if defined(BE_HOST)
    if (lsb_enable)
    {
        dnx_kbp_buffer_endians_fix(unit, lsb_data);
    }
    dnx_kbp_buffer_endians_fix(unit, msb_data);
#endif

    SHR_IF_ERR_EXIT(dnx_kbp_cpu_record_send
                    (unit, core, cbw_cmp_data->opcode, msb_data, lsb_data, lsb_enable, above_64_cmp_data));

    if (cbw_cmp_data->type == NLM_DNX_CB_INST_CMP1)
    {

        /*
         * reversed buffer
         */
#if defined(BE_HOST)
        dnx_kbp_buffer_endians_fix(unit, above_64_cmp_data);
#endif
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_total_result_size_get(unit, cbw_cmp_data->opcode, &cmp_resp_data_len));
        sal_memcpy(cmp_data,
                   ((uint8 *) above_64_cmp_data) + DNX_KBP_CPU_LOOKUP_REPLY_DATA_REGISTER_BYTE_LEN - cmp_resp_data_len,
                   cmp_resp_data_len);
        DNX_KBP_ROP_REVERSE_DATA(cmp_data, cbw_cmp_data->result.data_raw, cmp_resp_data_len);
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Verify input parameters for dnx_kbp_rop_last_request_get
 *   dnx_kbp_rop_last_response_get
 *
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] flags - flags.
 * \param [in] pkt - ROP packet structure.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static shr_error_e
dnx_kbp_rop_last_get_verify(
    int unit,
    bcm_core_t core,
    uint32 flags,
    dnx_kbp_rop_packet_t * pkt)
{
    int max_core_id = dnx_data_device.general.nof_cores_get(unit) - 1;

    SHR_FUNC_INIT_VARS(unit);
    /** Null checks */
    SHR_NULL_CHECK(pkt, _SHR_E_PARAM, "rop packet");
    /** Flags */
    SHR_MASK_VERIFY(flags, DNX_KBP_ROP_GET_SUPPORTED_FLAGS, _SHR_E_PARAM, "unexpected flags.\n");
    /** Core */
    SHR_RANGE_VERIFY(core, 0, max_core_id, _SHR_E_PARAM, "core id out of bound %d (max is %d).\n", core, max_core_id);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get ROP payload length according to the opcode.
 *
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] opcode - opcode.
 * \param [in] type - ROP packet type.
 * \param [out] len - payload length.
 *
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static shr_error_e
kbp_rop_payload_len_get(
    int unit,
    bcm_core_t core,
    uint8 opcode,
    dnx_kbp_rop_packet_type_e type,
    uint32 *len)
{
    dnx_kbp_opcode_map_t opcode_map;

    SHR_FUNC_INIT_VARS(unit);

    opcode_map.opcode = opcode;
    SHR_IF_ERR_EXIT(dnx_kbp_opcode_map_get(unit, core, &opcode_map));
    *len = (type == DNX_KBP_ROP_REQUEST) ? opcode_map.tx_size : opcode_map.rx_size;
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   DNX ROP packet FIFO memory index get function
 *
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] type - Rop packet type.
 * \param [out] index - FIFO index.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static shr_error_e
dnx_kbp_rop_fifo_index_get(
    int unit,
    bcm_core_t core,
    dnx_kbp_rop_packet_type_e type,
    uint32 *index)
{
    uint32 entry_handle_id;
    uint32 fifo_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGW_ROP_FIFO_STATUS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ROP_TYPE, type);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FIFO_WR_PTR, INST_SINGLE, &fifo_index);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *index = (fifo_index == 0) ? (dnx_data_elk.general.rop_fifo_depth_get(unit) - 1) : (fifo_index - 1);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get last ROP packet from DNX side.
 *
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] type - ROP packet type.
 * \param [out] pkt - ROP packet structure.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static shr_error_e
dnx_kbp_rop_last_dnx_get(
    int unit,
    bcm_core_t core,
    dnx_kbp_rop_packet_type_e type,
    dnx_kbp_rop_packet_t * pkt)
{
    uint32 entry_handle_id;
    uint32 fifo_index = 0;
    uint32 rop_data[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    uint8 rop_data_reverse[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_kbp_rop_fifo_index_get(unit, core, type, &fifo_index));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGW_ROP_FIFO_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_INDEX, fifo_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ROP_TYPE, type);

    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_ROP_DATA, INST_SINGLE, rop_data);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

#if defined(BE_HOST)
    dnx_kbp_buffer_endians_fix(unit, rop_data);
#endif

    DNX_KBP_ROP_REVERSE_DATA(((uint8 *) rop_data), rop_data_reverse, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);

    pkt->opcode = rop_data_reverse[DNX_KBP_ROP_OPCODE_OFFSET];
    sal_memcpy(&pkt->sequence_num, rop_data_reverse + DNX_KBP_ROP_SEQ_NUM_OFFSET, DNX_KBP_ROP_SEQ_NUM_LEN_IN_BYTE);

    SHR_IF_ERR_EXIT(kbp_rop_payload_len_get(unit, core, pkt->opcode, type, &pkt->payload_len));
    sal_memcpy(pkt->payload, rop_data_reverse + DNX_KBP_ROP_PAYLOAD_OFFSET, pkt->payload_len);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get last ROP request from KBP side.
 *
 * \param [in] unit - The unit number.
 * \param [in] kbp_port - KBP port ID, 0 or 1.
 * \param [out] pkt - ROP packet structure.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static shr_error_e
dnx_kbp_rop_last_request_kbp_get(
    int unit,
    int kbp_port,
    dnx_kbp_rop_packet_t * pkt)
{
    int i, reg_skip;
    int port0_reg_skip = 4, port1_reg_skip = 6;
    uint32 reg_addr;
    uint16 reg_data[DNX_KBP_OP_LAST_REQUEST_REG_COUNT_PER_PORT];
    uint64 reg_data_in_dw[BYTES2WORDS(DNX_KBP_OP_LAST_REQUEST_REG_COUNT_PER_PORT)];
    bcm_core_t core = dnx_data_elk.connectivity.topology_get(unit)->port_core_mapping[kbp_port];

    SHR_FUNC_INIT_VARS(unit);

    reg_addr =
        (kbp_port == 0) ? DNX_KBP_OP_DEV0_LAST_REQUEST_PORT0_REG_ADDR : DNX_KBP_OP_DEV0_LAST_REQUEST_PORT1_REG_ADDR;
    reg_skip = (kbp_port == 0) ? port0_reg_skip : port1_reg_skip;
    reg_addr = reg_addr + reg_skip;

    for (i = 0; i < BYTES2WORDS(DNX_KBP_OP_LAST_REQUEST_REG_COUNT_PER_PORT); ++i)
    {
        COMPILER_64_ZERO(reg_data_in_dw[i]);
    }

    for (i = 0; i < (DNX_KBP_OP_LAST_REQUEST_REG_COUNT_PER_PORT - reg_skip); i++)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_callback_mdio_read
                        (&Kbp_app_data[unit]->user_data[0], 0, DNX_KBP_OP_DEV0_LAST_REQUEST_REG_DEVAD, reg_addr,
                         &reg_data[i]));
        reg_addr++;
        /*
         * Re-assemble the reg data to Dwords
         */
        if ((i != 0) && (i % 4 == 0))
        {
            reg_data_in_dw[(i - 1) / 4] =
                ((uint64) reg_data[i - 1] << 48) | ((uint64) reg_data[i - 2] << 32) | ((uint64) reg_data[i - 3] << 16) |
                ((uint64) reg_data[i - 4]);
        }
    }

    pkt->opcode = (reg_data_in_dw[0] >> (SAL_UINT64_NOF_BITS - BYTES2BITS(DNX_KBP_ROP_OPCODE_LEN_IN_BYTE))) & 0xff;
    pkt->sequence_num =
        (reg_data_in_dw[0] >>
         (SAL_UINT64_NOF_BITS - BYTES2BITS(DNX_KBP_ROP_OPCODE_LEN_IN_BYTE + DNX_KBP_ROP_SEQ_NUM_LEN_IN_BYTE))) & 0xffff;

    SHR_IF_ERR_EXIT(kbp_rop_payload_len_get(unit, core, pkt->opcode, DNX_KBP_ROP_REQUEST, &pkt->payload_len));
    sal_memcpy(&pkt->payload,
               (uint8 *) reg_data_in_dw + (DNX_KBP_ROP_OPCODE_LEN_IN_BYTE + DNX_KBP_ROP_SEQ_NUM_LEN_IN_BYTE),
               pkt->payload_len);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get last ROP response from KBP side.
 *
 * \param [in] unit - The unit number.
 * \param [in] kbp_port - KBP port ID, 0 or 1.
 * \param [out] pkt - ROP packet structure.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static shr_error_e
dnx_kbp_rop_last_response_kbp_get(
    int unit,
    int kbp_port,
    dnx_kbp_rop_packet_t * pkt)
{
    int i, skip_bits = 37;
    uint32 reg_addr;
    uint16 reg_data[DNX_KBP_OP_LAST_RESPONSE_REG_COUNT_PER_PORT];
    uint64 reg_data_in_dw[BYTES2WORDS(DNX_KBP_OP_LAST_RESPONSE_REG_COUNT_PER_PORT)];
    bcm_core_t core = dnx_data_elk.connectivity.topology_get(unit)->port_core_mapping[kbp_port];

    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_APP_DATA_CHECK(unit);

    reg_addr = (kbp_port == 0) ? DNX_KBP_OP_LAST_RESPONSE_PORT0_REG_ADDR : DNX_KBP_OP_LAST_RESPONSE_PORT1_REG_ADDR;

    for (i = 0; i < BYTES2WORDS(DNX_KBP_OP_LAST_RESPONSE_REG_COUNT_PER_PORT); ++i)
    {
        COMPILER_64_ZERO(reg_data_in_dw[i]);
    }

    for (i = 0; i < DNX_KBP_OP_LAST_RESPONSE_REG_COUNT_PER_PORT; i++)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_callback_mdio_read
                        (&Kbp_app_data[unit]->user_data[0], 0, DNX_KBP_OP_LAST_RESPONSE_REG_DEVAD, reg_addr,
                         &reg_data[i]));
        reg_addr++;

        if ((i != 0) && (i % 4 == 0))
        {
            /*
             * Re-assemble the reg data to Dwords
             */
            reg_data_in_dw[(i - 1) / 4] =
                ((uint64) reg_data[i - 1] << 48) | ((uint64) reg_data[i - 2] << 32) | ((uint64) reg_data[i - 3] << 16) |
                ((uint64) reg_data[i - 4]);
        }
    }
    /*
     * Skip the unneccessary bits
     */
    for (i = 0; i < BYTES2WORDS(DNX_KBP_OP_LAST_RESPONSE_REG_COUNT_PER_PORT) - 1; i++)
    {
        reg_data_in_dw[i] =
            ((reg_data_in_dw[i + 1] & 0x1FFFFFFFFF) << (SAL_UINT64_NOF_BITS - skip_bits)) | (reg_data_in_dw[i] >>
                                                                                             skip_bits);
    }

    pkt->opcode = (reg_data_in_dw[0] >> (SAL_UINT64_NOF_BITS - BYTES2BITS(DNX_KBP_ROP_OPCODE_LEN_IN_BYTE))) & 0xff;
    pkt->sequence_num =
        (reg_data_in_dw[0] >>
         (SAL_UINT64_NOF_BITS - BYTES2BITS(DNX_KBP_ROP_OPCODE_LEN_IN_BYTE + DNX_KBP_ROP_SEQ_NUM_LEN_IN_BYTE))) & 0xffff;

    SHR_IF_ERR_EXIT(kbp_rop_payload_len_get(unit, core, pkt->opcode, DNX_KBP_ROP_RESPONSE, &pkt->payload_len));
    sal_memcpy(&pkt->payload, (uint8 *) reg_data_in_dw + DNX_KBP_ROP_PAYLOAD_OFFSET, pkt->payload_len);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get last ROP packet from KBP side.
 *
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] type - ROP packet type.
 * \param [out] pkt - ROP packet structure.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
static shr_error_e
dnx_kbp_rop_last_kbp_get(
    int unit,
    bcm_core_t core,
    dnx_kbp_rop_packet_type_e type,
    dnx_kbp_rop_packet_t * pkt)
{
    int kbp_port = 0;

    SHR_FUNC_INIT_VARS(unit);

    kbp_port =
        (dnx_data_elk.connectivity.topology_get(unit)->port_core_mapping[kbp_port] == core) ? kbp_port : kbp_port + 1;

    if (type == DNX_KBP_ROP_REQUEST)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_rop_last_request_kbp_get(unit, kbp_port, pkt));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_kbp_rop_last_response_kbp_get(unit, kbp_port, pkt));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get last ROP request packet.
 *
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] flags - Flags, DNX side or KBP side.
 * \param [out] pkt - ROP packet structure.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_rop_last_request_get(
    int unit,
    bcm_core_t core,
    uint32 flags,
    dnx_kbp_rop_packet_t * pkt)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_APP_DATA_CHECK(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_kbp_rop_last_get_verify(unit, core, flags, pkt));

    sal_memset(pkt, 0, sizeof(dnx_kbp_rop_packet_t));

    if (flags & DNX_KBP_ROP_GET_DNX_SIDE)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_rop_last_dnx_get(unit, core, DNX_KBP_ROP_REQUEST, pkt));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_kbp_rop_last_kbp_get(unit, core, DNX_KBP_ROP_REQUEST, pkt));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get last ROP response packet.
 *
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] flags - Flags, DNX side or KBP side.
 * \param [out] pkt - ROP packet structure.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_rop_last_response_get(
    int unit,
    bcm_core_t core,
    uint32 flags,
    dnx_kbp_rop_packet_t * pkt)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_KBP_APP_DATA_CHECK(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_kbp_rop_last_get_verify(unit, core, flags, pkt));

    sal_memset(pkt, 0, sizeof(dnx_kbp_rop_packet_t));

    if (flags & DNX_KBP_ROP_GET_DNX_SIDE)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_rop_last_dnx_get(unit, core, DNX_KBP_ROP_RESPONSE, pkt));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_kbp_rop_last_kbp_get(unit, core, DNX_KBP_ROP_RESPONSE, pkt));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * }
 */
#endif /* defined(INCLUDE_KBP) */

#undef BSL_LOG_MODULE
