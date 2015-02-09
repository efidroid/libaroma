/********************************************************************[libaroma]*
 * Copyright (C) 2011-2015 Ahmad Amarullah (http://amarullz.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *______________________________________________________________________________
 *
 * Filename    : aroma.c
 * Description : libaroma Main Source File
 *
 * + This is part of libaroma, an embedded ui toolkit.
 * + 19/01/15 - Author(s): Ahmad Amarullah
 *
 */
#ifndef __libaroma_aroma_c__
#define __libaroma_aroma_c__
#include "config.h"           /* libaroma config header */
#include "config_features.h"  /* libaroma features config header */

/* Debugging */
#ifdef LIBAROMA_CONFIG_DEBUG_MEMORY
#if LIBAROMA_CONFIG_DEBUG_MEMORY >=1
#include "aroma/debug/memory_tracking.c"
#endif
#endif

#include <aroma.h>      /* libaroma main header */
#include "aroma/core.c" /* libaroma core module */

/*
 * Variable    : _libaroma_config
 * Type        : LIBAROMA_CONFIG
 * Descriptions: runtime configuration
 */
static LIBAROMA_CONFIG _libaroma_config;
static byte _libaroma_config_ready=0;

/*
 * Function    : _libaroma_config_default
 * Return Value: void
 * Descriptions: set default runtime configuration
 */
void _libaroma_config_default() {
  if (LIBAROMA_FB_SHMEM_NAME){
    snprintf(_libaroma_config.fb_shm_name,64,"%s",LIBAROMA_FB_SHMEM_NAME);
  }
  else{
    _libaroma_config.fb_shm_name[0]=0;
  }
  _libaroma_config.runtime_monitor = LIBAROMA_START_UNSAFE;
  _libaroma_config_ready = 1;
} /* End of libaroma_config_default */

/*
 * Function    : libaroma_config
 * Return Value: LIBAROMA_CONFIGP
 * Descriptions: get runtime config
 */
LIBAROMA_CONFIGP libaroma_config(){
  if (!_libaroma_config_ready){
    _libaroma_config_default();
  }
  return &_libaroma_config;
} /* End of libaroma_config */

/*
 * Function    : libaroma_start
 * Return Value: byte
 * Descriptions: Start libaroma application
 */
byte libaroma_start() {
  /* Welcome Message */
  ALOGI(
    "%s Version %s",
    libaroma_info(LIBAROMA_INFO_NAME),
    libaroma_info(LIBAROMA_INFO_FULLVER)
  );
  ALOGI("  %s", libaroma_info(LIBAROMA_INFO_COPYRIGHT));
  ALOGI(" ");
  
  /* Init Safe Process Monitoring */
  if (libaroma_config()->runtime_monitor) {
    libaroma_runtime_init();
  }
  
#ifdef LIBAROMA_CONFIG_DEBUG_MEMORY
#if LIBAROMA_CONFIG_DEBUG_MEMORY >=1
  ALOGV("STARTING MEMORY TRACKING");
#endif
#endif
  
  /* Mute Parent */
  if (libaroma_config()->runtime_monitor == LIBAROMA_START_MUTEPARENT) {
    libaroma_runtime_mute_parent();
  }
  
  ALOGI("===================================================");
  
  if (!libaroma_graph_init()) {
    ALOGE("libaroma_start cannot start graph engine...");
    return 0;
  }

  if (!libaroma_hid_init()) {
    ALOGE("libaroma_start cannot start hid engine...");
    return 0;
  }
  
  if (!libaroma_msg_init()) {
    ALOGE("libaroma_start cannot start message queue...");
    return 0;
  }
  
  if (!libaroma_lang_init()) {
    ALOGE("libaroma_start cannot start language engine...");
    return 0;
  }
  
  if (!libaroma_timer_init()) {
    ALOGE("libaroma_start cannot start timer engine...");
    return 0;
  }
  
  if (!libaroma_wm_init()){
    ALOGE("libaroma_start cannot start window manager...");
    return 0;
  }
  
  ALOGI("___________________________________________________");
  ALOGI(" ");
  return 1;
}

/*
 * Function    : libaroma_end
 * Return Value: byte
 * Descriptions: End libaroma application
 */
byte libaroma_end() {
  ALOGI("___________________________________________________");
  ALOGI(" ");
  
  /* Release Engines */
  libaroma_wm_release();
  libaroma_timer_release();
  libaroma_lang_release();
  libaroma_msg_release();
  libaroma_hid_release();
  libaroma_graph_release();
  
  ALOGI("===================================================");
#ifdef LIBAROMA_CONFIG_DEBUG_MEMORY
#if LIBAROMA_CONFIG_DEBUG_MEMORY >=1
  ALOGV("DUMPING MEMORY TRACKING");
  ___mtrack_dump_leak();
#endif
#endif
  return 1;
}

/* libaroma high level api */
#include "aroma/api.c"

/* show compiler info */
#ifdef LIBAROMA_CONFIG_COMPILER_MESSAGE
#include "aroma/debug/compiler_message.c"
#endif

#endif /* __libaroma_aroma_c__ */