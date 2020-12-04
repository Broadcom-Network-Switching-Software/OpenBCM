/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip main functions
 */
#ifdef _TDM_STANDALONE
    #include <tdm_top.h>
#else
    #include <soc/tdm/core/tdm_top.h>
#endif

#define NO_FLOATING 1 /* avoid floating point */

int GH2_CAL_LEN; /* Calendar length ( TDM table size ) for ... */

/* #ifdef _TDM_DB_STACK
    size_t stack_size = 0;
#endif */

/**
@name: tdm_gh2_corereq
@param:

Allocate memory for core data execute request to core executive
 */
int
tdm_gh2_corereq( tdm_mod_t *_tdm )
{
    _tdm->_core_data.vars_pkg.cal_id=_tdm->_core_data.vars_pkg.pipe;

    return ( _tdm->_core_exec[TDM_CORE_EXEC__INIT]( _tdm ) );
}


/**
@name: tdm_gh2_scheduler_wrap_pipe
@param:

Code wrapper for ingress TDM scheduling
 */
int
tdm_gh2_scheduler_wrap_pipe( tdm_mod_t *_tdm )
{
    int iter, idx1=0, idx2=0, ethernet_encap=BOOL_TRUE;
    int param_cal_len=0, param_lr_limit=0, param_ancl_num=0/*,
        param_clk_freq*/;

    _tdm->_core_data.vars_pkg.pipe=(_tdm->_chip_data.soc_pkg.soc_vars.gh2.pipe_start/32);
    if (_tdm->_core_data.vars_pkg.pipe>3) {
        TDM_ERROR1("Invalid pipe ID - %0d\n",_tdm->_core_data.vars_pkg.pipe);
        return (TDM_EXEC_CHIP_SIZE+1);
    }
    tdm_gh2_parse_pipe(_tdm);
    for (iter=0; iter<TDM_AUX_SIZE; iter++) {
        _tdm->_core_data.vars_pkg.lr_buffer[iter]=GH2_NUM_EXT_PORTS;
        _tdm->_core_data.vars_pkg.os_buffer[iter]=GH2_NUM_EXT_PORTS;
    }

    for (iter=(_tdm->_chip_data.soc_pkg.soc_vars.gh2.pipe_start-1); iter<(_tdm->_chip_data.soc_pkg.soc_vars.gh2.pipe_end); iter++) {
        if ( (_tdm->_chip_data.soc_pkg.state[iter]==PORT_STATE__LINERATE)||(_tdm->_chip_data.soc_pkg.state[iter]==PORT_STATE__LINERATE_HG) ) {
            if (idx1<TDM_AUX_SIZE){
                _tdm->_core_data.vars_pkg.lr_buffer[idx1++]=(iter+1);
            }
            if (idx1>32){
                TDM_PRINT2("WARNING: Pipe %d, line rate queue overflow, port %d may have been skipped.\n", _tdm->_core_data.vars_pkg.pipe, (iter+1));
            }
        }
        else if ( (_tdm->_chip_data.soc_pkg.state[iter]==PORT_STATE__OVERSUB)||(_tdm->_chip_data.soc_pkg.state[iter]==PORT_STATE__OVERSUB_HG) ) {
            if (idx2<TDM_AUX_SIZE){
                _tdm->_core_data.vars_pkg.os_buffer[idx2++]=(iter+1);
            }
            if (idx2>32){
                TDM_PRINT2("WARNING: Pipe %d, oversub queue overflow, port %d may have been skipped.\n", _tdm->_core_data.vars_pkg.pipe, (iter+1));
            }
        }
    }
    if ( (_tdm->_chip_data.soc_pkg.soc_vars.gh2.mgmt_pm_hg==BOOL_TRUE) &&
         (_tdm->_core_data.vars_pkg.pipe==1||_tdm->_core_data.vars_pkg.pipe==2) &&
         ( (_tdm->_core_data.vars_pkg.lr_buffer[0]!=GH2_NUM_EXT_PORTS&&_tdm->_chip_data.soc_pkg.clk_freq>=MIN_HG_FREQ)||
           (_tdm->_core_data.vars_pkg.lr_buffer[0]==GH2_NUM_EXT_PORTS) ) ) {
        _tdm->_chip_data.soc_pkg.soc_vars.gh2.higig_mgmt=BOOL_TRUE;
    }
    else {
        _tdm->_chip_data.soc_pkg.soc_vars.gh2.higig_mgmt=BOOL_FALSE;
    }
    for (iter=0; iter<TDM_AUX_SIZE; iter++) {
        if (_tdm->_core_data.vars_pkg.lr_buffer[iter]!=GH2_NUM_EXT_PORTS) {
            TDM_PUSH(_tdm->_core_data.vars_pkg.lr_buffer[iter],TDM_CORE_EXEC__ENCAP_SCAN,ethernet_encap);
            if (!ethernet_encap) {
                break;
            }
        }
        if (_tdm->_core_data.vars_pkg.os_buffer[iter]!=GH2_NUM_EXT_PORTS) {
            TDM_PUSH(_tdm->_core_data.vars_pkg.os_buffer[iter],TDM_CORE_EXEC__ENCAP_SCAN,ethernet_encap);
            if (!ethernet_encap) {
                break;
            }
        }
    }
    /* param_clk_freq = _tdm->_chip_data.soc_pkg.clk_freq;*/
    TDM_PRINT1( "  get GH2_CAL_LEN %d\n" , GH2_CAL_LEN );
    param_cal_len = GH2_CAL_LEN;
    param_ancl_num = GH2_ACC_PORT_NUM;
    param_lr_limit = param_cal_len - param_ancl_num;

    _tdm->_chip_data.soc_pkg.tvec_size    = param_ancl_num;
    _tdm->_chip_data.soc_pkg.lr_idx_limit = param_lr_limit;

    return ( _tdm->_chip_exec[TDM_CHIP_EXEC__COREREQ]( _tdm ) );
}


/**
@name: tdm_gh2_scheduler_wrap
@param:

Code wrapper for ingress TDM scheduling
 */
int
tdm_gh2_scheduler_wrap( tdm_mod_t *_tdm )
{
    int pipe_id, result=PASS;

    /* TDM_PRINT_STACK_SIZE("tdm_gh2_scheduler_wrap"); */

    for (pipe_id=0; pipe_id<1; pipe_id++){
        result = tdm_gh2_scheduler_wrap_pipe(_tdm);
        if (result!=PASS){
            break;
        }
    }

    return result;
}

/* Physical port numbers of GH2: */
#define     GPORT0_0    2
#define     GPORT1_0    10
#define     GPORT2_0    18
#define     GPORT3_0    26
#define     GPORT4_0    34
#define     GPORT5_0    42
#define     GPORT6_0    50
#define     XLPORT0_0   58
#define     XLPORT1_0   62
#define     XLPORT2_0   66
#define     XLPORT3_0   70
#define     XLPORT4_0   74
#define     XLPORT5_0   78
#define     XLPORT6_0   82
#define     CLPORT0_0   86

int             map_phy2psu [ GH2_NUM_PHY_PORTS ];
/*
 * map_phy2psu[] contains the mapping from physical ports to pseudo
 * ports. If map_phy2psu[i] = j, it means physical port i maps to
 * pseudo port j. Before this program generates TDM table for a port
 * configuration, the chip's physical ports are mapped to pseudo
 * ports. This program generates TDM table by placing the pseudo ports
 * into it, as evenly as possible. Then, post process is performed,
 * mapping the pseudo ports in the TDM table back to physical ports.
 */

int             map_psu2phy [ GH2_NUM_PHY_PORTS ];
/*
 * map_phy2psu[] contains the mapping from pseudo ports to physical
 * ports ( the reverse of map_phy2psu[] ). If map_psu2phy[j] = i, it
 * means pseudo port j maps to physical port i.
 */

int             port_speed  [ GH2_NUM_PHY_PORTS ];
/* port_speed[i] stores speed of physical port i. */

/* -------------------------------------------------------------------- */
/* */
/* -------------------------------------------------------------------- */
void    init_port_map(void)
{
    int i , j , phy_pnum , psu_pnum;

    for( i = 0 ; i < GH2_NUM_PHY_PORTS ; i++ )
    {
        map_phy2psu[ i ] = -1;
        map_psu2phy[ i ] = -1;
    }

    /* CPU-port: */
    map_phy2psu[ 0 ] = 0;
    map_psu2phy[ 0 ] = 0;
    psu_pnum = 1;

    /* GPORTs: only the 4 lower sub-ports of each GPORT are mapped. */
    for( i = 0 ; i <= 6 ; i++ )
    {
        for( j = 0 ; j < 4 ; j++ )
        {
            phy_pnum = GPORT0_0 + ( i * 8 ) + j;
            map_phy2psu[ phy_pnum ] = psu_pnum;
            map_psu2phy[ psu_pnum ] = phy_pnum;
            psu_pnum++;
        }
    }

    /* XLPORTs: */
    for( i = 0 ; i <= 6 ; i++ )
    {
        for( j = 0 ; j < 2 ; j++ )
        {
            phy_pnum = XLPORT0_0 + ( i * 4 ) + j;
            map_phy2psu[ phy_pnum ] = psu_pnum;
            map_psu2phy[ psu_pnum ] = phy_pnum;
            psu_pnum++;
        }
        psu_pnum += 2;
        for( j = 0 ; j < 2 ; j++ )
        {
            phy_pnum = XLPORT0_0 + ( i * 4 ) + j + 2;
            map_phy2psu[ phy_pnum ] = psu_pnum;
            map_psu2phy[ psu_pnum ] = phy_pnum;
            psu_pnum++;
        }
        psu_pnum += 2;
    }

    /* CLPORTs: */
    for( i = 0 ; i <= 0 ; i++ )
    {
        for( j = 0 ; j < 4 ; j++ )
        {
            phy_pnum = CLPORT0_0 + ( i * 4 ) + j;
            map_phy2psu[ phy_pnum ] = psu_pnum;
            map_psu2phy[ psu_pnum ] = phy_pnum;
            psu_pnum++;
        }
    }

    TDM_PRINT0( "init_port_map: map_phy2psu( mapping from " );
    TDM_PRINT0( "physical ports to pseudo ports )\n" );
    for( i = 0 ; i < GH2_NUM_PHY_PORTS ; i++ )
    {
        if( i % 10 == 0 )
            TDM_PRINT1( "  @%03d: " , i );
        TDM_PRINT1( "%3d " , map_phy2psu[ i ] );
        if( ( i % 10 == 9 ) || ( i == GH2_NUM_PHY_PORTS - 1 ) )
            TDM_PRINT0( "\n" );
    }
    TDM_PRINT0( "init_port_map: map_psu2phy( mapping from " );
    TDM_PRINT0( "pseudo ports to physical ports )\n" );
    for( i = 0 ; i < GH2_NUM_PHY_PORTS ; i++ )
    {
        if( i % 10 == 0 )
            TDM_PRINT1( "  @%03d: " , i );
        TDM_PRINT1( "%3d " , map_psu2phy[ i ] );
        if( ( i % 10 == 9 ) || ( i == GH2_NUM_PHY_PORTS - 1 ) )
            TDM_PRINT0( "\n" );
    }
}

