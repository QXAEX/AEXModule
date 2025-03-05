#include "../h/AEXStore.h"
static const Byteset structCode({ 255,255,255,255,255 });
static const int structSize = 55;
static const Byteset Encrypt(const Byteset data) {
	return Encryption::MatrixEncrypt(data, {3,3});
};
static const Byteset Decrypt(const Byteset data) {
	return Encryption::MatrixDecrypt(data, { 3,3 });
};

Byteset AEXStore::createStruct(const Byteset name, const Byteset data, bool& flag, PAEX_STORE_STRUCT structData) const
{
	Byteset result;
	if (name.size() > 255) {
		flag = false;
		return result;
	}
	if (structData == NULL) {
		std::string time = System::GetTime(true);
		result.append(this->token);
		result.append(structCode);
		result.append(static_cast<char>(name.size()));
		result.append(structSize);
		result.append(time);
		result.append(time);
		result.append((long long)data.size());
		result.append(0);
		result.append(Encrypt(name));
		result.append(Encrypt(data));
	} else {
		result.append(structData->token);
		result.append(structCode);
		result.append(static_cast<char>(structData->name.size()));
		result.append(structData->structSize);
		result.append(structData->createTime);
		result.append(structData->updateTime);
		result.append(structData->dataSize);
		result.append(structData->updateCount);
		result.append(Encrypt(structData->name));
		result.append(Encrypt(structData->data));
	}
	flag = true;
	return result;
}

AEX_STORE_STRUCT AEXStore::findStruct(const Byteset name, bool& flag)
{
	AEX_STORE_STRUCT result;
	flag = false;
	size_t index = file.Find(this->token,NULL);
	while (index != -1) {
		AEX_STORE_STRUCT obj;
		obj.index = index;
		__int64 readSize = structSize;
		file.Seek(obj.index +this->token.size() + structCode.size());
		Byteset temp = file.Read(readSize);
		int nameSize = (int)temp.subBytes(0, 1).data()[0];
		obj.dataSize = (__int64)*temp.subBytes(43, 8).data();
		readSize += nameSize + obj.dataSize;
		obj.structSize = (int)*temp.subBytes(1, 4).data();
		file.Seek(obj.index +this->token.size() + structCode.size()+ obj.structSize);
		obj.name = Decrypt(file.Read(nameSize));
		if (obj.name == name) {
			file.Seek(obj.index + this->token.size() + structCode.size());
			temp = file.Read(readSize);
			obj.token = this->token;
			obj.createTime = std::string((char*)temp.subBytes(5, 19).append({0,0}).data());
			obj.updateTime = std::string((char*)temp.subBytes(24, 19).append({0,0}).data());
			obj.updateCount = (int)*temp.subBytes(51, 4).data();
			obj.data = Decrypt(temp.subBytes(55 + obj.name.size(), obj.dataSize));
			temp = file.Read(obj.dataSize);
			result = obj;
			flag = true;
			break;
		}
		index = file.Find(this->token, index+1);
	};
	return result;
}

bool AEXStore::updateStruct(const Byteset name, const Byteset data)
{
	bool flag = false;
	AEX_STORE_STRUCT obj = this->findStruct(name, flag);
	if (flag) {
		file.Seek(obj.index + this->token.size() + structCode.size() + 43);
		file.Write((long long)data.size());
		file.Seek(obj.index + this->token.size() + structCode.size() + 55 + obj.name.size());
		file.Replace(Encrypt(data), obj.dataSize);
		file.Seek(obj.index + this->token.size() + structCode.size() + 51);
		file.Write(static_cast<char>(obj.updateCount + 1));
		file.Seek(obj.index + this->token.size() + structCode.size() + 24);
		file.Write(System::GetTime(true));
	}
	return flag;
}

bool AEXStore::deleteStruct(const Byteset name)
{
	bool flag = false;
	AEX_STORE_STRUCT obj = this->findStruct(name, flag);
	if (flag) {
		file.Seek(obj.index);
		flag = file.Delete(obj.token.size() + structCode.size() + obj.structSize);
	}
	return flag;
}

