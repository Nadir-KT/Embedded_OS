/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : C register parameter defines
 ****************************************************************************
 * Version Control Information :
 * DO NOT EDIT THIS FILE - This file will be automatically generated by
 * 'gen_type_headers_sw.py'
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/    
/*----------------------------------------------------------------------------------------------------------*/
/* Renesas D1x Platform gets platform constrained parameterfile, default config for all other*/
/**/

#ifndef DAVEHD_REGISTERPARAMS_H_INCLUDED
#define DAVEHD_REGISTERPARAMS_H_INCLUDED

#ifdef RENESAS_D1X
  #include "davehd_registerparams_d1x.h"
#else
  #include "davehd_registerparams_default.h"
#endif

#endif /* DAVEHD_REGISTERPARAMS_H_INCLUDED */