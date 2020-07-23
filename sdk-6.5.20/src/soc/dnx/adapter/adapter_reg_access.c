
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. $
 */

int
tmp_workaround_func(
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
#include <soc/register.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_adapter.h>
#include <soc/dnx/swstate/auto_generated/access/adapter_access.h>
#include <soc/dnx/swstate/auto_generated/types/adapter_types.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>

#include <soc/dcmn/error.h>

#include <netinet/tcp.h>
#include <soc/dnx/adapter/adapter_reg_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>




#define DEFAULT_ADAPTER_SERVER_PORT 6816

#define DEFAULT_ADAPTER_SERVER_ADDRESS "localhost"

#define HOST_ADDRESS_MAX_LEN 40

#define MAX_PACKET_HEADER_SIZE_ADAPTER 30

#define MEM_RESPONSE_HEADER_SIZE_ADAPTER 13

#define SIGNALS_RESPONSE_LENGTH_SIZE 4

#define ADAPTER_SIGNALS_CONSTANT_HEADER_SIZE 9

#define ADAPTER_BLOCK_NAMES_CONSTANT_HEADER_SIZE 5

#define MIN_SIGNALS_DATA_SIZE 9

#define MAX_SIGNALS_DATA_SIZE 8192*2

#define SIGNALS_REQUEST_HEADER_SIZE 5

#define ADAPTER_HIT_INDICATION_HEADER_SIZE 5

#define ADAPTER_TBL_CONTROL_GET_HEADER_SIZE 5

#define ADAPTER_HIT_INDICATION_MAX_PACKET_SIZE 500000

#define SIGNALS_ALL_MS_ID -1

#define TX_PACKET_SIGNAL 3

#define TX_PACKET_PTC_SIGNAL_ID 1

#define TX_PACKET_PTC_SIGNAL_WIDTH 10

#define TX_PACKET_TAG_SWAP_RES_SIGNAL_ID 6

#define TX_PACKET_TAG_SWAP_RES_SIGNAL_WIDTH 2

#define TX_PACKET_RECYCLE_COMMAND_SIGNAL_ID 4

#define TX_PACKET_RECYCLE_COMMAND_SIGNAL_WIDTH 16

#define TX_PACKET_VALID_BYTES_SIGNAL_ID 2

#define TX_PACKET_VALID_BYTES_SIGNAL_WIDTH 8


#define RX_THREAD_NOTIFY_CLOSED (-2)

#define MAX_ACCESS_BUFFER_SIZE 2048

#define MAX_SIZE_OF_ACCESS_ENTRY 100



#define UPDATE_FRM_SOC_WRITE_REG 128

#define UPDATE_FRM_SOC_WRITE_TBL 129


#define UPDATE_FRM_SOC_READ_REG_REQ 133

#define UPDATE_FRM_SOC_READ_TBL_REQ 134

#define UPDATE_TO_SOC_READ_REG_REP 8

#define UPDATE_TO_SOC_READ_TBL_REP 9



#define UPDATE_FRM_SOC_RCRD_ACC_ON 136

#define UPDATE_FRM_SOC_RCRD_ACC_OFF 137

#define UPDATE_FRM_SOC_READ_RCRD_ACC 138

#define UPDATE_FRM_SOC_READ_CLR_RCRD_ACC 139

#define UPDATE_TO_SOC_READ_RCRD_ACC 11

#define UPDATE_FRM_SOC_WRAPPER 200
#define FRM_SOC_WRAPPER_NOF_BYTES (5)

#define INIT_PACKET_SIZE 8


const char *conversion_tbl[] = {
    "00000000",
    "00000001",
    "00000010",
    "00000011",
    "00000100",
    "00000101",
    "00000110",
    "00000111",
    "00001000",
    "00001001",
    "00001010",
    "00001011",
    "00001100",
    "00001101",
    "00001110",
    "00001111",
    "00010000",
    "00010001",
    "00010010",
    "00010011",
    "00010100",
    "00010101",
    "00010110",
    "00010111",
    "00011000",
    "00011001",
    "00011010",
    "00011011",
    "00011100",
    "00011101",
    "00011110",
    "00011111",
    "00100000",
    "00100001",
    "00100010",
    "00100011",
    "00100100",
    "00100101",
    "00100110",
    "00100111",
    "00101000",
    "00101001",
    "00101010",
    "00101011",
    "00101100",
    "00101101",
    "00101110",
    "00101111",
    "00110000",
    "00110001",
    "00110010",
    "00110011",
    "00110100",
    "00110101",
    "00110110",
    "00110111",
    "00111000",
    "00111001",
    "00111010",
    "00111011",
    "00111100",
    "00111101",
    "00111110",
    "00111111",
    "01000000",
    "01000001",
    "01000010",
    "01000011",
    "01000100",
    "01000101",
    "01000110",
    "01000111",
    "01001000",
    "01001001",
    "01001010",
    "01001011",
    "01001100",
    "01001101",
    "01001110",
    "01001111",
    "01010000",
    "01010001",
    "01010010",
    "01010011",
    "01010100",
    "01010101",
    "01010110",
    "01010111",
    "01011000",
    "01011001",
    "01011010",
    "01011011",
    "01011100",
    "01011101",
    "01011110",
    "01011111",
    "01100000",
    "01100001",
    "01100010",
    "01100011",
    "01100100",
    "01100101",
    "01100110",
    "01100111",
    "01101000",
    "01101001",
    "01101010",
    "01101011",
    "01101100",
    "01101101",
    "01101110",
    "01101111",
    "01110000",
    "01110001",
    "01110010",
    "01110011",
    "01110100",
    "01110101",
    "01110110",
    "01110111",
    "01111000",
    "01111001",
    "01111010",
    "01111011",
    "01111100",
    "01111101",
    "01111110",
    "01111111",
    "10000000",
    "10000001",
    "10000010",
    "10000011",
    "10000100",
    "10000101",
    "10000110",
    "10000111",
    "10001000",
    "10001001",
    "10001010",
    "10001011",
    "10001100",
    "10001101",
    "10001110",
    "10001111",
    "10010000",
    "10010001",
    "10010010",
    "10010011",
    "10010100",
    "10010101",
    "10010110",
    "10010111",
    "10011000",
    "10011001",
    "10011010",
    "10011011",
    "10011100",
    "10011101",
    "10011110",
    "10011111",
    "10100000",
    "10100001",
    "10100010",
    "10100011",
    "10100100",
    "10100101",
    "10100110",
    "10100111",
    "10101000",
    "10101001",
    "10101010",
    "10101011",
    "10101100",
    "10101101",
    "10101110",
    "10101111",
    "10110000",
    "10110001",
    "10110010",
    "10110011",
    "10110100",
    "10110101",
    "10110110",
    "10110111",
    "10111000",
    "10111001",
    "10111010",
    "10111011",
    "10111100",
    "10111101",
    "10111110",
    "10111111",
    "11000000",
    "11000001",
    "11000010",
    "11000011",
    "11000100",
    "11000101",
    "11000110",
    "11000111",
    "11001000",
    "11001001",
    "11001010",
    "11001011",
    "11001100",
    "11001101",
    "11001110",
    "11001111",
    "11010000",
    "11010001",
    "11010010",
    "11010011",
    "11010100",
    "11010101",
    "11010110",
    "11010111",
    "11011000",
    "11011001",
    "11011010",
    "11011011",
    "11011100",
    "11011101",
    "11011110",
    "11011111",
    "11100000",
    "11100001",
    "11100010",
    "11100011",
    "11100100",
    "11100101",
    "11100110",
    "11100111",
    "11101000",
    "11101001",
    "11101010",
    "11101011",
    "11101100",
    "11101101",
    "11101110",
    "11101111",
    "11110000",
    "11110001",
    "11110010",
    "11110011",
    "11110100",
    "11110101",
    "11110110",
    "11110111",
    "11111000",
    "11111001",
    "11111010",
    "11111011",
    "11111100",
    "11111101",
    "11111110",
    "11111111"
};

typedef enum
{
    ADAPTER_SIGNALS_REQUEST_OPCODE,
    ADAPTER_SIGNALS_RESPONSE_OPCODE,
    ADAPTER_BLOCK_NAMES_REQUEST_OPCODE,
    ADAPTER_BLOCK_NAMES_RESPONSE_OPCODE,
    ADAPTER_TBL_CONTROL_GET_REQUEST_OPCODE,
    ADAPTER_TBL_CONTROL_GET_RESPONSE_OPCODE
} adapter_opcode_e;


typedef struct
{
    
    uint8 *p_access_buffer;
    uint8 *p_access_buffer_current_position;
    
    uint32 collect_adapter_accesses;

} adapter_access_info_t;


adapter_access_info_t *adapter_info[BCM_LOCAL_UNITS_MAX][DNX_DATA_MAX_DEVICE_GENERAL_NOF_SUB_UNITS] = { {0} };

int adapter_sub_unit_id = 0;


int adapter_reg32_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 *data);


