

tcask-c desgin

## bitcask的定位与目标

**bitcask是基于磁盘的、日志型（即追加方式）的KV系统，**

## 整体设计

* **支持一写多读架构，索引存放于共享内存，以支持多进程模式**
* **仅支持linux for now**
* **支持秒级过期机制**
* **所有相关文件全部存放在一个指定目录下，即一个目录代表一个bitcask实例**
* **数据文件命名规则为：0、1、2、...、n**

## 磁盘数据结构
**crc** + **ts** + **expire** + **key_len** + **value_len** + **key** + **value**

* **crc**: 当前kv对的crc值，32bit。
* **ts**: 当前kv插入的unix时间戳，32bit。
* **expire**: 超时时间，32bit, 即插入数据expire秒之后当前kv过期。
* **key_len**: key的长度，16bit, 即要求key不能超过65535字节
* **value_len**: value的长度，32bit。
* **key**: key的内容。
* **value**: value的内容。


## 内存数据结构

内存中存放所有KV对的索引，以hash表的形式组织，hash表的节点结构如下：
<br>
**key** + **key_len** + **file_id** + **file_pos** + **value_len** + **ts** + **expire**

* **key**: key的内容。
* **key_len**： key的长度，16bit。
* **file_id**： 当前kv所在的磁盘数据文件的id，16bit，即一个bitcask实例最多只能有65535个数据文件。
* **file_pos**： 当前kv在磁盘文件中的起始位置，32bit，即一个数据文件最大不能超过4GB。
* **value_len**： value的长度。
* **ts**： 当前kv插入的unix时间戳，32bit。
* **expire**: 超时时间，32bit, 即插入数据expire秒之后当前kv过期。

## 系统数据规格说明
* key的长度上限为1KB，考虑到实际应用中的需求及数据传输性能。
* value的长度上限为1MB，出发点同上。
* 每个数据文件的大小上限为4GB（@file_pos为32bit），处于性能考虑（？）
* 数据文件的数量上限为65535（@file_id为16bit），一个bitcask实例的总空间上限为4GB*65535，即256TB，考虑到bitcask的定位，该容量足够。
* 



 



