#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for hawkbit_update.h */
#ifndef STUB_HAWKBIT_UPDATE_H_E9251A8F
#define STUB_HAWKBIT_UPDATE_H_E9251A8F
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_hawkbit_update_h_e9251a8f;
#endif
/* stub for oc_helpers.h */
#ifndef STUB_OC_HELPERS_H_E20E8764
#define STUB_OC_HELPERS_H_E20E8764
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_oc_helpers_h_e20e8764;
#endif
#endif

/****************************************************************************
 *
 * Copyright (c) 2022 Jozef Kralik, All Rights Reserved.
 * Copyright (c) 2022 Daniel Adam, All Rights Reserved.
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
#include <string.h>

hawkbit_async_update_t
hawkbit_update_create(oc_string_view_t deployment_id, oc_string_view_t version,
                      const uint8_t *sha256, size_t sha256_size,
                      const uint8_t *partition_sha256,
                      size_t partition_sha256_size)
{
  assert(deployment_id.data != NULL);
  assert(version.data != NULL);

  hawkbit_async_update_t update;
  memset(&update, 0, sizeof(hawkbit_async_update_t));
  oc_new_string(&update.deployment_id, deployment_id.data,
                deployment_id.length);
  oc_new_string(&update.version, version.data, version.length);
  assert(sha256_size == sizeof(update.sha256));
  memcpy(update.sha256, sha256, sha256_size);
  assert(partition_sha256_size == sizeof(update.partition_sha256));
  memcpy(update.partition_sha256, partition_sha256, partition_sha256_size);
  return update;
}

void
hawkbit_update_free(hawkbit_async_update_t *update)
{
  if (update == NULL) {
    return;
  }
  oc_free_string(&update->deployment_id);
  oc_free_string(&update->version);
  memset(update, 0, sizeof(hawkbit_async_update_t));
}
