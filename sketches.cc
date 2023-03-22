#include <iostream>
#include <cmath>
#include <vector>
#include <functional>
#include <set>
#include <bits/stdc++.h>
#include "hashutil.h"

using namespace std;

typedef pair<uint64_t, uint64_t> pi;

class Sketch {

	protected:
		uint64_t N; 			// Total number of elements in the stream so far
		double phi;		  	// Phi value used to calculate T
		double confidence; 		// Confidence % that the results will be episolon % accurace
		double epsilon;			// Accuracy %
		set<uint64_t> heavy_hitters;	// Set of pre computed heavy hitters
		priority_queue<pi, vector<pi>, greater<pi>> pq;
		map<uint64_t, uint64_t> counts;

		// Returns the threshold to be a heavy hitters
		uint64_t get_threshold() { 
			return (uint64_t) ceil(N * phi); 
		};

	public:
		virtual void Add(uint64_t x) {};
		virtual uint64_t Estimate(uint64_t item) { return 0; };
		virtual set<uint64_t> HeavyHitters() { 
			set<uint64_t> hh;
			return hh;	
		};
		virtual uint64_t Size() { return 0; };
};

class CountSketchGeneric: public Sketch {
	
	protected:

		uint64_t rows;
		uint64_t cols;
		vector<uint64_t> table;
		vector<uint64_t> primes = {53, 97, 193, 389, 796, 1543, 3079, 6151, 12289};

		void Increment(uint64_t row, uint64_t col)
		{
			table[row * cols + col]++;
		}

		uint64_t Hash(void * key, size_t size, uint64_t seed, uint64_t mod)
		{
			return MurmurHash64A(key, size, primes[seed]) % mod; 
		}

		int S_Hash(uint64_t hash_value, uint64_t mod)
		{
			uint64_t mid = mod / 2;
			if(hash_value > mid) return 1;
			else return 0;
		}

		void set_table(uint64_t row, uint64_t col, uint64_t val)
		{
			table[row * cols + col] = val;
		}

		uint64_t get_table(uint64_t row, uint64_t col)
		{
			return table[row * cols + col];
		}

		void update_heap(uint64_t item, uint64_t estimate) {

			if(estimate > this->get_threshold())
			{
				// If the item is not in the map, insert it
				if(counts[item] == 0)
				{
					heavy_hitters.insert(item);
					pq.push(make_pair(estimate, item));
				}

				// Update the frequency
				counts[item] = estimate;
			}

			if(pq.empty()) return;

			pair<uint64_t, uint64_t> p = pq.top();
			uint64_t count = p.first;
			item = p.second;
			uint64_t T = get_threshold();

			while(count < T && !pq.empty())
			{
				counts.erase(item);
				heavy_hitters.erase(item);
				p = pq.top();
				pq.pop();
				count = p.first;
				item = p.second;
			}	
		}

		CountSketchGeneric(double phi, double confidence, double epsilon)
		{
			this->confidence = confidence;
			this->phi = phi;
			this->epsilon = epsilon;
			this->rows = (uint64_t) ceil(log2(1/confidence));
			this->cols = (uint64_t) (1.0 / (phi * epsilon));
			this->N = 0;

			int table_size = rows * cols;
			table.resize(table_size);
		}

	public:

		set<uint64_t> HeavyHitters()
		{
			return heavy_hitters;
		}

		uint64_t Size() 
		{
			size_t table_size = sizeof(uint64_t) * table.size();
			size_t hh_size = sizeof(uint64_t) * heavy_hitters.size();
			size_t heap_size = sizeof(uint64_t) * pq.size();
			return table_size + hh_size + heap_size;
		}
};

class CountMinSketch: public CountSketchGeneric {

	public:
		void Add(uint64_t item) {
			this->N++;
			
			const void * key = (void*) item;
			size_t size = sizeof(key);

			uint64_t min = this->N;

			for (uint64_t row = 0; row < rows; row++)
			{
				uint64_t hash = Hash(&key, size, row, cols);
				Increment(row, hash);

				uint64_t frequency = get_table(row, hash);
				if(frequency < min) min = frequency; 
			}
			
			update_heap(item, min);
		}
		

