#ifndef __ONESYNC_COMMON_H__
#define __ONESYNC_COMMON_H__

#include <include/kcom.h>

int bcm_common_onesync_hostlog_level_set(int unit, uint32 log_level);
int bcm_common_onesync_hostlog_level_get(int unit, uint32 *log_level);
int bcm_common_onesync_hostlog_start(int unit);
int bcm_common_onesync_hostlog_stop(int unit);

int bcm_common_onesync_init(int unit, int uc_num);
int bcm_common_onesync_deinit(int unit);

uint16 bcm_common_onesync_uint16_read( uint8* buffer);
uint32 bcm_common_onesync_uint32_read( uint8* buffer);
uint64 bcm_common_onesync_uint64_read(uint8* buffer);
void bcm_common_onesync_uint16_write( uint8* buffer, const uint16 value);
void bcm_common_onesync_uint32_write( uint8* buffer, const uint32 value);
void bcm_common_onesync_uint64_write( uint8* buffer, const uint64 value);
void bcm_common_onesync_int64_write( uint8* buffer, const int64 value);
int64 bcm_common_onesync_int64_read(uint8* buffer);

int
bcm_common_onesync_msg_send_receive(int unit, uint8 s_subclass,
                               uint16 s_len, uint32 s_data,
                               uint8 r_subclass, uint16 *r_len, uint32 *r_data,
                               sal_usecs_t timeout, uint8 *dma_buffer,
                               int dma_buffer_len);


/* DNX system information passed from Host to Ukernel */

/* DNX UDH DATA TYPE MAX */
#define ONESYNC_DNX_JER2_UDH_DATA_TYPE_MAX     (4)

/* PPH LIF Ext. 3 bit type */
#define ONESYNC_DNX_JER2_PPH_LIF_EXT_TYPE_MAX  (8)


typedef struct  onesync_dnx_jr2_devices_system_info_s {
    /* dnx JR2 system header info */
    uint32     ftmh_lb_key_ext_size;
    uint32     ftmh_stacking_ext_size;
    uint32     pph_base_size;
    uint32     pph_lif_ext_size[ONESYNC_DNX_JER2_PPH_LIF_EXT_TYPE_MAX];
    uint32     system_headers_mode;
    uint32     udh_enable;
    uint32     udh_data_lenght_per_type[ONESYNC_DNX_JER2_UDH_DATA_TYPE_MAX];

    /* CPU port information */
    uint32     cosq_port_cpu_channel;
    uint32     cosq_port_pp_port;
} onesync_dnx_jr2_devices_system_info_t;


/* ONESYNC Information SW Data Structure */
typedef struct onesync_info_s {
    int unit;                   /* Unit number */
    int initialized;            /* If set, ONESYNC has been initialized */
    int uc_num;                 /* uController number running ONESYNC appl */
    onesync_hostlog_info_t *hostlog_info;
    uint8 *hostlog_data;
    sal_thread_t hostlog_hread_id;
    int hostlog_thread_kill;
    uint32 sdk_version;
    uint32 appl_version;
    uint8  *uc_comm_payload_buffer;
    onesync_dnx_jr2_devices_system_info_t *jr2devs_system_info;

} onesync_info_t;

extern onesync_info_t *bcm_common_onesync_info[BCM_MAX_NUM_UNITS];

#define ONESYNC_INFO(u_)       (bcm_common_onesync_info[(u_)])

/* Timeout for Host <--> uC message */
#define ONESYNC_UC_MSG_TIMEOUT_USECS              5000000 /* 5 secs */

typedef struct kcom_msg_dnx_jr2devs_sys_config_s {
    kcom_msg_clock_cmd_t clock_cmd;
    onesync_dnx_jr2_devices_system_info_t jr2devs_sys_config;
} kcom_msg_dnx_jr2devs_sys_config_t;

int _bcm_dnx_onesync_device_config_read( int unit, int uc_num);
int _bcm_dnx_onesync_send_device_config_to_firmware( int unit, int uc_num);
int _bcm_dnx_onesync_send_device_config_to_ptp_clock_driver( int unit, int uc_num);
#endif
