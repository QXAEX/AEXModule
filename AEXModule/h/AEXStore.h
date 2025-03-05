#pragma once
#include "File.h"
#include "System.h"
#include "Encryption.h"
typedef struct AEX_STORE_STRUCT {
	size_t  index;//数据位置
	Byteset token;//标识
	Byteset name;//名称
	Byteset data;//数据
	int structSize;//结构大小
	long long dataSize = NULL;//数据大小
	int updateCount;//修改次数
	std::string createTime;//创建时间 System::GetTime(true);
	std::string updateTime;//修改时间 System::GetTime(true);
} *PAEX_STORE_STRUCT;
/*
* version 1.0.0
*/
class AEXStore {
private:
	bool isOpened = false;
	std::string path;
	File file;
public:
	Byteset token;
public:
	/*
	* 创建结构
	* @param name 名称
	* @param data 数据
	* @param flag 是否成功
	* @param structData 结构数据,默认值为空
	* @return 返回结构
	*/
	Byteset createStruct(const Byteset name, const Byteset data, bool& flag, PAEX_STORE_STRUCT structData = NULL) const;
	/*
	* 找到指定名称的结构
	* @param name 名称
	* @param flag 是否存在
	* @return 返回结构
	*/
	AEX_STORE_STRUCT findStruct(const Byteset name, bool& flag);
	/*
	* 找到指定名称的结构
	* @param name 名称
	* @param flag 是否存在
	* @return 返回结构
	*/
	//AEX_STORE_STRUCT findStruct(const Byteset name, bool& flag) const;
	/*
	* 更新指定名称的结构
	* @param name 名称
	* @param data 数据
	* @return 是否成功
	*/
	bool updateStruct(const Byteset name, const Byteset data);
	/*
	* 删除指定名称的结构
	* @param name 名称
	* @return 返回是否成功
	*/
	bool deleteStruct(const Byteset name);
	/*
	* 分析结构
	* @param data 结构数据
	* @param tokenName token名称
	* @param index 索引,如果结构数据中包含多个token结构,则需要指定索引,从0开始
	* @param flag 是否成功
	* @return 返回结构
	*/
	AEX_STORE_STRUCT analyzeStruct(const Byteset data, Byteset tokenName, int index, bool& flag) const;
	/*
	* 数据还原结构,将数据结构体的数据还原成字节集
	* @param data 结构数据
	* @return 返回字节集
	*/
	Byteset restoreStruct(AEX_STORE_STRUCT data) const;
public:
	AEXStore();
	AEXStore(std::string path, bool create = false);
	~AEXStore();
	// 获取仓库路径
	std::string getPath();
	/*
	* 获取当前token标识
	*/
	Byteset getToken() const;
	/*
	* 设置当前token标识
	*/
	void setToken(Byteset token);
	/*
	* 载入仓库
	* @param path 仓库路径
	* @param create 是否创建仓库
	* @return 成功返回true，失败返回false
	*/
	bool Load(std::string path, bool create = false);
	/*
	* 释放仓库
	* @param delFile 是否删除文件
	* @return 成功返回true，失败返回false
	*/
	bool Free(bool delFile = false);
	/*
	* 写数据
	* @param name 名称
	* @param data 数据
	* @param flag 不存在则创建,默认false
	* @return 是否成功
	*/
	bool writeData(const Byteset name, const Byteset data, bool flag = false);
	/*
	* 读数据
	* @param name 名称
	* @param data 数据结构
	* @return 是否成功
	*/
	bool readData(const Byteset name, AEX_STORE_STRUCT& data);
	/*
	* 删除数据
	* @param name 名称
	* @return 是否成功
	*/
	bool deleteData(const Byteset name);
	/**
	* 取Token下所有Key
	* @param tokenName Token名称
	* @param pegeNum 页数
	* @param pageSize 页大小
	* @return 所有Key
	*/
	std::vector<Byteset> getAllKeys(const Byteset tokenName, int pegeNum, int pageSize);
};