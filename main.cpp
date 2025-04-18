#include "AEXModule.h"

int main() {
    Memory::R3 r3;
    r3.Open(Process::ProcessGetID("TheLastSurvivor-Win64-Shipping.exe"));
    std::vector<PVOID> result = r3.Search("48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 83 EC 20 49 8B 59 38 48 8B F2 4D 8B F0 48 8B E9 49 8B D1 48 8B CE 49 8B F9 4C 8D 43 04",true);
    for (auto& ptr : result) {
        std::cout << "Found at " << ptr << std::endl;
    }
    return 0;
}