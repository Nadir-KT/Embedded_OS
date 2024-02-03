/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : Hardware initialisation codes
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 4170 $
 *  $Date: 2014-10-22 23:51:50 +0900 (水, 22 10 2014) $
 *  $LastChangedBy: florian.zimmermann $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/    

#ifndef DAVEHD_KERNEL_INIT_H_INCLUDED
#define DAVEHD_KERNEL_INIT_H_INCLUDED

#include "davehd_types.h"
#include "davehd_registerfile.h"

/*----------------------------------------------------------------------------------------------------------*/

extern const dhd_uint32_t dhd_reset_state[ DHD_REGFILE_SIZE ];

/*----------------------------------------------------------------------------------------------------------*/

dhd_enum_t dhd_gpu_hardware_setup(dhd_gpu_device_data_t *a_device);
dhd_enum_t dhd_gpu_hardware_shutdown(dhd_gpu_device_data_t *a_device);
dhd_enum_t dhd_gpu_wait_for_hw_status(dhd_gpu_device_data_t *a_device, dhd_gpu_status_t a_status);
const dhd_uint32_t *dhd_gpu_get_group_defaults(dhd_uint32_t a_group);

/*----------------------------------------------------------------------------------------------------------*/

#endif /*DAVEHD_KERNEL_INIT_H_INCLUDED*/