/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_oam_statistics.c Purpose: basic examples for OAM statistics. 
 */
/*
 * In general the OAMP Statistics refers to the functionality of counting OAM packets received and transmitted by the OAMP.
 * Opposed to previous devices, in Jericho2 OAMP has direct access to CRPS.
 * This enables OAMP to provide statistics functionality per endpoint. The user may configure the OAMP to count:
 *      Packets to/from specific MEPs.
 *      Rx and/or Tx packets
 *      Packets with specific opcodes
 *
 * This file includes examples of allocating counter engines and counter for OAM Statistics:
 *  - TX counting
 *  - RX counting
 *  - TX/RX counting
 * Enable counting of CCM and BFD packets per side.
 * Configures OampStatsShift index.
 */

/**
* Global defines
*/
int OAM_STAT_CFG_RX_PREFIX = 1;
int OAM_STAT_CFG_TX_PREFIX = 0;
int OAM_STAT_COUNT_TX = 0;
int OAM_STAT_COUNT_RX = 1;
int OAM_STAT_COUNT_PER_DIRECTION = 2;
int OAM_CCM_BFD_OPCODE = 1;
int OAM_STAT_COUNT_ENABLE = 1;
int database_id=5;

/**
* \brief
*   speed-up the counter processor sequential eviction.
*   routine speed-up the eviction only if No_Eviction action is set for counters equal to zero.
*   In this case, only counters > 0 will be evicted, and the number of records probably will be reduce.
*   For testing, when only few counters are updated, it help for reducing the time that test wait when reading the counters
*   By defaul SDK set the sequential timer to 2msec. Here we reduce it to 50usec.
*   In real system, user has to think how many records per seocnd his CPU can process, 
*   how many counters will be updated per second and set the sequential timer accordngly.
*
* \par DIRECT INPUT:
*   \param [in] unit                 -   UNIT_ID
*   \param [in] core_id              -   core id
*   \param [in] engine_id            -   engine id
*  
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int cint_oam_oamp_statistics_eviction_speed_adjust(
  int unit,
  int core_id,
  int engine_id)
{
    int rv = BCM_E_NONE;
    bcm_stat_engine_t engine;
    bcm_stat_eviction_t eviction;
    uint32 flags = 0;
    engine.core_id = core_id;
    engine.engine_id = engine_id;
    rv = bcm_stat_counter_eviction_get(unit, flags, &engine, &eviction);
    if (rv != BCM_E_NONE)
    {
    printf("Error in bcm_stat_counter_eviction_get. \n");
    return rv;
    }      

    if ((eviction.cond.qualifier == bcmStatEvictionConditionalQualOrEqualZero) &&
        (eviction.cond.action_flags & BCM_STAT_EVICTION_CONDITIONAL_ACTION_NO_EVICTION) &&
        (eviction.cond.condition_source_select == bcmStatEvictionConditionalSourceUserData) &&
        (COMPILER_64_LO(eviction.cond.condition_user_data) == 0) &&
        (COMPILER_64_HI(eviction.cond.condition_user_data) == 0))
    {
      rv = bcm_stat_counter_engine_control_set(unit, flags, &engine, bcmStatCounterSequentialSamplingInterval, 50);
      if (rv != BCM_E_NONE)
      {
      printf("Error in bcm_stat_counter_engine_control_set. \n");
      return rv;
      }      
    }
  
  return rv;
}
                        
                        
/**
* \brief
*   Procedure allocate up to 32k in total counters for RX and TX side.
*   Enables TX/RX counting for CCM and BFD packets.
*   Configures OampStatsShift index.
*
* \par DIRECT INPUT:
*   \param [in] unit                    -   UNIT_ID
*   \param [in] mep_id_min              -   ID of the first MEP.
*   \param [in] nof_statistics_meps     -   Number of MEPs that will use statistics.
*                                           (up to 2k since we allocate only 8k counters per side)
*   \param [in] tx_rx
*                      "0"              -   Enable TX counting
*                      "1"              -   Enable RX counting
*                      "2"              -   Enable TX and RX counting
*   \param [in] enable_eviction - should be 1 for
*          per-endpoint-stats, 0 for SLM/R
*  
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int cint_oam_oamp_statistics_main(
    int unit,
    int mep_id_min,
    int nof_statistics_meps,
    int tx_rx,
    int enable_eviction)
{

    int index;
    uint64 arg;
    int counter_base;
    int rx_tx_shift;
    int core_id = 0;
    int max_mep_id,max_counter_id;
    int rv = BCM_E_NONE;
    int is_rx_cnt_enable = 0;
    int oamp_command_id = 0; /** Command id for OAMP */
    bcm_oam_control_key_t key;
    int nof_requred_engines = 1;
    uint32 total_nof_counters_get;
    int engine_array_16k[1];
    bcm_stat_counter_interface_type_t oam_source = bcmStatCounterInterfaceOamp;
    /** Calculate the maximum ID for which a counter will be allocated */
    max_mep_id = mep_id_min + (nof_statistics_meps << 2);

    /** Calculate maximum counter ID */
    max_counter_id = (max_mep_id << (1 * is_rx_cnt_enable));
    /** Calculate the RX and TX shift index, based on max number of counters */
    while(max_counter_id)
    {
        max_counter_id = max_counter_id >> 1;
        rx_tx_shift++;
    };

    printf("Configuring  OampStatsShift with value of: %d\n",rx_tx_shift);

    /** Configure the RX and TX shift, based on needed counters */
    COMPILER_64_SET(arg,0,rx_tx_shift);
    rv = bcm_oam_control_set(unit,bcmOamControlOampStatsShift,arg);
    if (rv != BCM_E_NONE)
    {
       printf("Error in bcm_oam_control_set, bcmOamControlOampStatsShift. \n");
       return rv;
    }

    key.index = OAM_CCM_BFD_OPCODE;
    COMPILER_64_SET(arg,0,OAM_STAT_COUNT_ENABLE);


    rv = cint_oam_oamp_statistics_enable_disable_opcode_count(unit, bcmOamOpcodeLMM, tx_rx, 1);
    if (rv != BCM_E_NONE)
    {
       printf("Error in cint_oam_oamp_statistics_enable_disable_opcode_count, bcmOamControlOampStatsShift. \n");
       return rv;
    }

    /**
    * Configure TX side
    */
    /* Eviction required for per endpoint stats*/
    crps_oam_config_enable_eviction =enable_eviction;
    
    /**
    * Allocate up to 16k counters in engine 8 and database 1.
    */
    counter_base = mep_id_min;
    engine_array_16k[0] = 8; /** Engine 8 contained 16k counters */
    rv = crps_oam_database_set(unit, core_id, oam_source, oamp_command_id, nof_requred_engines, engine_array_16k, counter_base, database_id, &total_nof_counters_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_oam_database_set. \n");
        return rv;
    }
    rv = cint_oam_oamp_statistics_eviction_speed_adjust(unit, core_id, engine_array_16k[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_oam_oamp_statistics_eviction_speed_adjust. \n");
        return rv;
    }    

    printf("Counters from %d to %d were allocated in engine: %d, database ID: %d\n",
                         counter_base, counter_base+total_nof_counters_get, engine_array_16k[0], database_id);

    if ((tx_rx == OAM_STAT_COUNT_TX) || (tx_rx == OAM_STAT_COUNT_PER_DIRECTION))
    {
        /**
        * Enable TX counting for OAM/BFD packets, globally for the device.
        */
        key.type = bcmOamControlOampStatsTxOpcode;
        rv = bcm_oam_control_indexed_set(unit,key,arg);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_oam_control_indexed_set, bcmOamControlOampStatsTxOpcode. \n");
            return rv;
        }
    }

    /**
    * Configure RX side
    */
    /**
    * Allocate up to 16k counters in engine 9 and database 2.
    * Counter base for RX side is shifted, because of RX_PREFIX in the formula.
    */
    counter_base = (1 << rx_tx_shift) + mep_id_min;
    engine_array_16k[0] = 9; /** Engine 9 contained 16k counters */
    rv = crps_oam_database_set(unit, core_id, oam_source, oamp_command_id, nof_requred_engines, engine_array_16k, counter_base, database_id + 1, &total_nof_counters_get);
    if (rv != BCM_E_NONE)
    {
       printf("Error in crps_oam_database_set.\n");
       return rv;
    }
    rv = cint_oam_oamp_statistics_eviction_speed_adjust(unit, core_id, engine_array_16k[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_oam_oamp_statistics_eviction_speed_adjust. \n");
        return rv;
    }       
    /* Set it back to 0*/
    crps_oam_config_enable_eviction =0;
    printf("Counters from %d to %d were allocated in engine: %d, database ID: %d\n",
                         (1 << rx_tx_shift) + mep_id_min , (1 << rx_tx_shift) + mep_id_min + total_nof_counters_get, engine_array_16k[0], database_id+1);

    if ((tx_rx == OAM_STAT_COUNT_RX) || (tx_rx == OAM_STAT_COUNT_PER_DIRECTION))
    {
        /**
        * Enable RX counting for OAM/BFD packets, globally for the device.
        */
        is_rx_cnt_enable = 1;
        key.type = bcmOamControlOampStatsRxOpcode;
        rv = bcm_oam_control_indexed_set(unit,key,arg);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_oam_control_indexed_set, bcmOamControlOampStatsRxOpcode. \n");
            return rv;
        }
    }

    return rv;
}


