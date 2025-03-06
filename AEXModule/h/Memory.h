#pragma once
#include "Byteset.h"
#include "Text.h"
#include <iostream>
#include "./Memory/R3.h"
#include "./Memory/R3HOOK.h"
#include "./Memory/EmbeddedCall.h"
#include "./Memory/VEHHook.h"

/*
* 操作内存（注意管理员运行）
*/
namespace Memory {
	using namespace MemoryR3;//R3读写集合
	using namespace MemoryR3HOOK;//R3HOOK读写集合
	using namespace MemoryEmbeddedCall;//嵌入调用集合
	using namespace MemoryVEHHook;//VEHHook断点钩子集合
};