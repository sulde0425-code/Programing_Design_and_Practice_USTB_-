#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include <cctype>
using namespace std;

int main() {
    /* TODO: you should read the file test.txt and then count the number of every kind of words */
    ifstream fin("test.txt");
    string line;
    map<string,int> hash;
    while (getline(fin,line)) {
        istringstream ss(line);
        string word;
        while (ss>>word) {
            hash[word]++;
        }
    }
    fin.close();
    /* ---------------------------------------------------------------------------------*/

    string max_word;
    int max_size = 0;
    /* TODO: please find the max number of all words , and then print it  */
    for (auto i:hash) {
        if (max_size<i.second) {
            max_size=i.second;
            max_word=i.first;
        }
    }

    cout << max_word << " " << max_size << endl;

    /*-------------------------------------------------------------------*/

    map<char, int> cnt;
    /* TODO: flip the string of max_length_word and then count the number of every alpha , please the the HashMap named cnt to count ,
     *  type converts from char to int */
    reverse(max_word.begin(), max_word.end());
    for (char i:max_word) {
        cnt[i]++;
    }
    /*------------------------------------------------------------------*/
    for (auto &c : cnt) {
        cout << c.first << " " << c.second << endl;
    }

    /* TODO: convert the string into uppercase and erase the max number alpha, the print the end word  */
    char most_char = 0;
    int most_count = 0;
    for (auto i:cnt) {
        if (most_count<i.second) {
            most_count=i.second;
            most_char=i.first;
        }
    }

    auto temp=remove(max_word.begin(),max_word.end(),most_char);
    max_word.erase(temp,max_word.end());

    for (int i=0;i<max_word.size();i++) {
        if (max_word[i]>=97 && max_word[i]>=65) {
            max_word[i]=max_word[i]-32;
        }
    }

    cout << max_word << endl;

    return 0;
}
