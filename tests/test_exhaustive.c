#include "brp.h"
#include <stdio.h>

int main(void)
{
    uint8_t seen[4096]; uint32_t n,s,x; const uint32_t seeds[]={0u,UINT32_C(0xDEADBEEF),UINT32_MAX};
    for(n=1u;n<=4096u;++n) for(s=0u;s<3u;++s) {
        brp32_ctx_t c; if(!brp32_init_v1(&c,n,seeds[s])) return 1;
        for(x=0u;x<n;++x) seen[x]=0u;
        for(x=0u;x<n;++x) { uint32_t y=brp32_forward_v1(x,&c); if(y>=n||seen[y]!=0u||brp32_inverse_v1(y,&c)!=x) { fprintf(stderr,"exhaustive N=%u seed=%u x=%u y=%u\n",n,seeds[s],x,y); return 1; } seen[y]=1u; }
    }
    puts("bijection and inverse exhaustive N=1..4096, three seeds: PASS"); return 0;
}
