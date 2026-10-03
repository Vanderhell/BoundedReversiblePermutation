#include "brp.h"

#include <inttypes.h>
#include <stdio.h>

int main(void)
{
    brp32_ctx_t context;
    const uint32_t domain = 1000u;
    const uint32_t input = 42u;
    const uint32_t seed = UINT32_C(0x12345678);
    uint32_t mapped;

    if (!brp32_init_v1(&context, domain, seed)) {
        return 1;
    }

    mapped = brp32_forward_v1(input, &context);
    printf("%" PRIu32 " -> %" PRIu32 " -> %" PRIu32 "\n",
           input, mapped, brp32_inverse_v1(mapped, &context));
    return 0;
}
