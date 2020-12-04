/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    external_stack.c
 *
 * Purpose: 
 *
 * Functions:
 *      _bcm_ptp_external_stack_create
 *      _bcm_ptp_ext_stack_reset
 *      _bcm_ptp_ext_stack_start
 *      _bcm_ptp_write_pcishared_uint8_aligned_array
 *
 *      esw_set_ext_stack_config_uint32
 *      esw_set_ext_stack_config_array
 */

#if defined(INCLUDE_PTP)

#include <soc/defs.h>
#include <soc/drv.h>

#include <sal/appl/io.h>
#include <sal/core/dpc.h>

#include <bcm/ptp.h>
#include <bcm_int/common/ptp.h>
#include <bcm/error.h>

#if defined(BCM_PTP_EXTERNAL_STACK_SUPPORT)

#include <soc/uc_msg.h>

/* Constants */
#define FAULT_CHECK_US (100000)  /* every .1 seconds */

/* Event support (ToP OUT-OF-MEMORY).*/
#define TOP_OOM_MSGDATA_SIZE_OCTETS                 (8)
#define TOP_OOM_MINIMUM_FREE_MEMORY_THRESHOLD_BYTES (1024)
#define TOP_OOM_ORDBLK_FREE_MEMORY_THRESHOLD_BYTES  (1024)

/* For simplicity, a single FW-info structure.  Need per-unit/per-stack
   if multiple Keystone ToPs can be used simultaneously
*/
static _bcm_ptp_ext_fw_info_t top_fw_info;

extern int _bcm_ptp_write_ks_uint32(int idx, uint32 addr, uint32 val);
extern int _bcm_ptp_read_ks_uint32(int idx, uint32 addr, uint32 *value);

extern void _bcm_ptp_read_pcishared_uint8_aligned_array(
    int pci_idx,
    uint32 addr,
    uint8 * array,
    int array_len);

extern void _bcm_ptp_write_pcishared_uint8_aligned_array(
    int pci_idx,
    uint32 addr,
    uint8 * array,
    int array_len);

extern int _bcm_ptp_ext_stack_reset(int pci_idx);

/*
 * Function:
 *      _bcm_ptp_external_stack_create
 * Purpose:
 *      Create a PTP stack instance
 * Parameters:
 *      unit - (IN) Unit number.
 *      ptp_info - (IN/OUT) Pointer to an PTP Stack Info structure
 * Returns:
 *      BCM_E_...
 * Notes:
 */
int
_bcm_ptp_external_stack_create(
    int unit,
    bcm_ptp_stack_info_t *info,
    bcm_ptp_stack_id_t ptp_id)
{
    _bcm_ptp_info_t *ptp_info_p;
    _bcm_ptp_stack_info_t *stack_p;
    uint32 tpidvlan;

    SET_PTP_INFO;
    if (!SOC_HAS_PTP_EXTERNAL_STACK_SUPPORT(unit)) {
        return BCM_E_UNAVAIL;
    }

    stack_p = &ptp_info_p->stack_info[ptp_id];

    /* Set up dispatch for external transport */
    stack_p->transport_init = _bcm_ptp_external_transport_init;
    stack_p->tx = _bcm_ptp_external_tx;
    stack_p->tx_completion = _bcm_ptp_external_tx_completion;
    stack_p->rx_free = _bcm_ptp_external_rx_response_free;
    stack_p->transport_terminate = _bcm_ptp_external_transport_terminate;

    /* Assuming that the unit has been locked by the caller */
    sal_memcpy(&stack_p->ext_info, info->ext_stack_info, sizeof(bcm_ptp_external_stack_info_t));

    /* Set the PCI read and write functions */
    stack_p->ext_info.cookie = INT_TO_PTR(ptp_id);
    stack_p->ext_info.read_fn = &_bcm_ptp_read_pcishared_uint32;
    stack_p->ext_info.write_fn = &_bcm_ptp_write_pcishared_uint32;

    /* Config for Host <-> BCM53903 comms */
    esw_set_ext_stack_config_array(stack_p, CONFIG_HOST_OFFSET, stack_p->ext_info.host_mac, 6);

    esw_set_ext_stack_config_array(stack_p, CONFIG_HOST_OFFSET + 8, stack_p->ext_info.top_mac, 6);

    esw_set_ext_stack_config_uint32(stack_p, CONFIG_HOST_OFFSET + 16, stack_p->ext_info.host_ip_addr);

    esw_set_ext_stack_config_uint32(stack_p, CONFIG_HOST_OFFSET + 20, stack_p->ext_info.top_ip_addr);

    tpidvlan = 0x81000000 + ((int)(stack_p->ext_info.vlan_pri) << 13) + stack_p->ext_info.vlan;

    esw_set_ext_stack_config_uint32(stack_p, CONFIG_HOST_OFFSET + 24, tpidvlan);

    /* Config for BCM53903 that is currently hardwired on host side */
    /* outer / inner TPIDs for VLAN */
    esw_set_ext_stack_config_uint32(stack_p, CONFIG_VLAN_OFFSET, 0x91008100);
    /* MPLS label ethertype */
    esw_set_ext_stack_config_uint32(stack_p, CONFIG_MPLS_OFFSET, 0x88470000);

#if 0
    
    SOC_PBMP_PORT_ADD(pbmp, 0x03008000);
#endif

    /* Set config for loaded firmware */
    _bcm_ptp_write_pcishared_uint8_aligned_array(ptp_id, CONFIG_BASE, stack_p->persistent_config, CONFIG_TOTAL_SIZE);

    _bcm_ptp_ext_stack_start(ptp_id);

    return BCM_E_NONE;
}


