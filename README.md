# CS5968Final
Final project for CS5968 - Data structures &amp; algorithms for scalable computing

Project Structure: The logic for our core algorithm is in plagiarism_detection.cc. The flow of the algorithm is in the run() function:
- Make the k grams from a given list of files names
- Compute the min hash values of each document's k grams using the input number of hash functions
- Break these sketches into partitions of length P 
- Hash the sketches to a hash table, and when collisions are detected, mark those files as potnetially plagiarized

Our C++ algorithm implentnation uses helper functions from c_utils/solution_helpers.c and from hashutil.c
Our C++ test file is called test.cc and can take in up to 5 paramaters (but has defaults for all of them):
1) K: chars per k gram
2) H: num hash functions
3) P: partition length
4) Document folder to read files from
5) Max number of files to read from that folder (helpful for benchmarking)

The test file initializes our plagiarism detection algorithm with the input params (or defaults) and outputs the hash table to the file outputs/hash_table.txt. We have a python program that converts that file to a list of unique pairs of documents that are stored in pairs.txt.

Our project as a whole can be tested using the test_solution.py test script. This script can take in arguments for K, H, P, and the document folder to read from (--k=, --h=, --p=, and --d=, respectively). It can also take in the arguments --t to run a time/efficiency test on the benchmarking code vs our algorithm, or --a to run the accuracy code on our alogrithm. The --b flag can be used to run the benchmarking code the first time (and then save the output to the file outputs/benchmark_pairs.txt). Once the benchmarking code has been run once for a value of K and a set of documents, it doesn't need to be run again and python script can be run without the --b flag to compare the algorithm against the benchmarking output.

Python Helpers: We have a number of python helper scripts that have done different things for us during this project. Many of these were written using help from ChatGPT:

Files used by our program:
benchmarking.py -> outputs pairs of potentially plagirized docs to benchmark_pairs.txt
compute_accuracy.py -> Computes the accuracy of our code
file_conversion.py -> converts the file hash_table.py to pairs.txt
print_accuracy.py -> class that prints the recall and precision of our code
print_efficiency.py -> class that prints the efficiency of our code
python_timer.py -> class we made to help time out code


Files that help us in other ways:
download_docs.py -> downloads random docs of a given file size from wikipedia and outputs them to a folder
jaccard.py -> computes the jaccard index of a given folder with a given value of K
paraphrase.py -> paraphrases a given set of files and outputs those similar files



