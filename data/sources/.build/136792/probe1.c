#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for debug.h */
#ifndef STUB_DEBUG_H_B05E89E5
#define STUB_DEBUG_H_B05E89E5
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_debug_h_b05e89e5;
#endif
/* stub for encx24j600.h */
#ifndef STUB_ENCX24J600_H_E767CF04
#define STUB_ENCX24J600_H_E767CF04
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_encx24j600_h_e767cf04;
#endif
/* stub for encx24j600_params.h */
#ifndef STUB_ENCX24J600_PARAMS_H_3ED3EDD8
#define STUB_ENCX24J600_PARAMS_H_3ED3EDD8
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_encx24j600_params_h_3ed3edd8;
#endif
/* stub for include/init_devs.h */
#ifndef STUB_INCLUDE_INIT_DEVS_H_54A29665
#define STUB_INCLUDE_INIT_DEVS_H_54A29665
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_include_init_devs_h_54a29665;
#endif
/* stub for log.h */
#ifndef STUB_LOG_H_A26B3C1A
#define STUB_LOG_H_A26B3C1A
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_log_h_a26b3c1a;
#endif
/* stub for net/gnrc/netif/ethernet.h */
#ifndef STUB_NET_GNRC_NETIF_ETHERNET_H_C922B044
#define STUB_NET_GNRC_NETIF_ETHERNET_H_C922B044
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_net_gnrc_netif_ethernet_h_c922b044;
#endif
#endif

/*
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for ENCx24j600 ethernet devices
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */


/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define ENCX24J600_MAC_STACKSIZE    (GNRC_NETIF_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#ifndef ENCX24J600_MAC_PRIO
#define ENCX24J600_MAC_PRIO         (GNRC_NETIF_PRIO)
#endif

/**
 * @brief   Find out how many of these devices are present
 */
#define ENCX24J600_NUM    ARRAY_SIZE(encx24j600_params)

/**
 * @brief Allocate device descriptors.
 */
static encx24j600_t encx24j600[ENCX24J600_NUM];

/**
 * @brief Allocate GNRC interfaces.
 */
static gnrc_netif_t _netif[ENCX24J600_NUM];

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _netdev_eth_stack[ENCX24J600_NUM][ENCX24J600_MAC_STACKSIZE];

void auto_init_encx24j600(void)
{
    for (unsigned i = 0; i < ENCX24J600_NUM; i++) {
        LOG_DEBUG("[auto_init_netif] initializing encx24j600 #%u\n", i);

        /* setup netdev device */
        encx24j600_setup(&encx24j600[i], &encx24j600_params[i], i);

        /* initialize netdev<->gnrc adapter state */
        gnrc_netif_ethernet_create(&_netif[i], _netdev_eth_stack[i], ENCX24J600_MAC_STACKSIZE,
                                   ENCX24J600_MAC_PRIO, "encx24j600",
                                   &encx24j600[i].netdev);
    }
}
/** @} */
