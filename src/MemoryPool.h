#ifndef MEMORY_POOL_H_
#define MEMORY_POOL_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#define USE_CPLUSCPLUS_MEMORY_POOL_FOR_PARTS 1
#define MEMORY_POOL_CHUNK_SIZE 1024
#define MEMORY_POOL_BLOCK_SIZE 8

#if USE_CPLUSCPLUS_MEMORY_POOL_FOR_PARTS == 1

template<typename type,
  int CHUNK_SIZE = MEMORY_POOL_CHUNK_SIZE,
  int BLOCK_SIZE = MEMORY_POOL_BLOCK_SIZE> class MemPool {
  public:
    MemPool() :
      block_of_chunks_size(BLOCK_SIZE),
      chunk_size(CHUNK_SIZE),
      num_allocated_chunks(0),
      current_block_index(0),
      current_chunk_index(0) {
      data_pool.reserve(10);
    };

    ~MemPool() {
      Cleanup();
    };

    type* GetNextBuffer() {
      if (data_pool.size() == 0) {
        //first time
        data_pool = std::vector<type*>(block_of_chunks_size);
        num_available_pos_for_chunks = block_of_chunks_size;

        current_block_index = 0;
        current_chunk_index = 0;
        //Alloc new chunk
        data_pool[current_block_index] = static_cast<type*>(::operator new(chunk_size * sizeof(type)));
        num_allocated_chunks++;
      }
      if (current_chunk_index == chunk_size) {
        //go to next chunk
        current_block_index++;
        current_chunk_index = 0;

        if (current_block_index == num_available_pos_for_chunks) {
          //I need to realloc
          data_pool.resize(num_available_pos_for_chunks + block_of_chunks_size);
          num_available_pos_for_chunks += block_of_chunks_size;
        }
        //Alloc new chunk
        data_pool[current_block_index] = static_cast<type*>(::operator new(chunk_size * sizeof(type)));
        num_allocated_chunks++;
      }
      return &((data_pool[current_block_index])[current_chunk_index++]);
    };

    void Cleanup() {
      if (data_pool.size() != 0) {
        while (num_allocated_chunks > 0) {
          num_allocated_chunks--;
          ::operator delete(data_pool[num_allocated_chunks]);
        }
        data_pool.clear();
        current_chunk_index = 0;
        current_block_index = 0;
      }
    };

  private:
    std::vector<type*> data_pool;
    int block_of_chunks_size;
    int chunk_size;
    int current_block_index;
    int current_chunk_index;
    int num_available_pos_for_chunks;
    int num_allocated_chunks;
};

#else

template<typename type,
  int CHUNK_SIZE = MEMORY_POOL_CHUNK_SIZE,
  int BLOCK_SIZE = MEMORY_POOL_BLOCK_SIZE> class MemPool {
  public:
    MemPool() :
      data_pool(NULL),
      block_of_chunks_size(BLOCK_SIZE),
      chunk_size(CHUNK_SIZE),
      num_allocated_chunks(0),
      current_block_index(0),
      current_chunk_index(0) {};

    ~MemPool() {
      Cleanup();
    };

    type* GetNextBuffer() {
      if (data_pool == NULL) {
        //first time
        if ((data_pool = (type**)malloc(block_of_chunks_size * sizeof(type*))) == NULL) {
          return NULL;
        }
        num_available_pos_for_chunks = block_of_chunks_size;

        current_block_index = 0;
        current_chunk_index = 0;
        //Alloc new chunk
        if ((data_pool[current_block_index] =
          (type*)malloc(chunk_size * sizeof(type))) == NULL) {
          current_chunk_index = 0;
          return NULL;
        }
        num_allocated_chunks++;
      }
      if (current_chunk_index == chunk_size) {
        //go to next chunk
        current_block_index++;
        current_chunk_index = 0;

        if (current_block_index == num_available_pos_for_chunks) {
          //I need to realloc
          type ** tmp;
          tmp = (type**)realloc(data_pool,
            (num_available_pos_for_chunks + block_of_chunks_size) * sizeof(type*));
          if (tmp == NULL)
            return NULL;
          data_pool = tmp;
          num_available_pos_for_chunks += block_of_chunks_size;
        }
        //Alloc new chunk
        if ((data_pool[current_block_index] =
          (type*)malloc(chunk_size * sizeof(type))) == NULL) {
          current_chunk_index = 0;
          return NULL;
        }
        num_allocated_chunks++;
      }
      return &((data_pool[current_block_index])[current_chunk_index++]);
    };

    void Cleanup() {
      if (data_pool) {
        while (num_allocated_chunks > 0)
          free(data_pool[--num_allocated_chunks]);

        free(data_pool);
        data_pool = NULL;
        current_chunk_index = 0;
        current_block_index = 0;
      }
    };

  private:
    type** data_pool;
    int block_of_chunks_size;
    int chunk_size;
    int current_block_index;
    int current_chunk_index;
    int num_available_pos_for_chunks;
    int num_allocated_chunks;
};

#endif /* USE_CPLUSCPLUS_MEMORY_POOL_FOR_PARTS */

#endif /* MEMORY_POOL_H_ */
