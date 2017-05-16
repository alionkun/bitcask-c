/*************************************************************************
    > File Name: bitcask_test.cpp
    > Author: alionkun
    > Mail: alionkun@gmail.com 
    > Created Time: Sun 14 May 2017 03:17:30 AM PDT
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include "kv.pb.h"
#include "bitcask.h"


bitcask bc;

void onAdd(const KVTest::KvReq& req, KVTest::KvRsp& rsp)
{
	for (int i = 0; i < req.kv_list_size(); ++i)	
	{
		string key   = req.kv_list(i).key();
		string value = req.kv_list(i).value();
		KVTest::KV* kvRsp = rsp.add_kv_list();
		kvRsp->set_key(key);
		kvRsp->set_value(value);
		char old_value[1024*10];
		uint32_t old_value_len = 1024*10;
		int ret = bitcask_get(bc, key.data(), key.size(), old_value, &old_value_len);
		if (ret != -1)
		{
			kvRsp->set_ec(KVTest::EC::ec_already_exist);
			continue;
		}
		ret = bitcask_add()
	}
}

int main(int argc, const char** argv)
{
	if (argc < 2)
	{
		std::cerr << "argc < 2" << std::endl;
		return 1;
	}
	int ret = bitcask_init(&bc, argv[1]);
	if (ret != 0)
	{
		std::cerr << "bitcask_init() fail. dir=" << argv[1] << std::endl;
		return 1;
	}

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(12345);
	int s = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == s)
	{
		std::cerr << "socket() fail. errno=" << errno << std::endl;
		return -1;
	}
	int r = bind(s, (struct sockaddr*)&address, sizeof(address));
	if (-1 == r)
	{
		std::cerr << "bind() fail. errno=" << errno << std::endl;
		return -1;
	}

	char buf[1024*10];
	struct sockaddr src_addr;
	socklen_t addrlen;
	
	while (1) {

		addrlen = sizeof(src_addr);
		
		int ret = recvfrom(s, buf, 1024*10, 0, &src_addr, &addrlen);
		if (ret <= 4)
		{
			std::cerr << "recv() fail. ret=" << ret << ", errno=" << errno << std::endl;
			continue;
		}
		uint32_t len = *(uint32_t*)buf;	
		if (len + 4 != ret)
		{
			std::cerr << "bad request packet" << std::endl;
			continue;
		}
		KVTest::KvReq req;
		if (!req.ParseFromArray(*buf[4], len))
		{
			std::cerr << "req.ParseFromArray() fail" << std::endl;
			continue;
		}
		KVTest::KvRsp rsp;
		switch (req.op_type)
		{
			case KVTest::OP_TYPE::op_add:
				onAdd(req, rsp);
				break;

			default:
				std::cerr << "unknown req.op_type=" << req.op_type << std::endl;
				break;
		}
		string tmp;
		if (!rsp.SerializeToString(&tmp))
		{
			std::cerr << "rsp.SerializeToString() fail" << std::endl;
			continue;
		}
		*(uint32_t*)buf = htonl(tmp.size());
		memcpy(buf+4, tmp.data(), tmp.size());
		ret = sendto(fd, buf, tmp.data()+4, 0, &src_addr, addrlen);
		if (ret != tmp.data()+4)
		{
			std::cerr << "sendto() fail. ret=" << ret << ", errno=" << errno << std::endl;
			continue;
		}

	}



	return 0;
}
