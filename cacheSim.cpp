#include <bits/stdc++.h>
using namespace std;

/* input format
s 0x1fffff50 1
l 0x1fffff58 1
l 0x1fffff88 6
l 0x1fffff90 2
l 0x1fffff98 2
l 0x200000e0 2
l 0x200000e8 2
l 0x200000f0 2
l 0x200000f8 2
l 0x30031f10 3
s 0x3004d960 0
s 0x3004d968 1
s 0x3004caa0 1
s 0x3004d970 1
s 0x3004d980 6
l 0x30000008 1
l 0x1fffff58 4
l 0x3004d978 4
l 0x1fffff68 4
l 0x1fffff68 2
s 0x3004d980 9
l 0x30000008 1 */

// implement the cache simulator
class CacheSim {
    int no_of_sets;
    int no_of_blocks;
    int no_of_bytes;
    string write_back_policy;
    string write_allocate_policy;
    string replacement_policy;
    int hits;
    int misses;
    vector<vector<pair<string, bool>>> cache;
    vector<vector<int>> lru;
    vector<int> fifo;
    vector<int> dirty;
    vector<int> valid;
    vector<int> tag;
    int block_size;
    int set_size;
    int offset_size;
    int index_size;
    int tag_size;

    public:
    CacheSim(int no_of_sets, int no_of_blocks, int no_of_bytes, string write_back_policy, string write_allocate_policy, string replacement_policy) {
        this->no_of_sets = no_of_sets;
        this->no_of_blocks = no_of_blocks;
        this->no_of_bytes = no_of_bytes;
        this->write_back_policy = write_back_policy;
        this->write_allocate_policy = write_allocate_policy;
        this->replacement_policy = replacement_policy;
        this->hits = 0;
        this->misses = 0;
        this->block_size = no_of_bytes;
        this->set_size = no_of_blocks;
        this->offset_size = log2(block_size);
        this->index_size = log2(no_of_sets);
        this->tag_size = 32 - offset_size - index_size;
        cache.resize(no_of_sets, vector<pair<string, bool>>(no_of_blocks, {"", false}));
        lru.resize(no_of_sets, vector<int>(no_of_blocks, 0));
        fifo.resize(no_of_sets);
        dirty.resize(no_of_sets);
        valid.resize(no_of_sets);
        tag.resize(no_of_sets);
    }

    void load(string address, int size) {
        int addr = stoi(address, 0, 16);
        int offset = addr & ((1 << offset_size) - 1);
        int index = (addr >> offset_size) & ((1 << index_size) - 1);
        int tag = addr >> (offset_size + index_size);
        bool hit = false;
        for (int i = 0; i < no_of_blocks; i++) {
            if (cache[index][i].second && cache[index][i].first == to_string(tag)) {
                hit = true;
                if (replacement_policy == "lru") {
                    lru[index][i] = 0;
                    for (int j = 0; j < no_of_blocks; j++) {
                        if (j != i) {
                            lru[index][j]++;
                        }
                    }
                } else if (replacement_policy == "fifo") {
                    fifo[index] = (fifo[index] + 1) % no_of_blocks;
                }
                break;
            }
        }
        if (hit) {
            hits++;
            cout << "hit\n";
        } else {
            misses++;
            cout << "miss\n";
            int empty = -1;
            for (int i = 0; i < no_of_blocks; i++) {
                if (!cache[index][i].second) {
                    empty = i;
                    break;
                }
            }
            if (empty != -1) {
                cache[index][empty] = {to_string(tag), true};
                if (replacement_policy == "lru") {
                    lru[index][empty] = 0;
                    for (int j = 0; j < no_of_blocks; j++) {
                        if (j != empty) {
                            lru[index][j]++;
                        }
                    }
                } else if (replacement_policy == "fifo") {
                    fifo[index] = (fifo[index] + 1) % no_of_blocks;
                }
            } else {
                int replace = -1;
                if (replacement_policy == "lru") {
                    int max_lru = -1;
                    for (int i = 0; i < no_of_blocks; i++) {
                        if (lru[index][i] > max_lru) {
                            max_lru = lru[index][i];
                            replace = i;
                        }
                    }
                } else if (replacement_policy == "fifo") {
                    replace = fifo[index];
                }
                if (write_back_policy == "write-back" && dirty[index]) {
                    misses++;
                    dirty[index] = 0;
                }
                cache[index][replace] = {to_string(tag), true};
                if (replacement_policy == "lru") {
                    lru[index][replace] = 0;
                    for (int j = 0; j < no_of_blocks; j++) {
                        if (j != replace) {
                            lru[index][j]++;
                        }
                    }
                } else if (replacement_policy == "fifo") {
                    fifo[index] = (fifo[index] + 1) % no_of_blocks;
                }
            }
        }
    }