/* Set RCPU information for ToP processor and supply rule table. */
int
_bcm_ptp_rcpu_configuration_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    uint16 rcpu_ethertype,
    uint16 rcpu_signature,
    uint16 rcpu_tpid,
    uint16 rcpu_vlan,
    int top_port,
    uint16 rcpu_reflected_ethertype,
    bcm_mac_t switch_mac)
{
    _bcm_ptp_info_t *ptp_info_p;
    _bcm_ptp_stack_info_t *stack_p;

    SET_PTP_INFO;
    if (!SOC_HAS_PTP_EXTERNAL_STACK_SUPPORT(unit)) {
        return BCM_E_UNAVAIL;
    }

    stack_p = &ptp_info_p->stack_info[ptp_id];

    esw_set_ext_stack_config_uint32(stack_p, CONFIG_RCPU_OFFSET + 0, rcpu_ethertype);
    esw_set_ext_stack_config_uint32(stack_p, CONFIG_RCPU_OFFSET + 4, rcpu_signature);
    esw_set_ext_stack_config_uint32(stack_p, CONFIG_RCPU_OFFSET + 8, rcpu_tpid);
    esw_set_ext_stack_config_uint32(stack_p, CONFIG_RCPU_OFFSET + 12, rcpu_vlan);
    esw_set_ext_stack_config_uint32(stack_p, CONFIG_RCPU_OFFSET + 16, top_port);
    esw_set_ext_stack_config_uint32(stack_p, CONFIG_RCPU_OFFSET + 20, rcpu_reflected_ethertype);
    esw_set_ext_stack_config_array(stack_p, CONFIG_RCPU_OFFSET + 24, switch_mac, 6);

    return BCM_E_NONE;
}


