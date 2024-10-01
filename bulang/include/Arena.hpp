#pragma once

#include "Config.hpp"



const s64 stackSize = 16 * 1024;	
const s64 maxStackEntries = 256;
const s64 blockSizeCount = 14;

struct ArenaBlock;
struct ArenaChunk;

void *cMalloc(s64 size);

void cFree(void *p);

struct  StackEntry
{
	char* data;
	s64 size;
	bool usedMalloc;
};



class  BlockArena
{
public:
	BlockArena();
	~BlockArena();


	void* Allocate(s64 size);


	void Free(void* p, s64 size);

	void Reset();

	void Clear();

	s64  size() const { return m_total; }

private:

	ArenaChunk* m_chunks;
	s64 m_chunkCount;
	s64 m_chunkSpace;
	s64 m_total;
	s64 m_allocation;
	s64 m_deallocation;

	ArenaBlock* m_freeLists[blockSizeCount];
};




class  StackArena
{
public:
	StackArena();
	~StackArena();

	void* Allocate(s64 size);
	void Free(void* p);

	s64 GetMaxAllocation() const;

private:

	char m_data[stackSize];
	s64 m_index;

	s64 m_allocation;
	s64 m_maxAllocation;

	StackEntry m_entries[maxStackEntries];
	s64 m_entryCount;
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