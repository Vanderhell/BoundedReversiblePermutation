#include "brp.h"
#include <stdio.h>

int main(void)
{
    static const uint32_t domains[] = {1u,2u,3u,17u,32u,33u,255u,256u,257u,7317u,65535u,65536u,65537u,UINT32_MAX};
    static const uint32_t seeds[] = {0u,1u,UINT32_C(0x13579BDF),UINT32_MAX};
    size_t i, j;
    for (i=0u;i<sizeof(domains)/sizeof(domains[0]);++i) for(j=0u;j<sizeof(seeds)/sizeof(seeds[0]);++j) {
        brp32_ctx_t c; uint32_t a[5],k;
        if(!brp32_init_v1(&c,domains[i],seeds[j])) return 1;
        a[0]=0u; a[1]=domains[i]/2u; a[2]=domains[i]-1u; a[3]=domains[i]>1u?1u:0u; a[4]=domains[i]>1u?domains[i]-2u:0u;
        for(k=0u;k<5u;++k) { uint32_t y=brp32_forward_v1(a[k],&c); if(y>=domains[i]||brp32_inverse_v1(y,&c)!=a[k]||brp32_forward_v1(brp32_inverse_v1(a[k],&c),&c)!=a[k]) { fprintf(stderr,"basic N=%u seed=%u x=%u y=%u\n",domains[i],seeds[j],a[k],y); return 1; } }
    }
    { brp32_ctx_t c; if(brp32_init_v1(NULL,1u,0u)||brp32_init_v1(&c,0u,0u)||brp32_forward_v1(5u,NULL)!=0u||brp32_inverse_v1(5u,NULL)!=0u) return 1; }
    { static const uint32_t vector[][3]={{17u,0u,8u},{17u,16u,7u},{32u,0u,14u},{33u,32u,2u},{257u,0u,212u},{7317u,4096u,4042u},{UINT32_MAX,UINT32_MAX-1u,1995907464u}}; brp32_ctx_t c; if(!brp32_init_v1(&c,17u,0u))return 1; for(i=0u;i<sizeof(vector)/sizeof(vector[0]);++i){if(!brp32_init_v1(&c,vector[i][0],UINT32_C(0x13579BDF))||brp32_forward_v1(vector[i][1],&c)!=vector[i][2]){fprintf(stderr,"frozen vector N=%u x=%u expected=%u\n",vector[i][0],vector[i][1],vector[i][2]);return 1;}} }
    puts("basic boundaries, seeds, invalid arguments: PASS"); return 0;
}
