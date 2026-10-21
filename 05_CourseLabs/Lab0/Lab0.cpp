#include <iostream>
using namespace std;
int main() {
    int n ; cin >> n ;
    int result ;
    /*TODO: if n is a odd number ,result is 3 times of n and then plus 1
     * if n is a even number ,result is the half of n;
     */


    if (n%2==0) {
        result=n>>1;
    }else {
        result=n*3+1;
    }



    /*----------------------------------------------------*/
    cout << result << endl ;
    return 0 ;
}