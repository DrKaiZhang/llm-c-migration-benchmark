#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for oc_clock_util.h */
#ifndef STUB_OC_CLOCK_UTIL_H_EA0BFD95
#define STUB_OC_CLOCK_UTIL_H_EA0BFD95
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_oc_clock_util_h_ea0bfd95;
#endif
/* stub for oc_log.h */
#ifndef STUB_OC_LOG_H_6F2A79EC
#define STUB_OC_LOG_H_6F2A79EC
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_oc_log_h_6f2a79ec;
#endif
/* stub for plgd_dps_log_internal.h */
#ifndef STUB_PLGD_DPS_LOG_INTERNAL_H_955F57A2
#define STUB_PLGD_DPS_LOG_INTERNAL_H_955F57A2
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_plgd_dps_log_internal_h_955f57a2;
#endif
/* stub for util/oc_atomic.h */
#ifndef STUB_UTIL_OC_ATOMIC_H_A99C916C
#define STUB_UTIL_OC_ATOMIC_H_A99C916C
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_util_oc_atomic_h_a99c916c;
#endif
/* stub for util/oc_compiler.h */
#ifndef STUB_UTIL_OC_COMPILER_H_C047E481
#define STUB_UTIL_OC_COMPILER_H_C047E481
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_util_oc_compiler_h_c047e481;
#endif
#endif

/****************************************************************************
 *
 * Copyright (c) 2022-2024 plgd.dev, s.r.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/



#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

static struct
{
  plgd_dps_print_log_fn_t fn; ///< logging function
  OC_ATOMIC_INT8_T level;     ///< enabled log level
} g_dps_logger = {
  .fn = NULL,
  .level = OC_LOG_LEVEL_INFO,
};

void
plgd_dps_set_log_fn(plgd_dps_print_log_fn_t log_fn)
{
  g_dps_logger.fn = log_fn;
}

plgd_dps_print_log_fn_t
plgd_dps_get_log_fn(void)
{
  return g_dps_logger.fn;
}

void
plgd_dps_log_set_level(oc_log_level_t level)
{
  assert(level >= INT8_MIN);
  assert(level <= INT8_MAX);
  OC_ATOMIC_STORE8(g_dps_logger.level, (int8_t)level);
}

oc_log_level_t
plgd_dps_log_get_level(void)
{
  return OC_ATOMIC_LOAD8(g_dps_logger.level);
}
