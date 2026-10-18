#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for api/oc_core_res_internal.h */
#ifndef STUB_API_OC_CORE_RES_INTERNAL_H_4404A29D
#define STUB_API_OC_CORE_RES_INTERNAL_H_4404A29D
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_api_oc_core_res_internal_h_4404a29d;
#endif
/* stub for api/oc_helpers_internal.h */
#ifndef STUB_API_OC_HELPERS_INTERNAL_H_B708C3C5
#define STUB_API_OC_HELPERS_INTERNAL_H_B708C3C5
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_api_oc_helpers_internal_h_b708c3c5;
#endif
/* stub for api/oc_resource_internal.h */
#ifndef STUB_API_OC_RESOURCE_INTERNAL_H_8AC92DC3
#define STUB_API_OC_RESOURCE_INTERNAL_H_8AC92DC3
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_api_oc_resource_internal_h_8ac92dc3;
#endif
/* stub for mbedtls/x509_crt.h */
#ifndef STUB_MBEDTLS_X509_CRT_H_B73E1CFA
#define STUB_MBEDTLS_X509_CRT_H_B73E1CFA
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_mbedtls_x509_crt_h_b73e1cfa;
#endif
/* stub for oc_config.h */
#ifndef STUB_OC_CONFIG_H_CDACE0BE
#define STUB_OC_CONFIG_H_CDACE0BE
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_oc_config_h_cdace0be;
#endif
/* stub for port/oc_log_internal.h */
#ifndef STUB_PORT_OC_LOG_INTERNAL_H_769FF793
#define STUB_PORT_OC_LOG_INTERNAL_H_769FF793
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_port_oc_log_internal_h_769ff793;
#endif
/* stub for security/oc_cred_util_internal.h */
#ifndef STUB_SECURITY_OC_CRED_UTIL_INTERNAL_H_83759BB4
#define STUB_SECURITY_OC_CRED_UTIL_INTERNAL_H_83759BB4
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_security_oc_cred_util_internal_h_83759bb4;
#endif
/* stub for security/oc_roles_internal.h */
#ifndef STUB_SECURITY_OC_ROLES_INTERNAL_H_9D20A4F8
#define STUB_SECURITY_OC_ROLES_INTERNAL_H_9D20A4F8
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_security_oc_roles_internal_h_9d20a4f8;
#endif
/* stub for security/oc_tls_internal.h */
#ifndef STUB_SECURITY_OC_TLS_INTERNAL_H_4E094DD9
#define STUB_SECURITY_OC_TLS_INTERNAL_H_4E094DD9
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_security_oc_tls_internal_h_4e094dd9;
#endif
#endif

