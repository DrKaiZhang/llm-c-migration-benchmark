#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for esp_timer.h */
#ifndef STUB_ESP_TIMER_H_2ABAAC3A
#define STUB_ESP_TIMER_H_2ABAAC3A
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_esp_timer_h_2abaac3a;
#endif
/* stub for port/oc_clock.h */
#ifndef STUB_PORT_OC_CLOCK_H_359D6C25
#define STUB_PORT_OC_CLOCK_H_359D6C25
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_port_oc_clock_h_359d6c25;
#endif
/* stub for port/oc_log_internal.h */
#ifndef STUB_PORT_OC_LOG_INTERNAL_H_769FF793
#define STUB_PORT_OC_LOG_INTERNAL_H_769FF793
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_port_oc_log_internal_h_769ff793;
#endif
/* stub for unistd.h */
#ifndef STUB_UNISTD_H_5DAE45BB
#define STUB_UNISTD_H_5DAE45BB
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_unistd_h_5dae45bb;
#endif
#endif

/******************************************************************
 *
 * Copyright (c) 2016 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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
 *
 ******************************************************************/

#include <math.h>
#include <time.h>


void
oc_clock_init(void)
{
}

oc_clock_time_t
oc_clock_time(void)
{
  oc_clock_time_t time = 0;
  struct timespec t;
  if (clock_gettime(CLOCK_REALTIME, &t) != -1) {
    time = (oc_clock_time_t)t.tv_sec * OC_CLOCK_SECOND +
           (oc_clock_time_t)ceil((double)t.tv_nsec / (1.e09 / OC_CLOCK_SECOND));
  }

  return time;
}

bool
oc_clock_time_has_monotonic_clock(void)
{
  return true;
}

oc_clock_time_t
oc_clock_time_monotonic(void)
{
  // esp_timer_get_time returns microseconds from the boot time
  return (oc_clock_time_t)ceil(
    (double)(esp_timer_get_time() / (1.e06 / OC_CLOCK_SECOND)));
}

uint64_t
oc_clock_seconds_v1(void)
{
  struct timespec t;
  if (clock_gettime(CLOCK_REALTIME, &t) != -1) {
    return t.tv_sec;
  }
  return 0;
}

unsigned long
oc_clock_seconds(void)
{
  return (unsigned long)oc_clock_seconds_v1();
}

void
oc_clock_wait(oc_clock_time_t t)
{
  double multiplier = ((double)OC_CLOCK_SECOND / 1.e06);
  __useconds_t interval = (__useconds_t)((double)t * multiplier);
  usleep(interval);
}