/**
* \brief
*   Procedure gets counter value based on counter pointer and database.
*
* \par DIRECT INPUT:
*   \param [in] unit            -   UNIT_ID
*   \param [in] flags           -   "0" - doesn't reset counter after read.
*                               -       BCM_STAT_COUNTER_CLEAR_ON_READ - reset counter after read
*   \param [in] core_id         -   core_id
*   \param [in] database_id     -   database_id
*   \param [in] counter_id      -   counter pointer to read
*   \param [out] counter_value  -   counter value
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int cint_oam_oamp_statistics_read_counter(
    int unit,
    int flags,
    int core_id,
    int database_id,
    int counter_id,
    int *counter_value)
{
    int rv, i, stat_array[1];
    uint64 counter_value_64b;
    bcm_stat_counter_input_data_t input_data;
    bcm_stat_counter_output_data_t output_data;
    bcm_stat_counter_value_t counter_val[1];

    bcm_stat_counter_input_data_t_init(&input_data);
    bcm_stat_counter_output_data_t_init(&output_data);

    /** fill the input structure */
    input_data.core_id = core_id;
    input_data.database_id = database_id;
    input_data.type_id = 0;
    input_data.counter_source_id = counter_id;
    input_data.nstat = 1;
    input_data.stat_arr = stat_array;
    output_data.value_arr = counter_val;
    /** call the API */
    rv = bcm_stat_counter_get(unit, flags, &input_data, &output_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_stat_counter_get\n");
        return rv;
    }

    counter_value_64b = counter_val[0].value;

    COMPILER_64_TO_32_LO(*counter_value,counter_value_64b);
    return rv;

}

