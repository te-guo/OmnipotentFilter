#include <iostream>
#include "LDCF.h"


using namespace std;


LogarithmicDynamicCuckooFilter::LogarithmicDynamicCuckooFilter(const size_t item_num, const size_t fingerprint_length, const size_t exp_block_num){

	capacity = item_num;

	single_table_length = upperpower2(capacity/4.0/exp_block_num);
	single_capacity = single_table_length * 4 * 0.9375;

	fingerprint_size = fingerprint_length;

	counter = 0;
	cuckoo_counter = 1;
	root = new CuckooFilter(single_table_length, fingerprint_size, single_capacity, 0);
	actual_size_counter = root->actual_size_in_bytes();
}

LogarithmicDynamicCuckooFilter::~LogarithmicDynamicCuckooFilter(){
	delete root;
}

void LogarithmicDynamicCuckooFilter::decompose(CuckooFilter* curCF){
	getChild0CF(curCF);
	getChild1CF(curCF);
	for(int index = 0; index < single_table_length; index++)
		for(int slot = 0; slot < 4; slot++){
			uint32_t fingerprint = curCF->read(index, slot);
			if(fingerprint != 0u)
				if(GetPre(fingerprint, curCF->level))
					curCF->_0_child->write(index, slot, MaskFP(fingerprint, curCF->_0_child->level));
				else
					curCF->_1_child->write(index, slot, MaskFP(fingerprint, curCF->_1_child->level));
		}
	cuckoo_counter--;
	actual_size_counter -= curCF->actual_size_in_bytes();
	curCF->invalidate();
}

bool LogarithmicDynamicCuckooFilter::insertItem(uint64_t item){
	size_t index;
	uint32_t fingerprint, prefix = 0;
	generateIF(item, index, fingerprint, fingerprint_size, single_table_length);

	CuckooFilter *curCF = root;
	while(!curCF->is_valid() || curCF->is_full == true){
		if(curCF->is_valid())
			decompose(curCF);
		if(GetPre(fingerprint, curCF->level))
			curCF = getChild0CF(curCF);
		else
			curCF = getChild1CF(curCF), prefix |= 1u << fingerprint_size - curCF->level;
		fingerprint = MaskFP(fingerprint, curCF->level);
	}

	if(curCF->insertItem(index, fingerprint, prefix, victim)){
		counter++;
	}else{
		while(true){
			decompose(curCF);
			if(GetPre(victim.fingerprint, curCF->level))
				curCF = getChild0CF(curCF);
			else
				curCF = getChild1CF(curCF), prefix |= 1u << fingerprint_size - curCF->level;
			victim.fingerprint = MaskFP(victim.fingerprint, curCF->level);
			if(curCF->insertItem(victim.index, victim.fingerprint, prefix, victim))
				break;
		}
		counter++;
	}

	return true;
}

CuckooFilter* LogarithmicDynamicCuckooFilter::getChild0CF(CuckooFilter* curCF){
	if(curCF->_0_child == NULL){
		cuckoo_counter++;
		curCF->_0_child = new CuckooFilter(single_table_length, fingerprint_size, single_capacity, curCF->level + 1);
		actual_size_counter += curCF->_0_child->actual_size_in_bytes();
	}
	return curCF->_0_child;
}

CuckooFilter* LogarithmicDynamicCuckooFilter::getChild1CF(CuckooFilter* curCF){
	if(curCF->_1_child == NULL){
		cuckoo_counter++;
		curCF->_1_child = new CuckooFilter(single_table_length, fingerprint_size, single_capacity, curCF->level + 1);
		actual_size_counter += curCF->_1_child->actual_size_in_bytes();
	}
	return curCF->_1_child;
}

bool LogarithmicDynamicCuckooFilter::queryItem(uint64_t item){
	size_t index, alt_index;
	uint32_t fingerprint;

	CuckooFilter* query_pt = root;
	generateIF(item, index, fingerprint, fingerprint_size, single_table_length);
	generateA(index, fingerprint, alt_index, single_table_length);
	while(!query_pt->is_valid()){
		if(GetPre(fingerprint, query_pt->level))
			query_pt = query_pt->_0_child;
		else
			query_pt = query_pt->_1_child;
		fingerprint = MaskFP(fingerprint, query_pt->level);
	}
	if(query_pt->queryImpl(index, fingerprint))
		return true;
	else if(query_pt->queryImpl(alt_index, fingerprint))
		return true;
	return false;
}

bool LogarithmicDynamicCuckooFilter::deleteItem(uint64_t item){
	size_t index, alt_index;
	uint32_t fingerprint;

	CuckooFilter* delete_pt = root;
	generateIF(item, index, fingerprint, fingerprint_size, single_table_length);
	generateA(index, fingerprint, alt_index, single_table_length);
	while(!delete_pt->is_valid()){
		if(GetPre(fingerprint, delete_pt->level))
			delete_pt = delete_pt->_0_child;
		else
			delete_pt = delete_pt->_1_child;
		fingerprint = MaskFP(fingerprint, delete_pt->level);
	}
	if(delete_pt->queryImpl(index, fingerprint)){
		if(delete_pt->deleteImpl(index, fingerprint)){
			counter--;
			return true;
		}
	}else if(delete_pt->queryImpl(alt_index, fingerprint)){
		if(delete_pt->deleteImpl(alt_index ,fingerprint)){
			counter--;
			return true;
		}
	}
	return false;
}



void LogarithmicDynamicCuckooFilter::generateIF(uint64_t item, size_t &index, uint32_t &fingerprint, size_t fingerprint_size, size_t single_table_length){
	uint64_t hash = hash_func_64bit(item);
	index = (hash >> 32) % single_table_length;
	fingerprint = hash & (1u << fingerprint_size) - 1u;
	fingerprint |= (uint32_t)(fingerprint == 0);
}

void LogarithmicDynamicCuckooFilter::generateA(size_t index, uint32_t fingerprint, size_t &alt_index, size_t single_table_length){
	alt_index = (index ^ hash_func3_32bit(fingerprint >> 1)) % single_table_length;
}

bool LogarithmicDynamicCuckooFilter::GetPre(uint32_t fingerprint, int level){
	if ((fingerprint >> (fingerprint_size - 1 - level) & 1) ==0)
		return true;
	return false;
}

uint32_t LogarithmicDynamicCuckooFilter::MaskFP(uint32_t fingerprint, int level){
	fingerprint &= (1ull << fingerprint_size - level) - 1ull;
	fingerprint |= (uint32_t) (fingerprint == 0);
	return fingerprint;
}



int LogarithmicDynamicCuckooFilter::getFingerprintSize(){
	return fingerprint_size;
}

float LogarithmicDynamicCuckooFilter::size_in_mb(){
	return fingerprint_size * 4.0 * single_table_length * cuckoo_counter / 8 / 1024 / 1024;
}

size_t LogarithmicDynamicCuckooFilter::actual_size_in_bytes(){
	return actual_size_counter;
}

uint64_t LogarithmicDynamicCuckooFilter::upperpower2(uint64_t x) {
  x--;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  x |= x >> 32;
  x++;
  return x;
}
