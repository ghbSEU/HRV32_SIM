#include <stdio.h>
#include "config.h"
#include "mem.h"

int imem[imem_size/4];			///// 32K/4 line
int instsize;

/*********************************************/
void txt2mem()
{
	FILE *fp;
	char data[9];
	fp = fopen("/home/ghb/HRV32/HRV32_SIM/imem.txt","r");
	instsize = 0;
	for(int line = 0;line < imem_size/4;line++)
	{
		fseek(fp,line*9,SEEK_SET);
		fgets(data,9,fp);
		if(*data != '.')
		{
            imem[line] = 0;
			for(int i=0;i<8;i++)
            {
                if((data[i] >= '0') && (data[i] <= '9'))
                {
                    imem[line] += ((data[i] - '0') << (28 - 4*i));
                }
                else if((data[i] >= 'a') && (data[i] <= 'f'))
                {
                    imem[line] += ((data[i] - 'a' + 10)<< (28 - 4*i));
                }
                else if((data[i] >= 'A') && (data[i] <= 'F'))
                {
                    imem[line] += ((data[i] - 'A' + 10)<< (28 - 4*i));
                }
            }
		}
        else
        {
            instsize = line;
            printf("instsize = %d\n",instsize);
            break;
        }
	}
	
	fclose(fp);
}


void printmem()
{
    for(int i = 0; i < instsize; i++)
    {
        printf("%8x\n",imem[i]);
    }
}