int adapter_reg64_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint64 *data);


int adapter_reg_above64_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    soc_reg_above_64_val_t data);


int adapter_reg32_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 data);


int adapter_reg64_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint64 data);


int adapter_reg_above64_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    soc_reg_above_64_val_t data);


int adapter_mem_array_read(
    int unit,
    soc_mem_t mem,
    unsigned int array_index,
    int copyno,
    int index,
    void *entry_data);


int adapter_mem_array_write(
    int unit,
    soc_mem_t mem,
    unsigned int array_index,
    int copyno,
    int index,
    void *entry_data);


soc_reg_access_t adapter_access = {
    adapter_reg32_get,
    adapter_reg64_get,
    adapter_reg_above64_get,

    adapter_reg32_set,
    adapter_reg64_set,
    adapter_reg_above64_set,

    adapter_mem_array_read,
    adapter_mem_array_write
};

extern char *getenv(
    const char *);
extern int _soc_mem_write_copyno_update(
    int unit,
    soc_mem_t mem,
    int *copyno,
    int *copyno_override);


static soc_error_t
adapter_memreg_read_response(
    int unit,
    int sub_unit_id,
    int is_mem,
    uint32 data_len,
    uint32 *data)
{
    fd_set read_vect;
    char swapped_header[MEM_RESPONSE_HEADER_SIZE_ADAPTER];
    uint32 nw_order_int;
    uint32 rtl_address;
    int8 opcode;
    int block_id;
    int read_length = 0;
    uint32 actual_data_len;
    uint32 half_actual_data_len;
    int ii;
    char *data_char_ptr = (char *) data;
    int adapter_mem_reg_fd;

    
    int tmp;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_mem_reg_fd.get(unit, sub_unit_id, &adapter_mem_reg_fd);

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
        read_length = readn(adapter_mem_reg_fd, &(swapped_header[0]), MEM_RESPONSE_HEADER_SIZE_ADAPTER);

        
        if (read_length < MEM_RESPONSE_HEADER_SIZE_ADAPTER)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "adapter_read_buffer: could not read packet length\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        
        opcode = *((int8 *) &(swapped_header[0]));

        
        sal_memcpy(&nw_order_int, &swapped_header[1], sizeof(int));
        
        actual_data_len = ntohl(nw_order_int) - 8;

        
        sal_memcpy(&nw_order_int, &swapped_header[5], sizeof(int));
        block_id = ntohl(nw_order_int);

        
        sal_memcpy(&nw_order_int, &swapped_header[9], sizeof(int));
        rtl_address = ntohl(nw_order_int);

        LOG_INFO(BSL_LS_SYS_VERINET,
                 (BSL_META("adapter_memreg_read_response: opcode=%d actual_data_len=%d block_id=%d rtl_address=%d\n"),
                  opcode, actual_data_len, block_id, rtl_address));

        read_length = readn(adapter_mem_reg_fd, data, actual_data_len);

        
        
        if (read_length < actual_data_len)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "adapter_memreg_read_response: could not read data\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        
        half_actual_data_len = actual_data_len >> 1;
        for (ii = 0; ii < half_actual_data_len; ii++)
        {
            tmp = data_char_ptr[ii];
            data_char_ptr[ii] = data_char_ptr[actual_data_len - 1 - ii];
            data_char_ptr[actual_data_len - 1 - ii] = tmp;
        }

    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_TIMEOUT);
        
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
adapter_signals_read_response(
    int unit,
    uint32 *signal_data_length,
    char **signal_data)
{
    fd_set read_vect;
    int packet_size;
    int8 opcode;
    uint32 ms_id;
    int read_constant_header = 0;
    char buffer[ADAPTER_SIGNALS_CONSTANT_HEADER_SIZE];
    char *signal_data_dyn;
    int buffer_offset = 0;
    uint32 long_val;
    int adapter_sdk_interface_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_sdk_interface_fd.get(unit, adapter_sub_unit_id, &adapter_sdk_interface_fd);

    FD_ZERO(&read_vect);
    FD_SET(adapter_sdk_interface_fd, &read_vect);

    while (1)
    {
        if (select(adapter_sdk_interface_fd + 1, &read_vect, (fd_set *) 0x0, (fd_set *) 0x0, NULL) < 0)
        {
            if (errno == EINTR)
            {
                
                continue;
            }
            perror("get_command: select error");
            SHR_ERR_EXIT(_SHR_E_FAIL, "select error\n");
        }
        else
        {
            break;
        }
    }

    
    if (FD_ISSET(adapter_sdk_interface_fd, &read_vect))
    {
        
        read_constant_header = readn(adapter_sdk_interface_fd, buffer, ADAPTER_SIGNALS_CONSTANT_HEADER_SIZE);

        if (read_constant_header < ADAPTER_SIGNALS_CONSTANT_HEADER_SIZE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "adapter_read_buffer: could not read the constant header \n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        
        long_val = ntohl(*(uint32 *) &(buffer[buffer_offset]));
        packet_size = long_val;
        buffer_offset += sizeof(packet_size);

        if ((packet_size < MIN_SIGNALS_DATA_SIZE) || (packet_size > MAX_SIGNALS_DATA_SIZE))
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Adapter signal invalid packet size\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        
        opcode = buffer[buffer_offset];
        buffer_offset += sizeof(opcode);

        
        if (opcode != ADAPTER_SIGNALS_RESPONSE_OPCODE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Wrong opcode in signals response message\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        
        long_val = ntohl(*(uint32 *) &(buffer[buffer_offset]));
        ms_id = long_val;
        buffer_offset += sizeof(ms_id);

        
        *signal_data_length = packet_size - buffer_offset + 4;
        signal_data_dyn = sal_alloc(*signal_data_length, "adapter signals buffer");
        readn(adapter_sdk_interface_fd, signal_data_dyn, *signal_data_length);
        *signal_data = signal_data_dyn;

    }
    else
    {
        
        SHR_ERR_EXIT(_SHR_E_TIMEOUT, "Timeout on signal request\n");
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
adapter_get_signal(
    int unit,
    uint32 ms_id,
    uint32 *signal_data_length,
    char **signal_data)
{
    uint32 nw_order_int;
    int write_index = 0;
    uint32 host_order_int;
    char buffer[SIGNALS_REQUEST_HEADER_SIZE];
    int adapter_sdk_interface_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_sdk_interface_fd.get(unit, adapter_sub_unit_id, &adapter_sdk_interface_fd);

    LOG_INFO(BSL_LS_SYS_VERINET, (BSL_META("adapter_signals_write_request: unit=%d ms_id=%d \n"), unit, ms_id));

    adapter_context_db.params.adapter_sdk_interface_mutex.take(unit, adapter_sub_unit_id, sal_mutex_FOREVER);

    
    buffer[write_index] = ADAPTER_SIGNALS_REQUEST_OPCODE;
    write_index += sizeof(char);

    
    host_order_int = ms_id;
    nw_order_int = htonl(host_order_int);
    sal_memcpy(&(buffer[write_index]), &nw_order_int, sizeof(ms_id));
    write_index += sizeof(ms_id);

    
    if (writen(adapter_sdk_interface_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_signals_write_request data failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }
    
    SHR_IF_ERR_EXIT(adapter_signals_read_response(unit, signal_data_length, signal_data));

exit:
    adapter_context_db.params.adapter_sdk_interface_mutex.give(unit, adapter_sub_unit_id);
    SHR_FUNC_EXIT;
}


int
adapter_blocks_read_response(
    int unit,
    int *block_names_length,
    char **block_names)
{
    fd_set read_vect;
    int packet_size;
    int8 opcode;
    uint32 long_val;
    int buffer_offset = 0;
    char buffer[ADAPTER_BLOCK_NAMES_CONSTANT_HEADER_SIZE];
    char *block_names_dyn;
    int read_constant_header = 0;
    int adapter_sdk_interface_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_sdk_interface_fd.get(unit, adapter_sub_unit_id, &adapter_sdk_interface_fd);

    FD_ZERO(&read_vect);
    FD_SET(adapter_sdk_interface_fd, &read_vect);

    while (1)
    {
        if (select(adapter_sdk_interface_fd + 1, &read_vect, (fd_set *) 0x0, (fd_set *) 0x0, NULL) < 0)
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

    
    if (FD_ISSET(adapter_sdk_interface_fd, &read_vect))
    {
        read_constant_header = readn(adapter_sdk_interface_fd, buffer, ADAPTER_BLOCK_NAMES_CONSTANT_HEADER_SIZE);
        
        if (read_constant_header < ADAPTER_BLOCK_NAMES_CONSTANT_HEADER_SIZE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "adapter_read_buffer: could not read the constant header \n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        
        long_val = ntohl(*(uint32 *) &(buffer[buffer_offset]));
        packet_size = long_val;
        buffer_offset += sizeof(packet_size);

        if ((packet_size <= MIN_SIGNALS_DATA_SIZE) || (packet_size > MAX_SIGNALS_DATA_SIZE))
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Adapter signal invalid packet size\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        
        opcode = buffer[buffer_offset];
        buffer_offset += sizeof(opcode);

        if (opcode != ADAPTER_BLOCK_NAMES_RESPONSE_OPCODE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Adapter block_names wrong opcode \n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        
        *block_names_length = packet_size - buffer_offset + 4;
        block_names_dyn = sal_alloc(*block_names_length, "adapter block names buffer");
        readn(adapter_sdk_interface_fd, block_names_dyn, *block_names_length);
        *block_names = block_names_dyn;
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_TIMEOUT);
        
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}


uint32
adapter_init_rx_tx_socket(
    int unit,
    int sub_unit_id)
{
    uint32 nw_order_int;
    uint32 index_position = 0;
    char packet_data[INIT_PACKET_SIZE];
    int adapter_rx_tx_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_rx_tx_mutex.take(unit, sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_rx_tx_fd.get(unit, sub_unit_id, &adapter_rx_tx_fd);

    
    packet_data[index_position++] = 0;
    packet_data[index_position++] = 0;

    
    packet_data[index_position++] = dnx_data_adapter.general.lib_ver_get(unit);

    
    packet_data[index_position++] = 2;

    
    nw_order_int = htonl(-1);
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    
    if (writen(adapter_rx_tx_fd, packet_data, index_position) != index_position)
    {
        adapter_context_db.params.adapter_rx_tx_mutex.give(unit, sub_unit_id);
        SHR_ERR_EXIT(_SHR_E_PORT, "Error: adapter_init_rx_tx_socket failed\n");
    }

    adapter_context_db.params.adapter_rx_tx_mutex.give(unit, sub_unit_id);

exit:
    SHR_FUNC_EXIT;
}


int
adapter_get_block_names(
    int unit,
    int *block_names_length,
    char **block_names)
{
    int write_index = 0;
    char buffer[SIGNALS_REQUEST_HEADER_SIZE];
    int adapter_sdk_interface_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_sdk_interface_mutex.take(unit, adapter_sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_sdk_interface_fd.get(unit, adapter_sub_unit_id, &adapter_sdk_interface_fd);

    
    buffer[write_index] = ADAPTER_BLOCK_NAMES_REQUEST_OPCODE;
    write_index += sizeof(char);

    
    if (writen(adapter_sdk_interface_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_get_block_names writing the opcode failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }
    
    SHR_IF_ERR_EXIT(adapter_blocks_read_response(unit, block_names_length, block_names));

exit:
    adapter_context_db.params.adapter_sdk_interface_mutex.give(unit, adapter_sub_unit_id);
    SHR_FUNC_EXIT;
}


int
adapter_read_hit_bits_response(
    int unit,
    uint32 *record_length,
    char **recorded_data)
{
    fd_set read_vect;
    uint32 packet_size;
    char opcode;
    uint32 long_val;
    int buffer_offset = 0;
    uint8 buffer[ADAPTER_HIT_INDICATION_HEADER_SIZE];
    char *record_dyn;
    int read_constant_header = 0;
    int adapter_mem_reg_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_mem_reg_fd.get(unit, adapter_sub_unit_id, &adapter_mem_reg_fd);

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
        
        read_constant_header = readn(adapter_mem_reg_fd, buffer, ADAPTER_HIT_INDICATION_HEADER_SIZE);

        if (read_constant_header < ADAPTER_HIT_INDICATION_HEADER_SIZE)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "adapter_read_hit_indication_record: could not read the constant header \n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        
        opcode = buffer[buffer_offset];
        buffer_offset += sizeof(opcode);

        if (opcode != UPDATE_TO_SOC_READ_RCRD_ACC)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Adapter read_record wrong opcode \n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        
        long_val = ntohl(*(uint32 *) &(buffer[buffer_offset]));
        packet_size = long_val;
        buffer_offset += sizeof(packet_size);

        if (packet_size < 0 || packet_size > ADAPTER_HIT_INDICATION_MAX_PACKET_SIZE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Adapter hit indication invalid packet size\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        
        *record_length = packet_size;
        record_dyn = sal_alloc(*record_length, "adapter hit indications buffer");
        readn(adapter_mem_reg_fd, record_dyn, *record_length);
        *recorded_data = record_dyn;
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_TIMEOUT);
        
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}


int
adapter_read_hit_bits(
    int unit,
    int read_and_delete,
    uint32 *record_length,
    char **recorded_data)
{
    int write_index = 0;
    uint8 buffer[ADAPTER_HIT_INDICATION_HEADER_SIZE];
    uint32 length = 0;
    uint32 nw_order_int;
    int adapter_mem_reg_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_mem_reg_mutex.take(unit, adapter_sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_mem_reg_fd.get(unit, adapter_sub_unit_id, &adapter_mem_reg_fd);

    if (read_and_delete == 1)
    {
        
        buffer[write_index] = UPDATE_FRM_SOC_READ_CLR_RCRD_ACC;
    }
    else
    {
        
        buffer[write_index] = UPDATE_FRM_SOC_READ_RCRD_ACC;
    }

    write_index += sizeof(char);

    
    nw_order_int = htonl(length);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(length));
    write_index += sizeof(length);

    
    if (writen(adapter_mem_reg_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_read_record: writing the opcode failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }

    
    adapter_read_hit_bits_response(unit, record_length, recorded_data);

exit:
    adapter_context_db.params.adapter_mem_reg_mutex.give(unit, adapter_sub_unit_id);
    SHR_FUNC_EXIT;
}


int
control_get_table_data_response(
    int unit,
    uint32 tbl_entry_data_allocated_size,
    uint32 *tbl_entry_size,
    char *tbl_entry_data)
{
    fd_set read_vect;
    int data_size;
    int8 opcode;
    int read_constant_header = 0;
    char buffer[ADAPTER_TBL_CONTROL_GET_HEADER_SIZE];
    int buffer_offset = 0;
    uint32 long_val;
    int adapter_sdk_interface_fd;

    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_sdk_interface_fd.get(unit, adapter_sub_unit_id, &adapter_sdk_interface_fd);

    FD_ZERO(&read_vect);
    FD_SET(adapter_sdk_interface_fd, &read_vect);

    while (1)
    {
        if (select(adapter_sdk_interface_fd + 1, &read_vect, (fd_set *) 0x0, (fd_set *) 0x0, NULL) < 0)
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

    
    if (FD_ISSET(adapter_sdk_interface_fd, &read_vect))
    {
        
        read_constant_header = readn(adapter_sdk_interface_fd, buffer, ADAPTER_TBL_CONTROL_GET_HEADER_SIZE);

        if (read_constant_header < ADAPTER_TBL_CONTROL_GET_HEADER_SIZE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                                                  "control_get_table_data_response: could not read the constant header\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        
        opcode = buffer[buffer_offset];
        buffer_offset += sizeof(opcode);

        if (opcode != ADAPTER_TBL_CONTROL_GET_RESPONSE_OPCODE)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Adapter control_get_table_data_response wrong opcode \n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        
        long_val = ntohl(*(uint32 *) &(buffer[buffer_offset]));
        data_size = long_val;
        buffer_offset += sizeof(data_size);

        if (data_size < 0 || data_size > tbl_entry_data_allocated_size)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Adapter control_get_table_data_response invalid pkt size\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }

        
        readn(adapter_sdk_interface_fd, (void *) tbl_entry_data, data_size);
        *tbl_entry_size = data_size;
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_TIMEOUT);
        
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}


int
control_get_table_data(
    int unit,
    char *tbl_name,
    char *blk_name,
    int entry_index,
    uint32 tbl_entry_data_allocated_size,
    uint32 *tbl_entry_size,
    char *tbl_entry_data)
{
    int block_name_length;
    int table_name_length;
    int write_index = 0;
    char buffer[500];
    uint32 nw_order_int;
    int adapter_sdk_interface_fd;

    SHR_FUNC_INIT_VARS(unit);

    block_name_length = sal_strlen(blk_name);
    table_name_length = sal_strlen(tbl_name);

    adapter_context_db.params.adapter_sdk_interface_mutex.take(unit, adapter_sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_sdk_interface_fd.get(unit, adapter_sub_unit_id, &adapter_sdk_interface_fd);

    buffer[write_index] = ADAPTER_TBL_CONTROL_GET_REQUEST_OPCODE;
    write_index += sizeof(char);

    
    nw_order_int = htonl(block_name_length + table_name_length + 3 * sizeof(uint32));
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += sizeof(uint32);

    
    nw_order_int = htonl(block_name_length);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += sizeof(nw_order_int);

    
    sal_memcpy(&buffer[write_index], blk_name, block_name_length);
    write_index += block_name_length;

    
    nw_order_int = htonl(table_name_length);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += sizeof(nw_order_int);

    
    sal_memcpy(&buffer[write_index], tbl_name, table_name_length);
    write_index += table_name_length;

    nw_order_int = htonl(entry_index);
    sal_memcpy(&buffer[write_index], &nw_order_int, sizeof(nw_order_int));
    write_index += sizeof(nw_order_int);

    
    if (writen(adapter_sdk_interface_fd, buffer, write_index) != write_index)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_read_record: writing the opcode failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }

    
    control_get_table_data_response(unit, tbl_entry_data_allocated_size, tbl_entry_size, tbl_entry_data);

exit:
    adapter_context_db.params.adapter_sdk_interface_mutex.give(unit, adapter_sub_unit_id);
    SHR_FUNC_EXIT;
}


int
adapter_disable_collect_accesses(
    int unit,
    int sub_unit_id)
{
    SHR_FUNC_INIT_VARS(unit);

    adapter_info[unit][sub_unit_id]->collect_adapter_accesses = 0;

    SHR_FUNC_EXIT;
}


void
adapter_collect_accesses(
    int unit,
    int sub_unit_id,
    int is_get,
    int is_mem,
    uint8 *do_collect)
{

    SHR_FUNC_INIT_VARS(unit);

    *do_collect = 0;

    if ((adapter_info[unit][sub_unit_id]->collect_adapter_accesses == 1)
        && !is_get
        && adapter_info[unit][sub_unit_id]->p_access_buffer_current_position -
        adapter_info[unit][sub_unit_id]->p_access_buffer < MAX_ACCESS_BUFFER_SIZE - MAX_SIZE_OF_ACCESS_ENTRY)
    {
        *do_collect = 1;
    }

    SHR_EXIT();

exit:
    SHR_VOID_FUNC_EXIT;
}

shr_error_e
adapter_access_transaction_wrapper_set(
    int unit,
    int sub_unit_id,
    int adapter_mem_reg_fd)
{
    int overall_length;
    uint32 nw_order_int;
    SHR_FUNC_INIT_VARS(unit);

    
    *adapter_info[unit][sub_unit_id]->p_access_buffer = UPDATE_FRM_SOC_WRAPPER;

    
    overall_length =
        adapter_info[unit][sub_unit_id]->p_access_buffer_current_position -
        adapter_info[unit][sub_unit_id]->p_access_buffer - FRM_SOC_WRAPPER_NOF_BYTES;

    
    nw_order_int = htonl(overall_length);
    sal_memcpy((adapter_info[unit][sub_unit_id]->p_access_buffer + 1), &nw_order_int, sizeof(overall_length));

    
    if (writen(adapter_mem_reg_fd, adapter_info[unit][sub_unit_id]->p_access_buffer, (overall_length + 1 + 4)) !=
        (overall_length + 1 + 4))
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_memreg_access data failed\n")));
        SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        SHR_EXIT();
    }

    
    adapter_info[unit][sub_unit_id]->p_access_buffer_current_position =
        adapter_info[unit][sub_unit_id]->p_access_buffer + FRM_SOC_WRAPPER_NOF_BYTES;

exit:
    SHR_FUNC_EXIT;
}

int
adapter_memreg_access(
    int unit,
    int cmic_block,
    uint32 addr,
    uint32 data_length,
    int is_mem,
    int is_get,
    uint32 *data)
{
    uint32 nw_order_int;
    uint8 opcode;
    uint32 host_order_int;
    char *data_char_ptr;
    int data_len = 0;
    uint32 rounded_data_length = 0;
    uint8 padding_length = 0;
    
    
    int ii = 0;
    uint8 do_collect;
    int adapter_mem_reg_fd;
    
    int sub_unit_id = adapter_sub_unit_id;
    uint8 is_created;
    SHR_FUNC_INIT_VARS(unit);

    
    
    adapter_context_db.params.adapter_mem_reg_mutex.is_created(unit, sub_unit_id, &is_created);
    if (is_created == FALSE)
    {
        LOG_DEBUG(BSL_LS_SOC_SCHAN, (BSL_META("mutex for sub_unit_id=%d was not created, exit!\n"), is_created));
        SHR_EXIT();
    }
    if (!is_get)
    {
        LOG_INFO(BSL_LS_SOC_SCHAN,
                 (BSL_META("adapter_memreg_access: unit=%d cmic_block=%d addr=0x%x data_length=%d is_mem=%d data=0x"),
                  unit, cmic_block, addr, data_length, is_mem));
        while (ii < ((data_length - 1) / 4 + 1))
        {
            LOG_INFO(BSL_LS_SOC_SCHAN, (BSL_META("%X "), data[ii]));
            ii++;
        }
        LOG_INFO(BSL_LS_SOC_SCHAN, (BSL_META("\n")));
    }

    adapter_context_db.params.adapter_mem_reg_mutex.take(unit, sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_mem_reg_fd.get(unit, sub_unit_id, &adapter_mem_reg_fd);

    
    rounded_data_length = (data_length + 3) & ~0x3;

    
    if (is_get)
    {
        if (is_mem)
        {
            opcode = UPDATE_FRM_SOC_READ_TBL_REQ;
        }
        else
        {
            opcode = UPDATE_FRM_SOC_READ_REG_REQ;
        }
    }
    else
    {
        
        data_len = data_length;
        padding_length = rounded_data_length - data_length;
        

        if (is_mem)
        {
            opcode = UPDATE_FRM_SOC_WRITE_TBL;
        }
        else
        {
            opcode = UPDATE_FRM_SOC_WRITE_REG;
        }
    }

    
    *adapter_info[unit][sub_unit_id]->p_access_buffer_current_position = opcode;
    adapter_info[unit][sub_unit_id]->p_access_buffer_current_position += sizeof(opcode);

    
    host_order_int = 4 + 4 + data_len + padding_length;

    
    if (is_get && !is_mem)
    {
        
        host_order_int++;
    }

    nw_order_int = htonl(host_order_int);
    sal_memcpy(adapter_info[unit][sub_unit_id]->p_access_buffer_current_position, &nw_order_int, sizeof(nw_order_int));
    adapter_info[unit][sub_unit_id]->p_access_buffer_current_position += sizeof(nw_order_int);

    
    host_order_int = cmic_block;
    nw_order_int = htonl(host_order_int);
    sal_memcpy(adapter_info[unit][sub_unit_id]->p_access_buffer_current_position, &nw_order_int, sizeof(nw_order_int));
    adapter_info[unit][sub_unit_id]->p_access_buffer_current_position += sizeof(nw_order_int);

    
    host_order_int = addr;
    nw_order_int = htonl(host_order_int);
    sal_memcpy(adapter_info[unit][sub_unit_id]->p_access_buffer_current_position, &nw_order_int, sizeof(nw_order_int));
    adapter_info[unit][sub_unit_id]->p_access_buffer_current_position += sizeof(nw_order_int);

    
    
    if (is_get && !is_mem)
    {
        if (rounded_data_length >= utilex_power_of_2(SAL_UINT8_NOF_BITS))
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U
                       (unit, "Error: rounded_data_length=%d. above uint8 max size, (cmic_block=%d, addr=%d)\n"),
                       rounded_data_length, cmic_block, addr));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }
        *adapter_info[unit][sub_unit_id]->p_access_buffer_current_position = (uint8) rounded_data_length;
        adapter_info[unit][sub_unit_id]->p_access_buffer_current_position += sizeof(uint8);
    }

    
    if (!is_get)
    {
        
        data_char_ptr = (char *) data;

        
        for (ii = 0; ii < rounded_data_length; ii++)
        {
            *(adapter_info[unit][sub_unit_id]->p_access_buffer_current_position + ii) =
                data_char_ptr[(rounded_data_length - 1) - ii];
        }
        adapter_info[unit][sub_unit_id]->p_access_buffer_current_position += rounded_data_length;
    }

    adapter_collect_accesses(unit, sub_unit_id, is_get, is_mem, &do_collect);

    if (do_collect == 0)
    {
        SHR_IF_ERR_EXIT(adapter_access_transaction_wrapper_set(unit, sub_unit_id, adapter_mem_reg_fd));

        
        if (is_get)
        {
            SHR_IF_ERR_EXIT(adapter_memreg_read_response(unit, sub_unit_id, is_mem, rounded_data_length, data));
        }

    }

exit:
    if (is_created == TRUE)
    {
        adapter_context_db.params.adapter_mem_reg_mutex.give(unit, sub_unit_id);
    }
    SHR_FUNC_EXIT;
}


static shr_error_e
adapter_mem_validity_check(
    int unit,
    soc_mem_t mem,
    unsigned int array_index,
    int copyno,
    int index)
{
    int entry_num_max;

    SHR_FUNC_INIT_VARS(unit);

    if (!soc_mem_is_valid(unit, mem))
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory is not valid.\n");
    }

    if (copyno != MEM_BLOCK_ALL && copyno != SOC_CORE_ALL && !SOC_MEM_BLOCK_VALID(unit, mem, copyno))
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory block is not valid.\n");
    }

    entry_num_max = soc_mem_index_max(unit, mem);

#if !defined(SOC_NO_NAMES)

    if (index < 0 || index > entry_num_max)
    {
        
#if 1
        char *mem_name;
        char mem_prefix[12];
        mem_name = soc_mem_name[mem];

        if (sal_strlen(mem_name) >= 11)
        {
            sal_strncpy(mem_prefix, mem_name, 11);
            mem_prefix[11] = 0;
        }

        if (sal_strcmp(mem_prefix, "OAMP_MEP_DB"))
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory's index is invalid.\n");
        }
#else
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory's index is invalid.\n");
#endif
    }

#endif

    if (SOC_MEM_IS_ARRAY(unit, mem))
    {
        soc_mem_array_info_t *maip = SOC_MEM_ARRAY_INFOP(unit, mem);
        if (maip && (array_index < maip->first_array_index || array_index >= maip->first_array_index + maip->numels))
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory's array index is invalid.\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
adapter_reg_validity_check(
    int unit,
    soc_reg_t reg,
    int index)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_REG_IS_VALID(unit, reg))
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Register is not invalid.\n");
    }

    if (SOC_REG_IS_ARRAY(unit, reg)
        && (index < SOC_REG_INFO(unit, reg).first_array_index
            || index >= SOC_REG_NUMELS(unit, reg) + SOC_REG_INFO(unit, reg).first_array_index))
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Register's array index is invalid.\n");
    }
exit:
    SHR_FUNC_EXIT;
}


static int
adapter_reg_access_handle(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    int is_get,
    soc_reg_above_64_val_t data)
{
    int reg_size;
    int rv;
    soc_reg_access_info_t access_info;
    int block;
    int cmic_block;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(adapter_reg_validity_check(unit, reg, index));

    if ((rv =
         soc_reg_xaddr_get(unit, reg, port, index, is_get ? SOC_REG_ADDR_OPTION_NONE : SOC_REG_ADDR_OPTION_WRITE,
                           &access_info)) != SOC_E_NONE)
    {
        return rv;
    }

    reg_size = soc_reg_bytes(unit, reg);

    for (block = 0; block < access_info.num_blks && rv == SOC_E_NONE; ++block)
    {
        cmic_block = access_info.blk_list[block];
        if (reg == DMU_CRU_RESETr)
        {
            cmic_block = 7;
        }
        rv = adapter_memreg_access(unit, cmic_block, access_info.offset, reg_size, 0, is_get, (uint32 *) data);
        if (is_get)
        {
            break;
        }
    }

    return rv;

exit:
    SHR_FUNC_EXIT;
}


static int
adapter_mem_access_handle(
    int unit,
    soc_mem_t mem,
    unsigned int array_index,
    int copyno,
    int index,
    int is_read,
    void *entry_data)
{
    uint32 data_byte_len, addr;
    int cmic_blk, blk;
    uint8 acc_type;
    int entry_dw;
    int rv = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(adapter_mem_validity_check(unit, mem, array_index, copyno, index));

    entry_dw = soc_mem_entry_words(unit, mem);
    data_byte_len = entry_dw * sizeof(uint32);
    
    if (copyno == SOC_CORE_ALL || SOC_BLOCK_IS_BROADCAST(unit, copyno))
    {
        copyno = COPYNO_ALL;
    }

    SOC_MEM_BLOCK_ITER(unit, mem, blk)
    {
        if (copyno != COPYNO_ALL && copyno != blk)
        {
            continue;
        }

        
        cmic_blk = SOC_BLOCK2SCH(unit, blk);

        addr = soc_mem_addr_get(unit, mem, array_index, blk, index, &acc_type);

        rv = adapter_memreg_access(unit, cmic_blk, addr, data_byte_len, 1, is_read, (uint32 *) entry_data);
        SHR_IF_ERR_EXIT(rv);

        
        if (is_read)
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}


int
adapter_reg32_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 *data)
{
    soc_reg_above_64_val_t data_a64;
    int rv;
    
    sal_memset(data_a64, 0, sizeof(data_a64));
    rv = adapter_reg_access_handle(unit, reg, port, index, 1, data_a64);
    if (rv < 0)
    {
        return rv;
    }

    data[0] = data_a64[0];

    return _SHR_E_NONE;
}


int
adapter_reg64_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint64 *data)
{
    soc_reg_above_64_val_t data_a64;
    int rv;

    sal_memset(data_a64, 0, sizeof(data_a64));
    rv = adapter_reg_access_handle(unit, reg, port, index, 1, data_a64);
    if (rv < 0)
    {
        return rv;
    }

    COMPILER_64_SET(*data, data_a64[1], data_a64[0]);

    return _SHR_E_NONE;
}


int
adapter_reg_above64_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    soc_reg_above_64_val_t data)
{
    int rv;
    rv = adapter_reg_access_handle(unit, reg, port, index, 1, data);
    return rv;
}


int
adapter_reg32_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 data)
{
    int rv;
    soc_reg_above_64_val_t data_a64;

    
    if (dnxc_ha_is_access_disabled(unit, UTILEX_SEQ_ALLOW_SCHAN))
    {
        LOG_ERROR(BSL_LS_SOC_SCHAN,
                  (BSL_META_U(unit, "adapter_reg32_set: ERROR, It is not currently allowed to modify HW\n")));
        return SOC_E_FAIL;
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION
    
    
    if (!DNX_ERR_RECOVERY_HW_ACCESS_IS_VALID(unit))
    {
        LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit, "adapter_reg32_set: Invalid error recovery hw access\n")));
        return SOC_E_FAIL;
    }
#endif

    SOC_REG_ABOVE_64_CLEAR(data_a64);
    data_a64[0] = data;

    rv = adapter_reg_access_handle(unit, reg, port, index, 0, data_a64);
    return rv;
}


