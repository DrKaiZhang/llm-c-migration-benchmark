#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for port/oc_assert.h */
#ifndef STUB_PORT_OC_ASSERT_H_262E03F5
#define STUB_PORT_OC_ASSERT_H_262E03F5
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_port_oc_assert_h_262e03f5;
#endif
#endif

/****************************************************************************
 *
 * Copyright (c) 2017 Intel Corporation
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
#include <stdlib.h>
void
abort_impl(void)
{
  assert(NULL);
}

void
exit_impl(int status)
{
  (void)status;
  assert(NULL);
}