int _bcm_ptp_ext_stack_reset(int pci_idx)
{
    /*** Reset Core & ChipCommon in DMP ***/
    /* CPU Master DMP : soft reset : resetctrl (0x18103800) <- 0x1 (bit[0] <- 1 enter reset) */
    BCM_IF_ERROR_RETURN(_bcm_ptp_write_ks_uint32(pci_idx, 0x18103800, 1));
    /* ChipCommon Master DMP : soft reset : resetctrl (0x18100800) <- 0x1 (bit[0] <- 1 enter reset) */
    BCM_IF_ERROR_RETURN(_bcm_ptp_write_ks_uint32(pci_idx, 0x18100800, 1));

    /* delay 1ms */
    sal_usleep(1000);

    /*** Enable Core & ChipCommon clocks and bring out of DMP reset ***/
    /* CPU Master DMP : enable clock : ioctrl    (0x18103408) <- 0x1 (bit[0] <- 1 enable clock) */
    BCM_IF_ERROR_RETURN(_bcm_ptp_write_ks_uint32(pci_idx, 0x18103408, 1));
    /* CPU Master DMP : soft reset : resetctrl (0x18103800) <- 0x0 (bit[0] <- 0 exit reset) */
    BCM_IF_ERROR_RETURN(_bcm_ptp_write_ks_uint32(pci_idx, 0x18103800, 0));
    /* ChipCommon Master DMP : enable clock : ioctrl    (0x18100408) <- 0x1 (bit[0] <- 1 enable clock) */
    BCM_IF_ERROR_RETURN(_bcm_ptp_write_ks_uint32(pci_idx, 0x18100408, 1));
    /* ChipCommon Master DMP : soft reset : resetctrl (0x18100800) <- 0x0 (bit[0] <- 1 exit reset) */
    BCM_IF_ERROR_RETURN(_bcm_ptp_write_ks_uint32(pci_idx, 0x18100800, 0));

    /*** Enable SOCRAM clocks and bring out of DMP reset ***/
    /* SOCRAM0 Slave DMP : enable clock : ioctrl (0x18107408) <- 0x1 (bit[0] <- 1 to enable clock) */
    BCM_IF_ERROR_RETURN(_bcm_ptp_write_ks_uint32(pci_idx, 0x18107408, 1));
    /* SOCRAM0 Slave DMP : enable clock : resetctrl (0x18107800) <- 0x0 (bit[0] <- 0 exit reset) */
    BCM_IF_ERROR_RETURN(_bcm_ptp_write_ks_uint32(pci_idx, 0x18107800, 0));

    /*** set MIPS resetvec to start of SOCRAM ***/
    /* Core Resetvec : set reset vector : resetvec (0x18003004) <- 0xb9000000 (start of SOCRAM) */
    BCM_IF_ERROR_RETURN(_bcm_ptp_write_ks_uint32(pci_idx, 0x18003004, 0xb9000000));

    /* MIPS Corecontrol : core soft reset : (0x18003000) <- 0x7 (bit[0] <- 1 force reset) */
    /*                                                          (bit[1] <- 1 alternate resetvec) */
    /*                                                          (bit[2] <- 1 HT clock) */
    _bcm_ptp_write_ks_uint32(pci_idx, 0x18003000, 7);

    return BCM_E_NONE;
}


#define MAGIC_READ_VALUE    0xdeadc0de

int _bcm_ptp_ext_stack_start(int pci_idx)
{
    int rv = BCM_E_NONE;
    int boot_iter;
    uint32 value = MAGIC_READ_VALUE;

    /* MIPS Corecontrol : core soft reset : (0x18003000) <- 0x6 (bit[0] <- 0 leave reset) */
    /*                                                          (bit[1] <- 1 alternate resetvec) */
    /*                                                          (bit[2] <- 1 HT clock) */
    _bcm_ptp_write_ks_uint32(pci_idx, 0x18003000, 6);

    for (boot_iter = 0; boot_iter < MAX_BOOT_ITER; ++boot_iter) {
        _bcm_ptp_read_ks_uint32(pci_idx, BOOT_STATUS_ADDR, &value);
        if (value != MAGIC_READ_VALUE) {
            break;
        }

        /* delay 1 ms */
        sal_usleep(1000);
    }

    if (boot_iter == MAX_BOOT_ITER) {
        rv = BCM_E_FAIL;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("external stack start failed")));
    }

    return rv;
}


/* Set a value both on BCM53903 and in the persistent config used to reset BCM53903 after a load. */
void
esw_set_ext_stack_config_uint32(_bcm_ptp_stack_info_t *stack_p, uint32 offset, uint32 value)
{
    stack_p->ext_info.write_fn(stack_p->ext_info.cookie, CONFIG_BASE + offset, value);
    _bcm_ptp_uint32_write(&stack_p->persistent_config[offset], value);
}


/* Set an array of values as above */
void
esw_set_ext_stack_config_array(_bcm_ptp_stack_info_t* stack_p, uint32 offset, const uint8 * array, int len)
{
    while (len--) {
        _bcm_ptp_write_pcishared_uint8(&stack_p->ext_info, CONFIG_BASE + offset, *array);
        stack_p->persistent_config[offset] = *array;
        ++array;
        ++offset;
    }
}


void _bcm_ptp_write_pcishared_uint8_aligned_array(int pci_idx, uint32 addr, uint8 * array, int array_len)
{
    while (array_len > 0) {
        uint32 value = ( (((uint32)array[0]) << 24) | (((uint32)array[1]) << 16) |
                         (((uint32)array[2]) << 8)  | (uint32)array[3] );
        _bcm_ptp_write_ks_uint32(pci_idx, addr, value);
        array += 4;
        array_len -= 4;
        addr += 4;
    }
}


