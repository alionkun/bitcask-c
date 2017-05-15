/*************************************************************************
    > File Name: bitcask_test.cpp
    > Author: alionkun
    > Mail: alionkun@gmail.com 
    > Created Time: Sun 14 May 2017 03:17:30 AM PDT
 ************************************************************************/

#include "bitcask.h"
#include <stdio.h>
#include <string.h>

int main(int argc, const char** argv)
{
	if (argc < 2)
	{
		printf("argc<2 \n");
		return 1;
	}
	bitcask bc;
	int ret = bitcask_init(&bc, argv[1]);
	if (ret != 0)
	{
		printf("bitcask_init() fail. dir=%s\n", argv[1]);
		return 1;
	}
	bitcask_dump_info(&bc);

	return 0;
}
