/* model=qwen2.5-coder-0.5b task=beremiz__2eee1d65 rep=0 */
#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for beremiz.h */
#ifndef STUB_BEREMIZ_H_BFBDD7C6
#define STUB_BEREMIZ_H_BFBDD7C6
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_beremiz_h_bfbdd7c6;
#endif
#endif

/** 
 * LOGGING
 **/

#include <string.h>

#ifndef LOG_BUFFER_SIZE
#define LOG_BUFFER_SIZE (1<<14) /*16Ko*/
#endif
#ifndef LOG_BUFFER_ATTRS
#define LOG_BUFFER_ATTRS
#endif

static char LogBuff[LOG_LEVELS][LOG_BUFFER_SIZE] LOG_BUFFER_ATTRS;

#define LOG_BUFFER_MASK (LOG_BUFFER_SIZE-1)

static void inline copy_to_log(uint8_t level, uint32_t buffpos, void* buf, uint32_t size){
    if(buffpos + size < LOG_BUFFER_SIZE){
        memcpy(&LogBuff[level][buffpos], buf, size);
    }else{
        uint32_t remaining = LOG_BUFFER_SIZE - buffpos; 
        memcpy(&LogBuff[level][buffpos], buf, remaining);
        memcpy(LogBuff[level], (char*)buf + remaining, size - remaining);
    }
}
static void inline copy_from_log(uint8_t level, uint32_t buffpos, void* buf, uint32_t size){
    if(buffpos + size < LOG_BUFFER_SIZE){
        memcpy(buf, &LogBuff[level][buffpos], size);
    }else{
        uint32_t remaining = LOG_BUFFER_SIZE - buffpos; 
        memcpy(buf, &LogBuff[level][buffpos], remaining);
        memcpy((char*)buf + remaining, LogBuff[level], size - remaining);
    }
}

/* Log buffer structure

 |<-Tail1.msgsize->|<-sizeof(mTail)->|<--Tail2.msgsize-->|<-sizeof(mTail)->|...
 |  Message1 Body  |      Tail1      |   Message2 Body   |      Tail2      |

*/
typedef struct {
    uint32_t msgidx;
    uint32_t msgsize;
    unsigned int tick;
    IEC_TIME time;
} mTail;

/* Log cursor : 32b index in log buffer, accessed atomically */
static uint32_t LogCursor[LOG_LEVELS] LOG_BUFFER_ATTRS = {0x0,0x0,0x0,0x0};
static uint32_t LogIndex[LOG_LEVELS] LOG_BUFFER_ATTRS = {0x0,0x0,0x0,0x0};

void __init_logging(void) {
    memset(LogBuff, 0, sizeof(LogBuff));
}

void ResetLogCount(void) {
	uint8_t level;
	for(level=0;level<LOG_LEVELS;level++){
		LogCursor[level] = 0;
	}
}