int
adapter_reg64_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint64 data)
{
    int rv;
    soc_reg_above_64_val_t data_a64;

    
    if (dnxc_ha_is_access_disabled(unit, UTILEX_SEQ_ALLOW_SCHAN))
    {
        LOG_ERROR(BSL_LS_SOC_SCHAN,
                  (BSL_META_U(unit, "adapter_reg64_set: ERROR, It is not currently allowed to modify HW\n")));
        return SOC_E_FAIL;
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION
    
    
    if (!DNX_ERR_RECOVERY_HW_ACCESS_IS_VALID(unit))
    {
        LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit, "adapter_reg64_set: Invalid error recovery hw access\n")));
        return SOC_E_FAIL;
    }
#endif

    SOC_REG_ABOVE_64_CLEAR(data_a64);
    data_a64[1] = COMPILER_64_HI(data);
    data_a64[0] = COMPILER_64_LO(data);

    rv = adapter_reg_access_handle(unit, reg, port, index, 0, data_a64);
    return rv;
}


int
adapter_reg_above64_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    soc_reg_above_64_val_t data)
{
    int rv;

    
    if (dnxc_ha_is_access_disabled(unit, UTILEX_SEQ_ALLOW_SCHAN))
    {
        LOG_ERROR(BSL_LS_SOC_SCHAN,
                  (BSL_META_U(unit, "adapter_reg_above64_set: ERROR, It is not currently allowed to modify HW\n")));
        return SOC_E_FAIL;
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION
    
    
    if (!DNX_ERR_RECOVERY_HW_ACCESS_IS_VALID(unit))
    {
        LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit, "adapter_reg_above64_set: Invalid error recovery hw access\n")));
        return SOC_E_FAIL;
    }
