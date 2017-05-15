/*************************************************************************
    > File Name: bitcask.c
    > Author: alionkun
    > Mail: alionkun@gmail.com 
    > Created Time: Sat 13 May 2017 11:19:52 PM PDT
 ************************************************************************/

#include "bitcask.h"
#include "hash.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>


int isNumber(const char* s)
{
	if (!s || !*s)
	{
		return 0;
	}
	size_t len = strlen(s);
	int first = 1;
	for (int i = 0; i < len; ++i)
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

#if 0
int _bitcask_check_data_file(bitcask* bc, const char* filename)
{
	char* find = strstr(filename, ".dat");
	if (!find)
	{
		return 1;
	}
	if (find+4 != filename+strlen(filename))
	{
		return 1;
	}
	char prefix[256];
	strncpy(prefix, filename, strlen(filename)-4);
	if (!isNumber(prefix))
	{
		return 1;
	}
	int32_t index = atoi(prefix);
	if (0 < index || index >= MAX_DATA_FILE_COUNT)
	{
		return 1;
	}
	strncpy(bc->data_file_names[index], prefix, 64);
	return 0;
}

int _bitcask_read_dir(bitcask* bc, const char* dir_name)
{
	DIR* dir = opendir(dir_name);
	if (!dir)
	{
		printf("opendir(%s) fail. errno=%d, errmsg=%s\n", dir_name, errno, strerror(errno));
		return 1;
	}
	struct dirent dirent, *p_dirent;
	while (1)
	{
		int ret = readdir_r(dir, &dirent, &p_dirent);
		if (ret)
		{
			printf("readdir_r() fail. ret=%d\n", ret);
			break;
		}
		if (!p_dirent)
		{
			break;
		}
		_bitcask_check_data_file(bc, dirent.d_name);
	}
	return 0;
}
#endif

int _bitcask_open_current_file(bitcask* bc)
{
	uint32_t index = bc->active_file_index;
	int fd = bc->data_file_fds[index];
	if (fd == -1)
	{
		char filename[1024*2];
		fd = open(filename, O_APPEND);
		if (fd == -1)
		{
			printf("open(%s) fail, errno=%d\n", filename, errno);
			return 1;
		}
		bc->data_file_fds[index] = fd;
	}
	return 0;
}


int _try_open_all_data_file(bitcask* bc)
{
	const char* dir = bc->directory;
	for (int i = 0; i < MAX_DATA_FILE_COUNT; ++i)
	{
		char filename[1024*2];
		sprintf(filename, "%s/%d", dir, i);
		printf("try open data file for read: %s\n", filename);
		int fd = open(filename, O_RDONLY);
		if (fd == -1)
		{
			if (errno != 2)
			{
				printf("open(%s) fail, errno=%d\n", filename, errno);
				return 1; //failed
			}
			break;//finish
		}
		bc->data_file_fds[i] = fd;
		bc->active_file_index = i;
	}
	//data_file_fds[active_file_index] is the active file
	if (bc->active_file_index == -1)
	{
		bc->active_file_index = 0;
	}
	int fd = bc->data_file_fds[bc->active_file_index];
	if (fd != -1)
	{
		close(fd);
	}
	char filename[1024*2];
	sprintf(filename, "%s/%d", dir, bc->active_file_index);
	printf("try open file for write: %s\n", filename);
	fd = open(filename, O_CREAT|O_RDWR|O_APPEND, S_IRUSR|S_IWUSR);
	if (fd == -1)
	{
		if (errno != 2)
		{
			printf("open(%s) fail, errno=%d\n", filename, errno);
			return 1; //failed
		}
	}
	bc->data_file_fds[bc->active_file_index] = fd;
	return 0;
}

int _read_item_from_fd(int fd, uint32_t* crc, uint64_t* ts_ms, uint16_t* key_len, uint32_t* value_len, void** key)
{
	int ret;
	uint32_t len;

	len = sizeof(*crc);
	ret = read(fd, crc, len);
	if (ret != len && ret != 0)
	{
		printf("read crc fail. ret=%d, errno=%d\n", ret, errno);
		return -1;
	}
	if (ret == 0)
	{
		printf("read current data file done.\n");
		return 0;
	}
	printf("crc=%d\n", *crc);

	len = sizeof(*ts_ms);
	ret = read(fd, ts_ms, len);
	if (ret != len)
	{
		printf("read ts_ms fail. ret=%d, errno=%d\n", ret, errno);
		return -1;
	}
	printf("ts_ms=%ld\n", *ts_ms);

	len = sizeof(*key_len);
	ret = read(fd, key_len, len);
	if (ret != len)
	{
		printf("read key_len fail. ret=%d, errno=%d\n", ret, errno);
		return -1;
	}
	printf("key_len=%d\n", *key_len);

	len = sizeof(*value_len);
	ret = read(fd, value_len, len);
	if (ret != len)
	{
		printf("read value_len fail. ret=%d, errno=%d\n", ret, errno);
		return -1;
	}
	printf("value_len=%d\n", *value_len);

	*key = (char*)malloc(*key_len);
	len = *key_len;
	ret = read(fd, *key, len);
	if (ret != len)
	{
		printf("read key fail. ret=%d, errno=%d\n", ret, errno);
		return -1;
	}

	//skip @value
	lseek(fd, *value_len, SEEK_CUR);
	return 1;
}


int _build_index(bitcask* bc)
{
	for (int i = 0; i <= bc->active_file_index; ++i)
	{
		int fd = bc->data_file_fds[i];
		assert(fd != -1);
		lseek(fd, 0, SEEK_SET);
		/* crc(32) ts_ms(64) key_len(16) value_len(32) key value */
		while (1)
		{
			uint32_t crc;
			uint64_t ts_ms;
			uint16_t key_len;
			uint32_t value_len;
			void* key;
			uint32_t pos = lseek(fd, 0, SEEK_CUR);
			int ret = _read_item_from_fd(fd, &crc, &ts_ms, &key_len, &value_len, &key);
			if (ret < 0)
			{
				return -1;
			}
			if (ret == 0)
			{
				break;//done
			}
			bitcask_item* item = _hash_table_find(bc->item_table, key, key_len);
			if (!item)
			{
				item = _hash_table_add(bc->item_table, key, key_len);
			}
			item->file_id = i;
			item->value_len = value_len;
			item->file_pos = pos;
			item->ts = ts_ms;
		}
	}

	return 0;
}

int bitcask_init(bitcask* bc, const char* dir_name)
{
	if (!bc || !dir_name)
	{
		return 1;
	}
	if (strlen(dir_name) >= MAX_DIRECTORY_LEN)
	{
		return 1;
	}
	bzero(bc, sizeof(bc));
	strncpy(bc->directory, dir_name, MAX_DIRECTORY_LEN);
	for (int i = 0; i < MAX_DATA_FILE_COUNT; ++i)
	{
		bc->data_file_fds[i] = -1;
	}
	bc->item_table_size = TABLE_SIZE;
	bc->item_table = (bitcask_item**)malloc(sizeof(bitcask_item*)*bc->item_table_size);
	for (int i = 0; i < TABLE_SIZE; ++i)
	{
		bc->item_table[i] = NULL;
	}

	bc->active_file_position = 0;

	int ret;
	ret = _try_open_all_data_file(bc);
	if (ret != 0)
	{
		printf("_try_open_all_data_file() fail, ret=%s\n", ret);
		return 1;
	}
	ret = _build_index(bc);
	if (ret != 0)
	{
		printf("_build_index() fail, ret=%d\n", ret);
		return 1;
	}

	ret = _bitcask_open_current_file(bc);
	if (ret != 0)
	{
		return 1;
	}
	return 0;
}

int _write_kv(bitcask* bc, const void* key, uint16_t key_len, const void* value, uint32_t value_len, uint64_t ts_ms)
{
	int32_t index = bc->active_file_index;
	int fd = bc->data_file_fds[index];
	uint32_t crc32 = 0;
	char buf[1024*1024];
	uint32_t len = 0;

	//crc32
	memcpy(&buf[len], &crc32, sizeof(crc32));
	len += sizeof(crc32);

	//ts_ms
	memcpy(&buf[len], &ts_ms, sizeof(ts_ms));
	len += sizeof(ts_ms);

	//key_len
	memcpy(&buf[len], &key_len, sizeof(key_len));
	len += sizeof(key_len);

	//value_len
	memcpy(&buf[len], &value_len, sizeof(value_len));
	len += sizeof(value_len);

	//key
	memcpy(&buf[len], key, key_len);
	len += key_len;

	//value
	memcpy(&buf[len], value, value_len);
	len += value_len;

	ssize_t ret = write(fd, buf, len);
	if (ret != len)
	{
		printf("write() crc fail, ret=%d, errno=%d\n", ret, errno);
		return 1;	
	}

	bc->active_file_position += len;
	
	return 0;
}


bitcask_item* _hash_table_find(bitcask_item** table, const void* key, uint16_t key_len)
{
	uint32_t hash_value = BKDRHash(key, key_len);
	uint32_t table_index = hash_value % TABLE_SIZE;
	bitcask_item* item = table[table_index];
	while (item)
	{
		if (item->key_len == key_len && !memcmp(item->key, key, key_len))
		{
			return item;
		}
		item = item->next;
	}
	return NULL;
}

bitcask_item* _hash_table_add(bitcask_item** table, const void* key, uint16_t key_len)
{
	bitcask_item* item = (bitcask_item*)malloc(sizeof(bitcask_item));
	item->key = (void*)malloc(key_len);
	memcpy(item->key, key, key_len);
	item->key_len = key_len;

	uint32_t hash_value = BKDRHash(key, key_len);
	uint32_t table_index = hash_value % TABLE_SIZE;
	if (!table[table_index])
	{
		table[table_index] = item;
		item->next = NULL;
	}
	else
	{
		item->next = table[table_index];
		table[table_index] = item;
	}
}

int bitcask_add(bitcask* bc, const void* key, uint16_t key_len, const void* value, uint32_t value_len)
{
	uint32_t active_file_old_position = bc->active_file_position;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	uint64_t ts_ms = tv.tv_sec * 1000 + tv.tv_usec/1000;

	int ret = _write_kv(bc, key, key_len, value, value_len, ts_ms);
	if (ret)
	{
		printf("_write_kv() fail. ret=%s\n", ret);
		return ret;
	}


	bitcask_item* item = _hash_table_find(bc->item_table, key, key_len);
	if (!item)
	{
		item = _hash_table_add(bc->item_table, key, key_len);
	}
	item->file_id = bc->active_file_index;
	item->value_len = value_len;
	item->file_pos = active_file_old_position;
	item->ts = ts_ms;

	return 0;
}

void bitcask_dump_info(bitcask* bc)
{
	printf("\n=================== dump info of bitcask context =====================\n");
	printf("directory=%s\n", bc->directory);
	for (int i = 0; i < MAX_DATA_FILE_COUNT; i++)
	{
		if (bc->data_file_fds[i] == -1)
		{
			continue;
		}
		printf("data_file_fds[%d]=%d\n", i, bc->data_file_fds[i]);
	}
	printf("active_file_index=%d\n", bc->active_file_index);
	printf("active_file_position=%d\n", bc->active_file_position);
	for (int i = 0; i < bc->item_table_size; i++)
	{
		bitcask_item* item = bc->item_table[i];
		while (item)
		{
			printf("table[%d] item: key_len=%d, file_id=%d, value_len=%d, file_pos=%d, ts=%ld\n",
					i, item->key_len, item->file_id, item->value_len, item->file_pos, item->ts);
			item = item->next;
		}
	}
}


