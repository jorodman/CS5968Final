#include <iostream>
#include <cassert>
#include <chrono>
#include <openssl/rand.h>
#include <map>
#include <unordered_map>

#include "zipf.h"
#include "sketches.cc"

using namespace std;

#define UNIVERSE 1ULL << 30
#define EXP 1.5 

bool debug = false;

double elapsed(chrono::high_resolution_clock::time_point t1, chrono::high_resolution_clock::time_point t2) {
	return (chrono::duration_cast<chrono::duration<double> >(t2 - t1)).count();
}


// Helper methods for reporting 
double compute_precision(int selected_elements, int true_positives);
double compute_recall(int relevent_elements, int true_positives);
int get_true_positives(multimap<uint64_t, uint64_t, greater<uint64_t>> topK, set<uint64_t> temp_hh);

// Setup objects for each algorithm
unordered_map<uint64_t, uint64_t> setup_map(uint64_t * numbers, uint64_t N, double * map_time);
CountMinSketch * setup_CountMinSketch(uint64_t * numbers, uint64_t Ni, double phi, double confidence, double epsilon, vector<double> * values);
CountSketch * setup_CountSketch(uint64_t * numbers, uint64_t Ni, double phi, double confidence, double epsilon, vector<double> * values);
MisraGries * setup_MisraGries(uint64_t * numbers, uint64_t Ni, double phi, double confidence, vector<double> * values);

// Methods for testing each algorithm
multimap<uint64_t, uint64_t, greater<uint64_t>> test_map(unordered_map<uint64_t, uint64_t> map, uint64_t N, double phi, double * map_time);
set<uint64_t> test_Sketch(Sketch * sketch, uint64_t N, vector<double> * values);

// Methods that handle the data for reporting
void save_results(multimap<uint64_t, uint64_t, greater<uint64_t>> *topK, set<uint64_t> *temp_hh, Sketch * sketch, vector<double> *precisions, vector<double> *recalls, vector<double> *sizes, vector<double> *times);
void print_pvt(vector<double> precision, vector<double> times);
void print_pvs(vector<double> recalls, vector<double> sizes);
void print_rvt(vector<double> precision, vector<double> times);
void print_rvs(vector<double> recalls, vector<double> sizes);

// Global variables
chrono::high_resolution_clock::time_point t1, t2;

int main(int argc, char** argv)
{
	if (argc < 3) {
		cerr << "Specify the number of items N and phi.\n";
		exit(1);
	}
	uint64_t N = atoi(argv[1]);
	double tau = atof(argv[2]);

	uint64_t *numbers = (uint64_t *)malloc(N * sizeof(uint64_t));
	if(!numbers) {
		cerr << "Malloc numbers failed.\n";
		exit(0);
	}
	generate_random_keys(numbers, UNIVERSE, N, EXP);

	if(debug) cout << "Done generating numbers...\n";

	unordered_map<uint64_t, uint64_t> map;
	CountMinSketch * cms;
	CountSketch * cs;
	MisraGries * mg;
	
	if(debug) cout << "Map setup complete...\n";

	// FOR THE REPORT
	double start_confidence = 0.001;
	double end_confidence = 0.01;
	double err_step = 0.001;
	double report_phi = 0.001;
	report_phi = tau;
	double report_epsilon = 0.99;
	double map_time;

	// Results of each of the algorithms
	multimap<uint64_t, uint64_t, greater<uint64_t> > topK;
	set<uint64_t> temp_hh;

	// Data structures to save the results
	vector<double> precisions;
	vector<double> recalls;
	vector<double> sizes;
	vector<double> times;

	// Test the map and save its values
	map = setup_map(numbers, N, &map_time);
	topK = test_map(map, N, report_phi, &map_time);
	
	double map_size = sizeof(uint64_t) * map.size();
	if(debug) cout << "Map testing done...\n";
	
	// Test my sketching alogirthms
	for(double confidence = start_confidence; confidence <= end_confidence; confidence += err_step)
	{
		if(debug) cout << "Testing target error " << confidence << endl;
		precisions.push_back(1.0);
		recalls.push_back(1.0);
		times.push_back(map_time);
		sizes.push_back(map_size);

		if(debug) cout << "Testing CMS...\n";

		// Test Count Min Sketch
		cms = setup_CountMinSketch(numbers, N, report_phi, confidence, report_epsilon, &times);
		temp_hh = test_Sketch(cms, N, &times);
		save_results(&topK, &temp_hh, cms, &precisions, &recalls, &sizes, &times);	
	
		if(debug) cout << "Testing CS...\n";

		// Test Count Sketch
		cs = setup_CountSketch(numbers, N, report_phi, confidence, report_epsilon, &times);
		temp_hh = test_Sketch(cs, N, &times);
		save_results(&topK, &temp_hh, cs, &precisions, &recalls, &sizes, &times);	

		if(debug) cout << "Testing MG...\n";

		// Test Misra Gries
		mg = setup_MisraGries(numbers, N, report_phi, confidence, &times);
		temp_hh = test_Sketch(mg, N, &times);
		save_results(&topK, &temp_hh, mg, &precisions, &recalls, &sizes, &times);	
	}
	
	print_pvt(precisions, times);
	print_pvs(precisions, sizes);
	print_rvt(recalls, times);
	print_rvs(recalls, sizes);

	free(numbers); // free stream
}