/**
* \brief
*   Map the actual OpCode value to OpCode for count index according to the table.
*
* \par DIRECT INPUT:
*   \param [in] opcode                          - Represent the actual packet opcode in range 0-255.
*   \param [out] opcode_for_count_index          - Represent the index for opcode for count in range 0-5
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   * None
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
void opcode_to_opcode_index(int opcode, int * opcode_for_count_index)
{
    if (opcode < 0 || opcode > 255)
    {
         printf("WARNING: provided opcode is not in range, supported range is 0 - 255. \n");
    }

    switch (opcode)
    {
        /** Opcode "0" represent the BFD opcode*/
        case 0:
        {
            *opcode_for_count_index = 0;
            print "asdasdas";
            break;
        }
        case bcmOamOpcodeCCM:
        {
            *opcode_for_count_index = 0;
            break;
        }
        case bcmOamOpcodeLMM:
        case bcmOamOpcodeSLM:
        case bcmOamOpcode1SL:
        {
            *opcode_for_count_index = 1;
            break;
        }
        case bcmOamOpcodeLMR:
        case bcmOamOpcodeSLR:
        {
            *opcode_for_count_index = 2;
            break;
        }
        case bcmOamOpcodeDMM:
        case bcmOamOpcode1DM:
        {
            *opcode_for_count_index = 3;
            break;
        }
        case bcmOamOpcodeDMR:
        {
            *opcode_for_count_index = 4;
            break;
        }
        default:
        {
            *opcode_for_count_index = 5;
            break;
        }
    }
}

