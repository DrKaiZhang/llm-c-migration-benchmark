#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for ifaddrs-android.h */
#ifndef STUB_IFADDRS_ANDROID_H_734290DA
#define STUB_IFADDRS_ANDROID_H_734290DA
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_ifaddrs_android_h_734290da;
#endif
/* stub for linux/netlink.h */
#ifndef STUB_LINUX_NETLINK_H_EBB1A9B0
#define STUB_LINUX_NETLINK_H_EBB1A9B0
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_linux_netlink_h_ebb1a9b0;
#endif
/* stub for linux/rtnetlink.h */
#ifndef STUB_LINUX_RTNETLINK_H_4BD342E4
#define STUB_LINUX_RTNETLINK_H_4BD342E4
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_linux_rtnetlink_h_4bd342e4;
#endif
/* stub for net/if.h */
#ifndef STUB_NET_IF_H_BE32BDA0
#define STUB_NET_IF_H_BE32BDA0
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_net_if_h_be32bda0;
#endif
/* stub for netinet/in.h */
#ifndef STUB_NETINET_IN_H_F2151659
#define STUB_NETINET_IN_H_F2151659
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_netinet_in_h_f2151659;
#endif
/* stub for sys/ioctl.h */
#ifndef STUB_SYS_IOCTL_H_6F0CA04C
#define STUB_SYS_IOCTL_H_6F0CA04C
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_sys_ioctl_h_6f0ca04c;
#endif
/* stub for sys/socket.h */
#ifndef STUB_SYS_SOCKET_H_A745C4BA
#define STUB_SYS_SOCKET_H_A745C4BA
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_sys_socket_h_a745c4ba;
#endif
/* stub for sys/types.h */
#ifndef STUB_SYS_TYPES_H_754A503C
#define STUB_SYS_TYPES_H_754A503C
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_sys_types_h_754a503c;
#endif
/* stub for sys/utsname.h */
#ifndef STUB_SYS_UTSNAME_H_CBF98D8E
#define STUB_SYS_UTSNAME_H_CBF98D8E
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_sys_utsname_h_cbf98d8e;
#endif
/* stub for unistd.h */
#ifndef STUB_UNISTD_H_5DAE45BB
#define STUB_UNISTD_H_5DAE45BB
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_unistd_h_5dae45bb;
#endif
/* stub for util/oc_macros_internal.h */
#ifndef STUB_UTIL_OC_MACROS_INTERNAL_H_78848549
#define STUB_UTIL_OC_MACROS_INTERNAL_H_78848549
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_util_oc_macros_internal_h_78848549;
#endif
#endif

