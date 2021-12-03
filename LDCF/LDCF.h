#ifndef LOGARITHMICDYNAMICCUCKOOFILTER_H_
#define LOGARITHMICDYNAMICCUCKOOFILTER_H_


#include"cuckoofilter.h"
#include<list>
#include<math.h>
#include<iostream>



class LogarithmicDynamicCuckooFilter{
private:

	int capacity;
	int single_capacity;

	int single_table_length;

	double false_positive;
	double single_false_positive;

	double fingerprint_size_double;
	int fingerprint_size;

	Victim victim;

	CuckooFilter *root;

public:

	//record the items inside DCF
	int counter;
	int cuckoo_counter;

	// the link tree strutcture of building blocks CF1, CF2, ...

	//construction & distruction functions
	LogarithmicDynamicCuckooFilter(const size_t capacity, const double false_positive, const size_t exp_block_num = 6);
	~LogarithmicDynamicCuckooFilter();

	void decompose(CuckooFilter* curCF);
	//insert & query & delete functions
	bool insertItem(uint64_t item);
	CuckooFilter* getChild0CF(CuckooFilter* curCF);
	CuckooFilter* getChild1CF(CuckooFilter* curCF);
	bool queryItem(uint64_t item);
	bool deleteItem(uint64_t item);

	bool remove(CuckooFilter* cf_remove);

	//generate 2 bucket addresses
	void generateIF(uint64_t item, size_t &index, uint32_t &fingerprint, size_t fingerprint_size, size_t single_table_length);
	void generateA(size_t index, uint32_t fingerprint, size_t &alt_index, size_t single_table_length);
	
	bool GetPre(uint32_t fingerprint, int level);
	uint32_t MaskFP(uint32_t fingerprint, int level);

	//get info of DCF
	int getFingerprintSize();
	float size_in_mb();

	//extra function to make sure the table length is the power of 2
	uint64_t upperpower2(uint64_t x);

	void info();
};







#endif //LOGARITHMICDYNAMICCUCKOOFILTER_H