#endif

    rv = adapter_reg_access_handle(unit, reg, port, index, 0, data);
    return rv;
}


int
adapter_mem_array_read(
    int unit,
    soc_mem_t mem,
    unsigned int array_index,
    int copyno,
    int index,
    void *entry_data)
{
    int rv;
    rv = adapter_mem_access_handle(unit, mem, array_index, copyno, index, 1, entry_data);
    return rv;
}


int
adapter_mem_array_write(
    int unit,
    soc_mem_t mem,
    unsigned int array_index,
    int copyno,
    int index,
    void *entry_data)
{
    int rv;

    
    if (dnxc_ha_is_access_disabled(unit, UTILEX_SEQ_ALLOW_SCHAN))
    {
        LOG_ERROR(BSL_LS_SOC_SCHAN,
                  (BSL_META_U(unit, "adapter_mem_array_write: ERROR, It is not currently allowed to modify HW\n")));
        return SOC_E_FAIL;
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION
    
    
    if (!DNX_ERR_RECOVERY_HW_ACCESS_IS_VALID(unit))
    {
        LOG_ERROR(BSL_LS_SOC_SCHAN, (BSL_META_U(unit, "adapter_mem_array_write: Invalid error recovery hw access\n")));
        return SOC_E_FAIL;
    }
#endif

    rv = adapter_mem_access_handle(unit, mem, array_index, copyno, index, 0, entry_data);
    return rv;
}


int
adapter_sockets_init(
    int unit,
    int sub_unit_id,
    socket_target_e socket_target)
{
    
    
    struct hostent *hostent_ptr = NULL;
    char *s = NULL;
    char *default_udp_port = "6817";
    char *adapter_host = NULL;
    char tmp[80];
    int adapter_host_port;
    int optval = 1;
    int soc_fd;
    struct sockaddr_in srv_addr;
    int socket_type = SOCK_STREAM;
    int u_index;
    SHR_FUNC_INIT_VARS(unit);

    
    
    u_index = (dnx_data_device.general.nof_sub_units_get(unit) > 1) ? sub_unit_id : unit;
    
    switch (socket_target)
    {
        case ADAPTER_RX_TX:
            adapter_context_db.params.adapter_rx_tx_mutex.create(unit, sub_unit_id);
            sal_snprintf(tmp, sizeof(tmp), "CMODEL_PACKET_PORT_%d", u_index);
            s = getenv(tmp);
            break;
        case ADAPTER_REGS:
            adapter_context_db.params.adapter_mem_reg_mutex.create(unit, sub_unit_id);
            sal_snprintf(tmp, sizeof(tmp), "CMODEL_MEMORY_PORT_%d", u_index);
            s = getenv(tmp);
            break;
        case ADAPTER_SIGNALS:
            adapter_context_db.params.adapter_sdk_interface_mutex.create(unit, sub_unit_id);
            sal_snprintf(tmp, sizeof(tmp), "CMODEL_SDK_INTERFACE_PORT_%d", u_index);
            s = getenv(tmp);
            break;
        case ADAPTER_EVENTS:
            adapter_context_db.params.adapter_external_events_mutex.create(unit, sub_unit_id);
            sal_snprintf(tmp, sizeof(tmp), "CMODEL_EXTERNAL_EVENTS_PORT_%d", u_index);
            s = getenv(tmp);
            if (!s)
            {
                
                s = default_udp_port;
            }
            socket_type = SOCK_DGRAM;
            break;
        default:
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_sockets_init failed\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
    }

    if (!s)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INIT);
        SHR_EXIT();
    }

    

    adapter_host = getenv("SOC_TARGET_SERVER");

    
    
    if (!adapter_host)
    {
        snprintf(tmp, sizeof(tmp), "SOC_TARGET_SERVER%d", 0  );
        adapter_host = getenv(tmp);
    }

    
    
    adapter_host_port = atoi(s);

    memset((void *) &srv_addr, 0, sizeof(srv_addr));

    hostent_ptr = gethostbyname(adapter_host);
    if (hostent_ptr == NULL)
    {
        
        SHR_SET_CURRENT_ERR(_SHR_E_INIT);
        SHR_EXIT();
    }
    memcpy(&srv_addr.sin_addr, hostent_ptr->h_addr, 4);

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(adapter_host_port);

    soc_fd = socket(AF_INET, socket_type, 0);
    if (soc_fd < 0)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INIT);
        SHR_EXIT();
    }

    switch (socket_target)
    {
        case ADAPTER_RX_TX:
            adapter_context_db.params.adapter_rx_tx_fd.set(unit, sub_unit_id, soc_fd);
            break;
        case ADAPTER_REGS:
            adapter_context_db.params.adapter_mem_reg_fd.set(unit, sub_unit_id, soc_fd);
            break;
        case ADAPTER_SIGNALS:
            adapter_context_db.params.adapter_sdk_interface_fd.set(unit, sub_unit_id, soc_fd);
            break;
        case ADAPTER_EVENTS:
            adapter_context_db.params.adapter_external_events_fd.set(unit, sub_unit_id, soc_fd);
            adapter_context_db.params.adapter_server_address.set(unit, sub_unit_id, srv_addr.sin_addr.s_addr);
            adapter_context_db.params.adapter_external_events_port.set(unit, sub_unit_id, srv_addr.sin_port);
            break;
        default:
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_sockets_init failed\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
    }

    
    if (socket_type == SOCK_STREAM)
    {
        
        if (setsockopt(soc_fd, IPPROTO_TCP, TCP_NODELAY, (char *) &optval, sizeof(optval)) < 0)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_INIT);
            SHR_EXIT();
        }

        if (connect(soc_fd, (struct sockaddr *) &srv_addr, sizeof(srv_addr)) < 0)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_INIT,
                                     "Failed connecting the Memory (1), Tx (0) or Signals (2) (%d) socket. Port (%d) adapter IP %s\r\n",
                                     socket_target, adapter_host_port, adapter_host);
        }
    }