/*--------------------------------------------------------------------*/
/* find_slot_unit() tries to find a slot unit that causes the lowest  */
/* bandwidth waste.                                                   */
/*                                                                    */
/* All ports referred in this function are physical ports.            */
/*--------------------------------------------------------------------*/
void    find_slot_unit
(
    tdm_mod_t*      _tdm
)
{
    int i , j , slot_unit = TDM_SLOT_UNIT_1G, best_unit = 0 ,
        slot_cnt , over , sum_over , min_over;

    for( i = 0 ; i < GH2_NUM_PHY_PORTS ; i++ )
    {
        if( i == 0 ) /* CPU port */
            port_speed[ i ] = SPEED_1G;
        else
            port_speed[ i ] = _tdm->_chip_data.soc_pkg.speed[ i ];
    }

    min_over = 0;
    for( i = 0 ; i <= 6 ; i++ )
    {
        switch( i )
        {
            case 0 : slot_unit = TDM_SLOT_UNIT_1G; break;
            case 1 : slot_unit = TDM_SLOT_UNIT_1P25G; break;
            case 2 : slot_unit = TDM_SLOT_UNIT_2P5G; break;
            case 3 : slot_unit = TDM_SLOT_UNIT_5G; break;
            case 4 : slot_unit = TDM_SLOT_UNIT_10G; break;
            case 5 : slot_unit = TDM_SLOT_UNIT_25G; break;
            case 6 : slot_unit = TDM_SLOT_UNIT_50G; break;
        }
        TDM_PRINT0( "find_slot_unit: " );
        TDM_PRINT1( "try slot_unit %d\n" , slot_unit );

        sum_over = 0;
        for( j = 0 ; j < GH2_NUM_PHY_PORTS ; j++ )
        {
            if( port_speed[ j ] > 0 )
            {
                slot_cnt = port_speed[ j ] / slot_unit;
                /* 
                 * Here slot_cnt is number of TDM slots allocated to
                 * port-j.
                 */

                while( slot_cnt * slot_unit < port_speed[ j ] )
                {
                    /*
                     * Value of slot_cnt * slot_unit is the bandwidth
                     * allocated to port-j. If it's lower than
                     * port_speed[ j ], we should increase slot_cnt
                     * give enough bandwidth to port-j.
                     */
                    slot_cnt++;
                }

                over = ( slot_cnt * slot_unit ) - port_speed[ j ];
                /*
                 * over > 0 means more bandwidth is allocated to
                 * port-j than it needs, and so causes waste. This
                 * function tries to find a slot unit that causes the
                 * minimum waste.
                 */

                sum_over += over;

                TDM_PRINT1( "  port-%d " , j );
                TDM_PRINT1( "speed %d " , port_speed[ j ] );
                TDM_PRINT1( "slot_cnt %d " , slot_cnt );
                TDM_PRINT1( "over %d\n" , over );
            }
        }
        if( ( i == 0 ) || ( sum_over < min_over ) )
        {
            best_unit = slot_unit;
            min_over = sum_over;
        }
        TDM_PRINT1( "  sum_over %d " , sum_over );
        TDM_PRINT1( "best_unit %d\n" , best_unit );
    }

    _tdm->_core_data.vars_pkg.pipe_info.slot_unit = best_unit;

    TDM_PRINT1( "find_slot_unit: set slot_unit %d\n" ,
        _tdm->_core_data.vars_pkg.pipe_info.slot_unit );
}

/*--------------------------------------------------------------------*/
/* find_cal_len() determines the TDM table size ( calendar length ).  */
/*                                                                    */
/* All ports referred in this function are physical ports.            */
/*--------------------------------------------------------------------*/
int     find_cal_len
(
    tdm_mod_t*      _tdm
)
{
    int i , j , slot_unit , cal_len , sum_slot_cnt;

    int slot_cnt [ GH2_NUM_PHY_PORTS ];
    /*
     * slot_cnt[ i ] stores the number of slots that will be allocated
     * to port-i.
     */

    slot_unit = _tdm->_core_data.vars_pkg.pipe_info.slot_unit;

    cal_len = 0;
    for( i = 0 ; i < GH2_NUM_PHY_PORTS ; i++ )
    {
        if( port_speed[ i ] > 0 )
        {
            slot_cnt[ i ] = port_speed[ i ] / slot_unit;
            /*
             * Here slot_cnt is number of TDM slots allocated to
             * port-i.
             */

            if( slot_cnt[ i ] * slot_unit < port_speed[ i ] )
            {
                /*
                 * Value of slot_cnt[ i ] * slot_unit is the
                 * bandwidth allocated to port-i. If it's lower than
                 * port_speed[ i ], we should increase slot_cnt[ i ]
                 * give enough bandwidth to port-i.
                 */
                while( slot_cnt[ i ] * slot_unit < port_speed[ i ] )
                {
                    slot_cnt[ i ]++;
                }

                port_speed[ i ] = slot_cnt[ i ] * slot_unit;
                _tdm->_chip_data.soc_pkg.speed[ i ] = port_speed[ i ];

                TDM_PRINT1( "find_cal_len: adjust port-%d " , i );
                TDM_PRINT1( "speed to %d\n" ,
                    _tdm->_chip_data.soc_pkg.speed[ i ] );
            }
        }
        else
        {
            slot_cnt[ i ] = 0;
        }
        cal_len += slot_cnt[ i ];
    }

    TDM_PRINT1( "find_cal_len: cal_len %d\n" , cal_len );

    /* Check TDM table size for MMU 5T constraint: */
    for( i = 0 ; i < GH2_NUM_PHY_PORTS ; i++ )
    {
        if( cal_len < slot_cnt[ i ] * 5 )
        {
            /*
             * Due to MMU 5T constraint ( let 'TDM latency' mean the
             * distance of the same port in TDM table; GH2 MMU design
             * requires TDM latency of each port equal to or greater
             * than 5 ), if a port occupies N slots, the TDM table
             * size must not be smaller than N * 5.
             */

            cal_len = slot_cnt[ i ] * 5;
            TDM_PRINT0( "find_cal_len: increase cal_len to " );
            TDM_PRINT2( "%d for port-%d ( slot " , cal_len , i );
            TDM_PRINT1( "cnt %d )\n" , slot_cnt[ i ] );
        }
    }

    /* Check TDM table size for GPORT sister port space: */
    for( i = 0 ; i <= 6 ; i++ )
    {
        sum_slot_cnt = 0;
        for( j = 0 ; j < 8 ; j++ )
        {
            sum_slot_cnt += slot_cnt[ GPORT0_0 + ( 8 * i ) + j ];
        }

        if( cal_len < sum_slot_cnt * 4 )
        {
            /*
             * Sister port space of GPORT is 4 ( TDM latency between
             * two sub-ports of the same GPORT must be equal to or
             * greater than 4 ), if a GPORT's sub-ports totally
             * occupies N slots, the TDM table size must not be
             * smaller than N * 4.
             */

            cal_len = sum_slot_cnt * 4;
            TDM_PRINT0( "find_cal_len: increase cal_len to " );
            TDM_PRINT2( "%d for GPORT%d ( slot " , cal_len , i );
            TDM_PRINT1( "cnt sum %d )\n" , sum_slot_cnt );
        }
    }

    /* Check TDM table size for XLPORT sister port space: */
    for( i = 0 ; i <= 6 ; i++ )
    {
        sum_slot_cnt = 0;
        for( j = 0 ; j < 2 ; j++ )
        {
            sum_slot_cnt += slot_cnt[ XLPORT0_0 + ( 4 * i ) + j ];
        }

        if( cal_len < sum_slot_cnt * 4 )
        {
            /*
             * Sister port space of XLPORTx_0/1 is 4 ( TDM latency
             * between XLPORTx_0 and XLPORTx_1 must be equal to or
             * greater than 4 ), if XLPORTx_0 and XLPORTx_1 totally
             * occupies N slots, the TDM table size must not be
             * smaller than N * 4.
             */

            cal_len = sum_slot_cnt * 4;
            TDM_PRINT0( "find_cal_len: increase cal_len to " );
            TDM_PRINT2( "%d for XLPORT%d_0/1 ( slot " , cal_len , i );
            TDM_PRINT1( "cnt sum %d )\n" , sum_slot_cnt );
        }

        sum_slot_cnt = 0;
        for( j = 2 ; j < 4 ; j++ )
        {
            sum_slot_cnt += slot_cnt[ XLPORT0_0 + ( 4 * i ) + j ];
        }

        if( cal_len < sum_slot_cnt * 4 )
        {
            /*
             * Sister port space of XLPORTx_2/3 is 4 ( TDM latency
             * between XLPORTx_2 and XLPORTx_3 must be equal to or
             * greater than 4 ), if XLPORTx_2 and XLPORTx_3 totally
             * occupies N slots, the TDM table size must not be
             * smaller than N * 4.
             */

            cal_len = sum_slot_cnt * 4;
            TDM_PRINT0( "find_cal_len: increase cal_len to " );
            TDM_PRINT2( "%d for XLPORT%d_2/3 ( slot " , cal_len , i );
            TDM_PRINT1( "cnt sum %d )\n" , sum_slot_cnt );
        }
    }

    /* Check TDM table size for CLPORT sister port space: */
    for( i = 0 ; i <= 0 ; i++ )
    {
        sum_slot_cnt = 0;
        for( j = 0 ; j < 4 ; j++ )
        {
            sum_slot_cnt += slot_cnt[ CLPORT0_0 + ( 4 * i ) + j ];
        }

        if( cal_len < sum_slot_cnt * 2 )
        {
            /*
             * Sister port space of CLPORT is 2 ( TDM latency between
             * two sub-ports of the same CLPORT must be equal to or
             * greater than 2 ), if an CLPORT's sub-ports totally
             * occupies N slots, the TDM table size must not be
             * smaller than N * 2.
             */

            cal_len = sum_slot_cnt * 2;
            TDM_PRINT0( "find_cal_len: increase cal_len to " );
            TDM_PRINT2( "%d for CLPORT%d ( slot " , cal_len , i );
            TDM_PRINT1( "cnt sum %d )\n" , sum_slot_cnt );
        }
    }

    return cal_len;
}

