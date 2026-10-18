#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for flash.h */
#ifndef STUB_FLASH_H_6C967D24
#define STUB_FLASH_H_6C967D24
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_flash_h_6c967d24;
#endif
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


int SPIEraseAreaEx(const uint32_t start, const uint32_t size)
{
    if (start & (FLASH_SECTOR_SIZE - 1) != 0) {
        return 1;
    }

    const uint32_t sectors_per_block = FLASH_BLOCK_SIZE / FLASH_SECTOR_SIZE;
    uint32_t current_sector = start / FLASH_SECTOR_SIZE;
    uint32_t sector_count = (size + FLASH_SECTOR_SIZE - 1) / FLASH_SECTOR_SIZE;
    const uint32_t end = current_sector + sector_count;

    for (; current_sector < end && (current_sector & (sectors_per_block-1)); 
        ++current_sector, --sector_count) {
        if (SPIEraseSector(current_sector)) {
            return 2;
        }
    }

    for (;current_sector + sectors_per_block <= end; 
        current_sector += sectors_per_block, 
        sector_count -= sectors_per_block) {
        if (SPIEraseBlock(current_sector / sectors_per_block)) {
            return 3;
        }
    }

    for (; current_sector < end; 
        ++current_sector, --sector_count) {
        if (SPIEraseSector(current_sector)) {
            return 4;
        }
    }

    return 0;
}