/**
* \brief
*   Procedure that calculates statistics counter pointer.
*
* \par DIRECT INPUT:
*   \param [in] unit           - UNIT_ID
*   \param [in] mep_id         - MEP ID
*   \param [in] opcode         - Opcode that will be counted
*   \param [in] is_rx_side     - "0" calculate TX side cntr pointer
*                              - "1" calculate RX side cntr pointer
*   \param [out] counter_id    - counter pointer
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int 
cint_oam_oamp_statistics_calc_cnt_index(
    int unit,
    int flags,
    int mep_id,
    int opcode,
    int is_rx_side,
    int *counter_id)
{

    int rv;
    int mep_id_shift = 0;
    int cfg_tx_rx_shift = 0;
    int opcode_for_count_index=0;
    uint64 arg;

    if (flags == BCM_OAM_ENDPOINT_FLAGS2_PER_OPCODE_STATISTICS)
    {
        mep_id_shift = 3;
        opcode_to_opcode_index(opcode, &opcode_for_count_index);
    }

    /** Configure the RX and TX shift, based on needed counters */
    rv = bcm_oam_control_get(unit,bcmOamControlOampStatsShift,&arg);
    if (rv != BCM_E_NONE)
    {
       printf("Error in bcm_oam_control_set, bcmOamControlOampStatsShift. \n");
       return rv;
    }

    COMPILER_64_TO_32_LO(cfg_tx_rx_shift,arg);

    if (!is_rx_side)
    {
        *counter_id  = (OAM_STAT_CFG_TX_PREFIX << cfg_tx_rx_shift) + (mep_id << mep_id_shift) + opcode_for_count_index;
    }
    else
    {
        *counter_id  = (OAM_STAT_CFG_RX_PREFIX << cfg_tx_rx_shift) + (mep_id << mep_id_shift) + opcode_for_count_index;
    }

    return rv;
}


/**
* \brief
*   Procedure gets counter value based on counter pointer and database.
*
* \par DIRECT INPUT:
*   \param [in] unit            -   UNIT_ID
*   \param [in] core_id         -   core_id
*   \param [in] database_id     -   database_id
*   \param [in] mep_id          -   Endpoint ID.
*   \param [in] opcode          -   Opcode to read.
*   \param [in] opcode_flags    -   If set, the per-opcode count is enabled
*   \param [in] is_rx_side      -   "1" - Read received packets to the MEP in the OAMP.
*                               -   "0" - Read transmitted packets from the MEP
*   \param [out] counter_value  -   counter value
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int cint_oam_oamp_statistics_read_counter_value(
    int unit,
    int core_id,
    int database_id,
    int mep_id,
    int opcode,
    int opcode_flags,
    int is_rx_side,
    int *counter_value)
{

    int rv;
    int flags = 0; /** Don't reset the counter on read */
    int counter_id;

    /** Calculate the counter pointer */
    rv = cint_oam_oamp_statistics_calc_cnt_index(unit,opcode_flags,mep_id,opcode,is_rx_side,&counter_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error in cint_oam_oamp_statistics_calc_cnt_index. \n");
       return rv;
    }

    /** Read the counter value */
    rv = cint_oam_oamp_statistics_read_counter(unit,flags,core_id,database_id,counter_id,counter_value);
    if (rv != BCM_E_NONE)
    {
       printf("Error in cint_oam_oamp_statistics_read_counter. \n");
       return rv;
    }

    return rv;

}