/**
@name: tdm_gh2_pmap_transcription
@param:

For Greyhound2
Transcription algorithm for generating port module mapping
 */
int
tdm_gh2_pmap_transcription( tdm_mod_t *_tdm )
{
    int i , j , phy_pnum , psu_pnum , sub_port , pm_idx;
    int psu_speed [ GH2_NUM_PHY_PORTS ];
    int** pmap; /* a pointer to _tdm->_chip_data.soc_pkg.pmap */

    /* Initialize pmap */
    pmap = _tdm->_chip_data.soc_pkg.pmap;
    for( i = 0 ; i < GH2_NUM_PHY_PM ; i++ )
    {
        for( j = 0 ; j < GH2_NUM_PM_LNS ; j++ )
        {
            pmap[ i ][ j ] = GH2_NUM_EXT_PORTS; /* Invalid port number */
        }
    }

    init_port_map( );
    find_slot_unit( _tdm );
    GH2_CAL_LEN = find_cal_len( _tdm );

    _tdm->_core_data.vmap_max_len = GH2_CAL_LEN;

    TDM_PRINT0( "tdm_gh2_pmap_transcription:" );
    TDM_PRINT1( "GH2_CAL_LEN %d " , GH2_CAL_LEN );
    TDM_PRINT1( "vmap_max_len %d\n" , _tdm->_core_data.vmap_max_len );

    /*
     * In each GPORT, merge speed of the upper 4 sub-ports to the
     * lower 4 sub-ports, that is, merge speed of GPORTx_4/5/6/7 to
     * GPORTx_0/1/2/3.
     */
    for( phy_pnum = GPORT0_0 ; phy_pnum < GPORT6_0 + 8 ; phy_pnum++ )
    {
        sub_port = ( phy_pnum - GPORT0_0 ) % 8;
        if( sub_port >= 4 )
        {
            _tdm->_chip_data.soc_pkg.speed[ phy_pnum - 4 ] +=
                _tdm->_chip_data.soc_pkg.speed[ phy_pnum ];
            _tdm->_chip_data.soc_pkg.speed[ phy_pnum ] = 0;
        }
    }

    /* Port mapping: set speed of pseudo ports. */
    for( psu_pnum = 0 ; psu_pnum < GH2_NUM_PHY_PORTS ; psu_pnum++ )
    {
        phy_pnum = map_psu2phy[ psu_pnum ];
        psu_speed[ psu_pnum ] =
            ( phy_pnum < 0 ) ? 0 :
                _tdm->_chip_data.soc_pkg.speed[ phy_pnum ];
    }
    /*
     * Before the following for-loop, _tdm->_chip_data.soc_pkg.speed[]
     * stores speed of physical ports.
     */
    for( psu_pnum = 0 ; psu_pnum < GH2_NUM_PHY_PORTS ; psu_pnum++ )
    {
        _tdm->_chip_data.soc_pkg.speed[ psu_pnum ] =
            psu_speed[ psu_pnum ];
    }
    /*
     * From now on, _tdm->_chip_data.soc_pkg.speed[] stores speed of
     * pseudo ports rather than of physical ports.
     */

    TDM_PRINT0( "tdm_gh2_pmap_transcription: " );
    TDM_PRINT0( "speed of pseudo ports\n" );
    for( psu_pnum = 0 ; psu_pnum < GH2_NUM_PHY_PORTS ; psu_pnum++ )
    {
        if( _tdm->_chip_data.soc_pkg.speed[ psu_pnum ] > 0 )
            TDM_PRINT2( "  speed[%d] = %d\n" , psu_pnum ,
                _tdm->_chip_data.soc_pkg.speed[ psu_pnum ] );
    }

    /* Set port macros for GPORTs: */
    pm_idx = 0;
    for( i = 0 ; i <= 6 ; i++ )
    {
        for( j = 0 ; j < 4 ; j++ )
        {
            phy_pnum = GPORT0_0 + ( i * 8 ) + j;
            psu_pnum = map_phy2psu[ phy_pnum ];
            pmap[ pm_idx ][ j ] = psu_pnum;
        }
        pm_idx++;
    }

    /* Set port macros for XLPORTs: */
    for( i = 0 ; i <= 6 ; i++ )
    {
        for( j = 0 ; j < 2 ; j++ )
        {
            phy_pnum = XLPORT0_0 + ( i * 4 ) + j;
            psu_pnum = map_phy2psu[ phy_pnum ];
            pmap[ pm_idx ][ j ] = psu_pnum;
        }
        pm_idx++;
        for( j = 0 ; j < 2 ; j++ )
        {
            phy_pnum = XLPORT0_0 + ( i * 4 ) + j + 2;
            psu_pnum = map_phy2psu[ phy_pnum ];
            pmap[ pm_idx ][ j ] = psu_pnum;
        }
        pm_idx++;
    }

    /* Set port macros for CLPORTs: */
    for( i = 0 ; i <= 0 ; i++ )
    {
        for( j = 0 ; j < 4 ; j++ )
        {
            phy_pnum = CLPORT0_0 + ( i * 4 ) + j;
            psu_pnum = map_phy2psu[ phy_pnum ];
            pmap[ pm_idx ][ j ] = psu_pnum;
        }
        pm_idx++;
    }

    TDM_PRINT0( "tdm_gh2_pmap_transcription: " );
    TDM_PRINT0( "port macros ( of pseudo ports )\n" );
    for( i = 0 ; i < GH2_NUM_PHY_PM ; i++ )
    {
        TDM_PRINT2( "  pmap[%d][0..%d] = " , i , GH2_NUM_PM_LNS - 1 );
        for( j = 0 ; j < GH2_NUM_PM_LNS ; j++ )
            TDM_PRINT1( "%d " , pmap[ i ][ j ] );
        TDM_PRINT0( "\n" );
    }

    for( i = 1 ; i < GH2_NUM_PHY_PORTS ; i++ )
    {
        if( _tdm->_chip_data.soc_pkg.speed[ i ] > SPEED_0 )
            _tdm->_chip_data.soc_pkg.state[ i - 1 ] = PORT_STATE__LINERATE;
        else
            _tdm->_chip_data.soc_pkg.state[ i - 1 ] = PORT_STATE__DISABLED;
    }

    TDM_PRINT0( "tdm_gh2_pmap_transcription:\n" );
    for( i = 0 ; i < GH2_NUM_PHY_PORTS ; i++ )
    {
        TDM_PRINT3( "  pseudo port %d: speed %d state %d\n" , i ,
            _tdm->_chip_data.soc_pkg.speed[ i ] ,
            _tdm->_chip_data.soc_pkg.state[ i ] );
    }

    /* print port config after transcription */
    tdm_print_config( _tdm );

    return ( _tdm->_chip_exec[TDM_CHIP_EXEC__INGRESS_WRAP]( _tdm ) );
}


/**
@name: tdm_gh2_init
@param:
 */
