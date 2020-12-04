/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * SAND send \ receive SR cell example:
 * 
 * The example simulate:
 *  1. define single route and send  sr cell to this route
 *  2. define route group and cell sr cell to the group
 *  3. receive sr cells
 */

/*
 * Global variables
 */
uint32 MAX_SEND_WORDS = 16;
uint32 MAX_RECEIVE_WORDS = 76;
uint32 MAX_SR_DATA_CELL_PAYLOAD_WORDS = 32;
uint32 data_set[MAX_SEND_WORDS];
uint32 data_get[MAX_RECEIVE_WORDS];
uint32 received_data_actual_size = 0;
uint32 FAP_LINK_ID_BIT_NOF = 6;

typedef enum source_routed_cell_datapath_t
{
    datapath_fap_to_fap,
    datapath_fap_to_fe,
    datapath_fe_to_fap
};


/**
 * \brief
 *   Send cell through a specific route.
 * \param [in] nof_hops - number of hops in route.
 * \param [in] link_ids - array of links to hop through.
 */
int
cint_sr_cell_system_send_route(int unit, int nof_hops, int* link_ids)
{
    int rv, route_id;
    bcm_fabric_route_t route;

    bcm_fabric_route_t_init(&route);

    /*
     * define a single route from FAP to FAP.
     * link_ids contains the list of links to hop through.
     */
    route.number_of_hops = nof_hops;
    route.hop_ids = link_ids;

    /*
     * build data
     */
    data_set[0] = 0x00000000;
    data_set[1] = 0x11111111;
    data_set[2] = 0x22222222;
    data_set[3] = 0x33333333;
    data_set[4] = 0x44444444;
    data_set[5] = 0x55555555;
    data_set[6] = 0x66666666;
    data_set[7] = 0x77777777;
    data_set[8] = 0x88888888;
    data_set[9] = 0x99999999;
    data_set[10] = 0xaaaaaaaa;
    data_set[11] = 0xbbbbbbbb;
    data_set[12] = 0xcccccccc;
    data_set[13] = 0xdddddddd;
    data_set[14] = 0xeeeeeeee;
    data_set[15] = 0xffffffff;

    /*
     * send source-routed cell
     */
    rv = bcm_fabric_route_tx(unit, 0, route, MAX_SEND_WORDS, data_set);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_fabric_route_tx, rv=%d, \n", rv);
        return rv;
    }

    printf("cint_sr_cell_system_send_route: PASS\n");
    return BCM_E_NONE;
}

/*receive sr cells*/
int
receive_sr_cell(
    int unit,
    uint32 flags,
    int max_messages,
    uint32 *data_actual_size_p)
{
    int rv, count, i;
    uint32 data_actual_size;

    /*
     * in case several sr cells received the cells are accumulated in SW 
     * For that reason it's important to read in loop (even if the relevant interrupt is down) 
     * until soc_receive_sr_cell return EMPTY error.
     */
    count = 0;
    while (count < max_messages)
    {
        /*
         * receive sr cell data
         */
        rv = bcm_fabric_route_rx(unit, flags, MAX_RECEIVE_WORDS, data_get, &data_actual_size);
        /*
         * all messages was read
         */
        if (BCM_E_EMPTY == rv)
        {
            printf("No more messages to read \n");
            break;
        }
        else if (rv != BCM_E_NONE)
        {
            printf("Error, in soc_receive_sr_cell, rv=%d, \n", rv);
            return rv;
        }

        /*
         * print received data
         */
        printf("Message(%d words) received: ", data_actual_size);
        for (i = 0; i < data_actual_size; i++)
        {
            printf("0x%x ", data_get[i]);
        }
        printf("\n");

        *data_actual_size_p = data_actual_size;

        count++;
    }

    printf("%d messages received \n", count);

    printf("receive_sr_cell: PASS\n");

    return BCM_E_NONE;
}

/**
 * \brief
 *   Clear the receive buffer
 */
void
cint_sr_cell_system_clear(int unit)
{
    uint32 data_actual_size;

    receive_sr_cell(unit, 0, 1000, &data_actual_size);

    printf("Receive buffer cleared\n");
    printf("cint_sr_cell_system_clear: PASS\n");
}

/**
 * \brief
 *   Read cell from received buffer
 */
int
cint_sr_cell_system_receive(int unit, uint32 flags)
{
    int rv;

    rv = receive_sr_cell(unit, flags, 1, &received_data_actual_size);
    if (rv != BCM_E_NONE)
    {
        printf("failed to receive cell\n");
        return rv;
    }

    printf("Receive buffer success\n");
    printf("cint_sr_cell_system_receive: PASS\n");

    return BCM_E_NONE;
}

/**
 * \brief
 *   Verify that the send and received packets are the same
 */
