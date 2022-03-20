
/*
#include<stdio.h>

int fputc(int ch, FILE *stream) {
    char* addr = (char *)0x100000ff;
    *addr = ch;
    return ch;
}
*/
void myprintS(char *s)
{
    int i = 0;
    char* addr = (char *)0x100000ff;
    while(s[i]!='\0')
    {
        *addr = s[i];
        i++;
    }
}


void myprintn(int n){
    char * addr = (char *)0x100000ff;
    if( (n >= 0) && (n <= 9) )
    {
        *addr = ('0' + n);
    }
    else if( (n >=0xa) && (n <= 0xf) )
    {
        *addr = ('a' + n - 10);
    }
}

void myprintX(int data)
{
    int i = 0;
    char* addr = (char *)0x100000ff;
    *addr = '0';
    *addr = 'x';
    switch (0)
    {
    case 0:{
        if((( data & 0xf0000000 ) >> 28) != 0){
            i++;
            myprintn(( data & 0xf0000000 ) >> 28);
        }}

    case 1:{
        if((( data & 0xf000000 ) >> 24) != 0){
            i++;
            myprintn(( data & 0xf000000 ) >> 24);
        }
        else{
            if( i != 0) myprintn(( data & 0xf000000 ) >> 24);
        }}
    case 2:{
        if((( data & 0xf00000 ) >> 20) != 0){
            i++;
            myprintn(( data & 0xf00000 ) >> 20);
        }
        else{
            if( i != 0) myprintn(( data & 0xf00000 ) >> 20);
        }}
    case 3:{
        if((( data & 0xf0000 ) >> 16) != 0){
            i++;
            myprintn(( data & 0xf0000 ) >> 16);
        }
        else{
            if( i != 0) myprintn(( data & 0xf0000 ) >> 16);
        }}
    case 4:{
        if((( data & 0xf000 ) >> 12) != 0){
            i++;
            myprintn(( data & 0xf000 ) >> 12);
        }
        else{
            if( i != 0) myprintn(( data & 0xf000 ) >> 12);
        }}
    case 5:{
        if((( data & 0xf00 ) >> 8) != 0){
            i++;
            myprintn(( data & 0xf00 ) >> 8);
        }
        else{
            if( i != 0) myprintn(( data & 0xf00 ) >> 8);
        }}
    case 6:{
        if((( data & 0xf0 ) >> 4) != 0){
            i++;
            myprintn(( data & 0xf0 ) >> 4);
        }
        else{
            if( i != 0) myprintn(( data & 0xf0 ) >> 4);
        }}
    case 7:{
        if((( data & 0xf )) != 0){
            i++;
            myprintn(( data & 0xf ));
        }
        else{
            if( i != 0) myprintn(( data & 0xf ) );
        }
    }
    
    default: break;
    }
    *addr = '\n';
    
}


int main()
{
    int i = 0;
    int sum = 0;
    for(i = 0; i < 1000; i++)
    {
        sum += i;
    }
    myprintS("hello HRV_SIM \n ");
    myprintS("     ##    ###  #######  ###    ###          ######  ###  ####     ####        \n\
      ##    ###  ##   ###  ##    ##           ##      ###  #####   #####        \n\
      #########  ### ###   ###  ###           ####    ###  ## ##   ## ##        \n\
      ###   ###  ######     ##  ##              ####  ###  ##  ## ##  ##        \n\
      ##    ###  ##   ##    #####                 ### ###  ##  ####   ##        \n\
      ##    ###  ##    ##    ####            #######  ###  ##   ###   ##        \n\
                                                                                \n\
                                    #########                                   \n");
    myprintS("1 + 2 + 3 ... + 999 =  ");
    myprintX(sum);
   // printf("hello %d",9);
    return 0;
}