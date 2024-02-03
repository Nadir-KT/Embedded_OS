/****************************************************************************
 *  License : All rights reserved for TES Electronic Solutions GmbH
 *            See included /docs/license.txt for details
 *  Project : D/AVE HD
 *  Purpose : Interface to kernel mode driver
 ****************************************************************************
 * Version Control Information :
 *  $Revision: 4771 $
 *  $Date: 2015-02-10 20:01:30 +0900 (火, 10 2 2015) $
 *  $LastChangedBy: florian.zimmermann $
 ****************************************************************************
 * Change History (autogenerated):
 ****************************************************************************/    

#ifndef DAVEHD_KERNEL_DRIVER_H_INCLUDED
#define DAVEHD_KERNEL_DRIVER_H_INCLUDED

#include "davehd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------------------------------------*/
/**/
typedef enum tagdhd_gpu_status {
  E_DHD_STATUS_IDLE,
  E_DHD_STATUS_HALTED,
  E_DHD_STATUS_ACTIVE

} dhd_gpu_status_t;

/*----------------------------------------------------------------------------------------------------------*/
/**/
typedef enum tagdhd_gpu_state_flags {
  E_DHD_STATE_INITIAL,
  E_DHD_STATE_CURRENT

} dhd_gpu_state_flags_t;

/*----------------------------------------------------------------------------------------------------------*/

#ifdef DHD_KERNEL_TLIST_SUPPORT
typedef struct tagdhd_tlist {
  const dhd_char_t                 *m_tlist_file;                       /* currently active tlist dump target (or 0)*/
  void                             *m_tlist_data;                       /* additional data related to tlist logging*/
} dhd_tlist_t;
#endif


/*----------------------------------------------------------------------------------------------------------*/
/* descriptor used to bind a kernel mode driver to a certain hardware instance*/
/**/
typedef struct tagdhd_gpu_config {

  dhd_uint32_t  m_sizeof;                                             /* size of this struct*/
  void         *m_register_base_address;                              /* IO register base address of this hw instance*/
    
  dhd_gpu_ptr_t m_vidmem_base_address;                                /* video memory */
  dhd_uint32_t  m_vidmem_size;  
  
  dhd_uint16_t  m_default_jobsize;                                    /* number of bytes in one jobbuffer*/
  dhd_uint16_t  m_default_ringsize;                                   /* number of bytes in ringbuffer, needs to be a power of two (allocation is 4 bytes more however!)*/
  dhd_uint16_t  m_default_jobcount;                                   /* recommended number of jobs in flight per client*/

  dhd_uint8_t   m_irq_index;                                          /* HW irq bound to this hw instance*/
  dhd_uint8_t   m_force_synchronous;                                  /* set nonzero to request a blocking kernel API (only makes a difference when kernel is compiled without setting DHD_KERNEL_SYNCHRONOUS_ONLY)*/

  void         *m_platform;                                           /* pointer for platform specific generic usage*/

} dhd_gpu_config_t;