void _bcm_ptp_read_pcishared_uint8_aligned_array(int pci_idx, uint32 addr, uint8 * array, int array_len)
{
    while (array_len > 0) {
        uint32 value;
        _bcm_ptp_read_ks_uint32(pci_idx, addr, &value);
        array[0] = ((value >> 24) & 0xff);
        array[1] = ((value >> 16) & 0xff);
        array[2] = ((value >> 8)  & 0xff);
        array[3] = ((value >> 0)  & 0xff);

        array += 4;
        array_len -= 4;
        addr += 4;
    }
}


/* Get hardware timestamp and associated PTP time */
int
_bcm_ptp_ext_stack_timestamp_get(int pci_idx, int event_id, soc_cmic_uc_ts_data_t *ts_data)
{
    const uint32 timestamp_base = 0x19000c90;
    const int max_iter = 100;
    int iter = 0;

    uint32 read_seconds_hi[2] = {0,0}, read_seconds_lo[2] = {0, 0}, read_nsec[2] = {0, 0};
    uint32 read_ts0[2] = {0, 0}, read_ts[2] = {0, 0}, read_prev_ts[2] = {0,0};

    if (event_id < 0 || event_id > 5) {
        return BCM_E_PARAM;
    }

    do {
        read_seconds_hi[1] = read_seconds_hi[0];
        read_seconds_lo[1] = read_seconds_lo[0];
        read_nsec[1] = read_nsec[0];
        read_ts0[1] = read_ts0[0];
        read_ts[1] = read_ts[0];
        read_prev_ts[1] = read_prev_ts[0];

        BCM_IF_ERROR_RETURN(_bcm_ptp_read_ks_uint32(pci_idx, timestamp_base, &read_seconds_hi[0]));
        BCM_IF_ERROR_RETURN(_bcm_ptp_read_ks_uint32(pci_idx, timestamp_base + 0x04, &read_seconds_lo[0]));
        BCM_IF_ERROR_RETURN(_bcm_ptp_read_ks_uint32(pci_idx, timestamp_base + 0x08, &read_nsec[0]));
        BCM_IF_ERROR_RETURN(_bcm_ptp_read_ks_uint32(pci_idx, timestamp_base + 0x0c, &read_ts0[0]));
        BCM_IF_ERROR_RETURN(_bcm_ptp_read_ks_uint32(pci_idx, timestamp_base + 0x10 + 4 * event_id, &read_ts[0]));
        BCM_IF_ERROR_RETURN(_bcm_ptp_read_ks_uint32(pci_idx, timestamp_base + 0x28 + 4 * event_id, &read_prev_ts[0]));
    }
    while ((read_seconds_hi[0] != read_seconds_hi[1] ||
            read_seconds_lo[0] != read_seconds_lo[1] ||
            read_nsec[0] != read_nsec[1] ||
            read_ts0[0] != read_ts0[1] ||
            read_ts[0] != read_ts[1] ||
            read_prev_ts[0] != read_prev_ts[1])
           && (++iter < max_iter));

    ts_data->hwts = read_ts[0];
    ts_data->prev_hwts = read_prev_ts[0];

    /* the seconds/nsec time corresponds to the ts0 timestamp, so find the timestamp difference to apply to the PTP time */

    ts_data->time.seconds = ((((uint64)read_seconds_hi[0]) << 32) | (uint64)read_seconds_lo[0]);
    {
        uint64 one_billion = 1000000000;
        int32 ts_minus_ts0 = read_ts[0] - read_ts0[0];
        int64 full_ns = (int64)(read_nsec[0]) + (int64)ts_minus_ts0;

        if (full_ns < 0) {
            full_ns += one_billion;
            ts_data->time.seconds--;
        }

        if (full_ns >= one_billion) {
            full_ns -= one_billion;
            ts_data->time.seconds++;
        }

        ts_data->time.nanoseconds = (uint32)full_ns;
    }

    return (iter < max_iter) ? BCM_E_NONE : BCM_E_TIMEOUT;
}


/* Returns Boot status (1 == booted successfully) and fault status (0 == no fault) */
int
_bcm_ptp_ext_stack_fault_status_get(int pci_idx, uint32 *boot_status, uint32 *fault_status)
{
    BCM_IF_ERROR_RETURN(_bcm_ptp_read_ks_uint32(pci_idx, BOOT_STATUS_ADDR, boot_status));
    BCM_IF_ERROR_RETURN(_bcm_ptp_read_ks_uint32(pci_idx, FAULT_STATUS_ADDR, fault_status));

    return BCM_E_NONE;
}


