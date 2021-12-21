#include <array>
#include <ctype.h>
#include <iostream>
#include <math.h>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <utility>
#include <vector>

template<class T>
class Hashtable 
{
public:
    // contructor
    Hashtable(bool d = false, unsigned int s = 11);

    // destructor
    ~Hashtable();

    // adds to hashtable with specified key and value
    // if k is already in the hashtable, does nothing
    // returns probes required to place k
    int add(std::string k, const T& val);

    // returns value associated with k
    const T& lookup(std::string k);

    // output all keys and values to stream
    void reportAll(std::ostream&) const;

    // calculates index based on key
    int hash(std::string k) const;

    // returns value of bool resized
    bool get_resized();

    // returns number of items in hashtable
    int get_items();

private:
    // approximately doubles the number of indices available
    void resize();

    // makes new random numbers for hashing;
    void randoms();

    // stores possible size values
    std::array<int, 17> size_arr
		{11, 23, 47, 97, 197, 397, 797, 1597, 3203, 6421, 12853, 25717, 51437, 102877, 205759, 411527, 823117};	

    // tracks of which size in the array is being used
    int size_ind;

    // sets random numbers to pre-generated values
    bool debug;

    // size of hash table
    unsigned int size;

    // stores random numbers
    std::array<int, 5> r;

    // number of items in the hashtable
    int items;

    // false if hashtable is at initial size, true after resize() is called
    bool resized;

    // stores dynamically allocated pairs of strings and Ts
    std::vector<std::pair<std::string, T>> table;
};

template<class T>
Hashtable<T>::Hashtable(bool d, unsigned int s) 
{
    debug = d;
    size = s;
    resized = false;
    table.resize(size);
    for (int i = 0; i < (int)size_arr.size(); i++)  // sets up size_ind if a size larger than 11 is inputted
    {
        if ((int)size < size_arr[i]) 
		{
            size_ind = i - 1;
            break;
        }
    }
    items = 0;
    randoms();
}

template<class T>
Hashtable<T>::~Hashtable() {}

template<class T>
int Hashtable<T>::add(std::string k, const T& val) 
{
    if (((double)items) / (double)size >= .5)  // checks load factor, resizes if over .5
    {
        resize();
    }
    int hash_val = hash(k);  // calculates hash value
    int probe = 0;
    for (int i = 0; i < (int)size; i++) 
	{
        if (table[hash_val].first == "")  // if index does not have an element
        {
            std::pair<std::string, T> temp(k, val);
            table[hash_val] = temp;
            items++;
            break;
        } else if (table[hash_val].first == k)  // if k is already in hashtable
        {
            break;
        }
        probe++;
        hash_val += (probe * probe);  // quadratic probing
        hash_val = hash_val % size;
    }
    return probe;
}

template<class T>
const T& Hashtable<T>::lookup(std::string k) 
{
    int hash_val = hash(k);
    int probe = 1;
    for (int i = 0; i < (int)size; i++) 
	{
        if (table[hash_val].first == k)  // returns value if found
        {
            return table[hash_val].second;
        }
        hash_val += (probe * probe);  // quadratic probing
        probe++;
        hash_val = hash_val % size;
    }
    return (T)NULL;  // if not found, return null
}

template<class T>
void Hashtable<T>::reportAll(std::ostream& outf) const {
    for (int i = 0; i < size; i++)  // writes elements one at a time to ostream
    {
        if (table[i].first != "") 
		{
            outf << table[i].first << " " << table[i].second << std::endl;
        }
    }
}

template<class T>
void Hashtable<T>::resize() 
{
    resized = true;
    std::vector<std::pair<std::string, T>> in_table;  // creates vector containing all elements in hashtable
    for (int i = 0; i < (int)size; i++)               // adds all elements in hashtable to in_table
    {
        if (table[i].first != "") 
		{
            std::pair<std::string, T> temp(table[i].first, table[i].second);
            in_table.push_back(temp);
            table[i].first = "";
        }
    }
    if (size_ind < (int)size_arr.size() - 1)  // increases size of hashtable
    {
        size_ind++;
    }
    size = size_arr[size_ind];
    table.resize(size);          // changes hashtable size
    items = 0;                   // resets item count before re-inserting items to hashtable
    while (in_table.size() > 0)  // adds all elements of hashtable back
    {
        add(in_table[0].first, in_table[0].second);
        in_table.erase(in_table.begin());
    }
}

template<class T>
int Hashtable<T>::hash(std::string k) const 
{
    std::string temp = "";
    int zeros = 30 - k.length();
    for (int i = 0; i < zeros; i++)  // sets up leading zeros
    {
        temp += '0';
    }
    temp += k;
    std::array<long long, 5> w;
    for (int j = 0; j < (int)w.size(); j++)  // calculates w values
    {
        std::array<int, 6> a;
        for (int i = 0; i < (int)a.size(); i++)  // calculates a values
        {
            if (temp[i] == '0')  // if leading 0, or 0 as part of initial string
            {
                a[i] = 0;
            } else if (isdigit(temp[i]))  // if digit 1-9
            {
                a[i] = temp[i] - 48;
            } else  // if letter a-z
            {
                a[i] = (int)temp[i] - 96;
            }
        }
        w[j] = (long long)(pow(27, 5) * (a[0])) + (long long)(pow(27, 4) * (a[1])) + (long long)(pow(27, 3) * (a[2]))
               + (long long)(pow(27, 2) * (a[3])) + (long long)((27) * (a[4])) + (long long)a[5];
        temp = temp.substr(a.size());
    }

    long long hash_val
            = ((long long)(r[0] * w[0]) + (long long)(r[1] * w[1]) + (long long)(r[2] * w[2]) + (long long)(r[3] * w[3])
               + (long long)(r[4] * w[4]));
    return (int)(hash_val % size);
}

template<class T>
void Hashtable<T>::randoms() 
{
    if (debug)  // sets to predetermined values
    {
        r[0] = 983132572;
        r[1] = 1468777056;
        r[2] = 552714139;
        r[3] = 984953261;
        r[4] = 261934300;
    } 
	else  // creates random values
    {
        srand(time(NULL));
        for (int i = 0; i < (int)r.size(); i++) 
		{
            r[i] = rand() % size;
        }
    }
}

template<class T>
bool Hashtable<T>::get_resized() 
{
    return resized;
}

template<class T>
int Hashtable<T>::get_items() 
{
    return items;
}