/*----------------------------------------------------------------------------------------------------------*/
/* internal structure for each kernel mode instance*/
/**/
/* PRQA S 3313 30 *//* $Misra: No definition for this structure for a clean include structure. $*/
typedef struct tagdhd_gpu_device_data {
  dhd_gpu_config_t                m_gpu_config;                        /* memory config the device was created with*/
  dhd_device_description_t        m_description;                       /* HW device description*/
  dhd_uint32_t                    m_connections;                       /* number of connected user mode clients*/
  dhd_uint32_t                    m_initialized;                       /* flag to indicate this device struct is ready  */
  dhd_uint32_t                    m_instanceid;                        /* GPU instance index */
  struct tagdhd_gpu_debug_data   *m_debug_data;                        /* optional data only used when kernel debugging is used*/
  struct tagdhd_gpu_connection   *m_connection_list;                   /* list of connected user mode clients  */
  struct tagdhd_gpu_vidmem_heap  *m_vidmem_heap;                       /* video memory heap to be used for this device*/
  struct tagdhd_gpu_job          *m_first_job;                         /* chain of jobs queued for this device*/
  struct tagdhd_gpu_job          *m_last_job;                          /* end of queued job chain*/
  struct tagdhd_gpu_job          *m_active_jobs;                       /* chain of (potentially) running jobs */
  struct tagdhd_gpu_job          *m_free_jobs;                         /* chain of (definitely) finished jobs */
  struct tagdhd_gpu_connection   *m_last_owner;                        /* last connection that put a job into the gpu ring*/
  dhd_uint32_t                    m_stc_control;                       /* current stc control config (valid only after gpu_hardware_setup)*/
  dhd_uint32_t                    m_job_id_sequence;                   /* counter for assigning job id's*/
  dhd_vidmem_t                    m_ringbuffer;                        /* start of ringbuffer video ram (0 if none)*/
  dhd_uint32_t                    m_ringpos;                           /* dword write offset in ringbuffer*/
  dhd_uint32_t                    m_ringsize;                          /* number of dwords in ringbuffer (power of two)*/
  volatile dhd_gpu_status_t       m_status;                            /* current GPU status (written by ISR)*/
  volatile dhd_uint32_t           m_last_sync_code;                    /* last processed GPU sync (written by ISR)*/
  volatile dhd_uint32_t           m_last_stop_code;                    /* stop code received by ISR  */
  dhd_uint32_t                    m_perfcount_allocmask;               /* current allocation of performance counters (0 is free)*/
  dhd_uint32_t                    m_perfcount_usemask;                 /* currently enabled performance counters*/
  dhd_uint32_t                    m_clut_usemask;                      /* currently allocated clut blocks (one bit for each block)*/
  dhd_uint32_t                    m_clut_blocksize;                    /* number of clut entries in one clut block (usually clutsize/32)*/
  dhd_uint32_t                    m_schedule_iteration;                /* current job schedule iteration  */
  dhd_uint8_t                     m_core_halting;                      /* true if halt request has been set and STATUS_HALTED is to be expected*/
#if defined(DHD_KERNEL_TLIST_SUPPORT) && defined(DHD_KERNEL_DUMP_KERNEL_TLIST)
  dhd_tlist_t                     m_tlist;                             /* hardware/kernel tlist dump*/
#endif

} dhd_gpu_device_data_t;

/*----------------------------------------------------------------------------------------------------------*/
dhd_uint32_t dhd_gpu_syncid_from_jobid(dhd_jobid_t a_id);
dhd_uint32_t dhd_gpu_generation_from_jobid(dhd_jobid_t a_id);
dhd_gpu_device_data_t *dhd_gpu_resolve_instance(dhd_uint32_t a_instance);
dhd_uint32_t dhd_gpu_resume_from_halt(dhd_gpu_device_data_t *a_device);
dhd_uint32_t dhd_gpu_get_client_count(void);

/*----------------------------------------------------------------------------------------------------------*/
dhd_enum_t dhd_gpu_os_init(void);
dhd_enum_t dhd_gpu_os_shutdown(void);

/*----------------------------------------------------------------------------------------------------------*/
/* Generic structure for parameter passing. */
/* All kernel functions have the same signature: dhd_uint32_t function(dhd_gpu_call_data_t *a_parameters)*/
/**/