exit:
    SHR_FUNC_EXIT;
}


soc_error_t
adapter_reg_access_init(
    int unit,
    int sub_unit_id)
{
    int adapter_mem_reg_fd;
    SHR_FUNC_INIT_VARS(unit);

    adapter_context_db.params.adapter_mem_reg_fd.get(unit, sub_unit_id, &adapter_mem_reg_fd);

    if (!adapter_info[unit][sub_unit_id])
    {

        
        adapter_info[unit][sub_unit_id] = sal_alloc(sizeof(adapter_access_info_t), "adapter access_info");

        if (!adapter_info[unit][sub_unit_id])
        {
            return _SHR_E_MEMORY;
        }

        sal_memset(adapter_info[unit][sub_unit_id], 0, sizeof(adapter_access_info_t));

        SHR_IF_ERR_EXIT(adapter_sockets_init(unit, sub_unit_id, ADAPTER_RX_TX));
        SHR_IF_ERR_EXIT(adapter_sockets_init(unit, sub_unit_id, ADAPTER_REGS));
        adapter_sockets_init(unit, sub_unit_id, ADAPTER_SIGNALS);
        SHR_IF_ERR_EXIT(adapter_sockets_init(unit, sub_unit_id, ADAPTER_EVENTS));
    }

    if (NULL == adapter_info[unit][sub_unit_id]->p_access_buffer)
    {
        adapter_info[unit][sub_unit_id]->p_access_buffer =
            sal_alloc(sizeof(char) * MAX_ACCESS_BUFFER_SIZE, "adapter access buffer");
    }
    
    adapter_info[unit][sub_unit_id]->p_access_buffer_current_position =
        adapter_info[unit][sub_unit_id]->p_access_buffer + 5;

    
    adapter_info[unit][sub_unit_id]->collect_adapter_accesses =
        dnx_data_adapter.reg_mem_access.do_collect_enable_get(unit);
    
    SHR_IF_ERR_EXIT(soc_reg_access_func_register(unit, &adapter_access));
    
    if (!sw_state_is_warm_boot(unit))
    {
        adapter_reg32_set(unit, DMU_CRU_RESETr, 200, 0, 1);
    }

    SHR_IF_ERR_EXIT(adapter_init_rx_tx_socket(unit, sub_unit_id));

exit:
    SHR_FUNC_EXIT;
}