/****************************************************************************
 *
 * Copyright (c) 2018-2019 Intel Corporation
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


#if defined(OC_SECURITY) && defined(OC_PKI)



#include <errno.h>
#include <stdlib.h>

#ifdef OC_CLIENT

OC_MEMB(g_role_cred_s, oc_role_t, OC_ROLES_NUM_ROLE_CREDS);
OC_LIST(g_role_creds);

static oc_role_t *
role_cred_allocate(oc_string_view_t role, oc_string_view_t authority)
{
  if (role.data == NULL || authority.data == NULL) {
    OC_ERR("invalid input");
    return NULL;
  }
  oc_role_t *role_cred = (oc_role_t *)oc_memb_alloc(&g_role_cred_s);
  if (role_cred == NULL) {
    OC_ERR("failed to allocate role cred");
    return NULL;
  }
  oc_new_string(&role_cred->role, role.data, role.length);
  oc_new_string(&role_cred->authority, authority.data, authority.length);
  oc_list_add(g_role_creds, role_cred);
  return role_cred;
}

static oc_role_t *
role_cred_find(oc_string_view_t role, oc_string_view_t authority)
{
  oc_role_t *role_cred = (oc_role_t *)oc_list_head(g_role_creds);
  while (role_cred != NULL) {
    if ((role.data != NULL &&
         oc_string_view_is_equal(oc_string_view2(&role_cred->role), role)) &&
        (authority.data != NULL &&
         oc_string_view_is_equal(oc_string_view2(&role_cred->authority),
                                 authority))) {
      return role_cred;
    }
    role_cred = role_cred->next;
  }
  return NULL;
}

oc_role_t *
oc_sec_role_cred_add_or_get(oc_string_view_t role, oc_string_view_t authority)
{
  oc_role_t *role_cred = role_cred_find(role, authority);
  if (!role_cred) {
    role_cred = role_cred_allocate(role, authority);
  }
  return role_cred;
}

static void
role_cred_free(oc_role_t *role_cred)
{
  oc_free_string(&role_cred->authority);
  oc_free_string(&role_cred->role);
  oc_memb_free(&g_role_cred_s, role_cred);
}

bool
oc_sec_role_cred_remove(oc_string_view_t role, oc_string_view_t authority)
{
  oc_role_t *role_cred = role_cred_find(role, authority);
  if (role_cred != NULL) {
    oc_list_remove(g_role_creds, role_cred);
    role_cred_free(role_cred);
    return true;
  }
  return false;
}

oc_role_t *
oc_sec_role_creds_get(void)
{
  return oc_list_head(g_role_creds);
}

void
oc_sec_role_creds_free(void)
{
  oc_role_t *role_cred = (oc_role_t *)oc_list_pop(g_role_creds);
  while (role_cred != NULL) {
    role_cred_free(role_cred);
    role_cred = (oc_role_t *)oc_list_pop(g_role_creds);
  }
}

#endif /* OC_CLIENT */

typedef struct oc_sec_roles_t
{
  struct oc_sec_roles_t *next;
  OC_LIST_STRUCT(roles);
  const oc_tls_peer_t *client;
  size_t device;
} oc_sec_roles_t;

OC_MEMB(g_x509_crt_s, mbedtls_x509_crt, OCF_SEC_ROLES_MAX_NUM);
OC_MEMB(g_roles_s, oc_sec_cred_t, OCF_SEC_ROLES_MAX_NUM);
OC_MEMB(g_clients_s, oc_sec_roles_t, OC_MAX_NUM_DEVICES);
OC_LIST(g_clients);

static oc_sec_roles_t *
roles_get_for_client(const oc_tls_peer_t *client)
{
  oc_sec_roles_t *roles = (oc_sec_roles_t *)oc_list_head(g_clients);
  while (roles != NULL) {
    if (roles->client == client) {
      return roles;
    }
    roles = roles->next;
  }
  return NULL;
}

static oc_sec_roles_t *
roles_add_for_client(const oc_tls_peer_t *client, size_t device)
{
  oc_sec_roles_t *roles = (oc_sec_roles_t *)oc_memb_alloc(&g_clients_s);
  if (roles == NULL) {
    OC_ERR("insufficient memory to allocate roles");
    return NULL;
  }
  roles->device = device;
  roles->client = client;
  OC_LIST_STRUCT_INIT(roles, roles);
  oc_list_add(g_clients, roles);
  return roles;
}

static oc_sec_cred_t *
role_add(oc_sec_roles_t *roles)
{
  oc_sec_cred_t *role = (oc_sec_cred_t *)oc_memb_alloc(&g_roles_s);
  if (role == NULL) {
    OC_ERR("insufficient memory to allocate role");
    return NULL;
  }
  role->ctx = oc_memb_alloc(&g_x509_crt_s);
  if (role->ctx == NULL) {
    OC_ERR("insufficient memory to allocate role context");
    oc_memb_free(&g_roles_s, role);
    return NULL;
  }
  mbedtls_x509_crt_init(role->ctx);
  oc_list_add(roles->roles, role);
  return role;
}

oc_sec_cred_t *
oc_sec_roles_add(const oc_tls_peer_t *client, size_t device)
{
  oc_sec_roles_t *roles = roles_get_for_client(client);
  if (roles == NULL) {
    roles = roles_add_for_client(client, device);
  }
  if (roles == NULL) {
    return NULL;
  }
  return role_add(roles);
}