int
tdm_gh2_init( tdm_mod_t *_tdm )
{
    int index;
/*
#ifdef _TDM_DB_STACK
    stack_size = (size_t)&index;
#endif */
    /* TDM_PRINT_STACK_SIZE("tdm_gh2_init"); */

    /* initialize chip/core parameters */
    _tdm->_chip_data.soc_pkg.pmap_num_modules = GH2_NUM_PM_MOD;
    _tdm->_chip_data.soc_pkg.pmap_num_lanes = GH2_NUM_PM_LNS;
    _tdm->_chip_data.soc_pkg.pm_num_phy_modules = GH2_NUM_PHY_PM;

    _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token = GH2_OVSB_TOKEN;
    _tdm->_chip_data.soc_pkg.soc_vars.idl1_token = GH2_IDL1_TOKEN;
    _tdm->_chip_data.soc_pkg.soc_vars.idl2_token = GH2_IDL2_TOKEN;
    _tdm->_chip_data.soc_pkg.soc_vars.ancl_token = GH2_ANCL_TOKEN;
    _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo = 1;
    _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi = GH2_NUM_PHY_PORTS;

    _tdm->_chip_data.cal_0.cal_len = GH2_LR_VBS_LEN;
    _tdm->_chip_data.cal_0.grp_num = GH2_OS_VBS_GRP_NUM;
    _tdm->_chip_data.cal_0.grp_len = GH2_OS_VBS_GRP_LEN;
    _tdm->_chip_data.cal_1.cal_len = GH2_LR_VBS_LEN;
    _tdm->_chip_data.cal_1.grp_num = GH2_OS_VBS_GRP_NUM;
    _tdm->_chip_data.cal_1.grp_len = GH2_OS_VBS_GRP_LEN;
    _tdm->_chip_data.cal_2.cal_len = GH2_LR_VBS_LEN;
    _tdm->_chip_data.cal_2.grp_num = GH2_OS_VBS_GRP_NUM;
    _tdm->_chip_data.cal_2.grp_len = GH2_OS_VBS_GRP_LEN;
    _tdm->_chip_data.cal_3.cal_len = GH2_LR_VBS_LEN;
    _tdm->_chip_data.cal_3.grp_num = GH2_OS_VBS_GRP_NUM;
    _tdm->_chip_data.cal_3.grp_len = GH2_OS_VBS_GRP_LEN;
    _tdm->_chip_data.cal_4.cal_len = GH2_LR_VBS_LEN;
    _tdm->_chip_data.cal_4.grp_num = GH2_OS_VBS_GRP_NUM;
    _tdm->_chip_data.cal_4.grp_len = GH2_OS_VBS_GRP_LEN;
    _tdm->_chip_data.cal_5.cal_len = GH2_LR_VBS_LEN;
    _tdm->_chip_data.cal_5.grp_num = GH2_OS_VBS_GRP_NUM;
    _tdm->_chip_data.cal_5.grp_len = GH2_OS_VBS_GRP_LEN;
    _tdm->_chip_data.cal_6.cal_len = GH2_LR_VBS_LEN;
    _tdm->_chip_data.cal_6.grp_num = GH2_OS_VBS_GRP_NUM;
    _tdm->_chip_data.cal_6.grp_len = GH2_OS_VBS_GRP_LEN;
    _tdm->_chip_data.cal_7.cal_len = GH2_LR_VBS_LEN;
    _tdm->_chip_data.cal_7.grp_num = GH2_OS_VBS_GRP_NUM;
    _tdm->_chip_data.cal_7.grp_len = GH2_OS_VBS_GRP_LEN;

    _tdm->_chip_data.soc_pkg.soc_vars.gh2.higig_mgmt=BOOL_FALSE;
    _tdm->_chip_data.soc_pkg.soc_vars.gh2.cal_hg_en=BOOL_FALSE;
    _tdm->_chip_data.soc_pkg.lr_idx_limit=LEN_850MHZ_EN;
    _tdm->_chip_data.soc_pkg.tvec_size = GH2_ACC_PORT_NUM;
    _tdm->_chip_data.soc_pkg.soc_vars.gh2.pipe_start=1;
    _tdm->_chip_data.soc_pkg.soc_vars.gh2.pipe_end=89;

    _tdm->_core_data.vars_pkg.pipe       = 0;
    _tdm->_core_data.rule__same_port_min = GH2_MIN_SPACING_SAME_PORT;
    _tdm->_core_data.rule__prox_port_min = GH2_MIN_SPACING_SISTER_PORT;
    _tdm->_core_data.vmap_max_wid        = GH2_VMAP_MAX_WID;
    _tdm->_core_data.vmap_max_len        = GH2_VMAP_MAX_LEN;
    _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_1G;

    /* Chip: cal_hg_en */
    if (_tdm->_chip_data.soc_pkg.soc_vars.gh2.cal_universal_en==BOOL_TRUE){
        _tdm->_chip_data.soc_pkg.soc_vars.gh2.cal_hg_en=BOOL_TRUE;
    }
    /* Chip: encap */
    for (index=0; index<GH2_NUM_PM_MOD; index++) {
        _tdm->_chip_data.soc_pkg.soc_vars.gh2.pm_encap_type[index] = (_tdm->_chip_data.soc_pkg.state[index*GH2_NUM_PM_LNS]==PORT_STATE__LINERATE_HG||_tdm->_chip_data.soc_pkg.state[index*GH2_NUM_PM_LNS]==PORT_STATE__OVERSUB_HG)?(PM_ENCAP__HIGIG2):(PM_ENCAP__ETHRNT);
    }
    /* Chip: pmap */
    _tdm->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
    for (index=0; index<(_tdm->_chip_data.soc_pkg.pmap_num_modules); index++) {
        _tdm->_chip_data.soc_pkg.pmap[index]=(int *) TDM_ALLOC((_tdm->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
        TDM_MSET(_tdm->_chip_data.soc_pkg.pmap[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.soc_pkg.pmap_num_lanes);
    }
    /* Chip: IDB Pipe 0 calendar group */
    _tdm->_chip_data.cal_0.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_0.cal_len)*sizeof(int), "TDM inst 0 main calendar");
    TDM_MSET(_tdm->_chip_data.cal_0.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_0.cal_len);
    _tdm->_chip_data.cal_0.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_num)*sizeof(int *), "TDM inst 0 groups");
    for (index=0; index<(_tdm->_chip_data.cal_0.grp_num); index++) {
        _tdm->_chip_data.cal_0.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_len)*sizeof(int), "TDM inst 0 group calendars");
        TDM_MSET(_tdm->_chip_data.cal_0.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_0.grp_len);
    }
    /* Chip: IDB Pipe 1 calendar group */
    _tdm->_chip_data.cal_1.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_1.cal_len)*sizeof(int), "TDM inst 1 main calendar");
    TDM_MSET(_tdm->_chip_data.cal_1.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_1.cal_len);
    _tdm->_chip_data.cal_1.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_1.grp_num)*sizeof(int *), "TDM inst 1 groups");
    for (index=0; index<(_tdm->_chip_data.cal_1.grp_num); index++) {
        _tdm->_chip_data.cal_1.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_1.grp_len)*sizeof(int), "TDM inst 1 group calendars");
        TDM_MSET(_tdm->_chip_data.cal_1.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_1.grp_len);
    }
    /* Chip: IDB Pipe 2 calendar group */
    _tdm->_chip_data.cal_2.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_2.cal_len)*sizeof(int), "TDM inst 2 main calendar");
    TDM_MSET(_tdm->_chip_data.cal_2.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_2.cal_len);
    _tdm->_chip_data.cal_2.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_2.grp_num)*sizeof(int *), "TDM inst 2 groups");
    for (index=0; index<(_tdm->_chip_data.cal_2.grp_num); index++) {
        _tdm->_chip_data.cal_2.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_2.grp_len)*sizeof(int), "TDM inst 2 group calendars");
        TDM_MSET(_tdm->_chip_data.cal_2.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_2.grp_len);
    }
    /* Chip: IDB Pipe 3 calendar group */
    _tdm->_chip_data.cal_3.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_3.cal_len)*sizeof(int), "TDM inst 3 main calendar");
    TDM_MSET(_tdm->_chip_data.cal_3.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_3.cal_len);
    _tdm->_chip_data.cal_3.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_3.grp_num)*sizeof(int *), "TDM inst 3 groups");
    for (index=0; index<(_tdm->_chip_data.cal_3.grp_num); index++) {
        _tdm->_chip_data.cal_3.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_3.grp_len)*sizeof(int), "TDM inst 3 group calendars");
        TDM_MSET(_tdm->_chip_data.cal_3.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_3.grp_len);
    }
    /* Chip: MMU Pipe 0 calendar group */
    _tdm->_chip_data.cal_4.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_4.cal_len)*sizeof(int), "TDM inst 0 main calendar");
    TDM_MSET(_tdm->_chip_data.cal_4.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_4.cal_len);
    _tdm->_chip_data.cal_4.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_4.grp_num)*sizeof(int *), "TDM inst 0 groups");
    for (index=0; index<(_tdm->_chip_data.cal_4.grp_num); index++) {
        _tdm->_chip_data.cal_4.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_4.grp_len)*sizeof(int), "TDM inst 0 group calendars");
        TDM_MSET(_tdm->_chip_data.cal_4.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_4.grp_len);
    }
    /* Chip: MMU Pipe 1 calendar group */
    _tdm->_chip_data.cal_5.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_5.cal_len)*sizeof(int), "TDM inst 1 main calendar");
    TDM_MSET(_tdm->_chip_data.cal_5.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_5.cal_len);
    _tdm->_chip_data.cal_5.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_5.grp_num)*sizeof(int *), "TDM inst 1 groups");
    for (index=0; index<(_tdm->_chip_data.cal_5.grp_num); index++) {
        _tdm->_chip_data.cal_5.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_5.grp_len)*sizeof(int), "TDM inst 1 group calendars");
        TDM_MSET(_tdm->_chip_data.cal_5.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_5.grp_len);
    }
    /* Chip: MMU Pipe 2 calendar group */
    _tdm->_chip_data.cal_6.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_6.cal_len)*sizeof(int), "TDM inst 2 main calendar");
    TDM_MSET(_tdm->_chip_data.cal_6.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_6.cal_len);
    _tdm->_chip_data.cal_6.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_6.grp_num)*sizeof(int *), "TDM inst 2 groups");
    for (index=0; index<(_tdm->_chip_data.cal_6.grp_num); index++) {
        _tdm->_chip_data.cal_6.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_6.grp_len)*sizeof(int), "TDM inst 2 group calendars");
        TDM_MSET(_tdm->_chip_data.cal_6.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_6.grp_len);
    }
    /* Chip: MMU Pipe 3 calendar group */
    _tdm->_chip_data.cal_7.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_7.cal_len)*sizeof(int), "TDM inst 3 main calendar");
    TDM_MSET(_tdm->_chip_data.cal_7.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_7.cal_len);
    _tdm->_chip_data.cal_7.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_7.grp_num)*sizeof(int *), "TDM inst 3 groups");
    for (index=0; index<(_tdm->_chip_data.cal_7.grp_num); index++) {
        _tdm->_chip_data.cal_7.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_7.grp_len)*sizeof(int), "TDM inst 3 group calendars");
        TDM_MSET(_tdm->_chip_data.cal_7.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_7.grp_len);
    }
    /* Core: vmap */
    _tdm->_core_data.vmap=(unsigned short **) TDM_ALLOC((_tdm->_core_data.vmap_max_wid)*sizeof(unsigned short *), "vector_map_l1");
    for (index=0; index<(_tdm->_core_data.vmap_max_wid); index++) {
        _tdm->_core_data.vmap[index]=(unsigned short *) TDM_ALLOC((_tdm->_core_data.vmap_max_len)*sizeof(unsigned short), "vector_map_l2");
    }

    return ( _tdm->_chip_exec[TDM_CHIP_EXEC__TRANSCRIPTION]( _tdm ) );
}


/*--------------------------------------------------------------------*/
/* One instance of tdm_gh2_port_t represents one physical port.       */
/*--------------------------------------------------------------------*/
typedef struct  tdm_gh2_port_s
{
    char                    name[ 64 ];

    int                     phy_pnum;
                            /* Physical port number. */
    int                     sub_pnum;
                            /* Sub-port number. */
    int                     enable;

    int                     speed;
                            /* Port speed, in MBit/sec. */

    int                     dat_cyc;
                            /*
                             * Number of clock cycles that the design
                             * needs to handle one packet cell of this
                             * port, before the next cell of this port
                             * comes. In GH2, PORT requires 4 clock
                             * cycles to send packet data of one cell
                             * to Iarb, while MMU requires 5 clock
                             * cycles to process each cell. So dat_cyc
                             * should be 5 in GH2. This number is also
                             * known as the 'minimum port latency'.
                             */

    int                     min_dist;
                            /*
                             * Minimum distance from this port to its
                             * conflicting ports.
                             */

    int                     num_cnfl_port;
    int*                    cnfl_ports;
                            /*
                             * cnfl_ports[] stores physical port
                             * number of all the conflicting ports,
                             * while num_cnfl_port is length of
                             * cnfl_ports[]. Conflicting ports are
                             * thoes sharing the same physical
                             * interface as this port, and are also
                             * called 'sister ports'. Note that every
                             * port conflicts to itself.
                             */
    int                     num_slot;
                            /* count of this port in TDM table */

    int                     chk_pass;
}   tdm_gh2_port_t;

