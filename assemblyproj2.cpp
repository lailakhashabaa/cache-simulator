#include <bitset>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class cacheEntry {
private:
    bool validBit = 0;
    string tag;
    string index;
    string offset;
    int tag_size;
    int index_size;

public:
    void set_tag(string t) { tag = t; }
    void set_index(string d) { index = d; }
    void set_off(string f) { offset = f; }
    cacheEntry() {
        validBit = 0;
        tag.resize(0);
        index.resize(0);
    }
    int get_index_size() { return index_size; }
    int get_tag_size() { return tag_size; }
    void setValidBit(bool val) { validBit = val; }
    bool getValidBit() { return validBit; }
    string get_index() { return index; }
    string get_tag() { return tag; }
    string get_offset() { return offset; }
    void settagSize(int x) { tag_size = x; }
    void setIndexSize(int index) { index_size = index; }
};
vector<string> readfile(string add) {
    ifstream read;
    vector<string> assemblycode;
    read.open(add);
    if (read.is_open())
        cout << " opened ! " << endl;
    while (!read.eof()) {
        string temp;
        getline(read, temp);
        // read2 >> temp;
        assemblycode.push_back(temp);
    }
    return assemblycode;
}

string tobinary(int num, int numberOFbits) {

    vector<string> bin;
    string binary = "";

    for (int i = numberOFbits; i >= 0; i--) {
        int k = num >> i;
        if (k & 1)
            bin.push_back("1");
        else
            bin.push_back("0");
    }
    for (int i = 0; i < bin.size(); i++)
        binary += bin[i];
    if (num < 0) {
        int b = binary.length();
        int i;
        for (i = b - 1; i >= 0; i--)
            if (binary[i] == '1')
                break;
        if (i == -1)
            return '1' + binary;
        for (int k = i - 1; k >= 0; k--) {
            if (binary[k] == '1')
                binary[k] = '0';
            else
                binary[k] = '1';
        }
    }
    binary.erase(0, 1);
    return binary;
}

vector<cacheEntry> initializeCacheSize(
    int s, int L) // s=total cache size, c= number of blocks L=line size //
{
    // int c = s / L;
    int noOfLines = s / L;
    vector<cacheEntry> cache;
    cache.resize(noOfLines);
    return cache;
}

void initializeCacheBitSizes(vector<cacheEntry>& cache, int lineSize,
    int cacheSize) {
    // 32 bit address and cache
    int numOflines = cache.size();
    int displacementSize = log2(lineSize);
    // displacementSize = log2(displacementSize);
    int indexSize = log2(cacheSize / lineSize);
    int tagSize = 32 - (indexSize + displacementSize);
    // initializing sizes for all the cache
    for (int i = 0; i < cache.size(); i++) {
        cache[i].settagSize(tagSize);
        cache[i].setValidBit(0);
        cache[i].setIndexSize(indexSize);
        cache[i].set_index(tobinary(i, indexSize));
        cache[i].set_tag("");
    }
}
//the entry should be binary 

// address is 32 bits : 
//index= log2(number of lines) 
//offset= (line size) 16 byte

