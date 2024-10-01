#pragma once

#include "Config.hpp"



const s32 stackSize = 16 * 1024;	
const s32 maxStackEntries = 256;
const s32 blockSizeCount = 14;

struct ArenaBlock;
struct ArenaChunk;

void *cMalloc(s32 size);

void cFree(void *p);

struct  StackEntry
{
	char* data;
	s32 size;
	bool usedMalloc;
};



class  BlockArena
{
public:
	BlockArena();
	~BlockArena();


	void* Allocate(s32 size);


	void Free(void* p, s32 size);

	void Reset();

	void Clear();

	s32  size() const { return m_total; }

private:

	ArenaChunk* m_chunks;
	s32 m_chunkCount;
	s32 m_chunkSpace;
	s32 m_total;
	s32 m_allocation;
	s32 m_deallocation;

	ArenaBlock* m_freeLists[blockSizeCount];
};




class  StackArena
{
public:
	StackArena();
	~StackArena();

	void* Allocate(s32 size);
	void Free(void* p);

	s32 GetMaxAllocation() const;

private:

	char m_data[stackSize];
	s32 m_index;

	s32 m_allocation;
	s32 m_maxAllocation;

	StackEntry m_entries[maxStackEntries];
	s32 m_entryCount;
};

class Chunk
{
    u32 m_capacity;

public:
    Chunk(u32 capacity = 512);
    Chunk(Chunk *other);
    ~Chunk();

    void clear();
    void reserve(u32 capacity);

    void write(u8 instruction, int line);

    u32 capacity() const { return m_capacity; }

	int line(u32 index) const;

	u8 operator[](u32 index);

    bool clone(Chunk *other);

    u8 *code;
    int *lines;
    u32 count;
};