tdm_gh2_port_t  chip_ports  [ GH2_NUM_PHY_PORTS ];

int     chk_tdm_tab_conflict
(
    int*            tdm_tab ,
    int             len_tdm_tab ,
    int             idx
)
{
    /*
     * ----------------------------------------------------------------
     * This function checks whether tdm_tab[idx] conflicts with its
     * nearby slots or not. If tdm_tab[idx] is occupied by physical
     * port-P, this function checks that the following two rules are
     * not violated:
     *
     * (r1) Let p denote chip_ports[P], and T denote tdm_tab. If
     *      p.dat_cyc = D, for slot
     * 
     *         T[idx] = P
     * 
     *     P must NOT be in anyone of the following slots:
     * 
     *         T[idx-D+1] ... T[idx-1] T[idx+1] ... T[idx+D-1]
     * 
     * (r2) If p.min_dist = M, for slot
     *     
     *         T[idx] = P
     * 
     *     conflicting ports of port-P ( the ports sharing the same
     *     interface with port-P ) must NOT be in anyone of the
     *     following slots:
     *
     *          T[idx-M+1] ... T[idx-1] T[idx+1] ... T[idx+M-1]
     * ----------------------------------------------------------------
     */
    int phy_pnum , dat_cyc , min_dist , near_pnum , found;
    int i , j , k;
    int verbose = 2;

    phy_pnum = tdm_tab[ idx ];
    dat_cyc = chip_ports[ phy_pnum ].dat_cyc;
    min_dist = chip_ports[ phy_pnum ].min_dist;
        
    for( j = -dat_cyc + 1 ; j < dat_cyc ; j++ )
    {
        if( j == 0 )
            continue; /* skip itself */

        i = idx + j;
        if( i < 0 )
        {
            while( i < 0 )
            {
                i += len_tdm_tab;
            }
        }
        if( i >= len_tdm_tab )
        {
            while( i >= len_tdm_tab )
            {
                i -= len_tdm_tab;
            }
        }
        near_pnum = tdm_tab[ i ];

        if( verbose >= 2 )
        {
            TDM_PRINT0( "chk_tdm_tab_conflict: check(1) " );
            TDM_PRINT2( "tdm_tab[%d] %d vs. " , idx , phy_pnum );
            TDM_PRINT2( "tdm_tab[%d] %d\n" , i , near_pnum );
        }

        if( phy_pnum == near_pnum )
        {
            if( verbose >= 1 )
            {
                TDM_PRINT0( "chk_tdm_tab_conflict failed because " );
                TDM_PRINT1( "tdm_tab[%d] " , idx );
                TDM_PRINT1( "port-%d conflict(1) to " , phy_pnum );
                TDM_PRINT2( "tdm_tab[%d] port-%d\n" , i , near_pnum );
            }
            return FAIL; /* conflict */
        }
    }
    for( j = -min_dist + 1 ; j < min_dist ; j++ )
    {
        if( j == 0 )
            continue; /* skip itself */

        i = idx + j;
        if( i < 0 )
        {
            while( i < 0 )
            {
                i += len_tdm_tab;
            }
        }
        if( i >= len_tdm_tab )
        {
            while( i >= len_tdm_tab )
            {
                i -= len_tdm_tab;
            }
        }
        near_pnum = tdm_tab[ i ];

        if( verbose >= 2 )
        {
            TDM_PRINT0( "chk_tdm_tab_conflict: check(2) " );
            TDM_PRINT2( "tdm_tab[%d] %d vs. " , idx , phy_pnum );
            TDM_PRINT2( "tdm_tab[%d] %d\n" , i , near_pnum );
        }

        found = 0;
        for( k = 0 ; k < chip_ports[ phy_pnum ].num_cnfl_port ; k++ )
        {
            if( near_pnum == chip_ports[ phy_pnum ].cnfl_ports[ k ] )
                found = 1;
        }
        if( found )
        {
            if( verbose )
            {
                TDM_PRINT0( "chk_tdm_tab_conflict failed because " );
                TDM_PRINT1( "tdm_tab[%d] " , idx );
                TDM_PRINT1( "port-%d conflict(2) to " , phy_pnum );
                TDM_PRINT2( "tdm_tab[%d] port-%d\n" , i , near_pnum );
            }
            return FAIL; /* conflict */
        }
    }
   
    return PASS; /* no conflict */
}

#if NO_FLOATING
void    cal_port_bw
(
    int             phy_pnum ,
    int             clk_freq ,    /* core clock frequency in KHz */
    int             len_tdm_tab , /* TDM table size */
    int             pkt_sz ,      /* packet size in bytes */
    int             cell_sz ,     /* cell size in bytes */
    int*            req_bw ,      /* required bandwidth in KHz */
    int*            eff_bw        /* effective bandwidth in KHz */
)
{
    /* req_bw = Total bandwidth required for the interface( Gbps ) */
    /* eff_bw = Effective bandwidth allocated to the interface( Gbps ) */
    int ifg = 12; /* Inter Frame Gap ( bytes ) */
    int preamble = 8; /* Preamble ( bytes ) */
    int pkt_sz_r , cell_sz_r , num_cyc , speed , num_slot;
    int v1 , v2 , v3;
    int verbose = 1;

    pkt_sz_r = pkt_sz;
    cell_sz_r = cell_sz;
    speed = chip_ports[ phy_pnum ].speed;
    num_slot = chip_ports[ phy_pnum ].num_slot;

    if( ( pkt_sz % cell_sz ) == 0 )
        num_cyc = pkt_sz / cell_sz;
    else
        num_cyc = ( pkt_sz / cell_sz ) + 1;

    v1 = pkt_sz_r + ifg + preamble;
    v2 = ( (pkt_sz_r * 1000) / v1 ) * ( speed / 1000 );
    v3 = (pkt_sz_r * 1000) / ( cell_sz_r * num_cyc );
    *req_bw = (v2 * 1000) / v3;

    v1 = ( clk_freq * cell_sz_r ) / 125;
    v2 = len_tdm_tab;
    v3 = num_slot;
    *eff_bw = ( v1 * v3 ) / v2;

    if( verbose >= 1 )
    {
        TDM_PRINT1( "cal_port_bw: port-%d speed " , phy_pnum );
        TDM_PRINT2( "%d num_slot %d\n  " , speed , num_slot );
        TDM_PRINT1( "clk_freq %d KHz " , clk_freq );
        TDM_PRINT1( "len_tdm_tab %d " , len_tdm_tab );
        TDM_PRINT2( "pkt_sz %d cell_sz %d\n    " , pkt_sz , cell_sz );
        TDM_PRINT1( "effective bw %dKbps\n    " , *eff_bw );
        TDM_PRINT1( "required  bw %dKbps\n" , *req_bw );
    }
}

int     chk_port_bw
(
    int             phy_pnum ,
    int             clk_freq ,  /* core clock frequency in KHz */
    int             len_tdm_tab /* TDM table size */
)
{
    int req_bw_64 = 0 , req_bw_145 = 0;
    int eff_bw_64 = 0 , eff_bw_145 = 0;
    int pass_64 , pass_145;

    if( chip_ports[ phy_pnum ].enable == 0 )
    {
        return PASS;
    }

    cal_port_bw( phy_pnum ,
        clk_freq ,     /* core clock frequency in MHz */
        len_tdm_tab ,  /* TDM table size */
        64 ,           /* packet size in bytes */
        144 ,          /* cell size in bytes */
        &req_bw_64 ,   /* required bandwidth */
        &eff_bw_64 );  /* effective bandwidth */
    pass_64 = ( eff_bw_64 >= req_bw_64 );

    cal_port_bw( phy_pnum ,
        clk_freq ,    /* core clock frequency in MHz */
        len_tdm_tab , /* TDM table size */
        145 ,         /* packet size in bytes */
        144 ,         /* cell size in bytes */
        &req_bw_145 ,  /* required bandwidth */
        &eff_bw_145 ); /* effective bandwidth */
    pass_145 = ( eff_bw_145 >= req_bw_145 );

    if( pass_64 && pass_145 )
    {
        return PASS;
    }
    else
    {
        TDM_PRINT0( "chk_port_bw: bandwidth check failed at " );
        TDM_PRINT1( "port-%d\n" , phy_pnum );
        return FAIL;
    }
}

int       cal_req_freq_port
(
    int*            tdm_tab ,
    int             len_tdm_tab ,
    int             phy_pnum ,
    int*            freq_req , /* output, in KHz */
    int             verbose
)
{
    int num_slot , num_dist , i , s1 , s2 , dist;
    int arr_slot [ 512 ];
    int arr_dist [ 512 ];
    int speed , max_dist , avg_dist , freq1 , freq2;

    *freq_req = 0;
    if( chip_ports[ phy_pnum ].enable == 0 )
        return PASS;

    num_slot = 0;
    for( i = 0 ; i < len_tdm_tab ; i++ )
    {
        if( tdm_tab[ i ] == phy_pnum )
            arr_slot[ num_slot++ ] = i;
    }

    if( num_slot != chip_ports[ phy_pnum ].num_slot )
    {
        TDM_PRINT0( "ERROR cal_req_freq_port: Internal " );
        TDM_PRINT0( "error, slot count mismatch\n" );
        TDM_PRINT1( "  port-%d should have " , phy_pnum );
        TDM_PRINT1( "%d " , chip_ports[ phy_pnum ].num_slot );
        TDM_PRINT0( "slot in TDM table, " );
        TDM_PRINT1( "but it has %d\n" , num_slot );
        return FAIL;
    }

    TDM_PRINT0( "cal_req_freq_port: calculate required frequency " );
    TDM_PRINT1( "for port-%d\n" , phy_pnum );

    num_dist = 0;
    max_dist = 0;
    avg_dist = 0;
    for( i = 0 ; i < num_slot ; i++ )
    {
        s1 = arr_slot[ i ];
        s2 = ( i + 1 == num_slot ) ?
                arr_slot[ 0 ] : arr_slot[ i + 1 ];

        dist = ( s2 > s1 ) ?
                   ( s2 - s1 ) : ( len_tdm_tab - s1 + s2 );
        arr_dist[ num_dist++ ] = dist;

        if( max_dist < dist )
            max_dist = dist;
        avg_dist += dist;

        if( verbose >= 2 )
        {
            TDM_PRINT2( "slot %d-%d " , s1 , s2 );
            TDM_PRINT1( "dist %d\n" , dist );
        }
    }
    if( num_dist == 0 )
    {
        /* This port has no slots, and so no required frequency */
        return PASS;
    }
    avg_dist = (avg_dist * 1000) / num_dist;

    speed = chip_ports[ phy_pnum ].speed;
    freq1 = (avg_dist * speed) / 660; /* 660 = 8 * 82.5*/
    freq2 = ((max_dist * 1000) * speed) / 800; /* 800 = 8 * 100.0 */

    if( freq1 > freq2 )
        *freq_req = freq1; /* KHz to MHz */
    else
        *freq_req = freq2; /* KHz to MHz */

    if( verbose >= 1 )
    {
        TDM_PRINT1( "port-%d has\n  " , phy_pnum );
        TDM_PRINT1( "avg_dist %d " , avg_dist );
        TDM_PRINT1( "freq1 %dMHz\n  " , freq1 );
        TDM_PRINT1( "max_dist %d " , max_dist );
        TDM_PRINT1( "freq2 %dMHz\n  " , freq2 );
        TDM_PRINT0( "  dist: " );
        for( i = 0 ; i < num_dist ; i++ )
            TDM_PRINT1( "%d " , arr_dist[ i ] );
        TDM_PRINT0( "\n" );
    }
    return PASS;
}

