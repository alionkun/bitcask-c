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

int main()
{
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
