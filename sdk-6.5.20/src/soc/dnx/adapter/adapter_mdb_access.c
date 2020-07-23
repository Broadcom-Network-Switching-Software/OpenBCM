  /** \file adapter_mdb_access.c
 * This file contains the logical MDB access functions.
 *
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. $
 */

int
tmp_mdb_workaround_func(
    void)
{
    return 0;
}
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_ADAPTERDNX

#ifdef ADAPTER_SERVER_MODE

#include <netdb.h>



#include <shared/bsl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <bde/pli/verinet.h>
#include <errno.h>
#include <sys/socket.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/adapter/adapter_reg_access.h>
#include <soc/dnx/adapter/adapter_mdb_access.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/swstate/auto_generated/access/adapter_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>






#define UPDATE_FRM_SOC_LOGICAL_ACCESS   145


#define UPDATE_FRM_SOC_LOGICAL_MEM_TYPE_DIRECT  180
#define UPDATE_FRM_SOC_LOGICAL_MEM_TYPE_EM   181
#define UPDATE_FRM_SOC_LOGICAL_MEM_TYPE_EEDB 182
#define UPDATE_FRM_SOC_LOGICAL_MEM_TYPE_LPM  183


#define UPDATE_FRM_SOC_LOGICAL_ACCESS_READ          160
#define UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE         161
#define UPDATE_FRM_SOC_LOGICAL_ACCESS_LOOKUP        162
#define UPDATE_FRM_SOC_LOGICAL_ACCESS_DELETE        163
#define UPDATE_FRM_SOC_LOGICAL_ACCESS_READ_RESPONSE 164


#define ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8 600

#define ADAPTER_MDB_ACCESS_NOF_LL_BITS 20


#define LOGICAL_MEM_ACCESS_ERR_UNKNOWN_TABLE_ID (-1)
#define LOGICAL_MEM_ACCESS_ERR_UNKNOWN_BLOCK_ID (-2)
#define LOGICAL_MEM_ACCESS_ERR_UNKNOWN_COMMAND  (-3)
#define LOGICAL_MEM_ACCESS_ERR_ILLEGAL_KEY      (-4)
#define LOGICAL_MEM_ACCESS_ERR_KEY_NOT_FOUND    (-5)
#define LOGICAL_MEM_ACCESS_ERR_UNKNOWN_ERR      (-6)




#define DNX_ADAPTER_MDB_HEADER_OFFSET      0
#define DNX_ADAPTER_MDB_HEADER_SIZE        5
#define DNX_ADAPTER_MDB_OPCODE_OFFSET      (DNX_ADAPTER_MDB_HEADER_OFFSET)
#define DNX_ADAPTER_MDB_OPCODE_SIZE        1
#define DNX_ADAPTER_MDB_PAYLOAD_LEN_OFFSET (DNX_ADAPTER_MDB_OPCODE_OFFSET + DNX_ADAPTER_MDB_OPCODE_SIZE)
#define DNX_ADAPTER_MDB_PAYLOAD_LEN_SIZE   4


#define DNX_ADAPTER_MDB_LOGICAL_ACCESS_OFFSET             (DNX_ADAPTER_MDB_HEADER_OFFSET + DNX_ADAPTER_MDB_HEADER_SIZE)
#define DNX_ADAPTER_MDB_LOGICAL_ACCESS_SIZE               7
#define DNX_ADAPTER_MDB_LOGICAL_ACCESS_BLOCK_ID_OFFSET    (DNX_ADAPTER_MDB_HEADER_SIZE)
#define DNX_ADAPTER_MDB_LOGICAL_ACCESS_BLOCK_ID_SIZE      4
#define DNX_ADAPTER_MDB_LOGICAL_ACCESS_MEMORY_TYPE_OFFSET (DNX_ADAPTER_MDB_LOGICAL_ACCESS_BLOCK_ID_OFFSET + DNX_ADAPTER_MDB_LOGICAL_ACCESS_BLOCK_ID_SIZE)
#define DNX_ADAPTER_MDB_LOGICAL_ACCESS_MEMORY_TYPE_SIZE   1
#define DNX_ADAPTER_MDB_LOGICAL_ACCESS_COMMAND_OFFSET     (DNX_ADAPTER_MDB_LOGICAL_ACCESS_MEMORY_TYPE_OFFSET + DNX_ADAPTER_MDB_LOGICAL_ACCESS_MEMORY_TYPE_SIZE)
#define DNX_ADAPTER_MDB_LOGICAL_ACCESS_COMMAND_SIZE       1
#define DNX_ADAPTER_MDB_LOGICAL_ACCESS_MEMORY_ID_OFFSET   (DNX_ADAPTER_MDB_LOGICAL_ACCESS_COMMAND_OFFSET + DNX_ADAPTER_MDB_LOGICAL_ACCESS_MEMORY_ID_SIZE)
#define DNX_ADAPTER_MDB_LOGICAL_ACCESS_MEMORY_ID_SIZE     1


#define DNX_ADAPTER_MDB_LPM_HEADER_OFFSET (DNX_ADAPTER_MDB_LOGICAL_ACCESS_OFFSET + DNX_ADAPTER_MDB_LOGICAL_ACCESS_SIZE)



#define DNX_ADAPTER_MDB_LPM_KEY_PREFIX_LEN_OFFSET  (DNX_ADAPTER_MDB_LPM_HEADER_OFFSET)
#define DNX_ADAPTER_MDB_LPM_KEY_PREFIX_LEN_SIZE    1
#define DNX_ADAPTER_MDB_LPM_KEY_LEN_OFFSET         (DNX_ADAPTER_MDB_LPM_KEY_PREFIX_LEN_OFFSET + DNX_ADAPTER_MDB_LPM_KEY_PREFIX_LEN_SIZE)
#define DNX_ADAPTER_MDB_LPM_KEY_LEN_SIZE           4
#define DNX_ADAPTER_MDB_LPM_KEY_OFFSET             (DNX_ADAPTER_MDB_LPM_KEY_LEN_OFFSET + DNX_ADAPTER_MDB_LPM_KEY_LEN_SIZE)




#define DNX_ADAPTER_MDB_LPM_READ_RES_ERROR_SIZE 1
#define DNX_ADAPTER_MDB_LPM_PAYLOAD_SIZE        4



#define DNX_ADAPTER_MDB_DIRECT_HEADER_OFFSET (DNX_ADAPTER_MDB_LOGICAL_ACCESS_OFFSET + DNX_ADAPTER_MDB_LOGICAL_ACCESS_SIZE)


#define DNX_ADAPTER_MDB_DIRECT_HEADER_KEY_OFFSET  (DNX_ADAPTER_MDB_DIRECT_HEADER_OFFSET)
#define DNX_ADAPTER_MDB_DIRECT_HEADER_KEY_SIZE    4


#define DNX_ADAPTER_MDB_DIRECT_READ_RES_ERROR_OFFSET       (DNX_ADAPTER_MDB_DIRECT_HEADER_KEY_OFFSET + DNX_ADAPTER_MDB_DIRECT_HEADER_KEY_SIZE)
#define DNX_ADAPTER_MDB_DIRECT_READ_RES_ERROR_SIZE         1
#define DNX_ADAPTER_MDB_DIRECT_READ_RES_PAYLOAD_LEN_OFFSET (DNX_ADAPTER_MDB_DIRECT_READ_RES_ERROR_OFFSET + DNX_ADAPTER_MDB_DIRECT_READ_RES_ERROR_SIZE)
#define DNX_ADAPTER_MDB_DIRECT_READ_RES_PAYLOAD_LEN_SIZE   4
#define DNX_ADAPTER_MDB_DIRECT_READ_RES_PAYLOAD_OFFSET     (DNX_ADAPTER_MDB_DIRECT_READ_RES_PAYLOAD_LEN_OFFSET + DNX_ADAPTER_MDB_DIRECT_READ_RES_PAYLOAD_LEN_SIZE)


