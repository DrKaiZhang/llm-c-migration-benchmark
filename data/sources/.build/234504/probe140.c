#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for oc_config.h */
#ifndef STUB_OC_CONFIG_H_CDACE0BE
#define STUB_OC_CONFIG_H_CDACE0BE
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_oc_config_h_cdace0be;
#endif
/* stub for port/oc_storage.h */
#ifndef STUB_PORT_OC_STORAGE_H_96B88175
#define STUB_PORT_OC_STORAGE_H_96B88175
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_port_oc_storage_h_96b88175;
#endif
/* stub for port/oc_storage_internal.h */
#ifndef STUB_PORT_OC_STORAGE_INTERNAL_H_54178EAA
#define STUB_PORT_OC_STORAGE_INTERNAL_H_54178EAA
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_port_oc_storage_internal_h_54178eaa;
#endif
/* stub for storage.h */
#ifndef STUB_STORAGE_H_C121BF37
#define STUB_STORAGE_H_C121BF37
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_storage_h_c121bf37;
#endif
/* stub for util/oc_secure_string_internal.h */
#ifndef STUB_UTIL_OC_SECURE_STRING_INTERNAL_H_8ADC50A6
#define STUB_UTIL_OC_SECURE_STRING_INTERNAL_H_8ADC50A6
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_util_oc_secure_string_internal_h_8adc50a6;
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


#ifdef OC_STORAGE

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static char g_store_path[OC_STORE_PATH_SIZE] = { 0 };
static uint8_t g_store_path_len = 0;

int
oc_storage_config(const char *store)
{
  if (store == NULL || store[0] == '\0') {
    return -EINVAL;
  }

  size_t store_len = oc_strnlen(store, OC_STORE_PATH_SIZE);
  if (store_len >= OC_STORE_PATH_SIZE) {
    return -ENOENT;
  }

  // remove multiple trailing slashes
  while (store_len > 1 && store[store_len - 2] == '/') {
    --store_len;
  }

  assert(store_len < UINT8_MAX);
  g_store_path_len = (uint8_t)store_len;
  memcpy(g_store_path, store, g_store_path_len);
  if (g_store_path[g_store_path_len - 1] != '/') {
    if (g_store_path_len + 1 >= OC_STORE_PATH_SIZE) {
      oc_storage_reset();
      return -ENOENT;
    }
    g_store_path[g_store_path_len] = '/';
    ++g_store_path_len;
  }
  g_store_path[g_store_path_len] = '\0';
  return 0;
}

bool
oc_storage_path(char *buffer, size_t buffer_size)
{
  if (g_store_path_len == 0) {
    return false;
  }
  if (buffer != NULL) {
    if (buffer_size < (size_t)(g_store_path_len + 1)) {
      return false;
    }
    memcpy(buffer, g_store_path, g_store_path_len);
    buffer[g_store_path_len] = '\0';
  }
  return true;
}

int
oc_storage_reset(void)
{
  g_store_path_len = 0;
  g_store_path[0] = '\0';
  return 0;
}

static int
storage_open(const char *store, FILE **fp)
{
  if (g_store_path_len == 0) {
    return -ENOENT;
  }

  size_t store_len = oc_strnlen_s(store, OC_STORE_PATH_SIZE);
  if ((store_len == 0) ||
      (store_len + g_store_path_len >= OC_STORE_PATH_SIZE)) {
    return -ENOENT;
  }
  memcpy(g_store_path + g_store_path_len, store, store_len);
  g_store_path[g_store_path_len + store_len] = '\0';

  FILE *file = fopen(g_store_path, "rb");
  if (file == NULL) {
    return -EINVAL;
  }
  *fp = file;
  return 0;
}

long
oc_storage_size(const char *store)
{
  FILE *fp = NULL;
  int ret = storage_open(store, &fp);
  if (ret != 0) {
    return ret;
  }

  if (fseek(fp, 0, SEEK_END) != 0) {
    fclose(fp);
    return -errno;
  }
  long fsize = ftell(fp);
  if (fsize < 0) {
    fclose(fp);
    return -errno;
  }
  fclose(fp);
  return fsize;
}

long
oc_storage_read(const char *store, uint8_t *buf, size_t size)
{
  FILE *fp = NULL;
  int ret = storage_open(store, &fp);
  if (ret != 0) {
    return ret;
  }

  if (fseek(fp, 0, SEEK_END) != 0) {
    goto error;
  }
  long fsize = ftell(fp);
  if (fsize < 0) {
    goto error;
  }
  if ((size_t)fsize > size) {
    errno = EINVAL;
    goto error;
  }
  if (fseek(fp, 0, SEEK_SET) != 0) {
    goto error;
  }

  size = fread(buf, 1, size, fp);
  fclose(fp);
  return (long)size;

error:
  fclose(fp);
  return -errno;
}

long
oc_storage_write(const char *store, const uint8_t *buf, size_t size)
{
  if (g_store_path_len == 0) {
    return -ENOENT;
  }

  size_t store_len = oc_strnlen_s(store, OC_STORE_PATH_SIZE);
  if ((store_len == 0) ||
      (store_len + g_store_path_len >= OC_STORE_PATH_SIZE)) {
    return -ENOENT;
  }
  memcpy(g_store_path + g_store_path_len, store, store_len);
  g_store_path[g_store_path_len + store_len] = '\0';

  FILE *fp = fopen(g_store_path, "wb");
  if (!fp)
    return -EINVAL;

  size = fwrite(buf, 1, size, fp);
  fclose(fp);
  return size;
}
#endif /* OC_STORAGE */
