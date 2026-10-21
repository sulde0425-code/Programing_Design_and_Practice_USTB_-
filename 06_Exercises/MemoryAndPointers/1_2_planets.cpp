#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;
int calculate(vector<int>& vec,int N){
	int mid=vec[N/2];
	int sum=0;
	for(int i=0;i<N;i++){
		if(i!=N/2&&i<N/2){
			sum=sum+mid-vec[i];
		}else if(i!=N && i>N/2){
			sum=sum-mid+vec[i];
		}
	} 
	return sum;
}
int main(){
    int N;
	cin>>N;
	
	vector<int> X;	
	vector<int> Y;	
	vector<int> Z;	
    
    for(int i=0;i<N;i++){
    	int x , y , z;
    	cin>>x>>y>>z;
    	X.push_back(x);
    	Y.push_back(y);
    	Z.push_back(z);
	}
	
	sort(X.begin(),X.end());
	sort(Y.begin(),Y.end());
	sort(Z.begin(),Z.end());
	
	int sum;
	sum=calculate(X,N)+calculate(Y,N)+calculate(Z,N);
	
	cout<<sum<<endl;
}