void save_results(multimap<uint64_t, uint64_t, greater<uint64_t>> *topK, set<uint64_t> *temp_hh, Sketch * sketch, vector<double> *precisions, vector<double> *recalls, vector<double> *sizes, vector<double> *times)
{
	int relevent_elements = topK->size();
	int true_positives = get_true_positives(*topK, *temp_hh);
	int selected_elements = temp_hh->size();
	double precision = compute_precision(selected_elements, true_positives);
	double recall = compute_recall(relevent_elements, true_positives);
	
	precisions->push_back(precision);
	recalls->push_back(recall);
	sizes->push_back(sketch->Size());
}


int get_true_positives(multimap<uint64_t, uint64_t, greater<uint64_t>> topK, set<uint64_t> temp_hh)
{
	int true_positives = 0;
	for (auto it = topK.begin(); it != topK.end(); ++it) {
		auto pos = temp_hh.find(it->second);
		if(pos != temp_hh.end()) true_positives++;
	}
	return true_positives;
}

void print_pvt(vector<double> precisions, vector<double> times)
{
	cout << "Precision vs time\n";

	for(int i = 0; i < (int) precisions.size(); i++)
	{
		cout << precisions[i] << ", ";
		cout << times[i];
			
		if((i + 1) % 4 == 0) cout << "\n";
		else cout << ", ";
	}

	cout << "\n";
}

void print_pvs(vector<double> precisions, vector<double> sizes)
{
	cout << "Precision vs size\n";

	for(int i = 0; i < (int) precisions.size(); i++)
	{
		cout << precisions[i] << ", ";
		cout << sizes[i];
			
		if((i + 1) % 4 == 0) cout << "\n";
		else cout << ", ";
	}

	cout << "\n";
}

void print_rvt(vector<double> recalls, vector<double> times)
{
	cout << "Recall vs time\n";

	for(int i = 0; i < (int) recalls.size(); i++)
	{
		cout << recalls[i] << ", ";
		cout << times[i];
			
		if((i + 1) % 4 == 0) cout << "\n";
		else cout << ", ";
	}

	cout << "\n";
}


void print_rvs(vector<double> recalls, vector<double> sizes)
{
	cout << "Recall vs size\n";

	for(int i = 0; i < (int) recalls.size(); i++)
	{
		cout << recalls[i] << ", ";
		cout << sizes[i];
			
		if((i + 1) % 4 == 0) cout << "\n";
		else cout << ", ";
	}

	cout << "\n";
}

unordered_map<uint64_t, uint64_t> setup_map(uint64_t * numbers, uint64_t N, double * map_time) {
	unordered_map<uint64_t, uint64_t> map(N);
	t1 = chrono::high_resolution_clock::now();
	if(debug) cout << elapsed(t1, t2) << endl;
	for (uint64_t i = 0; i < N; ++i) map[numbers[i]]++;
	t2 = chrono::high_resolution_clock::now();
	double t = elapsed(t1, t2) / N;
	*map_time = t;
	return map;
}

CountMinSketch * setup_CountMinSketch(uint64_t * numbers, uint64_t N, double phi, double confidence, double epsilon, vector<double> * values) {
	CountMinSketch * cms = new CountMinSketch(phi, confidence, epsilon);
	t1 = chrono::high_resolution_clock::now();
	if(debug) cout << elapsed(t1, t2) << endl;
	for (uint64_t i = 0; i < N; ++i) cms->Add(numbers[i]);	
	t2 = chrono::high_resolution_clock::now();
	double t = elapsed(t1, t2) / N;
	values->push_back(t);
	return cms;
}

CountSketch * setup_CountSketch(uint64_t * numbers, uint64_t N, double phi, double confidence, double epsilon, vector<double> * values) {
	CountSketch * cs = new CountSketch(phi, confidence, epsilon);
	t1 = chrono::high_resolution_clock::now();
	if(debug) cout << elapsed(t1, t2) << endl;
	for (uint64_t i = 0; i < N; ++i) cs->Add(numbers[i]);	
	t2 = chrono::high_resolution_clock::now();
	double t = elapsed(t1, t2) / N;
	values->push_back(t);
	return cs;
}

MisraGries * setup_MisraGries(uint64_t * numbers, uint64_t N, double phi, double confidence, vector<double> * values) {
	MisraGries * mg = new MisraGries(phi, confidence);
	t1 = chrono::high_resolution_clock::now();
	if(debug) cout << elapsed(t1, t2) << endl;
	for (uint64_t i = 0; i < N; ++i) mg->Add(numbers[i]);	
	t2 = chrono::high_resolution_clock::now();
	double t = elapsed(t1, t2) / N;
	values->push_back(t);
	return mg;
}

double compute_precision(int selected_elements, int true_positives)
{
	return (double) true_positives / (double) selected_elements;
}

double compute_recall(int relevent_elements, int true_positives)
{
	return (double) true_positives / (double) relevent_elements;
}

multimap<uint64_t, uint64_t, greater<uint64_t>> test_map(unordered_map<uint64_t, uint64_t> map, uint64_t N, double phi, double * map_time) {
	double threshold = N * phi;
	multimap<uint64_t, uint64_t, greater<uint64_t> > topK;
	t1 = chrono::high_resolution_clock::now();
	for (auto it = map.begin(); it != map.end(); ++it) {
		if (it->second >= threshold) {
			topK.insert(make_pair(it->second, it->first));
		}
	}
	t2 = chrono::high_resolution_clock::now();
	*map_time = elapsed(t1, t2);
	return topK;	
}

set<uint64_t> test_Sketch(Sketch * sketch, uint64_t N, vector<double> * values)
{
	t1 = chrono::high_resolution_clock::now();
	set<uint64_t> hh = sketch->HeavyHitters();
	t2 = chrono::high_resolution_clock::now();
//	values->push_back(elapsed(t1, t2));
	if(debug) cout << elapsed(t1, t2) << endl;
	return hh;
}

