/*************************************************************************
    > File Name: bitcask.h
    > Author: alionkun
    > Mail: alionkun@gmail.com 
    > Created Time: Sat 13 May 2017 11:08:59 PM PDT
 ************************************************************************/
#include <stdint.h>

#define MAX_KEY_LEN (1<<10)
#define MAX_VALUE_LEN (1<<20)

#define MAX_DIRECTORY_LEN 1024
#define MAX_DATA_FILE_COUNT 1024
#define TABLE_SIZE 10000

enum
{
	EC_OK = 0;
	EC_IO_FAIL = 1;
	EC_BAD_PARAM = 2;
	EC_OBJ_EXIST = 3;
	EC_OBJ_NOT_EXIST = 4;
}

typedef struct _bitcask_bitcask_item
{
	void* key;
	uint16_t key_len;
	uint16_t file_id;
	uint32_t value_len;
	uint32_t file_pos;
	uint64_t ts;
	struct _bitcask_bitcask_item* next;
} bitcask_item;

typedef struct
{
	char directory[MAX_DIRECTORY_LEN];
	int data_file_fds[MAX_DATA_FILE_COUNT];
	int32_t active_file_index;
	uint32_t active_file_position;
	bitcask_item** item_table;
	uint16_t item_table_size;
} bitcask;


bitcask_item* _hash_table_find(bitcask_item** table, const void* key, uint16_t key_len);
bitcask_item* _hash_table_add(bitcask_item** table, const void* key, uint16_t key_len);


int bitcask_init(bitcask* bc, const char* dir);

/* add when not exist */
int bitcask_add(bitcask* bc, const void* key, uint16_t key_len, const void* value, uint32_t value_len);

/* delete */
int bitcask_delete(bitcask* bc, const void* key, uint16_t key_len);

/* set anyway */
int bitcask_set(bitcask* bc, const void* key, uint16_t key_len, const void* value, uint32_t value_len);

/* get */
int bitcask_get(bitcask* bc, const void* key, uint16_t key_len, void* value, uint32_t* value_len);

/* merge */
int bitcask_merge();

void bitcask_dump_info(bitcask* bc);