    void store(string address, int size) {
        int addr = stoi(address, 0, 16);
        int offset = addr & ((1 << offset_size) - 1);
        int index = (addr >> offset_size) & ((1 << index_size) - 1);
        int tag = addr >> (offset_size + index_size);
        bool hit = false;
        for (int i = 0; i < no_of_blocks; i++) {
            if (cache[index][i].second && cache[index][i].first == to_string(tag)) {
                hit = true;
                if (replacement_policy == "lru") {
                    lru[index][i] = 0;
                    for (int j = 0; j < no_of_blocks; j++) {
                        if (j != i) {
                            lru[index][j]++;
                        }
                    }
                } else if (replacement_policy == "fifo") {
                    fifo[index] = (fifo[index] + 1) % no_of_blocks;
                }
                if (write_back_policy == "write-back") {
                    dirty[index] = 1;
                }
                break;
            }
        }
        if (hit) {
            hits++;
        } else {
            misses++;
            int empty = -1;
            for (int i = 0; i < no_of_blocks; i++) {
                if (!cache[index][i].second) {
                    empty = i;
                    break;
                }
            }
            if (empty != -1) {
                cache[index][empty] = {to_string(tag), true};
                if (replacement_policy == "lru") {
                    lru[index][empty] = 0;
                    for (int j = 0; j < no_of_blocks; j++) {
                        if (j != empty) {
                            lru[index][j]++;
                        }
                    }
                } else if (replacement_policy == "fifo") {
                    fifo[index] = (fifo[index] + 1) % no_of_blocks;
                }
                if (write_back_policy == "write-back") {
                    dirty[index] = 1;
                }
            } else {
                int replace = -1;
                if (replacement_policy == "lru") {
                    int max_lru = -1;
                    for (int i = 0; i < no_of_blocks; i++) {
                        if (lru[index][i] > max_lru) {
                            max_lru = lru[index][i];
                            replace = i;
                        }
                    }
                } else if (replacement_policy == "fifo") {
                    replace = fifo[index];
                }
                if (write_back_policy == "write-back" && dirty[index]) {
                    misses++;
                    dirty[index] = 0;
                }
                cache[index][replace] = {to_string(tag), true};
                if (replacement_policy == "lru") {
                    lru[index][replace] = 0;
                    for (int j = 0; j < no_of_blocks; j++) {
                        if (j != replace) {
                            lru[index][j]++;
                        }
                    }
                } else if (replacement_policy == "fifo") {
                    fifo[index] = (fifo[index] + 1) % no_of_blocks;
                }
                if (write_back_policy == "write-back") {
                    dirty[index] = 1;
                }
            }
        }
    }

// we need to print total load and store instructions,load and store hits, load and store misses, total cycles
// hit takes 1 cycle while miss takes 100 cycles
    void printResults() {
        cout << "Total loads: " << hits + misses << endl;
        cout << "Total stores: " << hits + misses << endl;
        cout << "Load hits: " << hits << endl;
        cout << "Load misses: " << misses << endl;
        cout << "Store hits: " << hits << endl;
        cout << "Store misses: " << misses << endl;
        cout << "Total cycles: " << hits + 100 * misses << endl;
    }  

    ~CacheSim() {
        cache.clear();
        lru.clear();
        fifo.clear();
        dirty.clear();
        valid.clear();
        tag.clear();
    }
    
};

// types to be used in CacheSim class are direct-mapped, set-associative, fully-associative, write-back, write-through, write-allocate, no-write-allocate, lru, fifo
// in main function get command line arguments like no. of sets in Cache, no. of blocks in each set, no. of bytes in each block, write-back policy, write-allocate policy, lru or fifo for certain cases

int main (int argc, char* argv[]) {
    // get command line arguments
    int no_of_sets = stoi(argv[1]);
    int no_of_blocks = stoi(argv[2]);
    int no_of_bytes = stoi(argv[3]);
    string write_back_policy = argv[4];
    string write_allocate_policy = argv[5];
    string replacement_policy = argv[6];
    
    CacheSim cache(no_of_sets, no_of_blocks, no_of_bytes, write_back_policy, write_allocate_policy, replacement_policy);

    string line;
    while (getline(cin, line)) {
        istringstream iss(line);
        string type, address, size;
        iss >> type >> address >> size;
        if (type == "l") {
            cache.load(address, stoi(size));
        } else if (type == "s") {
            cache.store(address, stoi(size));
        }
    }

    cache.printResults();

    return 0;
}