typedef union tagdhd_gpu_call_data {
  dhd_uint32_t                   m_response;                           /* used internally only*/

  struct tagget_instance_count {                                       /* dhd_gpu_get_instance_count */
    dhd_uint32_t                *m_out;
  } m_get_instance_count;

  struct tagquery_instance {                                           /* dhd_gpu_query_instance*/
    dhd_uint32_t                 m_instance;
    dhd_device_description_t    *m_out;
  } m_query_instance;

  struct taginit {                                                     /* dhd_gpu_init*/
    dhd_uint32_t                 m_instance;
    const dhd_gpu_config_t      *m_config;
    dhd_uint8_t                  m_version_major_out;                 
    dhd_uint8_t                  m_version_minor_out;
    dhd_uint8_t                  m_revision_out;
  } m_init;

  struct tagshutdown {                                                 /* dhd_gpu_shutdown*/
    dhd_uint32_t                 m_instance;
  } m_shutdown;

  struct tagconnect {                                                  /* dhd_gpu_connect*/
    dhd_uint32_t                 m_instance;
    dhd_handle_t                *m_out;
  } m_connect;

  struct tagdisconnect {                                               /* dhd_gpu_disconnect*/
    dhd_handle_t                 m_client;
  } m_disconnect;

  struct tagalloc_mem {                                                /* dhd_gpu_alloc_mem*/
    dhd_handle_t                 m_client;
    dhd_uint32_t                 m_size;
    dhd_uint32_t                 m_alignment;
    dhd_uint32_t                 m_tag;
    dhd_memory_usage_t           m_usage;
    dhd_vidmem_t                *m_result;
  } m_alloc_mem;

  struct tagfree_mem {                                                 /* dhd_gpu_free_mem*/
    dhd_handle_t                 m_client;
    dhd_vidmem_t                *m_memory;
  } m_free_mem;

  struct tagcan_map_mem {                                              /* dhd_gpu_can_map_mem*/
    dhd_handle_t                 m_client;
    dhd_vidmem_t                *m_memory;
    dhd_enum_t                   m_access;
  } m_can_map_mem;

  struct tagmap_mem {                                                  /* dhd_gpu_map_mem*/
    dhd_handle_t                 m_client;
    dhd_vidmem_t                *m_memory;
    dhd_enum_t                   m_access;
  } m_map_mem;

  struct tagunmap_mem {                                                /* dhd_gpu_unmap_mem*/
    dhd_handle_t                 m_client;
    dhd_vidmem_t                *m_memory;
    dhd_enum_t                   m_access;
  } m_unmap_mem;

  struct tagupload {                                                   /* dhd_gpu_upload*/
    dhd_handle_t                 m_client;
    const dhd_uint8_t           *m_src;
    dhd_gpu_ptr_t                m_dest;
    dhd_uint32_t                 m_size;
    dhd_bool_t                   m_tlistonly;
  } m_upload;

  struct tagdownload {                                                 /* dhd_gpu_download*/
    dhd_handle_t                 m_client;
    dhd_gpu_ptr_t                m_src;
    dhd_uint8_t                 *m_dest;
    dhd_uint32_t                 m_size;
  } m_download;

  struct tagcommit {                                                   /* dhd_gpu_commit*/
    dhd_handle_t                 m_client;
    struct tagdhd_gpu_job       *m_job;
    struct tagdhd_gpu_job      **m_reclaimed_jobs;    
  } m_commit;

  struct tagget_sync {                                                 /* dhd_gpu_get_sync*/
    dhd_handle_t                 m_client;
    dhd_uint32_t                *m_jobid;
    dhd_uint32_t                *m_clid;
  } m_get_sync;

  struct tagwait_for_job {                                             /* dhd_gpu_wait_for_job*/
    dhd_uint32_t                 m_unused;                             /* unused to enable setting response before processing*/
    dhd_handle_t                 m_client;
    dhd_jobid_t                  m_id;
  } m_wait_for_job;

  struct tagget_reclaimed_jobs {                                       /* dhd_gpu_get_reclaimed_jobs*/
    dhd_handle_t                 m_client;
    struct tagdhd_gpu_job      **m_out;
  } m_get_reclaimed_jobs;

  struct tagforce_yield {                                              /* dhd_gpu_force_yield*/
    dhd_handle_t                 m_client;    
  } m_force_yield;

  struct tagdownload_state {                                           /* dhd_gpu_download_state*/
    dhd_handle_t                 m_client;
    dhd_uint32_t                *m_destination;
    dhd_uint32_t                 m_count;
    dhd_gpu_state_flags_t        m_flags;
  } m_download_state;

  struct tagtlist_enable {                                             /* dhd_gpu_tlist_enable*/
    dhd_handle_t                 m_client;
    const dhd_char_t            *m_file;
  } m_tlist_enable;

  struct tagtlist_write {                                              /* dhd_gpu_tlist_write*/
    dhd_handle_t                 m_client;
    const dhd_char_t            *m_string;
    dhd_bool_t                   m_to_kernel_tlist;
  } m_tlist_write;

  struct tagalloc_pc {                                                 /* dhd_gpu_alloc_pc*/
    dhd_handle_t                 m_client;
    dhd_uint8_t                 *m_pcid;
  } m_alloc_pc;

  struct tagfree_pc {                                                  /* dhd_gpu_free_pc*/
    dhd_handle_t                 m_client;
    dhd_uint8_t                  m_pcid;
  } m_free_pc;

  struct tagalloc_clut {                                               /* dhd_gpu_alloc_clut*/
    dhd_handle_t                 m_client;
    dhd_uint32_t                 m_size;
    dhd_uint32_t                *m_clutindex;
  } m_alloc_clut;

  struct tagfree_clut {                                                /* dhd_gpu_free_clut*/
    dhd_handle_t                 m_client;
    dhd_uint32_t                 m_size;
    dhd_uint32_t                 m_clutindex;
  } m_free_clut;  

  struct tagresume {                                                   /* dhd_gpu_resume*/
    dhd_uint32_t                 m_instance;
  } m_resume;

  struct tagcan_access_mem {                                           /* dhd_gpu_can_access_mem*/
    dhd_handle_t                 m_client;
    dhd_uint32_t                 m_size;
    dhd_uint8_t                 *m_cpu_address;
  } m_can_access_mem;

} dhd_gpu_call_data_t;