oc_sec_cred_t *
oc_sec_roles_get(const oc_tls_peer_t *client)
{
  oc_sec_roles_t *roles = roles_get_for_client(client);
  if (roles != NULL) {
    return (oc_sec_cred_t *)oc_list_head(roles->roles);
  }
  return NULL;
}

static void
sec_free_role(oc_sec_cred_t *cred)
{
  mbedtls_x509_crt_free(cred->ctx);
  oc_memb_free(&g_x509_crt_s, cred->ctx);
  oc_free_string(&cred->role.role);
  oc_free_string(&cred->role.authority);
  oc_free_string(&cred->publicdata.data);
  oc_memb_free(&g_roles_s, cred);
}

bool
oc_sec_free_role(const oc_sec_cred_t *role, const oc_tls_peer_t *client)
{
  oc_sec_roles_t *roles = roles_get_for_client(client);
  if (roles == NULL) {
    return false;
  }
  oc_sec_cred_t *r = (oc_sec_cred_t *)oc_list_head(roles->roles);
  while (r != NULL) {
    if (role == r) {
      oc_list_remove(roles->roles, r);
      sec_free_role(r);
      return true;
    }
    r = r->next;
  }
  return false;
}

int
oc_sec_free_roles_for_device(size_t device)
{
  int removed = 0;
  oc_sec_roles_t *roles = (oc_sec_roles_t *)oc_list_head(g_clients);
  while (roles != NULL) {
    oc_sec_roles_t *next = roles->next;
    if (roles->device == device) {
      removed += oc_sec_free_roles(roles->client);
    }
    roles = next;
  }
  return removed;
}

int
oc_sec_free_roles(const oc_tls_peer_t *client)
{
  oc_sec_roles_t *roles = roles_get_for_client(client);
  if (roles == NULL) {
    return 0;
  }

  int removed = 0;
  oc_sec_cred_t *r = (oc_sec_cred_t *)oc_list_pop(roles->roles);
  while (r != NULL) {
    sec_free_role(r);
    ++removed;
    r = (oc_sec_cred_t *)oc_list_pop(roles->roles);
  }
  oc_list_remove(g_clients, roles);
  oc_memb_free(&g_clients_s, roles);
  return removed;
}

bool
oc_sec_free_role_by_credid(int credid, const oc_tls_peer_t *client)
{
  oc_sec_roles_t *roles = roles_get_for_client(client);
  if (roles == NULL) {
    return false;
  }
  oc_sec_cred_t *r = (oc_sec_cred_t *)oc_list_head(roles->roles);
  while (r != NULL) {
    if (r->credid == credid) {
      oc_list_remove(roles->roles, r);
      sec_free_role(r);
      return true;
    }
    r = r->next;
  }
  return false;
}

static void
oc_sec_encode_roles(const oc_tls_peer_t *client, size_t device,
                    oc_interface_mask_t iface_mask)
{
  oc_rep_start_root_object();
  if ((iface_mask & OC_IF_BASELINE) != 0) {
    oc_process_baseline_interface(
      oc_core_get_resource_by_index(OCF_SEC_ROLES, device));
  }
  oc_rep_set_array(root, roles);
  oc_sec_cred_t *cr = client != NULL ? oc_sec_roles_get(client) : NULL;
  while (cr != NULL) {
    oc_rep_object_array_start_item(roles);
    /* credid */
    oc_rep_set_int(roles, credid, cr->credid);
    /* credtype */
    oc_rep_set_int(roles, credtype, cr->credtype);
    /* credusage */
    oc_string_view_t credusage_string =
      oc_cred_credusage_to_string(cr->credusage);
    if (credusage_string.length > 4) {
      oc_rep_set_text_string_v1(roles, credusage, credusage_string.data,
                                credusage_string.length);
    }
    /* publicdata */
    if (oc_string_len(cr->publicdata.data) > 0) {
      oc_rep_set_object(roles, publicdata);
      if (cr->publicdata.encoding == OC_ENCODING_PEM) {
        oc_rep_set_text_string_v1(publicdata, data,
                                  oc_string(cr->publicdata.data),
                                  oc_string_len(cr->publicdata.data));
      } else {
        oc_rep_set_byte_string(publicdata, data,
                               oc_cast(cr->publicdata.data, const uint8_t),
                               oc_string_len(cr->publicdata.data));
      }
      oc_string_view_t encoding_string =
        oc_cred_encoding_to_string(cr->publicdata.encoding);
      if (encoding_string.length > 7) {
        oc_rep_set_text_string_v1(publicdata, encoding, encoding_string.data,
                                  encoding_string.length);
      }
      oc_rep_close_object(roles, publicdata);
    }
    oc_rep_object_array_end_item(roles);
    cr = cr->next;
  }
  oc_rep_close_array(root, roles);
  oc_rep_end_root_object();
}

