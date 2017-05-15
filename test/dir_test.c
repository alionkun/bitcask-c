/*************************************************************************
    > File Name: dir_test.cpp
    > Author: alionkun
    > Mail: alionkun@gmail.com 
    > Created Time: Sat 13 May 2017 11:35:17 PM PDT
 ************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <ctype.h>

int isNumber(const char* s)
{
	if (!s || !*s)
	{
		return 0;
	}
	size_t len = strlen(s);
	int first = 1;
	for (size_t i = 0; i < len; ++i)
	{
		if (!isdigit(s[i]))
		{
			return 0;
		}
		if (i == 0 && s[i] == '0' && len != 1)
		{
			return 0;
		}
	}
	return 1;
}

int main(int argc, const char** argv)
{
	if (argc < 2) {
		printf("argc < 2.\n");
		return 1;
	}
	DIR* dir = opendir(argv[1]);
	if (!dir)
	{
		printf("opendir(%s) fail, errno=%d\n", argv[1], errno);
		return errno;
	}
	struct dirent dirent, *p_dirent;
	while (1)
	{
		int ret = readdir_r(dir, &dirent, &p_dirent);
		if (ret != 0)
		{
			printf("readdir_r() fail. ret=%d\n", ret);
			break;
		}
		if (p_dirent == NULL)
		{
			printf("readdir_r() finish.\n");
			break;
		}
		//printf("filename=%s\n", dirent.d_name);
		char* name = dirent.d_name;
		char* find = strstr(name, ".dat");
		if (!find)
		{
			continue;
		}
		if (find+4 != name+strlen(name))
		{
			continue;
		}
		char prefix[256];
		strncpy(prefix, name, strlen(name)-4);
		if (!isNumber(prefix))
		{
			continue;
		}
		printf("xxx=%s\n", name);
	}

	return 0;
}