int       cal_req_freq
(
    int*            tdm_tab ,
    int             len_tdm_tab ,
    int*            max_freq , /* output, in KHz */
    int             verbose
)
{
    int port_freq;
    int i , result;

    *max_freq = 0;
    for( i = 0 ; i < GH2_NUM_PHY_PORTS ; i++ )
    {
        result = cal_req_freq_port( tdm_tab ,
                        len_tdm_tab , i , &port_freq , verbose );
        if( result == FAIL )
            return FAIL;

        if( *max_freq < port_freq )
            *max_freq = port_freq;
    }
    *max_freq += 100;

    if( verbose >= 1 )
    {
        TDM_PRINT1( "cal_req_freq: max_freq %d KHz\n" , *max_freq );
    }
    return PASS;
}
#else
void    cal_port_bw
(
    int             phy_pnum ,
    double          clk_freq ,    /* core clock frequency in MHz */
    int             len_tdm_tab , /* TDM table size */
    int             pkt_sz ,      /* packet size in bytes */
    int             cell_sz ,     /* cell size in bytes */
    double*         req_bw ,      /* required bandwidth */
    double*         eff_bw        /* effective bandwidth */
)
{
    /* req_bw = Total bandwidth required for the interface( Gbps ) */
    /* eff_bw = Effective bandwidth allocated to the interface( Gbps ) */

    double ifg = 12; /* Inter Frame Gap ( bytes ) */
    double preamble = 8; /* Preamble ( bytes ) */
    double pkt_sz_r , cell_sz_r , num_cyc , speed , num_slot;
    double v1 , v2 , v3;
    int verbose = 1;

    pkt_sz_r = pkt_sz;
    cell_sz_r = cell_sz;
    speed = chip_ports[ phy_pnum ].speed;
    num_slot = chip_ports[ phy_pnum ].num_slot;

    if( ( pkt_sz % cell_sz ) == 0 )
        num_cyc = pkt_sz / cell_sz;
    else
        num_cyc = ( pkt_sz / cell_sz ) + 1;
    v1 = pkt_sz_r + ifg + preamble;
    v2 = ( pkt_sz_r / v1 ) * ( speed / 1000 );
    v3 = pkt_sz_r / ( cell_sz_r * num_cyc );
    *req_bw = v2 / v3;

    v1 = clk_freq * ( cell_sz_r * 8.0 ) / 1000.0;
    v2 = len_tdm_tab;
    v3 = num_slot;
    *eff_bw = ( v1 / v2 ) * v3;

    if( verbose >= 1 )
    {
        TDM_PRINT1( "cal_port_bw: port-%d speed " , phy_pnum );
        TDM_PRINT2( "%.0f num_slot %.0f\n  " , speed , num_slot );
        TDM_PRINT1( "clk_freq %f " , clk_freq );
        TDM_PRINT1( "len_tdm_tab %d " , len_tdm_tab );
        TDM_PRINT2( "pkt_sz %d cell_sz %d\n    " , pkt_sz , cell_sz );
        TDM_PRINT1( "effective bw %fGbps\n    " , *eff_bw );
        TDM_PRINT1( "required  bw %fGbps\n" , *req_bw );
    }
}

int     chk_port_bw
(
    int             phy_pnum ,
    double          clk_freq ,  /* core clock frequency in MHz */
    int             len_tdm_tab /* TDM table size */
)
{
    double req_bw_64 , req_bw_145;
    double eff_bw_64 , eff_bw_145;
    int pass_64 , pass_145;

    if( chip_ports[ phy_pnum ].enable == 0 )
    {
        return PASS;
    }

    cal_port_bw( phy_pnum ,
        clk_freq ,     /* core clock frequency in MHz */
        len_tdm_tab ,  /* TDM table size */
        64 ,           /* packet size in bytes */
        144 ,          /* cell size in bytes */
        &req_bw_64 ,   /* required bandwidth */
        &eff_bw_64 );  /* effective bandwidth */
    pass_64 = ( eff_bw_64 >= req_bw_64 );

    cal_port_bw( phy_pnum ,
        clk_freq ,    /* core clock frequency in MHz */
        len_tdm_tab , /* TDM table size */
        145 ,         /* packet size in bytes */
        144 ,         /* cell size in bytes */
        &req_bw_145 ,  /* required bandwidth */
        &eff_bw_145 ); /* effective bandwidth */
    pass_145 = ( eff_bw_145 >= req_bw_145 );

    if( pass_64 && pass_145 )
    {
        return PASS;
    }
    else
    {
        TDM_PRINT0( "chk_port_bw: bandwidth check failed at " );
        TDM_PRINT1( "port-%d\n" , phy_pnum );
        return FAIL;
    }
}

int       cal_req_freq_port
(
    int*            tdm_tab ,
    int             len_tdm_tab ,
    int             phy_pnum ,
    double*         freq_req , /* output */
    int             verbose
)
{
    int num_slot , num_dist , i , s1 , s2 , dist;
    int arr_slot [ 512 ];
    int arr_dist [ 512 ];
    double speed , max_dist , avg_dist , freq1 , freq2;

    *freq_req = 0;
    if( chip_ports[ phy_pnum ].enable == 0 )
        return PASS;

    num_slot = 0;
    for( i = 0 ; i < len_tdm_tab ; i++ )
    {
        if( tdm_tab[ i ] == phy_pnum )
            arr_slot[ num_slot++ ] = i;
    }

    if( num_slot != chip_ports[ phy_pnum ].num_slot )
    {
        TDM_PRINT0( "ERROR cal_req_freq_port: Internal " );
        TDM_PRINT0( "error, slot count mismatch\n" );
        TDM_PRINT1( "  port-%d should have " , phy_pnum );
        TDM_PRINT1( "%d " , chip_ports[ phy_pnum ].num_slot );
        TDM_PRINT0( "slot in TDM table, " );
        TDM_PRINT1( "but it has %d\n" , num_slot );
        return FAIL;
    }

    TDM_PRINT0( "cal_req_freq_port: calculate required frequency " );
    TDM_PRINT1( "for port-%d\n" , phy_pnum );

    num_dist = 0;
    max_dist = 0;
    avg_dist = 0;
    for( i = 0 ; i < num_slot ; i++ )
    {
        s1 = arr_slot[ i ];
        s2 = ( i + 1 == num_slot ) ?
                arr_slot[ 0 ] : arr_slot[ i + 1 ];

        dist = ( s2 > s1 ) ?
                   ( s2 - s1 ) : ( len_tdm_tab - s1 + s2 );
        arr_dist[ num_dist++ ] = dist;

        if( max_dist < dist )
            max_dist = dist;
        avg_dist += dist;

        if( verbose >= 2 )
        {
            TDM_PRINT2( "slot %d-%d " , s1 , s2 );
            TDM_PRINT1( "dist %d\n" , dist );
        }
    }
    if( num_dist == 0 )
    {
        /* This port has no slots, and so no required frequency */
        return PASS;
    }
    avg_dist = avg_dist / num_dist;

    speed = chip_ports[ phy_pnum ].speed;
    freq1 = avg_dist * speed / ( 8 * 82.5 );
    freq2 = max_dist * speed / ( 8 * 100.0 );

    if( freq1 > freq2 )
        *freq_req = freq1;
    else
        *freq_req = freq2;

    if( verbose >= 1 )
    {
        TDM_PRINT1( "port-%d has\n  " , phy_pnum );
        TDM_PRINT1( "avg_dist %.3f " , avg_dist );
        TDM_PRINT1( "freq1 %.3fMHz\n  " , freq1 );
        TDM_PRINT1( "max_dist %.1f " , max_dist );
        TDM_PRINT1( "freq2 %.3fMHz\n  " , freq2 );
        TDM_PRINT0( "  dist: " );
        for( i = 0 ; i < num_dist ; i++ )
            TDM_PRINT1( "%d " , arr_dist[ i ] );
        TDM_PRINT0( "\n" );
    }
    return PASS;
}

int       cal_req_freq
(
    int*            tdm_tab ,
    int             len_tdm_tab ,
    double*         max_freq , /* output */
    int             verbose
)
{
    double port_freq;
    int i , result;

    *max_freq = 0;
    for( i = 0 ; i < GH2_NUM_PHY_PORTS ; i++ )
    {
        result = cal_req_freq_port( tdm_tab ,
                        len_tdm_tab , i , &port_freq , verbose );
        if( result == FAIL )
            return FAIL;
        
        if( *max_freq < port_freq )
            *max_freq = port_freq;
    }
    *max_freq += 0.1;

    if( verbose >= 1 )
    {
        TDM_PRINT1( "cal_req_freq: max_freq %.2f\n" , *max_freq );
    }
    return PASS;
}
#endif

/**
@name: tdm_gh2_post
@param:
 */