int
adapter_reg_access_deinit(
    int unit,
    int sub_unit_id)
{
    int adapter_rx_tx_fd;
    int adapter_mem_reg_fd;
    int adapter_sdk_interface_fd;
    int adapter_external_events_fd;

    if (!adapter_info[unit][sub_unit_id])
    {
        return _SHR_E_INIT;
    }

    adapter_context_db.params.adapter_rx_tx_fd.get(unit, sub_unit_id, &adapter_rx_tx_fd);
    adapter_context_db.params.adapter_mem_reg_fd.get(unit, sub_unit_id, &adapter_mem_reg_fd);
    adapter_context_db.params.adapter_sdk_interface_fd.get(unit, sub_unit_id, &adapter_sdk_interface_fd);
    adapter_context_db.params.adapter_external_events_fd.get(unit, sub_unit_id, &adapter_external_events_fd);

    
    if (adapter_rx_tx_fd >= 0)
    {
        close(adapter_rx_tx_fd);
        adapter_context_db.params.adapter_rx_tx_fd.set(unit, sub_unit_id, -1);
    }

    if (adapter_mem_reg_fd >= 0)
    {
        close(adapter_mem_reg_fd);
        adapter_context_db.params.adapter_mem_reg_fd.set(unit, sub_unit_id, -1);
    }

    
    if (adapter_sdk_interface_fd >= 0)
    {
        close(adapter_sdk_interface_fd);
        adapter_context_db.params.adapter_sdk_interface_fd.set(unit, sub_unit_id, -1);
    }

    
    if (adapter_external_events_fd >= 0)
    {
        close(adapter_external_events_fd);
        adapter_context_db.params.adapter_external_events_fd.set(unit, sub_unit_id, -1);
    }

    sal_free(adapter_info[unit][sub_unit_id]->p_access_buffer);
    sal_free(adapter_info[unit][sub_unit_id]);
    adapter_info[unit][sub_unit_id] = NULL;
    
    return _SHR_E_NONE;
}


