#include "brp.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "sdkconfig.h"
#include "frozen_vectors.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

static uint8_t seen[4096];
static uint8_t seen16[4096];
static uint32_t rng = UINT32_C(0x6D2B79F5);
static uint32_t next32(void) { rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5; return rng; }
static uint64_t cases;
static void fail(const char *test, uint32_t n, uint32_t seed, uint32_t x, uint32_t y, uint32_t z)
{
    printf("BRP_HW_TEST_FAIL test=%s N=%" PRIu32 " seed=%08" PRIx32 " x=%" PRIu32 " y=%" PRIu32 " z=%" PRIu32 " cases=%" PRIu64 "\n", test,n,seed,x,y,z,cases);
    abort();
}
static void exhaustive(uint32_t n, uint32_t seed)
{
    brp32_ctx_t c32; brp16_ctx_t c16; uint32_t x;
    memset(seen,0,sizeof seen); memset(seen16,0,sizeof seen16);
    if (!brp32_init_v1(&c32,n,seed) || !brp16_init_v1(&c16,(uint16_t)n,seed)) fail("init",n,seed,0,0,0);
    for(x=0;x<n;++x){uint32_t y=brp32_forward_v1(x,&c32),z=brp32_inverse_v1(y,&c32);uint16_t a=brp16_forward_v1((uint16_t)x,&c16),b=brp16_inverse_v1(a,&c16); if(y>=n||z!=x||brp32_forward_v1(brp32_inverse_v1(x,&c32),&c32)!=x)fail("BRP32_roundtrip",n,seed,x,y,z);if(seen[y]!=0u)fail("BRP32_bijection",n,seed,x,y,z);seen[y]=1u;if(a>=n||b!=x||brp16_forward_v1(brp16_inverse_v1((uint16_t)x,&c16),&c16)!=(uint16_t)x)fail("BRP16_roundtrip",n,seed,x,a,b);if(seen16[a]!=0u)fail("BRP16_bijection",n,seed,x,a,b);seen16[a]=1u;++cases;if((cases&32767u)==0u)vTaskDelay(1);}
    for(x=0;x<n;++x)if(seen[x]==0u||seen16[x]==0u)fail("permutation_missing",n,seed,x,0,0);
}
static void boundaries(void)
{
    static const uint32_t ns[]={65535u,65536u,65537u,1000000u,UINT32_MAX}; unsigned k; uint32_t j;
    for(k=0;k<sizeof ns/sizeof ns[0];++k){brp32_ctx_t c;uint32_t n=ns[k];if(!brp32_init_v1(&c,n,UINT32_C(0x13579BDF)))fail("boundary_init",n,0,0,0,0);rng=UINT32_C(0xA341316C);for(j=0;j<100000u;++j){uint32_t x=j<8u?(j==0u?0u:j==1u?1u:j==2u?n-1u:j==3u?n/2u:j==4u?n-2u:j==5u?UINT32_MAX%n:n-j):next32()%n;uint32_t y=brp32_forward_v1(x,&c),z=brp32_inverse_v1(y,&c);if(y>=n||z!=x||brp32_forward_v1(brp32_inverse_v1(x,&c),&c)!=x)fail("boundary",n,0,x,y,z);++cases;}}
}
static void frozen_vectors(void)
{
    unsigned i;
    for(i=0u;i<BRP_HW_VECTOR_COUNT;++i){const brp_hw_vector_t*v=&brp_hw_vectors[i];brp32_ctx_t c;if(!brp32_init_v1(&c,v->n,v->seed)||brp32_forward_v1(v->x,&c)!=v->y||brp32_inverse_v1(v->y,&c)!=v->z)fail("frozen_vector",v->n,v->seed,v->x,v->y,v->z);++cases;}
}
void app_main(void)
{
    static const uint32_t seeds[]={0u,UINT32_C(0x13579BDF),UINT32_MAX}; unsigned s,run; uint32_t n;
    printf("BRP_HW_READY target=ESP32-S3 optimization=%s flash_app_partition=0x10000\n",HW_OPT_LABEL);
    for(run=0;run<HW_RUN_COUNT;++run){cases=0u;for(s=0;s<sizeof seeds/sizeof seeds[0];++s)for(n=1;n<=4096u;++n){exhaustive(n,seeds[s]);if((n&1023u)==0u)printf("BRP_PROGRESS run=%u seed=%u N=%" PRIu32 " cases=%" PRIu64 "\n",run+1u,s,n,cases);}boundaries();frozen_vectors();
        printf("BRP_HW_TEST_PASS run=%u cases=%" PRIu64 " mismatches=0 free_heap=%u stack_high_water_words=%u\n",run+1u,cases,(unsigned)esp_get_free_heap_size(),(unsigned)uxTaskGetStackHighWaterMark(NULL));}
    vTaskDelay(pdMS_TO_TICKS(3000));esp_restart();
}