		uint64_t Estimate(uint64_t item) {

			uint64_t min = this->N;

			const void * key = (void*) item;
			size_t size = sizeof(key);

			for (uint64_t row = 0; row < rows; row++)
			{
				uint64_t hash = Hash(&key, size, row, cols);
				uint64_t frequency = get_table(row, hash);
				if(frequency < min) min = frequency; 
			}

			return min;
		}

		CountMinSketch(double phi, double confidence, double epsilon) : CountSketchGeneric(phi, confidence, epsilon)
		{	
		}	
};


class CountSketch: public CountSketchGeneric {

	public:

		void Add(uint64_t item) {
			this->N++;

			const void * key = (void*) item;
			size_t size = sizeof(key);

			vector<uint64_t> frequencies;
			vector<uint64_t> hashes;
			
			for(uint64_t row = 0; row < rows; row++)
			{
				uint64_t hash = Hash(&key, size, row, cols);
				hashes.push_back(hash);
			}

			for (uint64_t row = 0; row < rows; row++)
			{
				uint64_t hash = hashes[row];
				int s_hash = S_Hash(hash, cols);
				uint64_t curr = get_table(row, hash);
				
				// Don't let table go negative bc its unsigned
				if(curr > 0 || s_hash > 0)
				{
					if(s_hash == -1) curr--;
					else curr++;
					set_table(row, hash, curr);
					uint64_t frequency = get_table(row, hash);
					frequencies.push_back(frequency);
				}
			}

			sort(frequencies.begin(), frequencies.end());
		
			uint64_t estimate = 0;
			if(frequencies.size() > 0) estimate = frequencies[frequencies.size() / 2];
	
			update_heap(item, estimate);
		}

		uint64_t Estimate(uint64_t item) {
			
			vector<uint64_t> frequencies;

			const void * key = (void*) item;
			size_t size = sizeof(key);

			for (uint64_t row = 0; row < rows; row++)
			{
				uint64_t hash = Hash(&key, size, row, cols);
				uint64_t frequency = get_table(row, hash);
				frequencies.push_back(frequency);
			}

			sort(frequencies.begin(), frequencies.end());
		
			if(frequencies.size() == 0) return 0;
			else return frequencies[frequencies.size() / 2];
		}

		CountSketch(double phi, double confidence, double epsilon) : CountSketchGeneric(phi, confidence, epsilon)
		{	
		}	
};

class MisraGries: public Sketch {

	public:

		void Add(uint64_t x) {
			this->N++;
			uint64_t T = get_threshold() - 1;

			if(counts[x] != 0) 
			{
				counts[x]++;
			}
			else if(counts.size() < T)
			{
				counts[x] = 1;
				heavy_hitters.insert(x);
			}
			else
			{
				map<uint64_t, uint64_t> copy;
				map<uint64_t, uint64_t>::iterator it = counts.begin();
 
			 	while (it != counts.end())
				{
				    	if(it->second > 1)
				    	{	
						copy[it->first] = it->second - 1;
					}
					else {
						heavy_hitters.erase(it->first);
					}
				    
					++it;
				}
			
				counts = copy;	
			}
		}

		uint64_t Estimate(uint64_t item) {
			if(counts[item] != 0)
			{
				return counts[item] / N;
			}
			else return 0;
		}

		set<uint64_t> HeavyHitters()
		{
			return heavy_hitters;
		}

		uint64_t Size() 
		{
			uint64_t map_size = sizeof(uint64_t) * counts.size();
			uint64_t hh_size = sizeof(uint64_t) * heavy_hitters.size();
			return map_size + hh_size;
		}

		MisraGries(double phi, double confidence)
		{
			this->confidence = confidence;
			this->phi = phi;
			this->N = 0;
		}
};