static void
roles_resource_get(oc_request_t *request, oc_interface_mask_t iface_mask,
                   void *data)
{
  (void)data;
  const oc_tls_peer_t *client = oc_tls_get_peer(request->origin);
  oc_sec_encode_roles(client, request->resource->device, iface_mask);
  oc_send_response_with_callback(request, OC_STATUS_OK, true);
}

static void
roles_resource_post(oc_request_t *request, oc_interface_mask_t iface_mask,
                    void *data)
{
  (void)iface_mask;
  (void)data;
  if (oc_sec_apply_cred(request->request_payload, request->resource,
                        request->origin,
                        /*on_apply_cred_cb*/ NULL,
                        /*on_apply_cred_data*/ NULL) != 0) {
    oc_send_response_with_callback(request, OC_STATUS_BAD_REQUEST, true);
    return;
  }
  oc_send_response_with_callback(request, OC_STATUS_CHANGED, true);
}

static void
roles_resource_delete(oc_request_t *request, oc_interface_mask_t iface_mask,
                      void *data)
{
  (void)iface_mask;
  (void)data;
  const oc_tls_peer_t *client = oc_tls_get_peer(request->origin);
  if (client == NULL) {
    OC_ERR("cannot delete roles credential: invalid client");
    oc_send_response_with_callback(request, OC_STATUS_NOT_FOUND, true);
    return;
  }
  const char *query_param = NULL;
  int ret = oc_get_query_value_v1(request, "credid",
                                  OC_CHAR_ARRAY_LEN("credid"), &query_param);
  if (ret == -1) {
    // no query param, delete all roles
    oc_sec_free_roles(client);
    oc_send_response_with_callback(request, OC_STATUS_DELETED, true);
    return;
  }

  errno = 0;
  long credid =
    strtol(query_param, NULL, 10); // NOLINT(readability-magic-numbers)
  if (errno != 0 || credid > INT32_MAX || credid < 0 ||
      !oc_sec_free_role_by_credid((int)credid, client)) {
    OC_ERR("cannot delete roles credential: invalid credid(%ld)", credid);
    oc_send_response_with_callback(request, OC_STATUS_NOT_FOUND, true);
    return;
  }
  oc_send_response_with_callback(request, OC_STATUS_DELETED, true);
}

void
oc_sec_roles_create_resource(size_t device)
{
  oc_core_populate_resource(OCF_SEC_ROLES, device, OCF_SEC_ROLES_URI,
                            (oc_interface_mask_t)OCF_SEC_ROLES_IF_MASK,
                            (oc_interface_mask_t)OCF_SEC_ROLES_DEFAULT_IF,
                            OC_DISCOVERABLE | OC_SECURE, roles_resource_get,
                            /*put*/ NULL, roles_resource_post,
                            roles_resource_delete, 1, OCF_SEC_ROLES_RT);
}

bool
oc_sec_is_roles_resource_uri(oc_string_view_t uri)
{
  return oc_resource_match_uri(OC_STRING_VIEW(OCF_SEC_ROLES_URI), uri);
}

#endif /* OC_SECURITY && OC_PKI */
