#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for api/cloud/oc_cloud_context_internal.h */
#ifndef STUB_API_CLOUD_OC_CLOUD_CONTEXT_INTERNAL_H_F2E84CD4
#define STUB_API_CLOUD_OC_CLOUD_CONTEXT_INTERNAL_H_F2E84CD4
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_api_cloud_oc_cloud_context_internal_h_f2e84cd4;
#endif
/* stub for api/cloud/oc_cloud_resource_internal.h */
#ifndef STUB_API_CLOUD_OC_CLOUD_RESOURCE_INTERNAL_H_42A29135
#define STUB_API_CLOUD_OC_CLOUD_RESOURCE_INTERNAL_H_42A29135
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_api_cloud_oc_cloud_resource_internal_h_42a29135;
#endif
/* stub for api/oc_core_res_internal.h */
#ifndef STUB_API_OC_CORE_RES_INTERNAL_H_4404A29D
#define STUB_API_OC_CORE_RES_INTERNAL_H_4404A29D
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_api_oc_core_res_internal_h_4404a29d;
#endif
/* stub for api/oc_rep_internal.h */
#ifndef STUB_API_OC_REP_INTERNAL_H_B667C81B
#define STUB_API_OC_REP_INTERNAL_H_B667C81B
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_api_oc_rep_internal_h_b667c81b;
#endif
/* stub for oc_api.h */
#ifndef STUB_OC_API_H_80DCE115
#define STUB_OC_API_H_80DCE115
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_oc_api_h_80dce115;
#endif
/* stub for oc_cloud_internal.h */
#ifndef STUB_OC_CLOUD_INTERNAL_H_55D6BD39
#define STUB_OC_CLOUD_INTERNAL_H_55D6BD39
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_oc_cloud_internal_h_55d6bd39;
#endif
/* stub for oc_cloud_log_internal.h */
#ifndef STUB_OC_CLOUD_LOG_INTERNAL_H_2EED9821
#define STUB_OC_CLOUD_LOG_INTERNAL_H_2EED9821
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_oc_cloud_log_internal_h_2eed9821;
#endif
/* stub for oc_cloud_store_internal.h */
#ifndef STUB_OC_CLOUD_STORE_INTERNAL_H_1E35AAAA
#define STUB_OC_CLOUD_STORE_INTERNAL_H_1E35AAAA
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_oc_cloud_store_internal_h_1e35aaaa;
#endif
/* stub for oc_config.h */
#ifndef STUB_OC_CONFIG_H_CDACE0BE
#define STUB_OC_CONFIG_H_CDACE0BE
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_oc_config_h_cdace0be;
#endif
/* stub for oc_core_res.h */
#ifndef STUB_OC_CORE_RES_H_31CD5F46
#define STUB_OC_CORE_RES_H_31CD5F46
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_oc_core_res_h_31cd5f46;
#endif
/* stub for util/oc_endpoint_address_internal.h */
#ifndef STUB_UTIL_OC_ENDPOINT_ADDRESS_INTERNAL_H_A623DCA3
#define STUB_UTIL_OC_ENDPOINT_ADDRESS_INTERNAL_H_A623DCA3
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_util_oc_endpoint_address_internal_h_a623dca3;
#endif
#endif

