/* model=qwen2.5-coder-1.5b task=iotivity__13a2879a rep=1 */
#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for oc_api.h */
#ifndef STUB_OC_API_H_80DCE115
#define STUB_OC_API_H_80DCE115
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_oc_api_h_80dce115;
#endif
/* stub for port/oc_clock.h */
#ifndef STUB_PORT_OC_CLOCK_H_359D6C25
#define STUB_PORT_OC_CLOCK_H_359D6C25
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_port_oc_clock_h_359D6C25;
#endif
/* stub for signal.h */
#ifndef STUB_SIGNAL_H_4567F046
#define STUB_SIGNAL_H_4567F046
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_signal_h_4567f046;
#endif
/* stub for windows.h */
#ifndef STUB_WINDOWS_H_CA443C89
#define STUB_WINDOWS_H_CA443C89
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_windows_h_ca443c89;
#endif
#endif

/*
// Copyright (c) 2016 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/


int quit = 0;

static CONDITION_VARIABLE cv;
static CRITICAL_SECTION cs;

static int
app_init(void)
{
  int ret = oc_init_platform("Apple", NULL, NULL);
  ret |= oc_add_device("/oic/d", "oic.d.phone", "Kishen's IPhone", "ocf.1.0.0",
                       "ocf.res.1.0.0", NULL, NULL);
  return ret;
}

#define MAX_URI_LENGTH (30)
static char a_light[MAX_URI_LENGTH];
static oc_endpoint_t *light_server;

static bool state;
static int power;
static oc_string_t name;

static oc_event_callback_retval_t
stop_observe(void *data)
{
  (void)data;
  PRINT("Stopping OBSERVE\n");
  oc_stop_observe(a_light, light_server);
  return OC_EVENT_DONE;
}