/*
Copyright (c) 2011, The WebRTC project authors. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the
    distribution.

  * Neither the name of Google nor the names of its contributors may
    be used to endorse or promote products derived from this software
    without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <errno.h>
#include <stdlib.h>
#include <string.h>

struct netlinkrequest
{
  struct nlmsghdr header;
  struct ifaddrmsg msg;
};

static int
set_ifname(struct ifaddrs *ifaddr, int interface)
{
  char buf[IFNAMSIZ] = { 0 };
  char *name = if_indextoname(interface, buf);
  if (name == NULL) {
    return -1;
  }
  ifaddr->ifa_name = malloc(strlen(name) + 1);
  strncpy(ifaddr->ifa_name, name, strlen(name) + 1);
  return 0;
}

static int
set_flags(struct ifaddrs *ifaddr)
{
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd == -1) {
    return -1;
  }
  struct ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));
  strncpy(ifr.ifr_name, ifaddr->ifa_name, IFNAMSIZ - 1);
  int rc = ioctl(fd, SIOCGIFFLAGS, &ifr);
  close(fd);
  if (rc == -1) {
    return -1;
  }
  ifaddr->ifa_flags = ifr.ifr_flags;
  return 0;
}

static int
set_addresses(struct ifaddrs *ifaddr, struct ifaddrmsg *msg, void *data,
              size_t len)
{
  if (msg->ifa_family == AF_INET) {
    struct sockaddr_in *sa = malloc(sizeof(struct sockaddr_in));
    memset(sa, 0, sizeof(struct sockaddr_in));
    sa->sin_family = AF_INET;
    memcpy(&sa->sin_addr, data, len);
    ifaddr->ifa_addr = (struct sockaddr *)sa;
  } else if (msg->ifa_family == AF_INET6) {
    struct sockaddr_in6 *sa = malloc(sizeof(struct sockaddr_in6));
    memset(sa, 0, sizeof(struct sockaddr_in6));
    sa->sin6_family = AF_INET6;
    sa->sin6_scope_id = msg->ifa_index;
    memcpy(&sa->sin6_addr, data, len);
    ifaddr->ifa_addr = (struct sockaddr *)sa;
  } else {
    return -1;
  }
  return 0;
}

static int
make_prefixes(struct ifaddrs *ifaddr, int family, int prefixlen)
{
  unsigned char *prefix = NULL;
  if (family == AF_INET) {
    struct sockaddr_in *mask = malloc(sizeof(struct sockaddr_in));
    memset(mask, 0, sizeof(struct sockaddr_in));
    mask->sin_family = AF_INET;
    memset(&mask->sin_addr, 0, sizeof(struct in_addr));
    ifaddr->ifa_netmask = (struct sockaddr *)mask;
    if (prefixlen > 32) {
      prefixlen = 32;
    }
    prefix = (unsigned char *)&mask->sin_addr;
  } else if (family == AF_INET6) {
    struct sockaddr_in6 *mask = malloc(sizeof(struct sockaddr_in6));
    memset(mask, 0, sizeof(struct sockaddr_in6));
    mask->sin6_family = AF_INET6;
    memset(&mask->sin6_addr, 0, sizeof(struct in6_addr));
    ifaddr->ifa_netmask = (struct sockaddr *)mask;
    if (prefixlen > 128) {
      prefixlen = 128;
    }
    prefix = (unsigned char *)&mask->sin6_addr;
  } else {
    return -1;
  }
  for (int i = 0; i < (prefixlen / 8); i++) {
    *prefix++ = 0xFF;
  }
  unsigned char remainder = 0xFF;
  remainder <<= (8 - prefixlen % 8);
  *prefix = remainder;
  return 0;
}

static int
populate_ifaddrs(struct ifaddrs *ifaddr, struct ifaddrmsg *msg, void *bytes,
                 size_t len)
{
  if (set_ifname(ifaddr, msg->ifa_index) != 0) {
    return -1;
  }
  if (set_flags(ifaddr) != 0) {
    return -1;
  }
  if (set_addresses(ifaddr, msg, bytes, len) != 0) {
    return -1;
  }
  if (make_prefixes(ifaddr, msg->ifa_family, msg->ifa_prefixlen) != 0) {
    return -1;
  }
  return 0;
}

int
android_getifaddrs(struct ifaddrs **result)
{
  int fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  if (fd < 0) {
    return -1;
  }

  struct netlinkrequest ifaddr_request;
  memset(&ifaddr_request, 0, sizeof(ifaddr_request));
  ifaddr_request.header.nlmsg_flags = NLM_F_ROOT | NLM_F_REQUEST;
  ifaddr_request.header.nlmsg_type = RTM_GETADDR;
  ifaddr_request.header.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));

  ssize_t count = send(fd, &ifaddr_request, ifaddr_request.header.nlmsg_len, 0);
  if ((size_t)count != ifaddr_request.header.nlmsg_len) {
    close(fd);
    return -1;
  }
  struct ifaddrs *start = NULL;
  struct ifaddrs *current = NULL;
#define MAX_READ_SIZE (4096)
  char buf[MAX_READ_SIZE] = { 0 };
#undef MAX_READ_SIZE
  ssize_t amount_read = recv(fd, &buf, OC_ARRAY_SIZE(buf), 0);
  while (amount_read > 0) {
    struct nlmsghdr *header = (struct nlmsghdr *)&buf[0];
    size_t header_size = (size_t)amount_read;
    CLANG_IGNORE_WARNING_START
    CLANG_IGNORE_WARNING("-Wsign-compare")
    for (; NLMSG_OK(header, header_size);
         header = NLMSG_NEXT(header, header_size)) {
      CLANG_IGNORE_WARNING_END
      switch (header->nlmsg_type) {
      case NLMSG_DONE:
        /* Success. Return. */
        *result = start;
        close(fd);
        return 0;
      case NLMSG_ERROR:
        close(fd);
        android_freeifaddrs(start);
        return -1;
      case RTM_NEWADDR: {
        struct ifaddrmsg *address_msg = (struct ifaddrmsg *)NLMSG_DATA(header);
        struct rtattr *rta = IFA_RTA(address_msg);
        ssize_t payload_len = IFA_PAYLOAD(header);
        while (RTA_OK(rta, payload_len)) {
          if (rta->rta_type == IFA_ADDRESS) {
            int family = address_msg->ifa_family;
            if (family == AF_INET || family == AF_INET6) {
              struct ifaddrs *newest = malloc(sizeof(struct ifaddrs));
              memset(newest, 0, sizeof(struct ifaddrs));
              if (current) {
                current->ifa_next = newest;
              } else {
                start = newest;
              }
              if (populate_ifaddrs(newest, address_msg, RTA_DATA(rta),
                                   RTA_PAYLOAD(rta)) != 0) {
                android_freeifaddrs(start);
                *result = NULL;
                return -1;
              }
              current = newest;
            }
          }
          rta = RTA_NEXT(rta, payload_len);
        }
        break;
      }
      }
    }
    amount_read = recv(fd, &buf, OC_ARRAY_SIZE(buf), 0);
  }
  close(fd);
  android_freeifaddrs(start);
  return -1;
}

void
android_freeifaddrs(struct ifaddrs *addrs)
{
  struct ifaddrs *last = NULL;
  struct ifaddrs *cursor = addrs;
  while (cursor) {
    free(cursor->ifa_name);
    free(cursor->ifa_addr);
    free(cursor->ifa_netmask);
    last = cursor;
    cursor = cursor->ifa_next;
    free(last);
  }
}
