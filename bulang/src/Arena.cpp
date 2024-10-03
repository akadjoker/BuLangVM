#include "pch.h"
#include "Arena.hpp"
#include "Utils.hpp"

static const s64 chunkSize = 16 * 1024;
static const s64 maxBlockSize = 640;
static const s64 chunkArrayIncrement = 128;

// These are the supported object sizes. Actual allocations are rounded up the next size.
static const s64 blockSizes[blockSizeCount] =
	{
		16,	 // 0
		32,	 // 1
		64,	 // 2
		96,	 // 3
		128, // 4
		160, // 5
		192, // 6
		224, // 7
		256, // 8
		320, // 9
		384, // 10
		448, // 11
		512, // 12
		640, // 13
};



void *cMalloc(s64 size)
{
	void *mem = malloc(size);

	return mem;
}

void cFree(void *mem)
{

	free(mem);
}



struct SizeMap
{
	SizeMap()
	{
		s64 j = 0;
		values[0] = 0;
		for (s64 i = 1; i <= maxBlockSize; ++i)
		{
			assert(j < blockSizeCount);
			if (i <= blockSizes[j])
			{
				values[i] = (u8)j;
			}
			else
			{
				++j;
				values[i] = (u8)j;
			}
		}
	}

	u8 values[maxBlockSize + 1];
};

static const SizeMap sizeMap;

struct ArenaChunk
{
	s64 blockSize;
	ArenaBlock *blocks;
};

struct ArenaBlock
{
	ArenaBlock *next;
};

BlockArena::BlockArena()
{
	assert(blockSizeCount < UCHAR_MAX);

	m_chunkSpace = chunkArrayIncrement;
	m_chunkCount = 0;
	m_chunks = (ArenaChunk *)cMalloc(m_chunkSpace * sizeof(ArenaChunk));

	memset(m_chunks, 0, m_chunkSpace * sizeof(ArenaChunk));
	memset(m_freeLists, 0, sizeof(m_freeLists));
	m_allocation = 0;
	m_deallocation = 0;
	m_total = 0;
}

BlockArena::~BlockArena()
{
	Clear();
}

void *BlockArena::Allocate(s64 size)
{
	if (size == 0)
	{
		return nullptr;
	}

	assert(0 < size);
	m_total += size;
	m_allocation += size;

	if (size > maxBlockSize)
	{
		return cMalloc(size);
	}

	s64 index = sizeMap.values[size];
	assert(0 <= index && index < blockSizeCount);

	if (m_freeLists[index])
	{
		ArenaBlock *block = m_freeLists[index];
		m_freeLists[index] = block->next;
		return block;
	}
	else
	{
		if (m_chunkCount == m_chunkSpace)
		{
			ArenaChunk *oldChunks = m_chunks;
			m_chunkSpace += chunkArrayIncrement;
			m_chunks = (ArenaChunk *)cMalloc(m_chunkSpace * sizeof(ArenaChunk));
			memcpy(m_chunks, oldChunks, m_chunkCount * sizeof(ArenaChunk));
			memset(m_chunks + m_chunkCount, 0, chunkArrayIncrement * sizeof(ArenaChunk));
			cFree(oldChunks);
		}

		ArenaChunk *chunk = m_chunks + m_chunkCount;
		chunk->blocks = (ArenaBlock *)cMalloc(chunkSize);
#if defined(_DEBUG)
		memset(chunk->blocks, 0xcd, chunkSize);
#endif
		s64 blockSize = blockSizes[index];
		chunk->blockSize = blockSize;
		s64 blockCount = chunkSize / blockSize;
		assert(blockCount * blockSize <= chunkSize);
		for (s64 i = 0; i < blockCount - 1; ++i)
		{
			ArenaBlock *block = (ArenaBlock *)((c8 *)chunk->blocks + blockSize * i);
			ArenaBlock *next = (ArenaBlock *)((c8 *)chunk->blocks + blockSize * (i + 1));
			block->next = next;
		}
		ArenaBlock *last = (ArenaBlock *)((c8 *)chunk->blocks + blockSize * (blockCount - 1));
		last->next = nullptr;

		m_freeLists[index] = chunk->blocks->next;
		++m_chunkCount;

		return chunk->blocks;
	}
}

void BlockArena::Free(void *p, s64 size)
{
	if (size == 0)
	{
		return;
	}

	assert(0 < size);

	m_deallocation += size;
	m_total -= size;

	if (size > maxBlockSize)
	{
		cFree(p);
		return;
	}

	s64 index = sizeMap.values[size];
	assert(0 <= index && index < blockSizeCount);

#if defined(_DEBUG)
	// Verify the memory address and size is valid.
	s64 blockSize = blockSizes[index];
	bool found = false;
	for (s64 i = 0; i < m_chunkCount; ++i)
	{
		ArenaChunk *chunk = m_chunks + i;
		if (chunk->blockSize != blockSize)
		{
			assert((c8 *)p + blockSize <= (c8 *)chunk->blocks ||
				   (c8 *)chunk->blocks + chunkSize <= (c8 *)p);
		}
		else
		{
			if ((c8 *)chunk->blocks <= (c8 *)p && (c8 *)p + blockSize <= (c8 *)chunk->blocks + chunkSize)
			{
				found = true;
			}
		}
	}

	assert(found);

	memset(p, 0xfd, blockSize);
#endif

	ArenaBlock *block = (ArenaBlock *)p;
	block->next = m_freeLists[index];
	m_freeLists[index] = block;
}

