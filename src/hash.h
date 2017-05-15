/*************************************************************************
    > File Name: hash.h
    > Author: alionkun
    > Mail: alionkun@gmail.com 
    > Created Time: Sat 13 May 2017 08:08:31 AM PDT
 ************************************************************************/

#include <sys/types.h>
#include <stdint.h>

uint32_t BKDRHash(const char* data, size_t size)
{
	uint32_t seed = 131;
	uint32_t hash = 0;
	for (size_t i = 0; i < size; ++i)
	{
		hash = hash * seed + (uint8_t)data[i];
	}
	return hash;
}

