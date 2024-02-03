/************************************************************************************************
PROJECT : simple_draw
FILE    : $Id: vowe.c 16855 2019-08-06 01:12:45Z shinya.tomari.wj@renesas.com $
============================================================================ 
DESCRIPTION
============================================================================
                            C O P Y R I G H T
============================================================================
                       Copyright (c) 2013 - 2014
                                  by 
                       Renesas Electronics (Europe) GmbH.
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================
Purpose: only for testing, not for mass production

DISCLAIMER

LICENSEE has read, understood and accepted the terms and conditions defined in
the license agreement, especially the usage rights. In any case, it is
LICENSEE's responsibility to make sure that any user of the software complies
with the terms and conditions of the signed license agreement.

SAMPLE CODE is not part of the licensed software, as such it must not be used in
mass-production applications. It can only be used for evaluation and
demonstration purposes at customer's premises listed in the signed license
agreement.
****************************************************************************
*/

/*******************************************************************************
  Section: Includes
*/

#include "r_typedefs.h"        /* Renesas basic types, e.g. uint32_t */
#include "r_bsp_api.h"         /* Board support package */
#include "r_bsp_stdio_api.h"
#include "r_vowe_api.h"
#include "r_tick_api.h"
#include "config.h"
#include "main.h"

#include "r_vowe_api.h"
#include "FanWise_Warping80_240_320.h"
#include "FanWise_Warping80_240_320_RingBuffer.h"
#include "FanWise_Warping10_240_320.h"
#include "FanWise_Warping10_240_320_RingBuffer.h"

#include <string.h>

/***********************************************************
  Section: Local Defines
*/
extern uint32_t loc_VOWE_DisplayList_Addr;
extern uint32_t loc_VOWE_ImageBuffer_Addr;

/***********************************************************
  Section: Local Variables
*/

/***********************************************************
  Section: Local Functions
*/

/*******************************************************************************
  Function: loc_VOWE_Callback

  This function is callback for VOWE.
*/
static void loc_VOWE_Callback(const uint32_t Unit,
                              const uint32_t Factor,
                              const uint32_t CallbackParam)
{
    if ((Factor & R_VOWE_FACTOR_IER) == R_VOWE_FACTOR_IER)
    {
        loc_Error(1);
    }
}

/*******************************************************************************
  Function: loc_VoweStart

  This function starts VOWE.
*/
void loc_VoweStart(void)
{
    r_vowe_Error_t         result_vowe;
    r_vowe_SourceConfig_t  src_conf;
    r_vowe_DisplayList_t   dsp_list;
    r_vowe_DestConfig_t    dst_conf;
    uint32_t   ii, ii_count;
    uint32_t  *dl_add;
    
    dl_add   = (uint32_t *)loc_VOWE_DisplayList_Addr;
    ii_count = LOC_VOWE_DL_ADDR_LENGTH_2 / 4;
    for (ii=0; ii<ii_count; ii++)
    {
        *dl_add = LOC_VOWE_DL_ADDR_2[ii];
        dl_add++;
    }
    dl_add   = (uint32_t *)(loc_VOWE_DisplayList_Addr + LOC_VOWE_DL_BUF_SIZE);
    ii_count = LOC_VOWE_DL_ADDR_LENGTH_1 / 4;
    for (ii=0; ii<ii_count; ii++)
    {
        *dl_add = LOC_VOWE_DL_ADDR_1[ii];
        dl_add++;
    }
    memset((void*)loc_VOWE_ImageBuffer_Addr, 0x20, LOC_VOWE_WORK_BUFF_SIZE);

    src_conf.LineStartPos    = LOC_VOWE_LSPR;        /* defined header file */
    src_conf.SourceWidth     = LOC_DISPLAY_WIDTH;    /* (Pixel) */
    src_conf.SourceHeight    = LOC_DISPLAY_HEIGHT;   /* (Pixel) */
    dsp_list.DspList         = (uint32_t *)(loc_VOWE_DisplayList_Addr + LOC_VOWE_DL_BUF_SIZE);  /* DL Top address */

    dsp_list.Size            = LOC_VOWE_DL_ADDR_LENGTH_1;
    dst_conf.WorkBufferMode  = LOC_VOWE_WORK_BUFF_MODE; /* VOWE Operation Mode */
    dst_conf.WorkBufferSize  = LOC_VOWE_WORK_BUFF_SIZE;
    dst_conf.WorkBufferAddr  = (uint8_t *)loc_VOWE_ImageBuffer_Addr;

    dst_conf.ColorFormat     = R_VOWE_FORMAT_ARGB8888;  /* Color Format Set */
    dst_conf.DitherMode      = R_FALSE;                 /* Dither mode set */
    dst_conf.RingBufferDelay = LOC_VOWE_RING_BUFF_DELAY;
    dst_conf.DestMode        = R_VOWE_DEST_MODE_NORMAL;
    /*-----------------------------------*/

    result_vowe = R_VOWE_Init(LOC_VOWE_UNIT);
    if (R_VOWE_ERR_OK != result_vowe)
    {
        loc_Error(1);
    }
    else
    {
        result_vowe = R_VOWE_Open(LOC_VOWE_UNIT, &src_conf,&dsp_list,&dst_conf,loc_VOWE_Callback,0);
        if (R_VOWE_ERR_OK != result_vowe)
        {
            loc_Error(1);
        }
        else
        {
            result_vowe = R_VOWE_Start(LOC_VOWE_UNIT, R_TRUE);
            if (R_VOWE_ERR_OK  != result_vowe)
            {
                loc_Error(1);
            }
        }
    }
}


/*******************************************************************************
  Function: loc_VoweStop

  This function stops VOWE.
*/
void loc_VoweStop(void)
{
    int32_t ret = 0;

    ret |= R_VOWE_Stop(LOC_VOWE_UNIT);
    R_TICK_WaitMS(0, 100); /* need to wait 1 frame for stop being effective */
    ret |= R_VOWE_Close(LOC_VOWE_UNIT);
    ret |= R_VOWE_DeInit(LOC_VOWE_UNIT);

}

/*******************************************************************************
  Function: loc_Vowe_DLChange

  Change the Display List.
*/
void loc_Vowe_DLChange(int32_t dl_cnt)
{
    r_vowe_DisplayList_t   dsp_list;
    
    /* change Display List */
    if (dl_cnt & 1)
    {
        dsp_list.Size = LOC_VOWE_DL_ADDR_LENGTH_2;
        dsp_list.DspList = (uint32_t *)(loc_VOWE_DisplayList_Addr);
    }
    else
    {
        dsp_list.Size = LOC_VOWE_DL_ADDR_LENGTH_1;
        dsp_list.DspList = (uint32_t *)(loc_VOWE_DisplayList_Addr + LOC_VOWE_DL_BUF_SIZE);
    }
    R_VOWE_DLChange(LOC_VOWE_UNIT, &dsp_list);
}