int
tdm_gh2_post( tdm_mod_t *_tdm )
{
    int i , j , k , m , phy_pnum , psu_pnum , result;
    int vmap_wid , tdm_len , empty_token , cpu_port_cnt;
    int port_lo , port_hi;
    unsigned short** vmap;
    int* tdm_tab;
#ifdef  _TDM_STANDALONE
    FILE* fp_o;
#endif    
    char port_name[ 64 ];
#if NO_FLOATING
    int req_freq; /* KHz */
#else
    double req_freq;
#endif

    /* TDM_PRINT_STACK_SIZE("tdm_gh2_post"); */
    TDM_PRINT0( "tdm_gh2_post: start\n" );

    vmap = _tdm->_core_data.vmap;
    vmap_wid = tdm_vmap_get_vmap_wid( _tdm , vmap );
    tdm_len = tdm_vmap_get_vmap_len( _tdm , vmap );
    empty_token = _tdm->_chip_data.soc_pkg.num_ext_ports;

    /* tdm_len is length of TDM table ( calendar ), while tdm_tab[]
     * stores the TDM table. */
    tdm_tab = TDM_ALLOC( sizeof( int ) * tdm_len , "TDM-table" );
    for( j = 0 ; j < tdm_len ; j++ )
    {
        tdm_tab[ j ] = -1; /* idle cycle */
        for( i = 0 ; i < vmap_wid ; i++ )
        {
            if( vmap[ i ][ j ] != empty_token )
            {
                tdm_tab[ j ] = vmap[ i ][ j ];
            }
        }
    }

    TDM_PRINT0( "TDM table ( before post-process ):\n" );
    for( j = 0 ; j < tdm_len ; j++ )
    {
        if( j % 16 == 0 )
            TDM_PRINT0( "    " );
        TDM_PRINT1( "%02d " , tdm_tab[ j ] );
        if( ( j % 16 == 15 ) || ( j == tdm_len - 1 ) )
            TDM_PRINT0( "\n" );
    }

    /* Map pseudo ports in the TDM table back to physical ports. */
    for( i = 0 ; i < tdm_len ; i++ )
    {
        psu_pnum = tdm_tab[ i ];
        if( psu_pnum >= 0 )
        {
            phy_pnum = map_psu2phy[ psu_pnum ];
            tdm_tab[ i ] = phy_pnum;
        }
    }

    /* Post-process: re-assign the slots of GPORTs. */
    for( i = 0 ; i <= 6 ; i++ )
    {
        for( j = 0 ; j <= 3 ; j++ )
        {
            port_lo = GPORT0_0 + ( i * 8 ) + j;
            port_hi = port_lo + 4;
            if( ( port_speed[ port_lo ] > 0 ) &&
                ( port_speed[ port_lo ] == port_speed[ port_hi ] ) )
            {
                TDM_PRINT0( "post-process: re-assign " );
                TDM_PRINT1( "slots of port %d\n" , port_lo );

                m = 0;
                for( k = 0 ; k < tdm_len ; k++ )
                {
                    if( tdm_tab[ k ] == port_lo )
                    {
                        if( m == 1 )
                        {
                            tdm_tab[ k ] = port_hi;
                            TDM_PRINT0( "  re-assign " );
                            TDM_PRINT1( "tdm_tab[%d] " , k );
                            TDM_PRINT1( "to %d\n" , tdm_tab[ k ] );
                        }
                        m = !m;
                    }
                }
            }
            else if( ( port_speed[ port_lo ] == 0 ) &&
                     ( port_speed[ port_hi ] > 0 ) )
            {
                TDM_PRINT0( "post-process: re-assign " );
                TDM_PRINT1( "slots of port %d\n" , port_lo );

                for( k = 0 ; k < tdm_len ; k++ )
                {
                    if( tdm_tab[ k ] == port_lo )
                    {
                        tdm_tab[ k ] = port_hi;
                        TDM_PRINT1( "  re-assign tdm_tab[%d] " , k );
                        TDM_PRINT1( "to %d\n" , tdm_tab[ k ] );
                    }
                }
            }
        }
    }

    /* Post-process ( insert CPU port ): assign idle cycles to port 0. */
    cpu_port_cnt = 0;
    for( j = 0 ; j < tdm_len ; j++ )
    {
        if( tdm_tab[ j ] < 0 )
        {
            /* This is an idle cycle, assign it to port-0. */
            tdm_tab[ j ] = 0;
            cpu_port_cnt++;
            break;
        }
    }
    if( cpu_port_cnt == 0 )
    {
        /*
         * The above for-loop doesn't assign any slot to CPU port, so
         * here append one slot of CPU port to the TDM table.
         */
        if( tdm_len >= GH2_LR_VBS_LEN )
        {
            TDM_PRINT0( "*ERROR* tdm_gh2_post() failed\n  " );
            TDM_PRINT0( "tdm_gh2_post() cannot append a slot of " );
            TDM_PRINT0( "CPU port, because TDM table size" );
            TDM_PRINT1( "( %d )\n  " , tdm_len );
            TDM_PRINT0( "is already at the maximumle" );
            TDM_PRINT1( "( %d )\n" , GH2_LR_VBS_LEN );
            TDM_FREE( tdm_tab );
            return FAIL;
        }
        tdm_tab[ tdm_len ] = 0;
        tdm_len++;
    }

    TDM_PRINT0( "TDM table ( after post-process ):\n" );
    for( j = 0 ; j < tdm_len ; j++ )
    {
        if( j % 16 == 0 )
            TDM_PRINT0( "    " );
        TDM_PRINT1( "%02d " , tdm_tab[ j ] );
        if( ( j % 16 == 15 ) || ( j == tdm_len - 1 ) )
            TDM_PRINT0( "\n" );
    }

#ifdef  _TDM_STANDALONE
    fp_o = fopen( "out.man" , "wt" );
    if( fp_o == NULL )
    {
        TDM_PRINT0( "*ERROR* tdm_gh2_post() failed\n  " );
        TDM_PRINT0( "tdm_gh2_post() cannot open file 'out.man'\n" );
        TDM_FREE( tdm_tab );
        return FAIL;
    }
    fprintf( fp_o , "clk 583.4MHz\n" );
    fprintf( fp_o , "tdm_tab " );
    for( j = 0 ; j < tdm_len ; j++ )
    {
        if( tdm_tab[ j ] >= 0 )
            fprintf( fp_o , "%d" , tdm_tab[ j ] );
        else
            fprintf( fp_o , "127" ); /* Invalid port of GH2 */
        if( j < tdm_len - 1 )
            fprintf( fp_o , "," );
    }
    fprintf( fp_o , "\ntdm_tsz %d\n" , tdm_len );
    fclose( fp_o );
#endif /* _TDM_STANDALONE */

    TDM_PRINT0( "tdm_gh2_post: change cal_0.cal_len " );
    TDM_PRINT1( "%d -> " , _tdm->_chip_data.cal_0.cal_len );
    TDM_PRINT1( "%d\n" , tdm_len );
    _tdm->_chip_data.cal_0.cal_len = tdm_len;
    for( j = 0 ; j < tdm_len ; j++ )
    {
        _tdm->_chip_data.cal_0.cal_main[ j ] = tdm_tab[ j ];
    }

    for( i = 0 ; i < GH2_NUM_PHY_PORTS ; i++ )
    {
        chip_ports[ i ].name[ 0 ] = '\0';
        chip_ports[ i ].phy_pnum = i;
        chip_ports[ i ].sub_pnum = 0;
        chip_ports[ i ].enable = 0;
        chip_ports[ i ].speed = 0;
        chip_ports[ i ].dat_cyc = 0;
        chip_ports[ i ].min_dist = 0;
        chip_ports[ i ].num_cnfl_port = 0;
        chip_ports[ i ].cnfl_ports = NULL;
        chip_ports[ i ].num_slot = 0;
        chip_ports[ i ].chk_pass = 0;
    }

    /* Set properties of CPU port into chip_ports[]: */
    strcpy( chip_ports[ 0 ].name , "CPU" );
    chip_ports[ 0 ].phy_pnum = 0;
    chip_ports[ 0 ].sub_pnum = 0;
    chip_ports[ 0 ].enable = 1;
    chip_ports[ 0 ].speed = port_speed[ 0 ];
    chip_ports[ 0 ].dat_cyc = 5;
    chip_ports[ 0 ].min_dist = 4;
    chip_ports[ 0 ].num_cnfl_port = 1;
    chip_ports[ 0 ].cnfl_ports =
        TDM_ALLOC( sizeof( int ) * 1 , "cnfl_ports" );
    chip_ports[ 0 ].cnfl_ports[ 0 ] = 0;

    /* Set properties of GPORTs into chip_ports[]: */
    for( i = 0 ; i <= 6 ; i++ )
    {
        for( j = 0 ; j < 8 ; j++ )
        {
            phy_pnum = GPORT0_0 + ( i * 8 ) + j;

            sal_sprintf( port_name , "GPORT%d_%d" , i , j );
            sal_strcpy( chip_ports[ phy_pnum ].name , port_name );
            chip_ports[ phy_pnum ].phy_pnum = phy_pnum;
            chip_ports[ phy_pnum ].sub_pnum = j;
            chip_ports[ phy_pnum ].enable =
                ( port_speed[ phy_pnum ] > 0 );
            chip_ports[ phy_pnum ].speed = port_speed[ phy_pnum ];
            chip_ports[ phy_pnum ].dat_cyc = 5;
            chip_ports[ phy_pnum ].min_dist = 4;
            chip_ports[ phy_pnum ].num_cnfl_port = 8;
            chip_ports[ phy_pnum ].cnfl_ports =
                TDM_ALLOC( sizeof( int ) * 8 , "cnfl_ports" );
            for( k = 0 ; k < 8 ; k++ )
                chip_ports[ phy_pnum ].cnfl_ports[ k ] =
                    GPORT0_0 + ( i * 8 ) + k;
        }
    }

    /* Set properties of XLPORTs into chip_ports[]: */
    for( i = 0 ; i <= 6 ; i++ )
    {
        for( j = 0 ; j < 2 ; j++ )
        {
            phy_pnum = XLPORT0_0 + ( i * 4 ) + j;

            sal_sprintf( port_name , "XLPORT%d_%d" , i , j );
            sal_strcpy( chip_ports[ phy_pnum ].name , port_name );
            chip_ports[ phy_pnum ].phy_pnum = phy_pnum;
            chip_ports[ phy_pnum ].sub_pnum = j;
            chip_ports[ phy_pnum ].enable =
                ( port_speed[ phy_pnum ] > 0 );
            chip_ports[ phy_pnum ].speed = port_speed[ phy_pnum ];
            chip_ports[ phy_pnum ].dat_cyc = 5;
            chip_ports[ phy_pnum ].min_dist = 4;
            chip_ports[ phy_pnum ].num_cnfl_port = 2;
            chip_ports[ phy_pnum ].cnfl_ports =
                TDM_ALLOC( sizeof( int ) * 2 , "cnfl_ports" );
            for( k = 0 ; k < 2 ; k++ )
                chip_ports[ phy_pnum ].cnfl_ports[ k ] =
                    XLPORT0_0 + ( i * 4 ) + k;
        }
        for( j = 2 ; j < 4 ; j++ )
        {
            phy_pnum = XLPORT0_0 + ( i * 4 ) + j;

            sal_sprintf( port_name , "XLPORT%d_%d" , i , j );
            sal_strcpy( chip_ports[ phy_pnum ].name , port_name );
            chip_ports[ phy_pnum ].phy_pnum = phy_pnum;
            chip_ports[ phy_pnum ].sub_pnum = j;
            chip_ports[ phy_pnum ].enable =
                ( port_speed[ phy_pnum ] > 0 );
            chip_ports[ phy_pnum ].speed = port_speed[ phy_pnum ];
            chip_ports[ phy_pnum ].dat_cyc = 5;
            chip_ports[ phy_pnum ].min_dist = 4;
            chip_ports[ phy_pnum ].num_cnfl_port = 2;
            chip_ports[ phy_pnum ].cnfl_ports =
                TDM_ALLOC( sizeof( int ) * 2 , "cnfl_ports" );
            for( k = 0 ; k < 2 ; k++ )
                chip_ports[ phy_pnum ].cnfl_ports[ k ] =
                    XLPORT0_0 + ( i * 4 ) + k + 2;
        }
    }

    /* Set properties of CLPORTs into chip_ports[]: */
    for( i = 0 ; i <= 0 ; i++ )
    {
        for( j = 0 ; j < 4 ; j++ )
        {
            phy_pnum = CLPORT0_0 + ( i * 4 ) + j;

            sal_sprintf( port_name , "CLPORT%d_%d" , i , j );
            sal_strcpy( chip_ports[ phy_pnum ].name , port_name );
            chip_ports[ phy_pnum ].phy_pnum = phy_pnum;
            chip_ports[ phy_pnum ].sub_pnum = j;
            chip_ports[ phy_pnum ].enable =
                ( port_speed[ phy_pnum ] > 0 );
            chip_ports[ phy_pnum ].speed = port_speed[ phy_pnum ];
            chip_ports[ phy_pnum ].dat_cyc = 5;
            chip_ports[ phy_pnum ].min_dist = 2;
            chip_ports[ phy_pnum ].num_cnfl_port = 4;
            chip_ports[ phy_pnum ].cnfl_ports =
                TDM_ALLOC( sizeof( int ) * 4 , "cnfl_ports" );
            for( k = 0 ; k < 4 ; k++ )
                chip_ports[ phy_pnum ].cnfl_ports[ k ] =
                    CLPORT0_0 + ( i * 4 ) + k;
        }
    }

    for( i = 0 ; i < tdm_len ; i++ )
    {
        if( tdm_tab[ i ] >= 0 )
        {
            phy_pnum = tdm_tab[ i ];
            chip_ports[ phy_pnum ].num_slot++;
        }
    }
    
    for( i = 0 ; i < GH2_NUM_PHY_PORTS ; i++ )
    {
        if( chip_ports[ i ].enable )
        {
            TDM_PRINT1( "DEBUG-TDM: port-%d " , i );
            TDM_PRINT1( "%s " , chip_ports[ i ].name );
            TDM_PRINT1( "speed %d " , chip_ports[ i ].speed );
            TDM_PRINT1( "dat_cyc %d " , chip_ports[ i ].dat_cyc );
            TDM_PRINT1( "min_dist %d " , chip_ports[ i ].min_dist );
            TDM_PRINT0( "cnfl_ports: " );
            for( j = 0 ; j < chip_ports[ i ].num_cnfl_port ; j++ )
                TDM_PRINT1( "%d " , chip_ports[ i ].cnfl_ports[ j ] );
            TDM_PRINT0( "\n" );
        }
    }

    for( i = 0 ; i < tdm_len ; i++ )
    {
        result = 
            chk_tdm_tab_conflict( tdm_tab , tdm_len , i );
        if( result != PASS )
        {
            TDM_PRINT0( "tdm_gh2_post: check failed(1)\n" );
            TDM_FREE( tdm_tab );
            return FAIL;
        }
    }

    /* req_freq is in KHz */
    result = cal_req_freq( tdm_tab , tdm_len , &req_freq , 2 );
    if( result != PASS )
    {
        TDM_PRINT0( "tdm_gh2_post: check failed(2)\n" );
        TDM_FREE( tdm_tab );
        return FAIL;
    }
#if NO_FLOATING
    if (req_freq > (_tdm->_chip_data.soc_pkg.clk_freq * 1000))
    {
        TDM_PRINT0("tdm_gh2_post: check frequency failed(2) system freq");
        TDM_PRINT2("(%dKHz) request freq(%dKHz)\n", \
                   _tdm->_chip_data.soc_pkg.clk_freq * 1000, req_freq );
        TDM_FREE( tdm_tab );
        return FAIL;
    }
#else
    if (req_freq > _tdm->_chip_data.soc_pkg.clk_freq)
    {
        TDM_PRINT0("tdm_gh2_post: check frequency failed(2) system freq");
        TDM_PRINT2("(%dKHz) request freq(%fMHz)\n", \
                   _tdm->_chip_data.soc_pkg.clk_freq * 1000, req_freq );
        TDM_FREE( tdm_tab );
        return FAIL;
    }
#endif

    for( i = 0 ; i < GH2_NUM_PHY_PORTS ; i++ )
    {
        if( chip_ports[ i ].enable )
        {
            result = 
                chk_port_bw( i , req_freq , tdm_len );
            if( result != PASS )
            {
                TDM_PRINT0( "tdm_gh2_post: check failed(3)\n" );
                TDM_FREE( tdm_tab );
                return FAIL;
            }
        }
    }

    _tdm->_chip_data.soc_pkg.soc_vars.gh2.pipe_start=1;
    _tdm->_chip_data.soc_pkg.soc_vars.gh2.pipe_end=89;

    if (_tdm->_chip_data.soc_pkg.soc_vars.gh2.pipe_end>128){
        TDM_SML_BAR
        TDM_PRINT0("\nTDM: TDM algorithm is completed.\n\n");
        TDM_SML_BAR

        /* TDM self-check */
        if (_tdm->_chip_data.soc_pkg.soc_vars.gh2.tdm_chk_en==BOOL_TRUE){
            _tdm->_chip_exec[TDM_CHIP_EXEC__CHECK](_tdm);
        }
    }

    TDM_FREE( tdm_tab );
    TDM_PRINT0( "tdm_gh2_post: end\n" );
    return PASS;
}


