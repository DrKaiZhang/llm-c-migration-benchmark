/* model=qwen2.5-coder-1.5b task=beremiz__66ee2715 rep=1 */
#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for iec_types.h */
#ifndef STUB_IEC_TYPES_H_1202E5DE
#define STUB_IEC_TYPES_H_1202E5DE
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_iec_types_h_1202e5de;
#endif
/* stub for unistd.h */
#ifndef STUB_UNISTD_H_5DAE45BB
#define STUB_UNISTD_H_5DAE45BB
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_unistd_h_5dae45bb;
#endif
#endif

#ifndef HAVE_RETAIN
#include <stdio.h>
#include <stdint.h>

int GetRetainSize();

/* Retain buffer.  */
FILE *retain_buffer;
const char rb_file[]      = "retain_buffer_file";
const char rb_file_bckp[] = "retain_buffer_file.bak";


/* Retain header struct.  */
struct retain_info_t {
	uint32_t retain_size;
	uint32_t hash_size;
	uint8_t* hash;
	uint32_t header_offset;
	uint32_t header_crc;
};

/* Init retain info structure.  */
struct retain_info_t retain_info;

/* CRC lookup table and initial state.  */
uint32_t crc32_table[256];
uint32_t retain_crc;


/* Generate CRC32 lookup table.  */
void GenerateCRC32Table(void)
{
	unsigned int i, j;
	/* Use CRC-32-IEEE 802.3 polynomial 0x04C11DB7 (bit reflected).  */
	uint32_t poly = 0xEDB88320;

	for (i = 0; i <= 0xFF; i++)
	{
		uint32_t c = i;
		for (j = 0 ; j < 8 ; j++)
			c = (c & 1) ? (c >> 1 ) ^ poly : (c >> 1);
		crc32_table[i] = c;
	}
}


/* Calculate CRC32 for len bytes from pointer buf with init starting value.  */
uint32_t GenerateCRC32Sum(const void* buf, unsigned int len, uint32_t init)
{
	uint32_t crc = ~init;
	unsigned char* current = (unsigned char*) buf;
	while (len--)
		crc = crc32_table[(crc ^ *current++) & 0xFF] ^ (crc >> 8);
	return ~crc;
}

/* Calc CRC32 for retain file byte by byte.  */
int CheckFileCRC(FILE* file_buffer)
{
	/* Set the magic constant for one-pass CRC calc according to ZIP CRC32.  */
	const uint32_t magic_number = 0x2144df1c;

	/* CRC initial state.  */
	uint32_t calc_crc32 = 0;
	char data_block = 0;

	while(!feof(file_buffer)){
		if (fread(&data_block, sizeof(data_block), 1, file_buffer))
			calc_crc32 = GenerateCRC32Sum(&data_block, sizeof(char), calc_crc32);
	}
