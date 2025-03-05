#include "../h/c/Byteset.h"

Byteset Byteset_init(unsigned char* sample, long long size) {
	if (size == NULL) {
		size = strlen(sample);
	}
	Byteset byteset = {NULL, size};
	byteset.data = malloc(byteset.size);
	if (byteset.data != NULL) {
		memcpy(byteset.data, sample, byteset.size);
	}
	return byteset;
}

bool Byteset_free(PByteset byteset)
{
	if (byteset->data!= NULL) {
		free(byteset->data);
		byteset->data = NULL;
		byteset->size = NULL;
	}
	return true;
}
bool Byteset_isEmpty(PByteset byteset)
{
	return (byteset->size == 0);
}