/* Get low-level fault information, should be included with any crash bug reports */
int
_bcm_ptp_ext_stack_fault_information_get(int pci_idx, int max_size, uint8 *fault_info, int *fault_info_size)
{
    /* Note: may change in future, caller should not assume this size */
    const int fault_dump_size = 168;
    *fault_info_size = fault_dump_size + sizeof(_bcm_ptp_ext_stack_processor_info_t);

    if (max_size < *fault_info_size) {
        return BCM_E_NONE;
    }

    if (!fault_info) {
        return BCM_E_PARAM;
    }

    _bcm_ptp_read_pcishared_uint8_aligned_array(pci_idx, BOOT_STATUS_ADDR, fault_info, fault_dump_size);

    BCM_IF_ERROR_RETURN( _bcm_ptp_ext_stack_processor_status_get(
        pci_idx, (_bcm_ptp_ext_stack_processor_info_t*)((fault_info + fault_dump_size))));

    return BCM_E_NONE;
}

/* Get current and high-water-mark status of ToP processor, including memory
 * usage and CPU core utilization.
 */
int
_bcm_ptp_ext_stack_processor_status_get(int pci_idx, _bcm_ptp_ext_stack_processor_info_t *info)
{
    BCM_IF_ERROR_RETURN(_bcm_ptp_ext_stack_system_status_get(pci_idx, info));
    BCM_IF_ERROR_RETURN(_bcm_ptp_ext_stack_task_status_get(pci_idx, info));

    return BCM_E_NONE;
}


int
_bcm_ptp_ext_stack_task_status_get(int pci_idx, _bcm_ptp_ext_stack_processor_info_t *info)
{
    const uint32 info_base = 0x19000bac;
    const int stack_base_offset = 0x28;
    const int stack_top_offset = 0x40;

    const int max_tasks = 6;
    const int max_reported_tasks = 10;  /* in case max_tasks increases in the future, API has space to report more */
    int task;
    uint32 cursor;
    uint32 stack_base;
    uint32 stack_top;

    /* for any task slots that aren't filled, just report max uint */
    for (task = 0; task < max_reported_tasks; ++task) {
        info->stack_free[task] = 0xffffffff;
    }

    for (task = 0; task < max_tasks; ++task) {
        BCM_IF_ERROR_RETURN( _bcm_ptp_read_ks_uint32(
            pci_idx, info_base + stack_base_offset + task * 4, &stack_base));

        stack_base &= 0x3fffffff;     /* translate to physical memory address */
        if (stack_base != 0) {
            /* there is a task in this slot.
             * Walk through memory to find lowest point in stack with nonzero value */
            BCM_IF_ERROR_RETURN( _bcm_ptp_read_ks_uint32(
                pci_idx, info_base + stack_top_offset + task * 4, &stack_top));

            stack_top &= 0x3fffffff;  /* translate to physical memory address */
            for (cursor = stack_base; cursor < stack_top; cursor += 4) {
                uint32 testval;
                BCM_IF_ERROR_RETURN( _bcm_ptp_read_ks_uint32(
                    pci_idx, cursor, &testval));
                if (testval != 0) break;
            }

            info->stack_free[task] = (cursor - stack_base);
        }
    }

    return BCM_E_NONE;
}


