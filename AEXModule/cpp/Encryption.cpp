#include "../h/Encryption.h"
#include <Windows.h>
Byteset Encryption::DictionariesEncrypt(const Byteset& data, const Byteset& key)
{
	ULONG64 data_size = data.size(), key_size = key.size(), index = (data_size % key_size);
	Byteset result = data;
	for (ULONG64 i = 0; i < data_size; i++) {
		result[i] = (result[i] ^ key[index]) + key[index];
		index = (index + 1) % key_size;
	}
	return result;
}

Byteset Encryption::DictionariesDecrypt(const Byteset& data, const Byteset& key)
{
	ULONG64 data_size = data.size(), key_size = key.size(), index = (data_size % key_size);
	Byteset result = data;
	for (ULONG64 i = 0; i < data_size; i++) {
		result[i] = (result[i] - key[index]) ^ key[index];
		index = (index + 1) % key_size;
	}
	return result;
}

Byteset Encryption::MatrixEncrypt(const Byteset& data, const Byteset matrix, const Byteset& key)
{
	if(matrix.size() != 2 || key.size() != 16) return Byteset();
	ULONG64 key_size = key.size(), index = (data.size() % key_size);
	ULONG64 arr = data.size() / matrix[0] + (data.size() % matrix[0] != NULL);
	ULONG64 ma_arr = static_cast<ULONG64>(matrix[0] / matrix[1]) + (matrix[0] % matrix[1] != NULL);
	Byteset result = data;
	for (ULONG64 i = 0; i < arr; i++) {
		for (ULONG64 j = 0; j < ma_arr; j++) {
			result[i*matrix[0] + j] = (result[i*matrix[0] + j] ^ key[index]) + key[index];
			index = (index + 1) % key_size;
		}
	};
	return result;
}

Byteset Encryption::MatrixDecrypt(const Byteset& data, const Byteset matrix, const Byteset& key)
{
	if (matrix.size() != 2 || key.size() != 16) return Byteset();
	ULONG64 key_size = key.size(), index = (data.size() % key_size);
	ULONG64 arr = data.size() / matrix[0] + (data.size() % matrix[0] != NULL);
	ULONG64 ma_arr = static_cast<ULONG64>(matrix[0] / matrix[1]) + (matrix[0] % matrix[1] != NULL);
	Byteset result = data;
	for (ULONG64 i = 0; i < arr; i++) {
		for (ULONG64 j = 0; j < ma_arr; j++) {
			result[i*matrix[0] + j] = (result[i*matrix[0] + j] - key[index]) ^ key[index];
			index = (index + 1) % key_size;
		}
	};
	return result;
}