AEX_STORE_STRUCT AEXStore::analyzeStruct(const Byteset data, Byteset tokenName, int index, bool& flag) const
{
	AEX_STORE_STRUCT obj;
	flag = false;
	__int64 index_ = NULL;
	int count = 0;
	index_ = data.find(tokenName, index_);
	while (index_ != -1) {
		AEX_STORE_STRUCT obj_;
		obj_.index = index_;
		obj_.token = tokenName;
		obj_.structSize = (int)*data.subBytes(obj_.index + obj_.token.size() + structCode.size() + 1, 4).data();
		obj_.name = Decrypt(data.subBytes(obj_.index + obj_.token.size() + structCode.size() + obj_.structSize, (long long)data.subBytes(obj_.index + obj_.token.size() + structCode.size(), 1).data()[0]));
		obj_.createTime = std::string((char*)data.subBytes(obj_.index + obj_.token.size() + structCode.size() + 5, 19).append({ 0,0 }).data());
		obj_.updateTime = std::string((char*)data.subBytes(obj_.index + obj_.token.size() + structCode.size() + 24, 19).append({ 0,0 }).data());
		obj_.dataSize = (long long)*data.subBytes(obj_.index + obj_.token.size() + structCode.size() + 43, 8).data();
		obj_.updateCount = (int)*data.subBytes(obj_.index + obj_.token.size() + structCode.size() + 51, 4).data();
		obj_.data = Decrypt(data.subBytes(obj_.index + obj_.token.size() + structCode.size() + 55 + obj_.name.size(), obj_.dataSize));
		flag = true;
		count++;
		if (count > index) {
			obj = obj_;
			break;
		}
		index_ = data.find(tokenName, obj_.index + 1);
	};
	return obj;
}

AEXStore::AEXStore()
{
	this->token = "<|--AEXStore--|>";
}

AEXStore::AEXStore(std::string path, bool create)
{
	this->Load(path,create);
}

AEXStore::~AEXStore()
{
	this->Free();
}

std::string AEXStore::getPath()
{
	return this->path;
}

Byteset AEXStore::getToken() const
{
	return this->token;
}

void AEXStore::setToken(Byteset token)
{
	this->token = token;
}

bool AEXStore::Load(std::string path, bool create)
{
	this->path = path;
	return this->file.Open(path,create);
}

bool AEXStore::Free(bool delFile)
{
	bool flag = this->file.Close();
	if (delFile) {
		this->file.DelFile();
	}
	return flag;
}

bool AEXStore::writeData(const Byteset name, const Byteset data, bool flag) 
{
	bool result = false;
	this->findStruct(name, result);
	if (result) {//ÐÞ¸Ä
		result = this->updateStruct(name, data);
	} else if(flag){//´´½¨
		file.Seek(file.Size());
		result = file.Write(this->createStruct(name, data, flag));
	};
	return result;
}

bool AEXStore::readData(const Byteset name, AEX_STORE_STRUCT& data)
{
	bool flag = false;
	AEX_STORE_STRUCT obj = this->findStruct(name, flag);
	if (flag) data = obj;
	return flag;
}

bool AEXStore::deleteData(const Byteset name)
{
	return this->deleteStruct(name);
}

std::vector<Byteset> AEXStore::getAllKeys(const Byteset tokenName,int pegeNum,int pageSize) 
{
	std::vector<Byteset> keys;
	size_t index = this->file.Find(tokenName,NULL);
	int count = 0;
	while (index != -1 && count <= (pegeNum * pageSize) + pageSize) {
		if (count >= pegeNum*pageSize) break;
		count++;
		AEX_STORE_STRUCT obj;
		obj.index = index;
		size_t readSize = structSize;
		this->file.Seek(obj.index + tokenName.size() + structCode.size());
		Byteset temp = this->file.Read(readSize);
		int nameSize = (int)temp.subBytes(0, 1).data()[0];
		obj.structSize = (int)*temp.subBytes(1, 4).data();
		this->file.Seek(obj.index + tokenName.size() + structCode.size() + obj.structSize);
		obj.name = Decrypt(this->file.Read(nameSize));
		keys.push_back(obj.name);
		index = this->file.Find(tokenName, index+1);
	};
	return keys;
}

Byteset AEXStore::restoreStruct(AEX_STORE_STRUCT data) const
{
	bool flag = false;
	return this->createStruct(NULL, NULL, flag, &data);
}