vector<cacheEntry> ProcessAddress(vector<string> address, int line_size, int tagSize, int indexSize)
{
    vector<cacheEntry> fixedAdd;
    int NUM_LINES = address.size();
    int displacementSize = log2(line_size);
    // displacementSize = log2(displacementSize);
    //int indexSize = log2(cache_size);
    //int tagSize = 32 - (indexSize + displacementSize);

    for (int i = 0; i < NUM_LINES; i++) {
        cacheEntry* ADD = new cacheEntry;
        string add_tag = "";
        string add_index = "";
        string offset = "";
        for (int j = 0; j < tagSize; j++) {
            add_tag.push_back(address[i][j]);
        }
        ADD->settagSize(tagSize);
        ADD->set_tag(add_tag);
        add_tag = "";
        // index
        for (int j = tagSize; j < tagSize + indexSize; j++) {
            add_index.push_back(address[i][j]);
        }
        ADD->setIndexSize(indexSize);
        ADD->set_index(add_index);
        add_index = "";
        for (int j = tagSize + indexSize;
            j < tagSize + indexSize + displacementSize; j++) {
            offset.push_back(address[i][j]);
        }
        ADD->setIndexSize(offset.size());
        ADD->set_off(offset);
        offset = "";
        fixedAdd.push_back(*ADD);
    }

    return fixedAdd;
}
void display(int i, cacheEntry entry, cacheEntry address, bool hitorMiss,
    int hitcount, int misscount, int numOfcycles) {
    double mat = 0;
    int memory = 100;
    cout << "Entry number " << i + 1 << endl;
    cout << "Valid bit= " << entry.getValidBit() << endl;
    cout << "Tag= " << address.get_tag() << endl;
    if (hitorMiss == 0) {
        cout << "Miss!" << endl;

    }
    else {
        cout << "hit! " << endl;
    }
    double numberOfacceses = hitcount + misscount;
    cout << "Total number of accesess " << numberOfacceses << endl;
    double hitratio = hitcount / numberOfacceses;
    cout << "Hit ratio = " << hitratio << endl;
    cout << "Miss ratio = " << (1 - hitratio) << endl;
    mat = hitratio * numOfcycles + (1 - hitratio) * memory;
    cout << "AMAT = " << mat << " cycles " << endl << endl << endl;


}
void printCache(vector<cacheEntry> cache)
{

    for (int i = 0; i < cache.size(); i++)
    {
        cout << "Valid bit: " << cache[i].getValidBit() << "    " << "Tag: " << cache[i].get_tag() << "      " << "Index: " << cache[i].get_index() << "     " << endl;

    }


}
void Operations_On_Cache(vector<cacheEntry> cache, vector<cacheEntry> FixedAddress, int numberOfLines, int& miss_count, int& hit_count, int& numOfcycles)
{
    //printCache(cache);
    cout << endl;
    for (int i = 0; i < FixedAddress.size(); i++)
    {

        //int indexAdress = stoi(FixedAddress[i].get_index());
        //int tagAdress = stoi(FixedAddress[i].get_tag());
        for (int j = 0; j < cache.size(); j++)
        {
            //int indexCache = stoi(cache[j].get_index());
            if (FixedAddress[i].get_index() == cache[j].get_index())
            {

                if (cache[j].getValidBit() == 0)
                {
                    miss_count++;
                    cache[j].set_tag(FixedAddress[i].get_tag());

                    display(i, cache[j], FixedAddress[i], 0, hit_count, miss_count,
                        numOfcycles);
                    cout << endl;
                    cache[j].setValidBit(1);
                    cout << "Cache content after the entry : " << endl;
                    printCache(cache);
                    //cache[j].setValidBit(1);

                }
                else {
                    //int tagCache = stoi(cache[j].get_tag());
                    if (cache[j].get_tag() == FixedAddress[i].get_tag()) {
                        hit_count++;
                        display(i, cache[j], FixedAddress[i], 1, hit_count, miss_count,
                            numOfcycles);
                        cout << endl;
                        cout << "Cache content after the entry : " << endl;
                        printCache(cache);
                    }
                    else {
                        miss_count++;

                        display(i, cache[j], FixedAddress[i], 0, hit_count, miss_count,
                            numOfcycles);
                        cout << endl;
                        cache[j].set_tag(FixedAddress[i].get_tag());
                        cout << "Cache content after the entry : " << endl;
                        printCache(cache);
                        //cache[j].set_tag(FixedAddress[i].get_tag());
                    }
                }
            }

        }
    }
}

int main() {
    string address = "Assembly project two.txt";
    vector<string> Add = readfile(address);
    int miss_count = 0;
    int hit_count = 0;
    vector<cacheEntry> cacheInput;
    vector<cacheEntry> addresses;
    cout << "Please, Enter the cache memory size : " << endl;
    int cacheSize;
    int LineSize;
    cin >> cacheSize;
    cout << "Please, Enter the cache line size : " << endl;
    cin >> LineSize;
    cout << "\n Please enter the number of cycles needed to access the cache"
        << endl;
    int numOfcycles;
    cin >> numOfcycles;
    if (numOfcycles < 1 or numOfcycles > 10) {
        cout << "invalid num of cycles" << endl;
        exit(0);
    }
    // hit rate* time to access cache + misss ratio* time to access memory
    cout << endl << endl;
    cacheInput = initializeCacheSize(cacheSize, LineSize);
    initializeCacheBitSizes(cacheInput, LineSize, cacheSize);
    addresses = ProcessAddress(Add, LineSize, cacheInput[0].get_tag_size(),
        cacheInput[0].get_index_size());
    Operations_On_Cache(cacheInput, addresses, cacheSize / LineSize, miss_count,
        hit_count, numOfcycles);
}