#define DNX_ADAPTER_MDB_DIRECT_WRITE_PAYLOAD_LEN_OFFSET (DNX_ADAPTER_MDB_DIRECT_HEADER_KEY_OFFSET + DNX_ADAPTER_MDB_DIRECT_HEADER_KEY_SIZE)
#define DNX_ADAPTER_MDB_DIRECT_WRITE_PAYLOAD_LEN_SIZE   4
#define DNX_ADAPTER_MDB_DIRECT_WRITE_PAYLOAD_OFFSET     (DNX_ADAPTER_MDB_DIRECT_WRITE_PAYLOAD_LEN_OFFSET + DNX_ADAPTER_MDB_DIRECT_WRITE_PAYLOAD_LEN_SIZE)


#define DNX_ADAPTER_MDB_EM_HEADER_OFFSET (DNX_ADAPTER_MDB_LOGICAL_ACCESS_OFFSET + DNX_ADAPTER_MDB_LOGICAL_ACCESS_SIZE)


#define DNX_ADAPTER_MDB_EM_APP_ID_OFFSET   (DNX_ADAPTER_MDB_EM_HEADER_OFFSET)
#define DNX_ADAPTER_MDB_EM_APP_ID_SIZE     1
#define DNX_ADAPTER_MDB_EM_KEY_LEN_OFFSET  (DNX_ADAPTER_MDB_EM_APP_ID_OFFSET + DNX_ADAPTER_MDB_EM_APP_ID_SIZE)
#define DNX_ADAPTER_MDB_EM_KEY_LEN_SIZE    4
#define DNX_ADAPTER_MDB_EM_KEY_OFFSET      (DNX_ADAPTER_MDB_EM_KEY_LEN_OFFSET + DNX_ADAPTER_MDB_EM_KEY_LEN_SIZE)




#define DNX_ADAPTER_MDB_EM_READ_RES_ERROR_SIZE       1
#define DNX_ADAPTER_MDB_EM_READ_RES_PAYLOAD_LEN_SIZE 4

#define DNX_ADAPTER_MDB_EM_WRITE_PAYLOAD_LEN_SIZE 4




#define DNX_ADAPTER_MDB_LOGICAL_EEDB 0

#define DNX_ADAPTER_MDB_EEDB_HEADER_OFFSET (DNX_ADAPTER_MDB_LOGICAL_ACCESS_OFFSET + DNX_ADAPTER_MDB_LOGICAL_ACCESS_SIZE)


#define DNX_ADAPTER_MDB_EEDB_KEY_OFFSET  (DNX_ADAPTER_MDB_EEDB_HEADER_OFFSET)
#define DNX_ADAPTER_MDB_EEDB_KEY_SIZE    4




#define DNX_ADAPTER_MDB_EEDB_READ_RES_ERROR_OFFSET       (DNX_ADAPTER_MDB_EEDB_KEY_OFFSET + DNX_ADAPTER_MDB_EEDB_KEY_SIZE)
#define DNX_ADAPTER_MDB_EEDB_READ_RES_ERROR_SIZE         1
#define DNX_ADAPTER_MDB_EEDB_READ_RES_LL_OFFSET          (DNX_ADAPTER_MDB_EEDB_READ_RES_ERROR_OFFSET + DNX_ADAPTER_MDB_EEDB_READ_RES_ERROR_SIZE)
#define DNX_ADAPTER_MDB_EEDB_READ_RES_LL_SIZE            4
#define DNX_ADAPTER_MDB_EEDB_READ_RES_PHASE_OFFSET       (DNX_ADAPTER_MDB_EEDB_READ_RES_LL_OFFSET + DNX_ADAPTER_MDB_EEDB_READ_RES_LL_SIZE)
#define DNX_ADAPTER_MDB_EEDB_READ_RES_PHASE_SIZE         1
#define DNX_ADAPTER_MDB_EEDB_READ_RES_PAYLOAD_LEN_OFFSET (DNX_ADAPTER_MDB_EEDB_READ_RES_PHASE_OFFSET + DNX_ADAPTER_MDB_EEDB_READ_RES_PHASE_SIZE)
#define DNX_ADAPTER_MDB_EEDB_READ_RES_PAYLOAD_LEN_SIZE   4
#define DNX_ADAPTER_MDB_EEDB_READ_RES_PAYLOAD_OFFSET (DNX_ADAPTER_MDB_EEDB_READ_RES_PAYLOAD_LEN_OFFSET + DNX_ADAPTER_MDB_EEDB_READ_RES_PAYLOAD_LEN_SIZE)


#define DNX_ADAPTER_MDB_EEDB_WRITE_LL_OFFSET          (DNX_ADAPTER_MDB_EEDB_KEY_LEN_OFFSET + DNX_ADAPTER_MDB_EEDB_KEY_LEN_SIZE)
#define DNX_ADAPTER_MDB_EEDB_WRITE_LL_SIZE            4
#define DNX_ADAPTER_MDB_EEDB_WRITE_PHASE_OFFSET       (DNX_ADAPTER_MDB_EEDB_READ_RES_LL_OFFSET + DNX_ADAPTER_MDB_EEDB_READ_RES_LL_SIZE)
#define DNX_ADAPTER_MDB_EEDB_WRITE_PHASE_SIZE         1
#define DNX_ADAPTER_MDB_EEDB_WRITE_PAYLOAD_LEN_OFFSET (DNX_ADAPTER_MDB_EEDB_READ_RES_PHASE_OFFSET + DNX_ADAPTER_MDB_EEDB_READ_RES_PHASE_SIZE)
#define DNX_ADAPTER_MDB_EEDB_WRITE_PAYLOAD_LEN_SIZE   4
#define DNX_ADAPTER_MDB_EEDB_WRITE_PAYLOAD_OFFSET (DNX_ADAPTER_MDB_EEDB_READ_RES_PAYLOAD_LEN_OFFSET + DNX_ADAPTER_MDB_EEDB_READ_RES_PAYLOAD_LEN_SIZE)




#define DNX_ADAPTER_MDB_LOGICAL_GLEM_1 1
#define DNX_ADAPTER_MDB_LOGICAL_GLEM_2 2
#define DNX_ADAPTER_MDB_LOGICAL_GLEM_3 3

#define DNX_ADAPTER_MDB_GLEM_HEADER_OFFSET (DNX_ADAPTER_MDB_LOGICAL_ACCESS_OFFSET + DNX_ADAPTER_MDB_LOGICAL_ACCESS_SIZE)


#define DNX_ADAPTER_MDB_GLEM_CONST_OFFSET (DNX_ADAPTER_MDB_GLEM_HEADER_OFFSET)
#define DNX_ADAPTER_MDB_GLEM_CONST_SIZE    4
#define DNX_ADAPTER_MDB_GLEM_KEY_OFFSET   (DNX_ADAPTER_MDB_GLEM_CONST_OFFSET + DNX_ADAPTER_MDB_GLEM_CONST_SIZE)
#define DNX_ADAPTER_MDB_GLEM_KEY_SIZE      4




#define DNX_ADAPTER_MDB_GLEM_READ_RES_ERROR_OFFSET   (DNX_ADAPTER_MDB_GLEM_KEY_OFFSET + DNX_ADAPTER_MDB_GLEM_KEY_SIZE )
#define DNX_ADAPTER_MDB_GLEM_READ_RES_ERROR_SIZE      1
#define DNX_ADAPTER_MDB_GLEM_READ_RES_PAYLOAD_OFFSET (DNX_ADAPTER_MDB_GLEM_READ_RES_ERROR_OFFSET + DNX_ADAPTER_MDB_GLEM_READ_RES_ERROR_SIZE)
#define DNX_ADAPTER_MDB_GLEM_READ_RES_PAYLOAD_SIZE    4


