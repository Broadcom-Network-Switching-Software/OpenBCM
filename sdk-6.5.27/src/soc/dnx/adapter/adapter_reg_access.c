
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. $
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
#include <assert.h>

#include <bde/pli/verinet.h>

#include <errno.h>
#include <sys/socket.h>
#include <soc/error.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_adapter.h>

#include <soc/dnxc/swstate/auto_generated/access/adapter_access.h>
#include <soc/dnxc/dnxc_adapter_reg_access.h>

#include <netinet/tcp.h>

#define MAX_PACKET_HEADER_SIZE_ADAPTER 30

#define RX_THREAD_NOTIFY_CLOSED (-2)

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

#endif
