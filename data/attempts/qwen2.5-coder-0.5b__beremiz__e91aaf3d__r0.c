/* model=qwen2.5-coder-0.5b task=beremiz__e91aaf3d rep=0 */
#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for POUS.h */
#ifndef STUB_POUS_H_AC5F108C
#define STUB_POUS_H_AC5F108C
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_pous_h_ac5f108c;
#endif
/* stub for accessor.h */
#ifndef STUB_ACCESSOR_H_34D0B355
#define STUB_ACCESSOR_H_34D0B355
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_accessor_h_34d0b355;
#endif
/* stub for beremiz.h */
#ifndef STUB_BEREMIZ_H_BFBDD7C6
#define STUB_BEREMIZ_H_BFBDD7C6
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_beremiz_h_bfbdd7c6;
#endif
/* stub for iec_types_all.h */
#ifndef STUB_IEC_TYPES_ALL_H_354B7B31
#define STUB_IEC_TYPES_ALL_H_354B7B31
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_iec_types_all_h_354b7b31;
#endif
#endif

/*

Template C code used to produce target C CIA402 code. CiA402 being the drive
profile of the CANopen application layer, this is shared by every field bus
reusing that application layer : EtherCAT (CoE) and CANopen itself.

Copyright (C) 2011-2014: Laurent BESSARD
                         RTES Lab : CRKim, JBLee, youcu
                         Higen Motor : Donggu Kang
Copyright (C) 2011-2026: Edouard TISSERANT


Distributed under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

See COPYING file for copyrights details.

*/

%(fieldbus_includes)s



/* From CiA402, page 27

        Table 30 - State coding
    Statusword      |      PDS FSA state
xxxx xxxx x0xx 0000 | Not ready to switch on
xxxx xxxx x1xx 0000 | Switch on disabled
xxxx xxxx x01x 0001 | Ready to switch on
xxxx xxxx x01x 0011 | Switched on
xxxx xxxx x01x 0111 | Operation enabled
xxxx xxxx x00x 0111 | Quick stop active
xxxx xxxx x0xx 1111 | Fault reaction active
xxxx xxxx x0xx 1000 | Fault
*/


/* From CiA402, Page 63 Statusword for homing mode

		Table 106 - Definition of bit 10, bit 12, bit 13

xx00 x0xx xxxx xxxx | Homing procedure is in progress
xx00 x1xx xxxx xxxx |
