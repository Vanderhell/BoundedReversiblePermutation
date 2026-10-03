#include "brp.h"

#include <stddef.h>

/* SplitMix32-style deterministic expander; not a cryptographic generator. */
static uint32_t brp_next32(uint32_t *state)
{
    uint32_t z;
    *state += UINT32_C(0x9E3779B9);
    z = *state;
    z = (z ^ (z >> 16)) * UINT32_C(0x85EBCA6B);
    z = (z ^ (z >> 13)) * UINT32_C(0xC2B2AE35);
    return z ^ (z >> 16);
}

/* MurmurHash3 finalizer; the low bit drives the symmetric pair decision. */
static uint32_t brp_mix32(uint32_t x)
{
    x ^= x >> 16;
    x *= UINT32_C(0x85EBCA6B);
    x ^= x >> 13;
    x *= UINT32_C(0xC2B2AE35);
    x ^= x >> 16;
    return x;
}

static uint32_t brp_partner32(uint32_t x, uint32_t pivot, uint32_t domain)
{
    return pivot >= x ? pivot - x : domain - (x - pivot);
}

static uint16_t brp_partner16(uint16_t x, uint16_t pivot, uint16_t domain)
{
    return pivot >= x ? (uint16_t)(pivot - x) : (uint16_t)(domain - (uint16_t)(x - pivot));
}

static uint32_t brp_round32(uint32_t x, const brp32_round_t *round, uint32_t domain)
{
    uint32_t p = brp_partner32(x, round->pivot, domain);
    uint32_t canonical = x < p ? x : p;
    return (brp_mix32(canonical ^ round->salt) & UINT32_C(1)) != 0u ? p : x;
}

static uint16_t brp_round16(uint16_t x, const brp16_round_t *round, uint16_t domain)
{
    uint16_t p = brp_partner16(x, round->pivot, domain);
    uint16_t canonical = x < p ? x : p;
    return (brp_mix32((uint32_t)canonical ^ (uint32_t)round->salt) & UINT32_C(1)) != 0u ? p : x;
}

bool brp32_init_v1(brp32_ctx_t *ctx, uint32_t domain, uint32_t seed)
{
    uint32_t state;
    uint32_t i;
    if (ctx == NULL || domain == 0u) return false;
    state = seed ^ domain ^ UINT32_C(0x42525031);
    ctx->domain = domain;
    for (i = 0u; i < BRP32_V1_ROUNDS; ++i) {
        ctx->rounds[i].pivot = brp_next32(&state) % domain;
        ctx->rounds[i].salt = brp_next32(&state);
    }
    return true;
}

bool brp16_init_v1(brp16_ctx_t *ctx, uint16_t domain, uint32_t seed)
{
    uint32_t state;
    uint32_t i;
    if (ctx == NULL || domain == 0u) return false;
    state = seed ^ (uint32_t)domain ^ UINT32_C(0x42525031);
    ctx->domain = domain;
    for (i = 0u; i < BRP16_V1_ROUNDS; ++i) {
        ctx->rounds[i].pivot = (uint16_t)(brp_next32(&state) % (uint32_t)domain);
        ctx->rounds[i].salt = (uint16_t)brp_next32(&state);
    }
    return true;
}

uint32_t brp32_forward_v1(uint32_t x, const brp32_ctx_t *ctx)
{
    uint32_t i;
    if (ctx == NULL) return 0u;
    if (ctx->domain == 0u || x >= ctx->domain) return x;
    for (i = 0u; i < BRP32_V1_ROUNDS; ++i) x = brp_round32(x, &ctx->rounds[i], ctx->domain);
    return x;
}

uint32_t brp32_inverse_v1(uint32_t x, const brp32_ctx_t *ctx)
{
    uint32_t i = BRP32_V1_ROUNDS;
    if (ctx == NULL) return 0u;
    if (ctx->domain == 0u || x >= ctx->domain) return x;
    while (i != 0u) { --i; x = brp_round32(x, &ctx->rounds[i], ctx->domain); }
    return x;
}

uint16_t brp16_forward_v1(uint16_t x, const brp16_ctx_t *ctx)
{
    uint32_t i;
    if (ctx == NULL) return 0u;
    if (ctx->domain == 0u || x >= ctx->domain) return x;
    for (i = 0u; i < BRP16_V1_ROUNDS; ++i) x = brp_round16(x, &ctx->rounds[i], ctx->domain);
    return x;
}

uint16_t brp16_inverse_v1(uint16_t x, const brp16_ctx_t *ctx)
{
    uint32_t i = BRP16_V1_ROUNDS;
    if (ctx == NULL) return 0u;
    if (ctx->domain == 0u || x >= ctx->domain) return x;
    while (i != 0u) { --i; x = brp_round16(x, &ctx->rounds[i], ctx->domain); }
    return x;
}