void BlockArena::Reset()
{
	m_chunkCount = 0;
	memset(m_chunks, 0, m_chunkSpace * sizeof(ArenaChunk));
	memset(m_freeLists, 0, sizeof(m_freeLists));
}

void BlockArena::Clear()
{
	//if (m_chunkCount == 0) return;
	for (s64 i = 0; i < m_chunkCount; ++i)
	{
		cFree(m_chunks[i].blocks);
		
	}

	cFree(m_chunks);
	m_chunks = nullptr;

	m_chunkCount = 0;
	memset(m_freeLists, 0, sizeof(m_freeLists));
	m_allocation = 0;
	m_deallocation = 0;
	m_total = 0;
	
}

StackArena::StackArena()
{
	m_index = 0;
	m_allocation = 0;
	m_maxAllocation = 0;
	m_entryCount = 0;
}

StackArena::~StackArena()
{
	assert(m_index == 0);
	assert(m_entryCount == 0);
}

void *StackArena::Allocate(s64 size)
{
	assert(m_entryCount < maxStackEntries);

	StackEntry *entry = m_entries + m_entryCount;
	entry->size = size;
	if (m_index + size > stackSize)
	{
		entry->data = (char *)cMalloc(size);
		entry->usedMalloc = true;
	}
	else
	{
		entry->data = m_data + m_index;
		entry->usedMalloc = false;
		m_index += size;
	}

	m_allocation += size;
	m_maxAllocation = Max<s64>(m_maxAllocation, m_allocation);
	++m_entryCount;

	return entry->data;
}

void StackArena::Free(void *p)
{
	assert(m_entryCount > 0);
	StackEntry *entry = m_entries + m_entryCount - 1;
	assert(p == entry->data);
	if (entry->usedMalloc)
	{
		cFree(p);
	}
	else
	{
		m_index -= entry->size;
	}
	m_allocation -= entry->size;
	--m_entryCount;

	p = nullptr;
}

s64 StackArena::GetMaxAllocation() const
{
	return m_maxAllocation;
}




Chunk::Chunk(u32 capacity)
    :  m_capacity(capacity), count(0)
{
    code  = (u8*)  std::malloc(capacity * sizeof(u8));
    lines = (int*) std::malloc(capacity * sizeof(int));
	if (!code || !lines)
	{
		std::free(code);
		std::free(lines);
		DEBUG_BREAK_IF(code == nullptr || lines == nullptr);
		return;
	}
    
}

Chunk::Chunk(Chunk *other)
{

    code  = (u8*)  std::malloc(other->m_capacity * sizeof(u8));
    lines = (int*) std::malloc(other->m_capacity * sizeof(int));
	if (!code || !lines)
	{
		std::free(code);
		std::free(lines);
		DEBUG_BREAK_IF(code == nullptr || lines == nullptr);
		return;
	}
    
    m_capacity = other->m_capacity;
    count = other->count;

    std::memcpy(code, other->code, other->m_capacity * sizeof(u8));
    std::memcpy(lines, other->lines, other->m_capacity * sizeof(int));
}


bool Chunk::clone(Chunk *other)
{
     if (!other)
        return false;

    
    std::free(other->code);
    std::free(other->lines);

    
    other->m_capacity = m_capacity;
    other->count = count;

    other->code = (u8*) std::malloc(m_capacity * sizeof(u8));
    other->lines = (int*) std::malloc(m_capacity * sizeof(int));

    if (!other->code || !other->lines)
    {
        std::free(other->code);
        std::free(other->lines);
        DEBUG_BREAK_IF(other->code == nullptr || other->lines == nullptr);
        return false;
    }

    std::memcpy(other->code, code, count * sizeof(u8));
    std::memcpy(other->lines, lines, count * sizeof(int));

    return true;
    
}


Chunk::~Chunk()
{
    std::free(code);
    std::free(lines);
}

void Chunk::reserve(u32 capacity)
{
    if (capacity > m_capacity)
    {
       

        u8 *newCode  = (u8*) (std::realloc(code,  capacity * sizeof(u8)));
        int *newLine = (int*)(std::realloc(lines, capacity * sizeof(int)));

        if (!newCode || !newLine)
        {
            std::free(newCode);
            std::free(newLine);
            DEBUG_BREAK_IF(newCode == nullptr || newLine == nullptr);
            return;
        }



        code = newCode;
        lines = newLine;      
        m_capacity = capacity;
    }
}



void Chunk::write(u8 instruction, int line)
{
    if (m_capacity < count + 1)
    {
        int oldCapacity = m_capacity;
        m_capacity = GROW_CAPACITY(oldCapacity);
        u8 *newCode  = (u8*) (std::realloc(code,  m_capacity * sizeof(u8)));
        int *newLine = (int*)(std::realloc(lines, m_capacity * sizeof(int)));
        if (!newCode || !newLine)
        {
            std::free(newCode);
            std::free(newLine);
            DEBUG_BREAK_IF(newCode == nullptr || newLine == nullptr);
            return;
        }
        code = newCode;
        lines = newLine;      

    }
    
    code[count]  = instruction;
    lines[count] = line;
    count++;
}


int Chunk::line(u32 index) const
{
	DEBUG_BREAK_IF(index > m_capacity);
	return lines[index];
}
u8 Chunk::operator[](u32 index)
{
    DEBUG_BREAK_IF(index > m_capacity);
    return code[index];
}