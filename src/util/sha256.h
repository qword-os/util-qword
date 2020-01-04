#pragma once

#include <stdint.h>
#include <stddef.h>

#define SHA256_BLOCK_SIZE 32
#define SHA256_STR_LEN    64

struct sha256_ctx {
	uint8_t data[64];
	uint32_t datalen;
	unsigned long long bitlen;
	uint32_t state[8];
};

void sha256_init(struct sha256_ctx *ctx);
void sha256_update(struct sha256_ctx *ctx, const uint8_t data[], size_t len);
void sha256_final(struct sha256_ctx *ctx, uint8_t hash[]);
