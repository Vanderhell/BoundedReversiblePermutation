#ifndef BRP_H
#define BRP_H

#include <stdbool.h>
#include <stdint.h>

/* BRP32-V1 uses exactly six rounds. The struct layout is not a wire format. */
#define BRP_ALGORITHM_VERSION 1u
#define BRP32_V1_ROUNDS 6u
#define BRP16_V1_ROUNDS 6u

typedef struct { uint32_t pivot; uint32_t salt; } brp32_round_t;
typedef struct { uint32_t domain; brp32_round_t rounds[BRP32_V1_ROUNDS]; } brp32_ctx_t;
typedef struct { uint16_t pivot; uint16_t salt; } brp16_round_t;
typedef struct { uint16_t domain; brp16_round_t rounds[BRP16_V1_ROUNDS]; } brp16_ctx_t;

/* Domains are 1..UINT32_MAX for BRP32 and 1..UINT16_MAX for BRP16. */
bool brp32_init_v1(brp32_ctx_t *ctx, uint32_t domain, uint32_t seed);
bool brp16_init_v1(brp16_ctx_t *ctx, uint16_t domain, uint32_t seed);

/*
 * Keep initialized contexts immutable. Valid calls require x < ctx->domain.
 * Invalid x/domain returns x unchanged; a NULL context returns zero.
 * These fallback values are error behavior, not part of the permutation.
 */
uint32_t brp32_forward_v1(uint32_t x, const brp32_ctx_t *ctx);
uint32_t brp32_inverse_v1(uint32_t x, const brp32_ctx_t *ctx);
uint16_t brp16_forward_v1(uint16_t x, const brp16_ctx_t *ctx);
uint16_t brp16_inverse_v1(uint16_t x, const brp16_ctx_t *ctx);

#endif
