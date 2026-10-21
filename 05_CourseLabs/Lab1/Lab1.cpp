#include <iostream>
#include <vector>
#include <cmath>
using namespace std;
/* you may add the header file in here */


bool is_prime (int x) {
    /* TODO: now you should judge whether a number is a prime or not */
    for (int i=1;i<=sqrt(x);i++) {
        if (x%i==0&&i!=1) return false;
    }
    return true;
}


void memory_prime(vector<int> & primes,int n) {
    /*TODO: now you hava a number n ,please storage the primes between 1 and n into the vector v
     * you may use the function that judge whether a number is a prime or not.
     */
    for (int i=2;i<=n;i++) {
        if (is_prime(i)) {
            primes.push_back(i);
        }
    }
}



void iteration_for(const vector<int> &v) {
    /*TODO: now you hava a vector v including the primes,you should use the range-based for loop
     * to print all primes storaging in the v.
     */
    for (auto i:v) {
        cout << i << " ";
    }
    cout<<endl;
}


void iteration(const vector<int> &v) {
    /*TODO: now you hava a vector v include the primes
     *  you should use the iterator to print all primes storaging in the v
     */
    vector<int>::const_iterator it;

    for (it=v.begin();it!=v.end();it++) {
        cout<<*it<<" ";
    }
    cout<<endl;
}


int sum(const vector<int>&v ) {
    /*TODO: return the sum of v including the primes*/
    int sum=0;
    for (auto i:v) {
        sum+=i;
    }
    return sum;
}


int main() {
    vector<int>primes ;
    int n ;cin >> n ;
    memory_prime(primes , n) ;
    iteration_for(primes);
    iteration(primes) ;
    cout <<sum(primes) <<endl ;
    return 0 ;
}