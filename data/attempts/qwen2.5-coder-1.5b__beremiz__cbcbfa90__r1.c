/* model=qwen2.5-coder-1.5b task=beremiz__cbcbfa90 rep=1 */
#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for beremiz.h */
#ifndef STUB_BEREMIZ_H_BFBDD7C6
#define STUB_BEREMIZ_H_BFBDD7C6
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_beremiz_h_bfbdd7c6;
#endif
/* stub for dlfcn.h */
#ifndef STUB_DLFCN_H_B8C97086
#define STUB_DLFCN_H_B8C97086
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_dlfcn_h_b8c97086;
#endif
/* stub for iec_types_all.h */
#ifndef STUB_IEC_TYPES_ALL_H_354B7B31
#define STUB_IEC_TYPES_ALL_H_354B7B31
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_iec_types_all_h_354b7b31;
#endif
/* stub for stdatomic.h */
#ifndef STUB_STDATOMIC_H_BD29E286
#define STUB_STDATOMIC_H_BD29E286
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_stdatomic_h_bd29e286;
#endif
#endif

/**
 * Head of code for the IOs shared object.
 *
 * Contains: extension dispatch, PLC cycle entry, and hot-swap mechanism.
 * The PLC logic (config_run__, ScanInstances, etc.) lives in a separate
 * shared object loaded via loadPLCLogic() / preparePLCLogicSwap().
 **/

#include <string.h>

/*
 * Prototypes of functions provided by generated target C code
 **/

// Retain buffer handling
int CheckRetainBuffer(void);
void ValidateRetainBuffer(void);
void InValidateRetainBuffer(void);
void Retain(unsigned int offset, unsigned int count, void *p);
void Remind(unsigned int offset, unsigned int count, void *p);
void CleanupRetain(void);
int InitRetain(size_t buffer_size);

// Debug
#ifndef PLC_NO_DEBUG
int TryEnterDebugSection(void);
void InitiateDebugTransfer(int tick);
#endif

// Logging
#ifndef PLC_USES_ABI /* not on PLC side if using ABI */
#ifndef PLC_NO_LOGGING
void __init_logging(void);
#endif
#endif

// Python eval purge — defined in plc_python.c when python_eval extension is active
extern void PythonSetPurge(int value) __attribute__((weak));

/*
 *  Variables used by generated C softPLC and plugins
 **/
IEC_TIME __CURRENT_TIME = {0, 0};
unsigned int __tick = 0;
char *PLC_ID = 0;

/*
 * Local copies of PLC timing constants.
 * Defined here so plc_Linux_main.c (GetCommonTickTime) can access them.
 * Updated from logic .so values by loadPLCLogic().
 **/
unsigned long long common_ticktime__ = 1000000; /* 1ms default */
unsigned long greatest_tick_count__ = 0;

/* Help to quit cleanly when init fail at a certain level */
static int init_level = 0;

/*
 * Prototypes of functions exported by IO extensions
 **/
%(calls_prototypes)s

/*
 * Function