#define DNX_ADAPTER_MDB_GLEM_WRITE_PAYLOAD_OFFSET (DNX_ADAPTER_MDB_GLEM_KEY_OFFSET + DNX_ADAPTER_MDB_GLEM_KEY_SIZE)
#define DNX_ADAPTER_MDB_GLEM_WRITE_PAYLOAD_SIZE    4






shr_error_e
adapter_mdb_access_uint32_to_uint8(
    int unit,
    uint32 *data_in,
    uint8 *data_out,
    uint32 nof_bits)
{
    uint32 nof_bytes = BITS2BYTES(nof_bits);
    int32 byte_index;
    int byte_counter = 0;
    int32 uint32_index;
    uint8 align_bits = BYTES2BITS(nof_bytes) - nof_bits;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(data_in, _SHR_E_PARAM, "data_in");
    SHR_NULL_CHECK(data_out, _SHR_E_PARAM, "data_out");

    byte_index = nof_bytes;
    uint32_index = 0;
    while (byte_index--)
    {
        int data_offset = (((nof_bytes - byte_index - 1) % 4) * 8);
        byte_counter++;
        data_out[byte_index] = (data_in[uint32_index] >> data_offset) & 0xFF;
        if ((byte_counter) % 4 == 0)
        {
            uint32_index++;
        }
    }

     
    if (align_bits != 0)
    {
        uint8 data_remainder = 0x00;
        byte_index = nof_bytes;
        while (byte_index--)
        {
            uint8 tmp_data_out = (data_out[byte_index] << align_bits) | data_remainder;
            data_remainder = data_out[byte_index] >> (8 - align_bits);
            data_out[byte_index] = tmp_data_out;
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
adapter_mdb_access_uint8_to_uint32(
    int unit,
    uint8 *data_in,
    uint32 *data_out,
    uint32 nof_bits)
{
    uint32 nof_bytes = BITS2BYTES(nof_bits);
    uint32 nof_uint32 = BITS2WORDS(nof_bits);
    int32 byte_index;
    int32 uint32_index;
    int byte_counter = 0;
    uint8 align_bits = BYTES2BITS(nof_bytes) - nof_bits;
    uint32 tmp_data_out;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(data_in, _SHR_E_PARAM, "data_in");
    SHR_NULL_CHECK(data_out, _SHR_E_PARAM, "data_out");

    
    byte_index = nof_bytes;
    uint32_index = 0;
    data_out[uint32_index] = 0x00000000;

    while (byte_index--)
    {
        int data_offset = (((nof_bytes - byte_index - 1) % 4) * 8);
        byte_counter++;
        tmp_data_out = data_in[byte_index] << (data_offset);
        data_out[uint32_index] |= tmp_data_out;

        if ((byte_counter) % 4 == 0)
        {
            uint32_index++;
            data_out[uint32_index] = 0x0;
        }
    }

    
    if (align_bits != 0)
    {
        uint32 data_remainder = 0x00000000;
        uint32_index = nof_uint32;
        while (uint32_index--)
        {
            tmp_data_out = (data_out[uint32_index] >> align_bits) | data_remainder;
            data_remainder = (data_out[uint32_index] << (32 - align_bits));
            data_out[uint32_index] = tmp_data_out;
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
adapter_mdb_access_lpm_prep_buffer(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 command,
    int *write_index_outer,
    uint8 buffer[ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8])
{
    uint32 nw_order_int;
    int key_size_bits;
    int cmic_blk;
    int total_length;
    int write_index;
    uint32 entry_key[MDB_MAX_DIRECT_PAYLOAD_SIZE_32];
    
    int padding_length;
    int padding_iter;

    SHR_FUNC_INIT_VARS(unit);

    
    key_size_bits = dnx_data_mdb.kaps.key_width_in_bits_get(unit);

    write_index = *write_index_outer;

    
    total_length = DNX_ADAPTER_MDB_LOGICAL_ACCESS_SIZE + DNX_ADAPTER_MDB_LPM_KEY_PREFIX_LEN_SIZE +
        DNX_ADAPTER_MDB_LPM_KEY_LEN_SIZE + (sizeof(uint8) * BITS2BYTES(key_size_bits));

    
    if (command == UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE)
    {
        total_length += DNX_ADAPTER_MDB_LPM_PAYLOAD_SIZE;
    }

    padding_length = sizeof(uint32) - (total_length % sizeof(uint32));
    total_length += padding_length;

    
    buffer[write_index] = UPDATE_FRM_SOC_LOGICAL_ACCESS;
    write_index += DNX_ADAPTER_MDB_OPCODE_SIZE;

    
    nw_order_int = htonl(total_length);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += DNX_ADAPTER_MDB_PAYLOAD_LEN_SIZE;

    
    cmic_blk = SOC_BLOCK2SCH(unit, MDB_BLOCK(unit));

    
    nw_order_int = htonl(cmic_blk);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_BLOCK_ID_SIZE;

    
    buffer[write_index] = UPDATE_FRM_SOC_LOGICAL_MEM_TYPE_LPM;
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_MEMORY_TYPE_SIZE;

    
    buffer[write_index] = command;
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_COMMAND_SIZE;

    
    buffer[write_index] = dnx_data_mdb.pdbs.mdb_adapter_mapping_get(unit, dbal_physical_table_id)->memory_id;
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_MEMORY_ID_SIZE;

    
    buffer[write_index] = entry->prefix_length;
    write_index += DNX_ADAPTER_MDB_LPM_KEY_PREFIX_LEN_SIZE;

    
    nw_order_int = htonl(key_size_bits);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += DNX_ADAPTER_MDB_LPM_KEY_LEN_SIZE;

    
    sal_memset(entry_key, 0x0, MDB_MAX_DIRECT_PAYLOAD_SIZE_32 * sizeof(entry_key[0]));
    SHR_BITCOPY_RANGE(entry_key, 0, entry->key, 0, key_size_bits);

    SHR_IF_ERR_EXIT(adapter_mdb_access_uint32_to_uint8(unit, entry_key, &buffer[write_index], key_size_bits));
    write_index += sizeof(uint8) * BITS2BYTES(key_size_bits);

    
    if (command == UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE)
    {
        nw_order_int = htonl(entry->payload[0]);
        sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
        write_index += DNX_ADAPTER_MDB_LPM_PAYLOAD_SIZE;
    }

    
    for (padding_iter = 0; padding_iter < padding_length; padding_iter++)
    {
        buffer[write_index] = 0;
        write_index += sizeof(char);
    }

    *write_index_outer = write_index;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
adapter_mdb_access_lpm_write(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    int adapter_mem_reg_fd;
    int write_index = 0;
    uint8 buffer[ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8];

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_mem_reg_mutex.take(unit, adapter_sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_mem_reg_fd.get(unit, adapter_sub_unit_id, &adapter_mem_reg_fd);

    
    SHR_IF_ERR_EXIT(adapter_mdb_access_lpm_prep_buffer
                    (unit, dbal_physical_table_id, app_id, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE,
                     &write_index, buffer));

    
    if (writen(adapter_mem_reg_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error: adapter_mdb_access_lpm_write: writing the opcode failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }

exit:
    adapter_context_db.params.adapter_mem_reg_mutex.give(unit, adapter_sub_unit_id);
    SHR_FUNC_EXIT;
}


shr_error_e
adapter_mdb_access_lpm_delete(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    int write_index = 0;
    uint8 buffer[ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8];
    int adapter_mem_reg_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_mem_reg_mutex.take(unit, adapter_sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_mem_reg_fd.get(unit, adapter_sub_unit_id, &adapter_mem_reg_fd);

    
    SHR_IF_ERR_EXIT(adapter_mdb_access_lpm_prep_buffer
                    (unit, dbal_physical_table_id, app_id, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_DELETE,
                     &write_index, buffer));

    
    if (writen(adapter_mem_reg_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error: adapter_mdb_access_lpm_delete: writing the opcode failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }

exit:
    adapter_context_db.params.adapter_mem_reg_mutex.give(unit, adapter_sub_unit_id);
    SHR_FUNC_EXIT;
}


shr_error_e
adapter_mdb_access_lpm_read(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    int write_index = 0;
    uint8 buffer[ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8];
    int adapter_mem_reg_fd;
    fd_set read_vect;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_mem_reg_mutex.take(unit, adapter_sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_mem_reg_fd.get(unit, adapter_sub_unit_id, &adapter_mem_reg_fd);

    
    SHR_IF_ERR_EXIT(adapter_mdb_access_lpm_prep_buffer
                    (unit, dbal_physical_table_id, app_id, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_READ,
                     &write_index, buffer));

    
    if (writen(adapter_mem_reg_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error: adapter_mdb_access_lpm_read: writing the opcode failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }

    FD_ZERO(&read_vect);
    FD_SET(adapter_mem_reg_fd, &read_vect);

    
    while (1)
    {
        if (select(adapter_mem_reg_fd + 1, &read_vect, (fd_set *) 0x0, (fd_set *) 0x0, NULL) < 0)
        {
            if (errno == EINTR)
            {
                
                continue;
            }
            perror("get_command: select error");
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }
        else
        {
            break;
        }
    }

    
    if (FD_ISSET(adapter_mem_reg_fd, &read_vect))
    {
        uint32 nw_order_int;
        int8 err_id;

        
        readn(adapter_mem_reg_fd, &(buffer[DNX_ADAPTER_MDB_OPCODE_OFFSET]), DNX_ADAPTER_MDB_HEADER_SIZE);

        
        sal_memcpy(&nw_order_int, &buffer[DNX_ADAPTER_MDB_PAYLOAD_LEN_OFFSET], DNX_ADAPTER_MDB_PAYLOAD_LEN_SIZE);

        
        readn(adapter_mem_reg_fd, &(buffer[DNX_ADAPTER_MDB_LOGICAL_ACCESS_OFFSET]), ntohl(nw_order_int));

        sal_memcpy(&nw_order_int, &buffer[DNX_ADAPTER_MDB_LPM_KEY_LEN_OFFSET], DNX_ADAPTER_MDB_LPM_KEY_LEN_SIZE);
        
        err_id = (int8) buffer[DNX_ADAPTER_MDB_LPM_KEY_OFFSET + BITS2BYTES(ntohl(nw_order_int))];

        if (err_id != 0)
        {
            if (err_id == LOGICAL_MEM_ACCESS_ERR_KEY_NOT_FOUND)
            {
                
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            }
            else
            {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "adapter LPM read returned an error (%d)\n"), err_id));
                SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            }
            SHR_EXIT();
        }

        
        sal_memcpy(&nw_order_int,
                   &buffer[DNX_ADAPTER_MDB_LPM_KEY_OFFSET + BITS2BYTES(ntohl(nw_order_int)) +
                           DNX_ADAPTER_MDB_LPM_READ_RES_ERROR_SIZE], DNX_ADAPTER_MDB_LPM_PAYLOAD_SIZE);
        entry->payload[0] = ntohl(nw_order_int);
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_TIMEOUT);
        
        SHR_EXIT();
    }

exit:
    adapter_context_db.params.adapter_mem_reg_mutex.give(unit, adapter_sub_unit_id);
    SHR_FUNC_EXIT;
}


shr_error_e
adapter_mdb_access_direct_table_prep_buffer(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 command,
    int *write_index_outer,
    uint8 buffer[ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8])
{
    uint32 nw_order_int;
    uint32 table_entry_size;
    uint32 payload_size_rounded_up;
    uint32 payload_size_rounded_up_diff;
    int cmic_blk;
    int total_length;
    int write_index;
    int padding_length;
    int padding_iter;
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id;
    uint32 entry_payload[MDB_MAX_DIRECT_PAYLOAD_SIZE_32];

    SHR_FUNC_INIT_VARS(unit);

    write_index = *write_index_outer;

    mdb_physical_table_id = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical;
    table_entry_size =
        MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(dnx_data_mdb.pdbs.
                                              pdbs_info_get(unit, mdb_physical_table_id)->direct_max_payload_type);

    payload_size_rounded_up = UTILEX_DIV_ROUND_UP(entry->payload_size, table_entry_size) * table_entry_size;
    payload_size_rounded_up_diff = payload_size_rounded_up - entry->payload_size;

    sal_memset(entry_payload, 0x0, MDB_MAX_DIRECT_PAYLOAD_SIZE_32 * sizeof(entry_payload[0]));
    SHR_BITCOPY_RANGE(entry_payload, payload_size_rounded_up_diff, entry->payload, 0, entry->payload_size);

    
    total_length = DNX_ADAPTER_MDB_LOGICAL_ACCESS_SIZE + DNX_ADAPTER_MDB_DIRECT_HEADER_KEY_SIZE;

    
    if (command == UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE)
    {
        
        total_length += DNX_ADAPTER_MDB_DIRECT_WRITE_PAYLOAD_LEN_SIZE + (sizeof(uint8) * BITS2BYTES(table_entry_size));

    }

    padding_length = ((total_length % sizeof(uint32)) == 0) ? 0 : sizeof(uint32) - (total_length % sizeof(uint32));
    total_length += padding_length;

    
    buffer[write_index] = UPDATE_FRM_SOC_LOGICAL_ACCESS;
    write_index += DNX_ADAPTER_MDB_OPCODE_SIZE;

    
    nw_order_int = htonl(total_length);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += DNX_ADAPTER_MDB_PAYLOAD_LEN_SIZE;

    
    cmic_blk = SOC_BLOCK2SCH(unit, MDB_BLOCK(unit));

    
    nw_order_int = htonl(cmic_blk);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_BLOCK_ID_SIZE;

    
    buffer[write_index] = UPDATE_FRM_SOC_LOGICAL_MEM_TYPE_DIRECT;
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_MEMORY_TYPE_SIZE;

    
    buffer[write_index] = command;
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_COMMAND_SIZE;

    
    buffer[write_index] = dnx_data_mdb.pdbs.mdb_adapter_mapping_get(unit, dbal_physical_table_id)->memory_id;
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_MEMORY_ID_SIZE;

    
    nw_order_int = htonl(entry->key[0]);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += DNX_ADAPTER_MDB_DIRECT_HEADER_KEY_SIZE;

    
    if (command == UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE)
    {
        nw_order_int = htonl(table_entry_size);
        sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
        write_index += DNX_ADAPTER_MDB_DIRECT_WRITE_PAYLOAD_LEN_SIZE;

        SHR_IF_ERR_EXIT(adapter_mdb_access_uint32_to_uint8
                        (unit, entry_payload, &buffer[write_index], table_entry_size));
        write_index += sizeof(uint8) * BITS2BYTES(table_entry_size);
    }

    
    for (padding_iter = 0; padding_iter < padding_length; padding_iter++)
    {
        buffer[write_index] = 0;
        write_index += sizeof(char);
    }

    *write_index_outer = write_index;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
adapter_mdb_access_glem_prep_buffer(
    int unit,
    int logical_db_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 command,
    int *write_index_outer,
    uint8 buffer[ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8])
{
    uint32 nw_order_int;
    int cmic_blk;
    int total_length;
    int write_index;

    
    int padding_length;
    int padding_iter;
    

    SHR_FUNC_INIT_VARS(unit);

    write_index = *write_index_outer;

    
    total_length = DNX_ADAPTER_MDB_LOGICAL_ACCESS_SIZE +
        DNX_ADAPTER_MDB_GLEM_CONST_SIZE + DNX_ADAPTER_MDB_GLEM_KEY_SIZE;

    
    if (command == UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE)
    {
        
        total_length += DNX_ADAPTER_MDB_GLEM_WRITE_PAYLOAD_SIZE;

    }

    padding_length = ((total_length % sizeof(uint32)) == 0) ? 0 : sizeof(uint32) - (total_length % sizeof(uint32));
    total_length += padding_length;

    
    buffer[write_index] = UPDATE_FRM_SOC_LOGICAL_ACCESS;
    write_index += DNX_ADAPTER_MDB_OPCODE_SIZE;

    
    nw_order_int = htonl(total_length);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += DNX_ADAPTER_MDB_PAYLOAD_LEN_SIZE;

    
    cmic_blk = SOC_BLOCK2SCH(unit, MDB_BLOCK(unit));

    
    nw_order_int = htonl(cmic_blk);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_BLOCK_ID_SIZE;

    
    buffer[write_index] = UPDATE_FRM_SOC_LOGICAL_MEM_TYPE_EEDB;
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_MEMORY_TYPE_SIZE;

    
    buffer[write_index] = command;
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_COMMAND_SIZE;

    
    buffer[write_index] = logical_db_id;
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_MEMORY_ID_SIZE;

    
    nw_order_int = 0xFFFFFFFF;
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += DNX_ADAPTER_MDB_GLEM_CONST_SIZE;

    nw_order_int = htonl(entry->key[0]);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += DNX_ADAPTER_MDB_GLEM_KEY_SIZE;

    
    if (command == UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE)
    {
        nw_order_int = htonl(entry->payload[0]);
        sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
        write_index += DNX_ADAPTER_MDB_GLEM_WRITE_PAYLOAD_SIZE;

    }

    
    for (padding_iter = 0; padding_iter < padding_length; padding_iter++)
    {
        buffer[write_index] = 0;
        write_index += sizeof(char);
    }

    *write_index_outer = write_index;

    SHR_FUNC_EXIT;
}


shr_error_e
adapter_mdb_access_em_prep_buffer(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 command,
    int *write_index_outer,
    uint8 buffer[ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8])
{
    uint32 table_payload_size;
    uint32 table_key_size;
    uint32 nw_order_int;
    int cmic_blk;
    int total_length;
    int write_index;
    uint8 vmv_size;
    uint8 vmv_value;
    uint32 vmv_value_32;
    uint32 entry_payload[MDB_MAX_EM_PAYLOAD_SIZE_32];
    uint32 payload_size_rounded_up;

    
    int padding_length;
    int padding_iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_em_vmv_calculation(unit, dbal_physical_table_id, app_id, entry, &vmv_size, &vmv_value));
    vmv_value_32 = (uint32) vmv_value;

    table_payload_size = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->max_payload_size;

    table_key_size = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->max_key_size;

    payload_size_rounded_up = UTILEX_DIV_ROUND_UP(entry->payload_size, table_payload_size) * table_payload_size;

    sal_memset(entry_payload, 0x0, MDB_MAX_EM_PAYLOAD_SIZE_32 * sizeof(entry_payload[0]));

    SHR_BITCOPY_RANGE(entry_payload, payload_size_rounded_up - vmv_size, &vmv_value_32, 0, vmv_size);
    SHR_BITCOPY_RANGE(entry_payload, payload_size_rounded_up - (entry->payload_size + vmv_size), entry->payload, 0,
                      entry->payload_size);

    write_index = *write_index_outer;

    
    total_length = DNX_ADAPTER_MDB_LOGICAL_ACCESS_SIZE +
        DNX_ADAPTER_MDB_EM_APP_ID_SIZE + DNX_ADAPTER_MDB_EM_KEY_LEN_SIZE + BITS2BYTES(table_key_size);

    
    if (command == UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE)
    {
        
        total_length += DNX_ADAPTER_MDB_EM_WRITE_PAYLOAD_LEN_SIZE + (sizeof(uint8) * BITS2BYTES(table_payload_size));

    }

    padding_length = ((total_length % sizeof(uint32)) == 0) ? 0 : sizeof(uint32) - (total_length % sizeof(uint32));
    total_length += padding_length;

    
    buffer[write_index] = UPDATE_FRM_SOC_LOGICAL_ACCESS;
    write_index += DNX_ADAPTER_MDB_OPCODE_SIZE;

    
    nw_order_int = htonl(total_length);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += DNX_ADAPTER_MDB_PAYLOAD_LEN_SIZE;

    
    cmic_blk = SOC_BLOCK2SCH(unit, MDB_BLOCK(unit));

    
    nw_order_int = htonl(cmic_blk);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_BLOCK_ID_SIZE;

    
    buffer[write_index] = UPDATE_FRM_SOC_LOGICAL_MEM_TYPE_EM;
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_MEMORY_TYPE_SIZE;

    
    buffer[write_index] = command;
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_COMMAND_SIZE;

    
    buffer[write_index] = dnx_data_mdb.pdbs.mdb_adapter_mapping_get(unit, dbal_physical_table_id)->memory_id;
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_MEMORY_ID_SIZE;

    
    buffer[write_index] = app_id;
    write_index += DNX_ADAPTER_MDB_EM_APP_ID_SIZE;

    nw_order_int = htonl(table_key_size);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += DNX_ADAPTER_MDB_EM_KEY_LEN_SIZE;

    SHR_IF_ERR_EXIT(adapter_mdb_access_uint32_to_uint8(unit, entry->key, &buffer[write_index], table_key_size));
    write_index += sizeof(uint8) * BITS2BYTES(table_key_size);

    
    if (command == UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE)
    {
        nw_order_int = htonl(table_payload_size);
        sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
        write_index += DNX_ADAPTER_MDB_EM_READ_RES_PAYLOAD_LEN_SIZE;

        SHR_IF_ERR_EXIT(adapter_mdb_access_uint32_to_uint8
                        (unit, entry_payload, &buffer[write_index], table_payload_size));
        write_index += sizeof(uint8) * BITS2BYTES(table_payload_size);
    }

    
    for (padding_iter = 0; padding_iter < padding_length; padding_iter++)
    {
        buffer[write_index] = 0;
        write_index += sizeof(char);
    }

    *write_index_outer = write_index;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
adapter_mdb_access_direct_table_write(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    int adapter_mem_reg_fd;
    int write_index = 0;
    uint8 buffer[ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8];
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(buffer, 0x0, sizeof(buffer));
    adapter_context_db.params.adapter_mem_reg_mutex.take(unit, adapter_sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_mem_reg_fd.get(unit, adapter_sub_unit_id, &adapter_mem_reg_fd);
    
    SHR_IF_ERR_EXIT(adapter_mdb_access_direct_table_prep_buffer
                    (unit, dbal_physical_table_id, app_id, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE,
                     &write_index, buffer));

    
    if (writen(adapter_mem_reg_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error: adapter_mdb_access_direct_table_write: writing the opcode failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }
exit:
    adapter_context_db.params.adapter_mem_reg_mutex.give(unit, adapter_sub_unit_id);
    SHR_FUNC_EXIT;
}


shr_error_e
adapter_mdb_access_eedb_prep_buffer(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    int ll_exist,
    dbal_physical_entry_t * entry,
    uint32 command,
    int *write_index_outer,
    uint8 buffer[ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8])
{
    uint32 nw_order_int;
    int cmic_blk;
    int total_length;
    int write_index;
    int physical_phase;
    int physical_phase_tmp;

    
    int padding_length;
    int padding_iter;
    uint32 entry_payload[MDB_MAX_DIRECT_PAYLOAD_SIZE_32];
    uint32 ll_data;
    dnx_algo_local_outlif_logical_phase_e logical_phase;
    uint32 table_payload_size;
    uint32 payload_size_rounded_up;
    uint32 out_lif_null_value;

    SHR_FUNC_INIT_VARS(unit);

    write_index = *write_index_outer;

    
    total_length = DNX_ADAPTER_MDB_LOGICAL_ACCESS_SIZE + DNX_ADAPTER_MDB_EEDB_KEY_SIZE;
    out_lif_null_value = dnx_data_lif.global_lif.egress_out_lif_null_value_get(unit);
    
    if (command == UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE)
    {

        table_payload_size = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->max_payload_size;
        payload_size_rounded_up = UTILEX_DIV_ROUND_UP(entry->payload_size, table_payload_size) * table_payload_size;
        sal_memset(entry_payload, 0x0, MDB_MAX_DIRECT_PAYLOAD_SIZE_32 * sizeof(entry_payload[0]));

        if (ll_exist)
        {
            ll_data = entry->payload[0] & ((1 << ADAPTER_MDB_ACCESS_NOF_LL_BITS) - 1);
            entry->payload[0] &= ~((1 << ADAPTER_MDB_ACCESS_NOF_LL_BITS) - 1);
        }
        else
        {
            ll_data = out_lif_null_value;
        }

        SHR_BITCOPY_RANGE(entry_payload, payload_size_rounded_up - (entry->payload_size), entry->payload, 0,
                          entry->payload_size);
        
        SHR_BITCOPY_RANGE(entry_payload, 0, entry->key, 0, entry->key_size);

        
        total_length += DNX_ADAPTER_MDB_EEDB_WRITE_LL_SIZE +
            DNX_ADAPTER_MDB_EEDB_WRITE_PHASE_SIZE +
            DNX_ADAPTER_MDB_EEDB_WRITE_PAYLOAD_LEN_SIZE + (sizeof(uint8) * BITS2BYTES(table_payload_size));

    }

    padding_length = ((total_length % sizeof(uint32)) == 0) ? 0 : sizeof(uint32) - (total_length % sizeof(uint32));
    total_length += padding_length;

    
    buffer[write_index] = UPDATE_FRM_SOC_LOGICAL_ACCESS;
    write_index += DNX_ADAPTER_MDB_OPCODE_SIZE;

    
    nw_order_int = htonl(total_length);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += DNX_ADAPTER_MDB_PAYLOAD_LEN_SIZE;

    
    cmic_blk = SOC_BLOCK2SCH(unit, MDB_BLOCK(unit));

    
    nw_order_int = htonl(cmic_blk);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_BLOCK_ID_SIZE;

    
    buffer[write_index] = UPDATE_FRM_SOC_LOGICAL_MEM_TYPE_EEDB;
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_MEMORY_TYPE_SIZE;

    
    buffer[write_index] = command;
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_COMMAND_SIZE;

    
    buffer[write_index] = DNX_ADAPTER_MDB_LOGICAL_EEDB;
    write_index += DNX_ADAPTER_MDB_LOGICAL_ACCESS_MEMORY_ID_SIZE;

    nw_order_int = htonl(entry->key[0]);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += DNX_ADAPTER_MDB_EEDB_KEY_SIZE;

    
    if (command == UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE)
    {

        nw_order_int = htonl(ll_data);
        sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
        write_index += DNX_ADAPTER_MDB_EEDB_WRITE_LL_SIZE;

        physical_phase = dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1;

        for (logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1;
             logical_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; logical_phase++)
        {
            physical_phase_tmp =
                (dnx_data_lif.out_lif.logical_to_physical_phase_map_get(unit, logical_phase + 1))->physical_phase;

            if (physical_phase == physical_phase_tmp)
            {
                break;
            }
        }

        buffer[write_index] = logical_phase;
        write_index += DNX_ADAPTER_MDB_EEDB_WRITE_PHASE_SIZE;

        nw_order_int = htonl(table_payload_size);
        sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
        write_index += DNX_ADAPTER_MDB_EEDB_WRITE_PAYLOAD_LEN_SIZE;

        SHR_IF_ERR_EXIT(adapter_mdb_access_uint32_to_uint8
                        (unit, entry_payload, &buffer[write_index], table_payload_size));
        write_index += sizeof(uint8) * BITS2BYTES(table_payload_size);
    }

    
    for (padding_iter = 0; padding_iter < padding_length; padding_iter++)
    {
        buffer[write_index] = 0;
        write_index += sizeof(char);
    }

    *write_index_outer = write_index;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
adapter_mdb_access_eedb_read(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    int write_index = 0;
    uint8 buffer[ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8];
    int adapter_mem_reg_fd;
    fd_set read_vect;
    uint32 out_lif_null_value;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(buffer, 0x0, sizeof(buffer));
    adapter_context_db.params.adapter_mem_reg_mutex.take(unit, adapter_sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_mem_reg_fd.get(unit, adapter_sub_unit_id, &adapter_mem_reg_fd);
    
    SHR_IF_ERR_EXIT(adapter_mdb_access_eedb_prep_buffer
                    (unit, dbal_physical_table_id, app_id, 0, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_READ,
                     &write_index, buffer));

    
    if (writen(adapter_mem_reg_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error: adapter_mdb_access_eedb_read: writing the opcode failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }

    FD_ZERO(&read_vect);
    FD_SET(adapter_mem_reg_fd, &read_vect);

    
    while (1)
    {
        if (select(adapter_mem_reg_fd + 1, &read_vect, (fd_set *) 0x0, (fd_set *) 0x0, NULL) < 0)
        {
            if (errno == EINTR)
            {
                
                continue;
            }
            perror("get_command: select error");
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }
        else
        {
            break;
        }
    }

    
    if (FD_ISSET(adapter_mem_reg_fd, &read_vect))
    {
        uint32 entry_payload[MDB_MAX_DIRECT_PAYLOAD_SIZE_32];
        uint32 nw_order_int;
        uint32 ll_data;
        int8 err_id;

        sal_memset(entry_payload, 0x0, sizeof(entry_payload));
        sal_memset(buffer, 0x0, sizeof(buffer));

        
        readn(adapter_mem_reg_fd, &(buffer[DNX_ADAPTER_MDB_OPCODE_OFFSET]), DNX_ADAPTER_MDB_HEADER_SIZE);

        
        sal_memcpy(&nw_order_int, &buffer[DNX_ADAPTER_MDB_PAYLOAD_LEN_OFFSET], DNX_ADAPTER_MDB_PAYLOAD_LEN_SIZE);

        
        readn(adapter_mem_reg_fd, &(buffer[DNX_ADAPTER_MDB_LOGICAL_ACCESS_OFFSET]), ntohl(nw_order_int));

        
        err_id = (int8) buffer[DNX_ADAPTER_MDB_EEDB_READ_RES_ERROR_OFFSET];

        if (err_id != 0)
        {
            if (err_id == LOGICAL_MEM_ACCESS_ERR_KEY_NOT_FOUND)
            {
                
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            }
            else
            {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "adapter EEDB read returned an error (%d)\n"), err_id));
                SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            }
            SHR_EXIT();
        }
        else
        {

            sal_memcpy(&ll_data, &buffer[DNX_ADAPTER_MDB_EEDB_READ_RES_LL_OFFSET],
                       DNX_ADAPTER_MDB_EEDB_READ_RES_LL_SIZE);

            
            sal_memcpy(&nw_order_int, &buffer[DNX_ADAPTER_MDB_EEDB_READ_RES_PAYLOAD_LEN_OFFSET],
                       DNX_ADAPTER_MDB_EEDB_READ_RES_PAYLOAD_LEN_SIZE);

            
            SHR_IF_ERR_EXIT(adapter_mdb_access_uint8_to_uint32
                            (unit, &buffer[DNX_ADAPTER_MDB_EEDB_READ_RES_PAYLOAD_OFFSET], entry_payload,
                             ntohl(nw_order_int)));

            out_lif_null_value = dnx_data_lif.global_lif.egress_out_lif_null_value_get(unit);

            SHR_BITCOPY_RANGE(entry->payload, ADAPTER_MDB_ACCESS_NOF_LL_BITS, entry_payload,
                              ntohl(nw_order_int) - entry->payload_size, entry->payload_size);

            SHR_BITCOPY_RANGE(entry->payload, 0, entry_payload, ntohl(nw_order_int) - entry->payload_size,
                              entry->payload_size);

            if (ll_data != out_lif_null_value)
            {
                entry->payload[0] &= ~((1 << ADAPTER_MDB_ACCESS_NOF_LL_BITS) - 1);
                entry->payload[0] |= (ntohl(ll_data) & ((1 << ADAPTER_MDB_ACCESS_NOF_LL_BITS) - 1));
            }
        }
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_TIMEOUT);
        
        SHR_EXIT();
    }
exit:
    adapter_context_db.params.adapter_mem_reg_mutex.give(unit, adapter_sub_unit_id);
    SHR_FUNC_EXIT;
}


shr_error_e
adapter_mdb_access_eedb_write(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    int ll_exist,
    dbal_physical_entry_t * entry)
{
    int adapter_mem_reg_fd;
    int write_index = 0;
    uint8 buffer[ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8];
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(buffer, 0x0, sizeof(buffer));
    adapter_context_db.params.adapter_mem_reg_mutex.take(unit, adapter_sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_mem_reg_fd.get(unit, adapter_sub_unit_id, &adapter_mem_reg_fd);
    
    SHR_IF_ERR_EXIT(adapter_mdb_access_eedb_prep_buffer
                    (unit, dbal_physical_table_id, app_id, ll_exist, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE,
                     &write_index, buffer));

    
    if (writen(adapter_mem_reg_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error: adapter_mdb_access_eedb_write: writing the opcode failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }
exit:
    adapter_context_db.params.adapter_mem_reg_mutex.give(unit, adapter_sub_unit_id);
    SHR_FUNC_EXIT;
}


shr_error_e
adapter_mdb_access_em_write(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    int adapter_mem_reg_fd;
    int write_index = 0;
    uint8 buffer[ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8];
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(buffer, 0x0, sizeof(buffer));

    adapter_context_db.params.adapter_mem_reg_mutex.take(unit, adapter_sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_mem_reg_fd.get(unit, adapter_sub_unit_id, &adapter_mem_reg_fd);
    
    if ((dbal_physical_table_id == DBAL_PHYSICAL_TABLE_GLEM_1) ||
        (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_GLEM_2))
    {

        SHR_IF_ERR_EXIT(adapter_mdb_access_glem_prep_buffer
                        (unit, DNX_ADAPTER_MDB_LOGICAL_GLEM_1, app_id, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE,
                         &write_index, buffer));

        SHR_IF_ERR_EXIT(adapter_mdb_access_glem_prep_buffer
                        (unit, DNX_ADAPTER_MDB_LOGICAL_GLEM_2, app_id, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE,
                         &write_index, buffer));

        SHR_IF_ERR_EXIT(adapter_mdb_access_glem_prep_buffer
                        (unit, DNX_ADAPTER_MDB_LOGICAL_GLEM_3, app_id, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE,
                         &write_index, buffer));
    }
    else
    {
        SHR_IF_ERR_EXIT(adapter_mdb_access_em_prep_buffer
                        (unit, dbal_physical_table_id, app_id, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_WRITE,
                         &write_index, buffer));
    }

    
    if (writen(adapter_mem_reg_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error: adapter_mdb_access_em_write: writing the opcode failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }
exit:
    adapter_context_db.params.adapter_mem_reg_mutex.give(unit, adapter_sub_unit_id);
    SHR_FUNC_EXIT;
}


shr_error_e
adapter_mdb_access_em_delete(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    int adapter_mem_reg_fd;
    int write_index = 0;
    uint8 buffer[ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8];
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(buffer, 0x0, sizeof(buffer));
    adapter_context_db.params.adapter_mem_reg_mutex.take(unit, adapter_sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_mem_reg_fd.get(unit, adapter_sub_unit_id, &adapter_mem_reg_fd);
    
    if ((dbal_physical_table_id == DBAL_PHYSICAL_TABLE_GLEM_1) ||
        (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_GLEM_2))
    {

        SHR_IF_ERR_EXIT(adapter_mdb_access_glem_prep_buffer
                        (unit, DNX_ADAPTER_MDB_LOGICAL_GLEM_1, app_id, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_DELETE,
                         &write_index, buffer));
        SHR_IF_ERR_EXIT(adapter_mdb_access_glem_prep_buffer
                        (unit, DNX_ADAPTER_MDB_LOGICAL_GLEM_2, app_id, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_DELETE,
                         &write_index, buffer));
        SHR_IF_ERR_EXIT(adapter_mdb_access_glem_prep_buffer
                        (unit, DNX_ADAPTER_MDB_LOGICAL_GLEM_3, app_id, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_DELETE,
                         &write_index, buffer));
    }
    else
    {
        SHR_IF_ERR_EXIT(adapter_mdb_access_em_prep_buffer
                        (unit, dbal_physical_table_id, app_id, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_DELETE,
                         &write_index, buffer));
    }

    
    if (writen(adapter_mem_reg_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error: adapter_mdb_access_em_write: writing the opcode failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }
exit:
    adapter_context_db.params.adapter_mem_reg_mutex.give(unit, adapter_sub_unit_id);
    SHR_FUNC_EXIT;
}


shr_error_e
adapter_mdb_access_em_read(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    int write_index = 0;
    uint8 buffer[ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8];
    int adapter_mem_reg_fd;
    fd_set read_vect;
    uint32 nw_order_transaction_byte_size;
    uint32 nw_order_key_bit_size;
    uint32 nw_order_payload_bit_size;
    uint8 vmv_size;
    uint8 vmv_value;
    uint32 entry_payload[MDB_MAX_EM_PAYLOAD_SIZE_32];
    int glem_logical_db_id;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(buffer, 0x0, sizeof(buffer));

    adapter_context_db.params.adapter_mem_reg_mutex.take(unit, adapter_sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_mem_reg_fd.get(unit, adapter_sub_unit_id, &adapter_mem_reg_fd);
    
    if ((dbal_physical_table_id == DBAL_PHYSICAL_TABLE_GLEM_1) ||
        (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_GLEM_2))
    {
        glem_logical_db_id =
            (dbal_physical_table_id ==
             DBAL_PHYSICAL_TABLE_GLEM_1) ? DNX_ADAPTER_MDB_LOGICAL_GLEM_1 : DNX_ADAPTER_MDB_LOGICAL_GLEM_2;
        
        SHR_IF_ERR_EXIT(adapter_mdb_access_glem_prep_buffer
                        (unit, glem_logical_db_id, app_id, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_READ,
                         &write_index, buffer));
    }
    else
    {
        SHR_IF_ERR_EXIT(adapter_mdb_access_em_prep_buffer
                        (unit, dbal_physical_table_id, app_id, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_READ,
                         &write_index, buffer));
    }

    
    if (writen(adapter_mem_reg_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_mdb_access_em_read: writing the opcode failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }

    FD_ZERO(&read_vect);
    FD_SET(adapter_mem_reg_fd, &read_vect);

    
    while (1)
    {
        if (select(adapter_mem_reg_fd + 1, &read_vect, (fd_set *) 0x0, (fd_set *) 0x0, NULL) < 0)
        {
            if (errno == EINTR)
            {
                
                continue;
            }
            perror("get_command: select error");
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }
        else
        {
            break;
        }
    }

    
    if (FD_ISSET(adapter_mem_reg_fd, &read_vect))
    {
        int8 err_id;
        uint32 nw_order_int;

        sal_memset(buffer, 0x0, sizeof(buffer));

        
        readn(adapter_mem_reg_fd, &(buffer[DNX_ADAPTER_MDB_OPCODE_OFFSET]), DNX_ADAPTER_MDB_HEADER_SIZE);

        
        sal_memcpy(&nw_order_transaction_byte_size, &buffer[DNX_ADAPTER_MDB_PAYLOAD_LEN_OFFSET],
                   DNX_ADAPTER_MDB_PAYLOAD_LEN_SIZE);

        
        readn(adapter_mem_reg_fd, &(buffer[DNX_ADAPTER_MDB_LOGICAL_ACCESS_OFFSET]),
              ntohl(nw_order_transaction_byte_size));

        
        if ((dbal_physical_table_id == DBAL_PHYSICAL_TABLE_GLEM_1) ||
            (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_GLEM_2))
        {
            err_id = (int8) buffer[DNX_ADAPTER_MDB_GLEM_READ_RES_ERROR_OFFSET];
        }
        else
        {
            sal_memcpy(&nw_order_key_bit_size, &buffer[DNX_ADAPTER_MDB_EM_KEY_LEN_OFFSET],
                       DNX_ADAPTER_MDB_EM_KEY_LEN_SIZE);
            
            err_id = (int8) buffer[DNX_ADAPTER_MDB_EM_KEY_OFFSET + BITS2BYTES(ntohl(nw_order_key_bit_size))];
        }

        if (err_id != 0)
        {
            if (err_id == LOGICAL_MEM_ACCESS_ERR_KEY_NOT_FOUND)
            {
                
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            }
            else
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "adapter Exact match Table read returned an error (%d)\n"), err_id));
                SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            }
            SHR_EXIT();
        }
        else
        {

            if ((dbal_physical_table_id == DBAL_PHYSICAL_TABLE_GLEM_1) ||
                (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_GLEM_2))
            {
                
                sal_memcpy(&nw_order_int, &buffer[DNX_ADAPTER_MDB_GLEM_READ_RES_PAYLOAD_OFFSET],
                           DNX_ADAPTER_MDB_GLEM_READ_RES_PAYLOAD_SIZE);
                entry->payload[0] = ntohl(nw_order_int);

            }
            else
            {
                sal_memset(entry_payload, 0x0, MDB_MAX_EM_PAYLOAD_SIZE_32 * sizeof(entry_payload[0]));
                SHR_IF_ERR_EXIT(mdb_em_vmv_calculation
                                (unit, dbal_physical_table_id, app_id, entry, &vmv_size, &vmv_value));

                
                sal_memcpy(&nw_order_payload_bit_size,
                           &buffer[DNX_ADAPTER_MDB_EM_KEY_OFFSET + BITS2BYTES(ntohl(nw_order_key_bit_size)) +
                                   DNX_ADAPTER_MDB_EM_READ_RES_ERROR_SIZE],
                           DNX_ADAPTER_MDB_EM_READ_RES_PAYLOAD_LEN_SIZE);

                SHR_IF_ERR_EXIT(adapter_mdb_access_uint8_to_uint32
                                (unit, &buffer[DNX_ADAPTER_MDB_EM_KEY_OFFSET +
                                               BITS2BYTES(ntohl(nw_order_key_bit_size)) +
                                               DNX_ADAPTER_MDB_EM_READ_RES_ERROR_SIZE +
                                               DNX_ADAPTER_MDB_EM_READ_RES_PAYLOAD_LEN_SIZE],
                                 entry_payload, ntohl(nw_order_payload_bit_size)));

                SHR_BITCOPY_RANGE(entry->payload, 0, entry_payload,
                                  ntohl(nw_order_payload_bit_size) - (entry->payload_size + vmv_size),
                                  entry->payload_size);

            }
        }
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_TIMEOUT);
        
        SHR_EXIT();
    }
exit:
    adapter_context_db.params.adapter_mem_reg_mutex.give(unit, adapter_sub_unit_id);
    SHR_FUNC_EXIT;
}


shr_error_e
adapter_mdb_access_direct_table_read(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    int write_index = 0;
    uint8 buffer[ADAPTER_MDB_ACCESS_MAX_BUFFER_SIZE_U8];
    int adapter_mem_reg_fd;
    fd_set read_vect;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(buffer, 0x0, sizeof(buffer));
    adapter_context_db.params.adapter_mem_reg_mutex.take(unit, adapter_sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_mem_reg_fd.get(unit, adapter_sub_unit_id, &adapter_mem_reg_fd);
    
    SHR_IF_ERR_EXIT(adapter_mdb_access_direct_table_prep_buffer
                    (unit, dbal_physical_table_id, app_id, entry, UPDATE_FRM_SOC_LOGICAL_ACCESS_READ,
                     &write_index, buffer));

    
    if (writen(adapter_mem_reg_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Error: adapter_mdb_access_direct_table_read: writing the opcode failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }

    FD_ZERO(&read_vect);
    FD_SET(adapter_mem_reg_fd, &read_vect);

    
    while (1)
    {
        if (select(adapter_mem_reg_fd + 1, &read_vect, (fd_set *) 0x0, (fd_set *) 0x0, NULL) < 0)
        {
            if (errno == EINTR)
            {
                
                continue;
            }
            perror("get_command: select error");
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }
        else
        {
            break;
        }
    }

    
    if (FD_ISSET(adapter_mem_reg_fd, &read_vect))
    {
        uint32 nw_order_int;
        int8 err_id;

        sal_memset(buffer, 0x0, sizeof(buffer));

        
        readn(adapter_mem_reg_fd, &(buffer[DNX_ADAPTER_MDB_OPCODE_OFFSET]), DNX_ADAPTER_MDB_HEADER_SIZE);

        
        sal_memcpy(&nw_order_int, &buffer[DNX_ADAPTER_MDB_PAYLOAD_LEN_OFFSET], DNX_ADAPTER_MDB_PAYLOAD_LEN_SIZE);

        
        readn(adapter_mem_reg_fd, &(buffer[DNX_ADAPTER_MDB_LOGICAL_ACCESS_OFFSET]), ntohl(nw_order_int));

        
        err_id = (int8) buffer[DNX_ADAPTER_MDB_DIRECT_READ_RES_ERROR_OFFSET];

        if (err_id != 0)
        {
            if (err_id == LOGICAL_MEM_ACCESS_ERR_KEY_NOT_FOUND)
            {
                
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            }
            else
            {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "adapter Direct Table read returned an error (%d)\n"), err_id));
                SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            }
            SHR_EXIT();
        }
        else
        {
            uint32 entry_payload[MDB_MAX_DIRECT_PAYLOAD_SIZE_32];

            sal_memset(entry_payload, 0x0, sizeof(entry_payload));
            
            sal_memcpy(&nw_order_int, &buffer[DNX_ADAPTER_MDB_DIRECT_READ_RES_PAYLOAD_LEN_OFFSET],
                       DNX_ADAPTER_MDB_DIRECT_READ_RES_PAYLOAD_LEN_SIZE);

            SHR_IF_ERR_EXIT(adapter_mdb_access_uint8_to_uint32
                            (unit, &buffer[DNX_ADAPTER_MDB_DIRECT_READ_RES_PAYLOAD_OFFSET], entry_payload,
                             ntohl(nw_order_int)));

            SHR_BITCOPY_RANGE(entry->payload, 0, entry_payload, ntohl(nw_order_int) - entry->payload_size,
                              entry->payload_size);
        }
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_TIMEOUT);
        
        SHR_EXIT();
    }
exit:
    adapter_context_db.params.adapter_mem_reg_mutex.give(unit, adapter_sub_unit_id);
    SHR_FUNC_EXIT;
}




#endif