/**
@name: tdm_gh2_free
@param:
 */
int
tdm_gh2_free( tdm_mod_t *_tdm )
{
    int index;
    /* Chip: pmap */
    for (index=0; index<(_tdm->_chip_data.soc_pkg.pmap_num_modules); index++) {
        TDM_FREE(_tdm->_chip_data.soc_pkg.pmap[index]);
    }
    TDM_FREE(_tdm->_chip_data.soc_pkg.pmap);
    /* Chip: IDB Pipe 0 calendar group */
    TDM_FREE(_tdm->_chip_data.cal_0.cal_main);
    for (index=0; index<(_tdm->_chip_data.cal_0.grp_num); index++) {
        TDM_FREE(_tdm->_chip_data.cal_0.cal_grp[index]);
    }
    TDM_FREE(_tdm->_chip_data.cal_0.cal_grp);
    /* Chip: IDB Pipe 1 calendar group */
    TDM_FREE(_tdm->_chip_data.cal_1.cal_main);
    for (index=0; index<(_tdm->_chip_data.cal_1.grp_num); index++) {
        TDM_FREE(_tdm->_chip_data.cal_1.cal_grp[index]);
    }
    TDM_FREE(_tdm->_chip_data.cal_1.cal_grp);
    /* Chip: IDB Pipe 2 calendar group */
    TDM_FREE(_tdm->_chip_data.cal_2.cal_main);
    for (index=0; index<(_tdm->_chip_data.cal_2.grp_num); index++) {
        TDM_FREE(_tdm->_chip_data.cal_2.cal_grp[index]);
    }
    TDM_FREE(_tdm->_chip_data.cal_2.cal_grp);
    /* Chip: IDB Pipe 3 calendar group */
    TDM_FREE(_tdm->_chip_data.cal_3.cal_main);
    for (index=0; index<(_tdm->_chip_data.cal_3.grp_num); index++) {
        TDM_FREE(_tdm->_chip_data.cal_3.cal_grp[index]);
    }
    TDM_FREE(_tdm->_chip_data.cal_3.cal_grp);
    /* Chip: MMU Pipe 0 calendar group */
    TDM_FREE(_tdm->_chip_data.cal_4.cal_main);
    for (index=0; index<(_tdm->_chip_data.cal_4.grp_num); index++) {
        TDM_FREE(_tdm->_chip_data.cal_4.cal_grp[index]);
    }
    TDM_FREE(_tdm->_chip_data.cal_4.cal_grp);
    /* Chip: MMU Pipe 1 calendar group */
    TDM_FREE(_tdm->_chip_data.cal_5.cal_main);
    for (index=0; index<(_tdm->_chip_data.cal_5.grp_num); index++) {
        TDM_FREE(_tdm->_chip_data.cal_5.cal_grp[index]);
    }
    TDM_FREE(_tdm->_chip_data.cal_5.cal_grp);
    /* Chip: MMU Pipe 2 calendar group */
    TDM_FREE(_tdm->_chip_data.cal_6.cal_main);
    for (index=0; index<(_tdm->_chip_data.cal_6.grp_num); index++) {
        TDM_FREE(_tdm->_chip_data.cal_6.cal_grp[index]);
    }
    TDM_FREE(_tdm->_chip_data.cal_6.cal_grp);
    /* Chip: MMU Pipe 3 calendar group */
    TDM_FREE(_tdm->_chip_data.cal_7.cal_main);
    for (index=0; index<(_tdm->_chip_data.cal_7.grp_num); index++) {
        TDM_FREE(_tdm->_chip_data.cal_7.cal_grp[index]);
    }
    TDM_FREE(_tdm->_chip_data.cal_7.cal_grp);
    /* Core: vmap */
    for (index=0; index<(_tdm->_core_data.vmap_max_wid); index++) {
        TDM_FREE(_tdm->_core_data.vmap[index]);
    }
    TDM_FREE(_tdm->_core_data.vmap);

    return PASS;
}
