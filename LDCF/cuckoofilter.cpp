#include <iostream>
#include "cuckoofilter.h"

CuckooFilter::CuckooFilter(const size_t table_length, const size_t fingerprint_bits, const int single_capacity, int curlevel){
	fingerprint_size = fingerprint_bits - curlevel;
	bytes_per_bucket = (fingerprint_size*4+7)>>3;
	single_table_length = table_length;
	counter = 0;
	capacity = single_capacity;
	is_full = false;
	_0_child = NULL;
	_1_child = NULL;
	level = curlevel;
	mask = (1ULL << fingerprint_size) - 1;

	bucket = new Bucket[single_table_length];
	for(size_t i = 0; i<single_table_length; i++){
		bucket[i].bit_array = new char[bytes_per_bucket];
		memset(bucket[i].bit_array, 0, bytes_per_bucket);
	}
	valid = true;

}

CuckooFilter::~CuckooFilter(){
	if(valid)delete[] bucket;
	delete _0_child;
	delete _1_child;
}

bool CuckooFilter::is_valid(){
	return valid;
}
void CuckooFilter::invalidate(){
	valid = false;
	delete[] bucket;
}

bool CuckooFilter::insertItem(size_t index, uint32_t fingerprint, const uint32_t prefix, Victim &victim){
	size_t alt_index;

	for(size_t count = 0; count<MaxNumKicks; count++){
		bool kickout = (count != 0);
		if(insertImpl(index, fingerprint, kickout, victim))
			return true;
		if (kickout){
			index = victim.index;
			fingerprint = victim.fingerprint;
			generateA(index, fingerprint, prefix, alt_index, single_table_length);
			index = alt_index;
		}else{
			generateA(index, fingerprint, prefix, alt_index, single_table_length);
			index = alt_index;
		}
	}
	return false;
}

bool CuckooFilter::insertImpl(const size_t index, const uint32_t fingerprint, const bool kickout, Victim &victim){
	for(size_t pos = 0; pos<4; pos++){
		if(read(index,pos) == 0){
			write(index,pos,fingerprint);
			counter++;
			if(this->counter == capacity)
				this->is_full = true;
			return true;
		}
	}
	if(kickout){
		int j = rand()%4;
		victim.index = index;
		victim.fingerprint = read(index,j);
		write(index,j, fingerprint);
	}
	return false;
}

bool CuckooFilter::queryImpl(const size_t index, const uint32_t fingerprint){
	const char* p = bucket[index].bit_array;
	uint64_t bits = *(uint64_t*)p;
	if(fingerprint_size <= 4)
		return hasvalue4(bits, fingerprint);
	else if(fingerprint_size <= 8)
		return hasvalue8(bits, fingerprint);
	else if(fingerprint_size <= 12)
		return hasvalue12(bits, fingerprint);
	else if(fingerprint_size <= 16)
		return hasvalue16(bits, fingerprint);
	else
		return false;
}

bool CuckooFilter::deleteImpl(const size_t index, const uint32_t fingerprint){
	for(size_t pos = 0; pos<4; pos++){
		if(read(index, pos) == fingerprint){
			write(index, pos, 0);
			counter--;
			if(counter < this->capacity){
				this->is_full = false;
			}
			return true;
		}
	}
	return false;
}

void CuckooFilter::generateA(size_t index, uint32_t fingerprint, uint32_t prefix, size_t &alt_index, size_t single_table_length){
	alt_index = (index ^ hash_func3_32bit((prefix | fingerprint) >> 1)) % single_table_length;
}


uint32_t CuckooFilter::read(size_t index, size_t pos){
	const char* p = bucket[index].bit_array;
	uint32_t fingerprint;

	if(fingerprint_size <= 4){
		p += (pos >> 1);
		uint8_t bits_8 = *(uint8_t*)p;
		if((pos & 1) == 0){
			fingerprint = (bits_8>>4) & 0xf;
		}else{
			fingerprint = bits_8 & 0xf;
		}
	}else if(fingerprint_size <= 8){
		p += pos;
		uint8_t bits_8 = *(uint8_t*)p;
		fingerprint = bits_8 & 0xff;
	}else if(fingerprint_size <= 12){
		p += pos+(pos>>1);
		uint16_t bits_16 = *(uint16_t*)p;
		if((pos & 1) == 0){
			fingerprint = bits_16 & 0xfff;
		}else{
			fingerprint = (bits_16 >> 4) & 0xfff;
		}
	}else if(fingerprint_size <= 16){
		p += (pos<<1);
		uint16_t bits_16 = *(uint16_t*)p;
		fingerprint = bits_16 & 0xffff;
	}else if(fingerprint_size <= 24){
		p += pos+(pos<<1);
		uint32_t bits_32 = *(uint32_t*)p;
		fingerprint = (bits_32 >> 4);
	}else if(fingerprint_size <= 32){
		p += (pos<<2);
		uint32_t bits_32 = *(uint32_t*)p;
		fingerprint = bits_32 & 0xffffffff;
	}else{
		fingerprint =0;
	}
	return fingerprint & mask;
}

void CuckooFilter::write(size_t index, size_t pos, uint32_t fingerprint){
	char* p = bucket[index].bit_array;
	if(fingerprint_size <= 4){
		p += (pos>>1);
		if((pos & 1) == 0){
			*((uint8_t*)p) &= 0x0f;
			*((uint8_t*)p) |= (fingerprint<<4);
		}else{
			*((uint8_t*)p) &= 0xf0;
			*((uint8_t*)p) |= fingerprint;
		}
	}else if(fingerprint_size <= 8){
		((uint8_t*)p)[pos] = fingerprint;
	}else if(fingerprint_size <= 12){
		p += (pos + (pos>>1));
		if((pos & 1) == 0){
			*((uint16_t*)p) &= 0xf000; //Little-Endian
			*((uint16_t*)p) |= fingerprint;
		}else{
			*((uint16_t*)p) &= 0x000f;
			*((uint16_t*)p) |= fingerprint<<4;
		}
	}else if(fingerprint_size <= 16){
		((uint16_t*) p)[pos] = fingerprint;
	}else if(fingerprint_size <= 24){
		p += (pos+ (pos<<1));
		*((uint32_t*)p) &= 0xff000000; //Little-Endian
		*((uint32_t*)p) |= fingerprint;
	}else if(fingerprint_size <= 32){
		((uint32_t*) p)[pos] = fingerprint;
	}
}




