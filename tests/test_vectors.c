#include "brp.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *field(char **cursor)
{
    char *start=*cursor,*comma=strchr(start,',');
    if(comma!=NULL){*comma='\0';*cursor=comma+1;}else *cursor=start+strlen(start);
    return start;
}
static int number(char *text,uint32_t *value)
{
    char *end=NULL;uintmax_t n=strtoumax(text,&end,0);
    if(end==text||*end!='\0'||n>UINT32_MAX)return 0;
    *value=(uint32_t)n;return 1;
}
int main(void)
{
    FILE *f=fopen("vectors_v1.csv","r");char line[256];unsigned count=0u;
    if(f==NULL){fputs("cannot open BRP frozen vectors\n",stderr);return 1;}
    if(fgets(line,sizeof(line),f)==NULL){fclose(f);return 1;}
    while(fgets(line,sizeof(line),f)!=NULL){char *cursor=line,*version;uint32_t v[5];size_t i;brp32_ctx_t c;line[strcspn(line,"\r\n")]='\0';version=field(&cursor);
        for(i=0u;i<5u;++i)if(!number(field(&cursor),&v[i])){fclose(f);fprintf(stderr,"malformed BRP vector row=%u\n",count+1u);return 1;}
        if(strcmp(version,"BRP32-V1")!=0||!brp32_init_v1(&c,v[0],v[1])||brp32_forward_v1(v[2],&c)!=v[3]||brp32_inverse_v1(v[3],&c)!=v[4]){fclose(f);fprintf(stderr,"BRP frozen row=%u N=%" PRIu32 " x=%" PRIu32 "\n",count+1u,v[0],v[2]);return 1;}
        ++count;
    }
    if(ferror(f)||fclose(f)!=0||count!=27u){fprintf(stderr,"BRP frozen vector count=%u expected=27\n",count);return 1;}
    puts("all 27 frozen BRP32-V1 compatibility vectors: PASS");return 0;
}
