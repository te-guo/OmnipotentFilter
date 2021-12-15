#ifndef CUCKOOFILTER_H_
#define CUCKOOFILTER_H_

#include<string.h>
#include<stdlib.h>
#include"hashing.h"
#include"bithack.h"

#define MaxNumKicks 500

using namespace std;

typedef struct {
	size_t index;
	uint32_t fingerprint;
} Victim;



class CuckooFilter{
private:

	int capacity;
	size_t single_table_length;
	size_t fingerprint_size;
	size_t bytes_per_bucket;

	char* bucket;

	uint32_t mask;
	bool valid;

public:

	bool is_full;
	int counter;
	CuckooFilter* _0_child;
	CuckooFilter* _1_child;
	int level;

	//construction and distruction function
	CuckooFilter(const size_t single_table_length, const size_t fingerprint_size, const int capacity, int curlevel);
	~CuckooFilter();

	bool is_valid();
	void invalidate();
	//insert & query & delete function
	bool insertItem(const size_t index, const uint32_t fingerprint, const uint32_t prefix, Victim &victim);
	bool insertImpl(const size_t index, const uint32_t fingerprint, const bool kickout, Victim &victim);
	bool queryImpl(const size_t index, const uint32_t fingerprint);
	bool deleteImpl(const size_t index, const uint32_t fingerprint);

	//generate two candidate bucket addresses
	void generateA(size_t index, uint32_t fingerprint, uint32_t prefix, size_t &alt_index, size_t single_table_length);

	//read from bucket & write into bucket
	uint32_t read(const size_t index, const size_t pos);
	void write(const size_t index, const size_t pos, const uint32_t fingerprint);
	size_t actual_size_in_bytes();

};

#endif //CUCKOOFILTER_H_
