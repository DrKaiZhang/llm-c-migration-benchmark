#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for debug.h */
#ifndef STUB_DEBUG_H_B05E89E5
#define STUB_DEBUG_H_B05E89E5
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_debug_h_b05e89e5;
#endif
/* stub for net/gnrc/netif/hdr.h */
#ifndef STUB_NET_GNRC_NETIF_HDR_H_AB406EBF
#define STUB_NET_GNRC_NETIF_HDR_H_AB406EBF
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_net_gnrc_netif_hdr_h_ab406ebf;
#endif
/* stub for net/gnrc/netif/raw.h */
#ifndef STUB_NET_GNRC_NETIF_RAW_H_83AE9387
#define STUB_NET_GNRC_NETIF_RAW_H_83AE9387
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_net_gnrc_netif_raw_h_83ae9387;
#endif
/* stub for net/gnrc/pktbuf.h */
#ifndef STUB_NET_GNRC_PKTBUF_H_1E9B798C
#define STUB_NET_GNRC_PKTBUF_H_1E9B798C
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_net_gnrc_pktbuf_h_1e9b798c;
#endif
/* stub for utlist.h */
#ifndef STUB_UTLIST_H_6AA2FA60
#define STUB_UTLIST_H_6AA2FA60
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_utlist_h_6aa2fa60;
#endif
#endif

/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */


#define ENABLE_DEBUG    0

#define IP_VERSION_MASK (0xf0U)
#define IP_VERSION4     (0x40U)
#define IP_VERSION6     (0x60U)

static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt);
static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif);

static const gnrc_netif_ops_t raw_ops = {
    .init = gnrc_netif_default_init,
    .send = _send,
    .recv = _recv,
    .get = gnrc_netif_get_from_netdev,
    .set = gnrc_netif_set_from_netdev,
};

int gnrc_netif_raw_create(gnrc_netif_t *netif, char *stack, int stacksize,
                          char priority, char *name, netdev_t *dev)
{
    return gnrc_netif_create(netif, stack, stacksize, priority, name, dev,
                             &raw_ops);
}

static inline uint8_t _get_version(uint8_t *data)
{
    return (data[0] & IP_VERSION_MASK);
}

static gnrc_pktsnip_t *_recv(gnrc_netif_t *netif)
{
    netdev_t *dev = netif->dev;
    int bytes_expected = dev->driver->recv(dev, NULL, 0, NULL);
    gnrc_pktsnip_t *pkt = NULL;

    if (bytes_expected > 0) {
        gnrc_pktsnip_t *hdr;
        int nread;

        pkt = gnrc_pktbuf_add(NULL, NULL, bytes_expected, GNRC_NETTYPE_UNDEF);

        if (!pkt) {
            DEBUG("gnrc_netif_raw: cannot allocate pktsnip.\n");
            /* drop packet */
            dev->driver->recv(dev, NULL, bytes_expected, NULL);
            return pkt;
        }
        nread = dev->driver->recv(dev, pkt->data, bytes_expected, NULL);
        if (nread <= 1) {   /* we need at least 1 byte to identify IP version */
            DEBUG("gnrc_netif_raw: read error.\n");
            gnrc_pktbuf_release(pkt);
            return NULL;
        }
        hdr = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
        if (!hdr) {
            DEBUG("gnrc_netif_raw: cannot allocate pktsnip.\n");
            gnrc_pktbuf_release(pkt);
            return NULL;
        }
        gnrc_netif_hdr_set_netif(hdr->data, netif);
        LL_APPEND(pkt, hdr);
#ifdef MODULE_NETSTATS_L2
        netif->stats.rx_count++;
        netif->stats.rx_bytes += nread;
#endif

        if (nread < bytes_expected) {
            /* we've got less then the expected packet size,
             * so free the unused space.*/
            DEBUG("gnrc_netif_raw: reallocating.\n");
            gnrc_pktbuf_realloc_data(pkt, nread);
        }
        switch (_get_version(pkt->data)) {
#ifdef MODULE_GNRC_IPV6
            case IP_VERSION6:
                pkt->type = GNRC_NETTYPE_IPV6;
                break;
#endif
            default:
                /* leave UNDEF */
                break;
        }
    }
    return pkt;
}

static gnrc_pktsnip_t *_skip_pkt_head(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    if (gnrc_netif_netdev_legacy_api(netif)) {
        /* we don't need the netif snip: remove it */
        return gnrc_pktbuf_remove_snip(pkt, pkt);
    }
    else {
        /* _tx_done() will free the entire list */
        return pkt->next;
    }
}

static int _send(gnrc_netif_t *netif, gnrc_pktsnip_t *pkt)
{
    int res = -ENOBUFS;

    if (pkt->type == GNRC_NETTYPE_NETIF) {
        pkt = _skip_pkt_head(netif, pkt);
    }

    netdev_t *dev = netif->dev;

#ifdef MODULE_NETSTATS_L2
    netif->stats.tx_unicast_count++;
#endif

    res = dev->driver->send(dev, (iolist_t *)pkt);
    if (gnrc_netif_netdev_legacy_api(netif)) {
        /* only for legacy drivers we need to release pkt here */
        gnrc_pktbuf_release(pkt);
    }
    return res;
}

/** @} */