int
_bcm_ptp_ext_stack_system_status_get(int pci_idx, _bcm_ptp_ext_stack_processor_info_t *info)
{
    const uint32 info_base = 0x19000bac;
    const int subheap_offset = 0x58;

    const int max_reported_tasks = 10;  /* in case max_tasks increases in the future, API has space to report more */
    int task;
    int heap;
    const int max_heaps = 10;

    BCM_IF_ERROR_RETURN(_bcm_ptp_read_ks_uint32(pci_idx, info_base + 0x00, &info->cpu_usage));
    BCM_IF_ERROR_RETURN(_bcm_ptp_read_ks_uint32(pci_idx, info_base + 0x04, &info->max_cpu_usage));
    BCM_IF_ERROR_RETURN(_bcm_ptp_read_ks_uint32(pci_idx, info_base + 0x08, &info->mem_free));
    BCM_IF_ERROR_RETURN(_bcm_ptp_read_ks_uint32(pci_idx, info_base + 0x0c, &info->min_mem_free));
    BCM_IF_ERROR_RETURN(_bcm_ptp_read_ks_uint32(pci_idx, info_base + 0x10, &info->tick_buffer));
    BCM_IF_ERROR_RETURN(_bcm_ptp_read_ks_uint32(pci_idx, info_base + 0x14, &info->min_tick_buffer));
    BCM_IF_ERROR_RETURN(_bcm_ptp_read_ks_uint32(pci_idx, info_base + 0x18, &info->one_sec_buffer));
    BCM_IF_ERROR_RETURN(_bcm_ptp_read_ks_uint32(pci_idx, info_base + 0x1c, &info->min_one_sec_buffer));
    BCM_IF_ERROR_RETURN(_bcm_ptp_read_ks_uint32(pci_idx, info_base + 0x20, &info->one_min_buffer));
    BCM_IF_ERROR_RETURN(_bcm_ptp_read_ks_uint32(pci_idx, info_base + 0x24, &info->min_one_min_buffer));

    /* just report uint_max since we aren't checking here */
    for (task = 0; task < max_reported_tasks; ++task) {
        info->stack_free[task] = 0xffffffff;
    }

    for (heap = 0; heap < max_heaps; ++heap) {
        BCM_IF_ERROR_RETURN(_bcm_ptp_read_ks_uint32(pci_idx, info_base + subheap_offset + heap * 4, &info->sub_heap_free[heap]));
    }

    return BCM_E_NONE;
}


/* Extract FW information from the READY event from the FW */
int
_bcm_ptp_ext_fw_info_set(int unit,
                         bcm_ptp_stack_id_t ptp_id,
                         uint8 *data)
{
    int i;
    int descr_len;
    int descr_limit_len;

    sal_memset(top_fw_info.firmware_version,0,sizeof(top_fw_info.firmware_version));
    sal_memset(top_fw_info.servo_version,0,sizeof(top_fw_info.servo_version));

    /*
     * Parse ToP ready event data.
     *    Octet 0...1         : Firmware version string length (N).
     *    Octet 2...N+1       : Firmware version.
     *    Octet N+2...N+3     : Servo version string length (P).
     *    Octet N+4...N+P+3   : Servo version.
     *    Octet N+P+4...N+P+7 : PTP stack timer frequency (Hz).
     *    Octet N+P+8         : Maximum number of PTP clocks (instances).
     *    Octet N+P+9         : Maximum number of PTP ports per PTP clock.
     *    Octet N+P+10        : Maximum number of unicast masters.
     *    Octet N+P+11        : Maximum number of unicast slaves.
     *    Octet N+P+12        : Maximum number of foreign master dataset entries (per port).
     */
    i = 0;
    descr_len = _bcm_ptp_uint16_read(data+i);
    i += sizeof(uint16);

    descr_limit_len = (descr_len < sizeof(top_fw_info.firmware_version)) ?
        (descr_len) : (sizeof(top_fw_info.firmware_version)-1);
    sal_memcpy(top_fw_info.firmware_version, data+i, descr_limit_len);
    i += descr_len;

    descr_len = _bcm_ptp_uint16_read(data+i);
    i += sizeof(uint16);

    descr_limit_len = (descr_len < sizeof(top_fw_info.servo_version)) ?
        (descr_len) : (sizeof(top_fw_info.servo_version)-1);
    sal_memcpy(top_fw_info.servo_version, data+i, descr_limit_len);
    i += descr_len;

    top_fw_info.ptp_stack_timer_hz = _bcm_ptp_uint32_read(data+i);
    i += sizeof(uint32);

    top_fw_info.ptp_clocks_max = data[i++];
    top_fw_info.ptp_ports_per_clock_max = data[i++];

    top_fw_info.unicast_masters_max = data[i++];
    top_fw_info.unicast_slaves_max = data[i++];
    top_fw_info.foreign_master_dataset_entries_max = data[i];

    return BCM_E_NONE;
}

int
_bcm_ptp_ext_fw_info_get(
                         int unit,
                         bcm_ptp_stack_id_t ptp_id,
                         _bcm_ptp_ext_fw_info_t *info)
{
    *info = top_fw_info;
    return BCM_E_NONE;
}

#endif /* defined(BCM_PTP_EXTERNAL_STACK_SUPPORT) */
#endif /* defined(INCLUDE_PTP)*/