/**
* \brief
*   Procedure gets counter value based on counter pointer and database.
*
* \par DIRECT INPUT:
*   \param [in] unit            -   UNIT_ID
*   \param [in] core_id         -   core_id
*   \param [in] database_id     -   database_id
*   \param [in] mep_id          -   Endpoint ID.
*   \param [in] opcode          -   Opcode to read.
*   \param [in] is_rx_side      -   "1" - Read received packets to the MEP in the OAMP.
*                               -   "0" - Read transmitted packets from the MEP
*   \param [out] counter_value  -   counter value
*
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int cint_oam_oamp_statistics_enable_disable_opcode_count(
    int unit,
    int opcode,
    int tx_rx,
    int enable_disable)
{

    bcm_oam_control_key_t key;
    uint64 arg;
    int rv;

    key.index = opcode;
    COMPILER_64_SET(arg,0,enable_disable);

    if ((tx_rx == OAM_STAT_COUNT_TX) || (tx_rx == OAM_STAT_COUNT_PER_DIRECTION))
    {
        /**
        * Enable TX counting for OAM/BFD packets, globally for the device.
        */
        key.type = bcmOamControlOampStatsTxOpcode;
        rv = bcm_oam_control_indexed_set(unit,key,arg);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_oam_control_indexed_set, bcmOamControlOampStatsTxOpcode. \n");
            return rv;
        }
    }

    if ((tx_rx == OAM_STAT_COUNT_RX) || (tx_rx == OAM_STAT_COUNT_PER_DIRECTION))
    {
        /**
        * Enable RX counting for OAM/BFD packets, globally for the device.
        */
        key.type = bcmOamControlOampStatsRxOpcode;
        rv = bcm_oam_control_indexed_set(unit,key,arg);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_oam_control_indexed_set, bcmOamControlOampStatsRxOpcode. \n");
            return rv;
        }
    }

    return rv;
}

/**
* \brief
*   Procedure allocates counters in a database with single engine and core.
*
* \par DIRECT INPUT:
*   \param [in] unit                    -   UNIT_ID
*   \param [in] core_id                 -   CORE_ID
*   \param [in] counter_base            -   Starting counter pointer to allocate
*   \param [in] database_id             -   Database ID in which will allocate
*   \param [in] engine_id               -   ID of the engine to be allocated
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   int -
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int cint_oam_oamp_counter_allocation_main(
    int unit,
    int core_id,
    int counter_base,
    int database_id,
    int engine_id)
{

    int index;
    uint64 arg;

    int rv = BCM_E_NONE;
    int is_rx_cnt_enable = 0;
    int oamp_command_id = 0; /** Command id for OAMP */
    bcm_oam_control_key_t key;
    int nof_requred_engines = 1;
    uint32 total_nof_counters_get;
    int engine_array[1];
    bcm_stat_counter_interface_type_t oam_source = bcmStatCounterInterfaceOamp;


    /**
    * Allocate up to 16k counters in engine 8 and database 1.
    */
    engine_array[0] = engine_id; /** Engine 8 contained 16k counters */
    rv = crps_oam_database_set(unit, core_id, oam_source, oamp_command_id, nof_requred_engines, engine_array, counter_base, database_id, &total_nof_counters_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error in crps_oam_database_set. \n");
        return rv;
    }

    printf("OAMP: Total allocated counters:%d on core:%d with database_id:%d, engine:%d, counter_base:%d !\n", total_nof_counters_get, core_id, database_id, engine_id, counter_base);

    return rv;
}