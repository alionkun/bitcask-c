/*************************************************************************
    > File Name: bitcask_test.cpp
    > Author: alionkun
    > Mail: alionkun@gmail.com 
    > Created Time: Sun 14 May 2017 03:17:30 AM PDT
 ************************************************************************/

#include "bitcask.h"
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

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

	for (int i = 0; i < 10000; i++)
	{
		struct timeval tv;
		gettimeofday(&tv, NULL);
		uint64_t ts_ms = tv.tv_sec * 1000 + tv.tv_usec/1000;

		char key[64];
		sprintf(key, "id_%d", i);
		char data[1024];
		sprintf(data, "value_1234567890-%d-abcdef-xxxxxxxx", i);
		ret = bitcask_add(&bc, key, strlen(key), data, strlen(data));
		printf("bitcask_add() ret=%d\n", ret);

		//bitcask_dump_info(&bc);
	}

	return 0;
}