int
cint_sr_cell_system_verify(
    int src_unit,
    int fip_link,
    uint32 flags,
    source_routed_cell_datapath_t datapath)
{
    int i = 0, data_offset = 0, data_valid_offset = 0;
    int src_device_id, rv = BCM_E_NONE, payload_data_len, expected_receive_len;
    /*
     * Verify the length
     */
    payload_data_len = (MAX_RECEIVE_WORDS > MAX_SR_DATA_CELL_PAYLOAD_WORDS) ? MAX_SR_DATA_CELL_PAYLOAD_WORDS : MAX_RECEIVE_WORDS;
    expected_receive_len = (flags & BCM_FABRIC_CELL_RX_HEADER_PREPEND) ? (payload_data_len + bcmFabricCellHeaderFieldTypeNof) : payload_data_len;
    if (received_data_actual_size != expected_receive_len)
    {
        printf("The expected received data length is %d, the received data length is %d!\n", expected_receive_len,
               received_data_actual_size);
        return BCM_E_FAIL;
    }
    /*
     * If there is prepend header, compare the header
     */
    if (flags & BCM_FABRIC_CELL_RX_HEADER_PREPEND)
    {
        data_offset = bcmFabricCellHeaderFieldTypeNof;

        if (datapath == datapath_fe_to_fap)
        {
            rv = bcm_stk_modid_get(src_unit, &src_device_id);
            if (rv != BCM_E_NONE)
            {
                printf("failed in bcm_stk_modid_get\n");
                return rv;
            }
        }
        else
        {
            src_device_id = src_unit;
        }

        if (data_get[bcmFabricCellHeaderFieldTypeSourceDevice] != src_device_id)
        {
            printf("The expected source device ID is %d, the received ID is %d!\n", src_device_id,
                   data_get[bcmFabricCellHeaderFieldTypeSourceDevice]);
            return BCM_E_FAIL;
        }

        if ((datapath != datapath_fe_to_fap) && (data_get[bcmFabricCellHeaderFieldTypeFIPLink] != (fip_link & ((1<<FAP_LINK_ID_BIT_NOF)-1))))
        {
            printf("The expected FIP Link ID is %d, the received ID is %d!\n", fip_link,
                   data_get[bcmFabricCellHeaderFieldTypeFIPLink]);
            return BCM_E_FAIL;
        }
        if (data_get[bcmFabricCellHeaderFieldTypePipe] != 0)
        {
            printf("The expected PIPE ID is 0, the received ID is %d!\n", data_get[bcmFabricCellHeaderFieldTypeFIPLink]);
            return BCM_E_FAIL;
        }
    }
    if ((datapath == datapath_fap_to_fap) || (datapath == datapath_fe_to_fap))
    {
        data_valid_offset = 13;
    }
    for (i = data_valid_offset; i < MAX_SEND_WORDS; i++)
    {
        if (data_set[i] != data_get[i + data_offset])
        {
            int k;
            printf("Failure: Received buffer is different from sent\n");
            for (k = data_offset; (k < received_data_actual_size && k < MAX_SEND_WORDS); k++)
            {
                printf("0x%08x 0x%08x\n", data_get[k], data_set[k]);
            }

            return BCM_E_FAIL;
        }
    }

    printf("cint_sr_cell_system_verify: PASS\n");
    return BCM_E_NONE;
}

/**
 * \brief
 *   Test sending source routed cell through a specific route.
 * \param [in] src_unit - unit id of the source device
 * \param [in] dest_unit - unit id of the destination device
 * \param [in] datapath - sr cell datapath
 * \param [in] flags - the flags for bcm_fabric_route_rx
 * \param [in] nof_hops - number of hops in route.
 * \param [in] link_ids - array of links to hop through.
 */
int
cint_sr_cell_system_test(
    int src_unit,
    int dest_unit,
    source_routed_cell_datapath_t datapath,
    uint32 flags,
    int nof_hops,
    int *link_ids)
{

    int i;
    int rv;
    uint32 data_actual_size;

    /*
     * Clear buffer
     */
    cint_sr_cell_system_clear(dest_unit);

    /*
     * Send source routed cell
     */
    rv = cint_sr_cell_system_send_route(src_unit, nof_hops, link_ids);
    if (BCM_FAILURE(rv))
    {
        printf("Error, in cint_sr_cell_system_send_route, rv=%d, \n", rv);
        return rv;
    }

    /*
     * Receive source routed cell
     */
    rv = cint_sr_cell_system_receive(dest_unit, flags);
    if (rv != BCM_E_NONE)
    {
        printf("failed to receive cell\n");
        return rv;
    }

    /*
     * Verified received source routed cell
     */
    rv = cint_sr_cell_system_verify(src_unit, link_ids[0], flags, datapath);
    if (rv != BCM_E_NONE)
    {
        printf("failed in cint_sr_cell_system_verify\n");
        return rv;
    }
    printf("cint_sr_cell_system_test: PASS\n");
    return BCM_E_NONE;
}