/****************************************************************************
 *
 * Copyright 2019 Jozef Kralik All Rights Reserved.
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


#ifdef OC_CLOUD


oc_string_view_t
oc_cps_to_string(oc_cps_t cps)
{
  switch (cps) {
  case OC_CPS_UNINITIALIZED:
    return OC_STRING_VIEW(OC_CPS_UNINITIALIZED_STR);
  case OC_CPS_READYTOREGISTER:
    return OC_STRING_VIEW(OC_CPS_READYTOREGISTER_STR);
  case OC_CPS_REGISTERING:
    return OC_STRING_VIEW(OC_CPS_REGISTERING_STR);
  case OC_CPS_REGISTERED:
    return OC_STRING_VIEW(OC_CPS_REGISTERED_STR);
  case OC_CPS_FAILED:
    return OC_STRING_VIEW(OC_CPS_FAILED_STR);
  case OC_CPS_DEREGISTERING:
    return OC_STRING_VIEW(OC_CPS_DEREGISTERING_STR);
  default:
    break;
  }
  return OC_STRING_VIEW_NULL;
}

bool
oc_cloud_encode(const oc_cloud_context_t *ctx)
{
  OC_CLOUD_DBG("Creating Cloud Response");
  oc_rep_start_root_object();

  const oc_resource_t *cloud_conf =
    oc_core_get_resource_by_index(OCF_COAPCLOUDCONF, ctx->device);
  oc_process_baseline_interface(cloud_conf);

  oc_string_view_t auth_provider = oc_string_view2(&ctx->store.auth_provider);
  oc_rep_set_text_string_v1(root, apn, auth_provider.data,
                            auth_provider.length);

  oc_string_view_t cis =
    oc_string_view2(oc_endpoint_addresses_selected_uri(&ctx->store.ci_servers));
  oc_rep_set_text_string_v1(root, cis, cis.data, cis.length);

  const oc_uuid_t *sid =
    oc_endpoint_addresses_selected_uuid(&ctx->store.ci_servers);
  char sid_str[OC_UUID_LEN] = { 0 };
  int sid_str_len = 0;
  if (sid != NULL) {
    sid_str_len = oc_uuid_to_str_v1(sid, sid_str, OC_ARRAY_SIZE(sid_str));
    assert(sid_str_len > 0);
  }
  oc_rep_set_text_string_v1(root, sid, sid_str, sid_str_len);

  g_err |= oc_endpoint_addresses_encode(
    oc_rep_object(root), &ctx->store.ci_servers,
    OC_STRING_VIEW(OCF_COAPCLOUDCONF_PROP_CISERVERS), true);

  oc_rep_set_int(root, clec, (int)ctx->last_error);

  OC_CLOUD_DBG(
    "Creating Cloud Response: auth provider=%s, cis=%s, sid=%s, clec=%d",
    auth_provider.data != NULL ? auth_provider.data : "",
    cis.data != NULL ? cis.data : "", sid_str, (int)ctx->last_error);

  oc_string_view_t cps = oc_cps_to_string(ctx->store.cps);
  if (cps.length > 0) {
    OC_CLOUD_DBG("Creating Cloud Response: cps=%s", cps.data);
    oc_rep_set_text_string_v1(root, cps, cps.data, cps.length);
  }

  oc_rep_end_root_object();
  return oc_rep_get_cbor_errno() == CborNoError;
}

static void
cloud_resource_get(oc_request_t *request, oc_interface_mask_t interface,
                   void *user_data)
{
  (void)user_data;
  (void)interface;
  OC_CLOUD_DBG("GET request received");

  const oc_cloud_context_t *ctx =
    oc_cloud_get_context(request->resource->device);
  if (ctx == NULL || !oc_cloud_encode(ctx)) {
    oc_send_response_with_callback(request, OC_STATUS_INTERNAL_SERVER_ERROR,
                                   true);
    return;
  }

  oc_send_response_with_callback(request, OC_STATUS_OK, true);
}

static const oc_string_t *
resource_payload_get_string_property(const oc_rep_t *payload,
                                     oc_string_view_t key, bool cannotBeEmpty)
{
  const oc_rep_t *rep =
    oc_rep_get_by_type_and_key(payload, OC_REP_STRING, key.data, key.length);
  if (rep == NULL ||
      (cannotBeEmpty && oc_string_is_empty(&rep->value.string))) {
    return NULL;
  }
  return &rep->value.string;
}

static bool
cloud_update_from_request(oc_cloud_context_t *ctx, const oc_request_t *request)
{
  oc_cloud_conf_update_t data;
  memset(&data, 0, sizeof(data));

  data.access_token = resource_payload_get_string_property(
    request->request_payload,
    OC_STRING_VIEW(OCF_COAPCLOUDCONF_PROP_ACCESSTOKEN), true);
  data.auth_provider = resource_payload_get_string_property(
    request->request_payload,
    OC_STRING_VIEW(OCF_COAPCLOUDCONF_PROP_AUTHPROVIDER), true);
  data.ci_server = resource_payload_get_string_property(
    request->request_payload, OC_STRING_VIEW(OCF_COAPCLOUDCONF_PROP_CISERVER),
    false);

  // OCF 2.0 spec version added sid property.
  const oc_string_t *sid = resource_payload_get_string_property(
    request->request_payload, OC_STRING_VIEW(OCF_COAPCLOUDCONF_PROP_SERVERID),
    true);
  if (sid != NULL) {
    oc_string_view_t sidv = oc_string_view2(sid);
    if (oc_str_to_uuid_v1(sidv.data, sidv.length, &data.sid) < 0) {
      OC_CLOUD_ERR("failed parsing sid(%s)", sidv.data);
      return false;
    }
  }

  const oc_rep_t *ci_servers = oc_rep_get_by_type_and_key(
    request->request_payload, OC_REP_OBJECT_ARRAY,
    OCF_COAPCLOUDCONF_PROP_CISERVERS,
    OC_CHAR_ARRAY_LEN(OCF_COAPCLOUDCONF_PROP_CISERVERS));
  if (ci_servers != NULL) {
    data.ci_servers = ci_servers->value.object_array;
  }

  if (data.ci_server != NULL && (oc_string_is_empty(data.ci_server) ||
                                 (data.access_token != NULL && sid != NULL))) {
    oc_cloud_update_by_resource(ctx, &data);
    return true;
  }
  return false;
}

static void
cloud_resource_post(oc_request_t *request, oc_interface_mask_t interface,
                    void *user_data)
{
  (void)user_data;
  (void)interface;
  OC_CLOUD_DBG("POST request received");

  oc_cloud_context_t *ctx = oc_cloud_get_context(request->resource->device);
  if (ctx == NULL) {
    oc_send_response_with_callback(request, OC_STATUS_INTERNAL_SERVER_ERROR,
                                   true);
    return;
  }

  bool request_invalid_in_state = true;
  switch (ctx->store.cps) {
  case OC_CPS_UNINITIALIZED:
  case OC_CPS_READYTOREGISTER:
  case OC_CPS_FAILED:
  case OC_CPS_DEREGISTERING:
    request_invalid_in_state = false;
    break;
  case OC_CPS_REGISTERING:
  case OC_CPS_REGISTERED: {
    // Update allowed but only for a "cis" of empty string
    //
    char *cis;
    size_t cis_len = 0;
    if (oc_rep_get_string(request->request_payload,
                          OCF_COAPCLOUDCONF_PROP_CISERVER, &cis, &cis_len) &&
        cis_len == 0) {
      request_invalid_in_state = false;
    }
  }
  }
  if (request_invalid_in_state) {
    oc_send_response_with_callback(request, OC_STATUS_BAD_REQUEST, true);
    return;
  }

  char *cps;
  size_t cps_len = 0;
  if (oc_rep_get_string(request->request_payload,
                        OCF_COAPCLOUDCONF_PROP_PROVISIONINGSTATUS, &cps,
                        &cps_len)) {
    oc_send_response_with_callback(request, OC_STATUS_BAD_REQUEST, true);
    return;
  }

  if (!cloud_update_from_request(ctx, request) || !oc_cloud_encode(ctx)) {
    oc_send_response_with_callback(request, OC_STATUS_BAD_REQUEST, true);
    return;
  }

  oc_send_response_with_callback(request, OC_STATUS_CHANGED, true);
  oc_cloud_store_dump_async(&ctx->store);
}

void
oc_create_cloudconf_resource(size_t device)
{
  OC_CLOUD_DBG("oc_cloud_resource: Initializing CoAPCloudConf resource");

  oc_core_populate_resource(
    OCF_COAPCLOUDCONF, device, OCF_COAPCLOUDCONF_URI, OCF_COAPCLOUDCONF_IF_MASK,
    OCF_COAPCLOUDCONF_DEFAULT_IF, OC_SECURE | OC_DISCOVERABLE | OC_OBSERVABLE,
    cloud_resource_get, /*put*/ NULL, cloud_resource_post, /*delete*/ NULL, 1,
    OCF_COAPCLOUDCONF_RT);
}

#endif /* OC_CLOUD */
