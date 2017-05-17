/*************************************************************************
    > File Name: hash_test.cpp
    > Author: alionkun
    > Mail: alionkun@gmail.com 
    > Created Time: Sat 13 May 2017 08:11:42 AM PDT
 ************************************************************************/

#include "hash.h"
#include <iostream>
#include <string>
#include <string.h>

using namespace std;

int main(int argc, char** argv)
{
	if (argc == 2)
	{
		cout << BKDRHash(argv[1], strlen(argv[1])) << "\t" << argv[1] << endl;
		return 0;
	}
	{
		char* p = "hello_1";
		cout << BKDRHash(p, strlen(p)) << "\t" << p << endl;
	}
	{
		char* p = "hello_2";
		cout << BKDRHash(p, strlen(p)) << "\t" << p << endl;
	}
	{
		char* p = "hello_a";
		cout << BKDRHash(p, strlen(p)) << "\t" << p << endl;
	}
	{
		char* p = "hello_abc";
		cout << BKDRHash(p, strlen(p)) << "\t" << p << endl;
	}
	return 0;
}