uint32
adapter_read_buffer(
    int unit,
    int sub_unit_id,
    adapter_ms_id_e * ms_id,
    uint32 *nof_signals,
    int *ifc_type,
    uint32 *port_channel,
    uint32 *port_first_lane,
    int *len,
    unsigned char *buf)
{
    fd_set read_vect;
    char swapped_header[MAX_PACKET_HEADER_SIZE_ADAPTER];
    uint32 packet_length;
    long long_val;
    int offset = 0;
    int nfds = 0;
    int rv = _SHR_E_NONE;
    
    int adapter_rx_tx_fd;
    int constant_header_size;
    int adapter_ver;
    struct timeval tout;
    SHR_FUNC_INIT_VARS(unit);
    constant_header_size = dnx_data_adapter.rx.constant_header_size_get(unit);
    adapter_ver = dnx_data_adapter.general.lib_ver_get(unit);
    adapter_context_db.params.adapter_rx_tx_fd.get(unit, sub_unit_id, &adapter_rx_tx_fd);
    assert(adapter_rx_tx_fd);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META("adapter_read_buffer: sockfd=%d\n"), adapter_rx_tx_fd));

    
    FD_ZERO(&read_vect);

    
    FD_SET(adapter_rx_tx_fd, &read_vect);

    
    FD_SET(pipe_fds[0], &read_vect);

    
    nfds = (adapter_rx_tx_fd > pipe_fds[0]) ? adapter_rx_tx_fd + 1 : pipe_fds[0] + 1;
    
    tout.tv_sec = 0;
    tout.tv_usec = 10;
    rv = select(nfds, &read_vect, (fd_set *) 0x0, (fd_set *) 0x0, &tout);
    
    if (rv == 0)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_TIMEOUT);
    }
    
    if ((rv < 0) && (errno != EINTR))
    {
        perror("get_command: select error");
        SHR_ERR_EXIT(_SHR_E_FAIL, "select function error");
    }

    
    if (FD_ISSET(pipe_fds[0], &read_vect))
    {
        return RX_THREAD_NOTIFY_CLOSED;
    }
    
    if (FD_ISSET(adapter_rx_tx_fd, &read_vect))
    {
        
        if (readn(adapter_rx_tx_fd, &(swapped_header[0]), constant_header_size) < constant_header_size)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Adapter server disconnected\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_DISABLED);
            SHR_EXIT();
        }
        long_val = ntohl(*(uint32 *) &(swapped_header[offset]));
        packet_length = long_val;
        offset += sizeof(uint32);

        
        if (adapter_ver >= 4)
        {
            long_val = ntohl(*(uint32 *) &(swapped_header[offset]));
            
            offset += sizeof(uint32);
        }
        long_val = ntohl(*(uint32 *) &(swapped_header[offset]));
        *ms_id = long_val;
        offset += sizeof(uint32);

        
        if (adapter_ver >= 5)
        {
            long_val = ntohl(*(uint32 *) &(swapped_header[offset]));
            *ifc_type = (int) long_val;
            offset += sizeof(uint32);
        }
        else
        {
            *ifc_type = -1;
        }

        
        if (adapter_ver >= 2)
        {
            long_val = ntohl(*(uint32 *) &(swapped_header[offset]));
            *port_first_lane = long_val;
            offset += sizeof(uint32);
        }

        
        if (adapter_ver >= 3)
        {
            long_val = ntohl(*(uint32 *) &(swapped_header[offset]));
            *port_channel = long_val;
            offset += sizeof(uint32);
        }

        long_val = ntohl(*(uint32 *) &(swapped_header[offset]));
        *nof_signals = long_val;
        offset += sizeof(uint32);

        LOG_INFO(BSL_LS_SYS_VERINET,
                 (BSL_META("adapter_read_buffer: packet_length=%d ms_id=%d nof_signals=%d\n"), packet_length, *ms_id,
                  *nof_signals));
        
        *len = (packet_length + 4) - constant_header_size;
        
        if (readn(adapter_rx_tx_fd, buf, *len) < *len)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "adapter_read_buffer: could not read packet\n")));
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
            SHR_EXIT();
        }
    }
exit:
    SHR_FUNC_EXIT;
}