/*----------------------------------------------------------------------------------------------------------*/
dhd_uint32_t dhd_gpu_get_instance_count(const dhd_gpu_call_data_t *a_parameters);
dhd_uint32_t dhd_gpu_query_instance(const dhd_gpu_call_data_t *a_parameters); 
dhd_uint32_t dhd_gpu_init(dhd_gpu_call_data_t *a_parameters);   /* intentionally the only function with a non-const parameter (will always dispatch directly/non-bridged)*/
dhd_uint32_t dhd_gpu_shutdown(const dhd_gpu_call_data_t *a_parameters); 
dhd_uint32_t dhd_gpu_connect(const dhd_gpu_call_data_t *a_parameters); 
dhd_uint32_t dhd_gpu_disconnect(const dhd_gpu_call_data_t *a_parameters);  

/*----------------------------------------------------------------------------------------------------------*/
dhd_uint32_t dhd_gpu_alloc_mem(const dhd_gpu_call_data_t *a_parameters);  
dhd_uint32_t dhd_gpu_free_mem(const dhd_gpu_call_data_t *a_parameters);  
dhd_uint32_t dhd_gpu_can_map_mem(const dhd_gpu_call_data_t *a_parameters);  
dhd_uint32_t dhd_gpu_map_mem(const dhd_gpu_call_data_t *a_parameters);  
dhd_uint32_t dhd_gpu_unmap_mem(const dhd_gpu_call_data_t *a_parameters);  
dhd_uint32_t dhd_gpu_upload(const dhd_gpu_call_data_t *a_parameters);  
dhd_uint32_t dhd_gpu_download(const dhd_gpu_call_data_t *a_parameters);  
dhd_uint32_t dhd_gpu_can_access_mem(const dhd_gpu_call_data_t *a_parameters);

/*----------------------------------------------------------------------------------------------------------*/
dhd_uint32_t dhd_gpu_commit(const dhd_gpu_call_data_t *a_parameters);
dhd_uint32_t dhd_gpu_get_sync(const dhd_gpu_call_data_t *a_parameters);
dhd_uint32_t dhd_gpu_wait_for_job(dhd_gpu_call_data_t *a_parameters);
dhd_uint32_t dhd_gpu_get_reclaimed_jobs(const dhd_gpu_call_data_t *a_parameters);
dhd_uint32_t dhd_gpu_force_yield(const dhd_gpu_call_data_t *a_parameters);
dhd_uint32_t dhd_gpu_resume(const dhd_gpu_call_data_t *a_parameters);

/*----------------------------------------------------------------------------------------------------------*/
dhd_uint32_t dhd_gpu_download_state(const dhd_gpu_call_data_t *a_parameters);

/*----------------------------------------------------------------------------------------------------------*/
dhd_uint32_t dhd_gpu_tlist_enable(const dhd_gpu_call_data_t *a_parameters);
dhd_uint32_t dhd_gpu_tlist_write(const dhd_gpu_call_data_t *a_parameters);

/*----------------------------------------------------------------------------------------------------------*/
dhd_uint32_t dhd_gpu_alloc_pc(const dhd_gpu_call_data_t *a_parameters);
dhd_uint32_t dhd_gpu_free_pc(const dhd_gpu_call_data_t *a_parameters);

/*----------------------------------------------------------------------------------------------------------*/
dhd_uint32_t dhd_gpu_alloc_clut(const dhd_gpu_call_data_t *a_parameters);
dhd_uint32_t dhd_gpu_free_clut(const dhd_gpu_call_data_t *a_parameters);

/*----------------------------------------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif 

#endif /*DAVEHD_KERNEL_DRIVER_H_INCLUDED*/