int
adapter_write_buffer_to_socket(
    int unit,
    adapter_ms_id_e ms_id,
    socket_target_e target,
    int len,
    int num_of_signals,
    uint8 *buf)
{
    int rv = 0;
    uint32 nw_order_int;
    uint32 index_position = 0;
    int adapter_fd;
    uint8 message_data[MAX_PACKET_SIZE_ADAPTER];
    struct sockaddr_in srv_addr;
    socklen_t serverlen;
    int adapter_ver;
    uint32 server_ip, server_port;

    LOG_INFO(BSL_LS_SYS_VERINET, (BSL_META("adapter_write_buffer_to_socket: len=0x%x\n"), len));

    adapter_ver = dnx_data_adapter.general.lib_ver_get(unit);

    
    index_position += sizeof(int);

    
    switch (target)
    {
        case ADAPTER_RX_TX:
            adapter_context_db.params.adapter_rx_tx_mutex.take(unit, adapter_sub_unit_id, sal_mutex_FOREVER);
            adapter_context_db.params.adapter_rx_tx_fd.get(unit, adapter_sub_unit_id, &adapter_fd);
            
            
            if (adapter_ver >= 4)
            {
                int device_id = -1;
                nw_order_int = htonl(device_id);
                sal_memcpy(&(message_data[index_position]), &nw_order_int, sizeof(int));
                index_position += sizeof(int);
            }
            break;
        case ADAPTER_EVENTS:
            adapter_context_db.params.adapter_external_events_mutex.take(unit, adapter_sub_unit_id, sal_mutex_FOREVER);
            adapter_context_db.params.adapter_external_events_fd.get(unit, adapter_sub_unit_id, &adapter_fd);
            break;
        default:
            break;
    }

    
    nw_order_int = htonl(ms_id);
    sal_memcpy(&(message_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    if (target == ADAPTER_RX_TX)
    {
        
        nw_order_int = htonl(0);
        sal_memcpy(&(message_data[index_position]), &nw_order_int, sizeof(int));
        index_position += sizeof(int);

        
        nw_order_int = htonl(0);
        sal_memcpy(&(message_data[index_position]), &nw_order_int, sizeof(int));
        index_position += sizeof(int);

        
        if (adapter_ver >= 3)
        {
            nw_order_int = htonl(0);
            sal_memcpy(&(message_data[index_position]), &nw_order_int, sizeof(int));
            index_position += sizeof(int);
        }
        
        nw_order_int = htonl(num_of_signals);
        sal_memcpy(&(message_data[index_position]), &nw_order_int, sizeof(int));
        index_position += sizeof(int);
    }

    
    sal_memcpy(&(message_data[index_position]), buf, len);
    index_position += len;

    
    nw_order_int = htonl(index_position - sizeof(uint32));
    sal_memcpy(message_data, &nw_order_int, sizeof(int));

    
    switch (target)
    {
        case ADAPTER_RX_TX:
            rv = (writen(adapter_fd, message_data, index_position) != index_position);
            adapter_context_db.params.adapter_rx_tx_mutex.give(unit, adapter_sub_unit_id);
            break;
        case ADAPTER_EVENTS:
            
            serverlen = sizeof(srv_addr);
            memset((void *) &srv_addr, 0, serverlen);
            srv_addr.sin_family = AF_INET;

            adapter_context_db.params.adapter_server_address.get(unit, adapter_sub_unit_id, &server_ip);
            adapter_context_db.params.adapter_external_events_port.get(unit, adapter_sub_unit_id, &server_port);
            srv_addr.sin_addr.s_addr = server_ip;
            srv_addr.sin_port = server_port;

            rv = (sendto(adapter_fd, message_data, index_position, 0, (struct sockaddr *) &srv_addr, serverlen) !=
                  index_position);

            adapter_context_db.params.adapter_external_events_mutex.give(unit, adapter_sub_unit_id);
            break;
        default:
            break;
    }

    if (rv)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_write_buffer_to_socket failed\n")));
    }

    
    sal_usleep(5000000);

    return 0;
}


void
adapter_send_buffer_add_signal(
    uint32 signal_id,
    uint32 signal_width,
    char *signal_value,
    uint32 *index_position,
    char *packet_data)
{
    uint32 nw_order_int;

    nw_order_int = htonl(signal_id);
    sal_memcpy(&(packet_data[*index_position]), &nw_order_int, sizeof(int));
    *index_position += sizeof(int);

    nw_order_int = htonl(signal_width);
    sal_memcpy(&(packet_data[*index_position]), &nw_order_int, sizeof(int));
    *index_position += sizeof(int);

    sal_memcpy(&(packet_data[*index_position]), signal_value, signal_width);
    *index_position += signal_width;
}


uint32
adapter_send_buffer(
    int unit,
    adapter_ms_id_e ms_id,
    uint32 src_port,
    uint32 src_channel,
    int len,
    unsigned char *buf,
    int nof_signals)
{
    uint32 nw_order_int;
    uint32 index_position = 0;
    uint32 nof_signals_index_position;
    uint32 packet_size_index_postion;
    uint32 buf_len_in_bits = len * 8;
    int index;
    char packet_data[MAX_PACKET_SIZE_ADAPTER];
    char signal_data[MAX_SIGNAL_SIZE];
    int adapter_rx_tx_fd;
    int adapter_ver;
    uint32 port_info;
    int nif_or_cpu_port;
    int core;
    bcm_port_config_t port_config;

    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO(BSL_LS_SYS_VERINET, (BSL_META("adapter_send_buffer: src_port=%d len=0x%x\n"), src_port, len));

    adapter_ver = dnx_data_adapter.general.lib_ver_get(unit);
    adapter_context_db.params.adapter_rx_tx_mutex.take(unit, adapter_sub_unit_id, sal_mutex_FOREVER);
    adapter_context_db.params.adapter_rx_tx_fd.get(unit, adapter_sub_unit_id, &adapter_rx_tx_fd);

    
    packet_size_index_postion = index_position;
    index_position += sizeof(int);

    
    if (adapter_ver >= 4)
    {
        int device_id = -1;
        nw_order_int = htonl(device_id);
        sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
        index_position += sizeof(int);
    }

    
    nw_order_int = htonl(ms_id);
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

    if (BCM_PBMP_MEMBER(port_config.cpu, src_port))
    {
        
        port_info = 0;
        nif_or_cpu_port = src_port;
    }
    else
    {
        
        port_info = 10;

        if (dnx_algo_port_nif_interface_id_get(unit, src_port, 0, &core, &nif_or_cpu_port) != 0)
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Error: adapter_send_packet dnx_algo_port_nif_interface_id_get failed\n")));
            adapter_context_db.params.adapter_rx_tx_mutex.give(unit, adapter_sub_unit_id);
            return -1;
        }
    }

    nw_order_int = htonl(port_info);
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    
    nw_order_int = htonl(nif_or_cpu_port);
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    
    if (adapter_ver >= 3)
    {
        nw_order_int = htonl(src_channel);
        sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
        index_position += sizeof(int);
    }

    
    nof_signals_index_position = index_position;
    index_position += sizeof(int);

    
    sal_memset(signal_data, 0, TX_PACKET_TAG_SWAP_RES_SIGNAL_WIDTH);
    adapter_send_buffer_add_signal(TX_PACKET_TAG_SWAP_RES_SIGNAL_ID, TX_PACKET_TAG_SWAP_RES_SIGNAL_WIDTH, signal_data,
                                   &index_position, packet_data);
    nof_signals++;

    
    sal_memset(signal_data, 0, TX_PACKET_RECYCLE_COMMAND_SIGNAL_WIDTH);
    adapter_send_buffer_add_signal(TX_PACKET_RECYCLE_COMMAND_SIGNAL_ID, TX_PACKET_RECYCLE_COMMAND_SIGNAL_WIDTH,
                                   signal_data, &index_position, packet_data);
    nof_signals++;

    
    sal_memcpy(&(signal_data[0]), conversion_tbl[0], 8);
    adapter_send_buffer_add_signal(TX_PACKET_VALID_BYTES_SIGNAL_ID, TX_PACKET_VALID_BYTES_SIGNAL_WIDTH, signal_data,
                                   &index_position, packet_data);
    nof_signals++;

    
    nw_order_int = htonl(TX_PACKET_SIGNAL);
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    
    nw_order_int = htonl(buf_len_in_bits);
    sal_memcpy(&(packet_data[index_position]), &nw_order_int, sizeof(int));
    index_position += sizeof(int);

    
    
    for (index = 0; index < len; index++)
    {
        sal_memcpy(&(packet_data[index_position + index * 8]), conversion_tbl[(int) buf[index]], 8);
    }
    index_position += buf_len_in_bits;

    
    nw_order_int = htonl(nof_signals); 
    sal_memcpy(&(packet_data[nof_signals_index_position]), &nw_order_int, sizeof(int));

    
    
    
    nw_order_int = htonl(index_position - sizeof(uint32));
    sal_memcpy(&(packet_data[packet_size_index_postion]), &nw_order_int, sizeof(int));

    
    if (writen(adapter_rx_tx_fd, packet_data, index_position) != index_position)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Error: adapter_send_packet data failed\n")));
        adapter_context_db.params.adapter_rx_tx_mutex.give(unit, adapter_sub_unit_id);
        return -1;
    }

    adapter_context_db.params.adapter_rx_tx_mutex.give(unit, adapter_sub_unit_id);

exit:
    SHR_FUNC_EXIT;
}


uint32
adapter_terminate_server(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(adapter_send_buffer(unit, ADAPTER_MS_ID_TERMINATE_SERVER, 0, 0, 0, NULL, 0));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
adapter_access_transaction_trigger(
    int unit,
    int sub_unit_id)
{
    int mutex_taken = 0;
    int adapter_mem_reg_fd;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(adapter_info[unit][sub_unit_id]->p_access_buffer, _SHR_E_INIT, "p_access_buffer");
    SHR_NULL_CHECK(adapter_info[unit][sub_unit_id]->p_access_buffer_current_position, _SHR_E_INIT,
                   "p_access_buffer_current_position");

    
    
    if (adapter_info[unit][sub_unit_id]->p_access_buffer + FRM_SOC_WRAPPER_NOF_BYTES <
        adapter_info[unit][sub_unit_id]->p_access_buffer_current_position)
    {
        adapter_context_db.params.adapter_mem_reg_fd.get(unit, sub_unit_id, &adapter_mem_reg_fd);
        adapter_context_db.params.adapter_mem_reg_mutex.take(unit, sub_unit_id, sal_mutex_FOREVER);
        mutex_taken = TRUE;
        SHR_IF_ERR_EXIT(adapter_access_transaction_wrapper_set(unit, sub_unit_id, adapter_mem_reg_fd));
        adapter_context_db.params.adapter_mem_reg_mutex.give(unit, sub_unit_id);
        mutex_taken = FALSE;
    }

exit:
    if (mutex_taken == TRUE)
    {
        adapter_context_db.params.adapter_mem_reg_mutex.give(unit, sub_unit_id);
    }
    SHR_FUNC_EXIT;
}


int
adapter_dnx_init_done(
    int unit)
{
    int sub_unit_id;
    SHR_FUNC_INIT_VARS(unit);

    for (sub_unit_id = 0; sub_unit_id < dnx_data_device.general.nof_sub_units_get(unit); sub_unit_id++)
    {
        adapter_disable_collect_accesses(unit, sub_unit_id);
        SHR_IF_ERR_EXIT(adapter_access_transaction_trigger(unit, sub_unit_id));
    }
exit:
    SHR_FUNC_EXIT;
}



#endif
