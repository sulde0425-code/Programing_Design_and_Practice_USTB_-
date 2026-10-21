#include<iostream>
#include<vector>
#include<string>
#include<sstream>
using namespace std;
vector<int> get_input(){
	string line;
	getline(cin,line);
	
	stringstream ss(line);
	
	vector<int> nums;
	int num;
	while(ss>>num){
		nums.push_back(num);
	}
	
	return nums;
}

int Func(int* A,int nSize){
	int pointer=nSize-1;
	
	for(int i=0;i<nSize-1;i++){
		for(int j=0;j<nSize-i-1;j++){
			if(A[j]<A[j+1]){
				
				int temp=A[j];
				A[j]=A[j+1];
				A[j+1]=temp;
				
			}
		}
	}
	
	for(int i=0;i<nSize;i++){
		if(A[i]==0){
			pointer=i;
			break;
		}
	}
	
	return pointer;
}

int main(){
	vector<int> nums=get_input();
	int nSize=nums.size();
	int *A;
	A=nums.data();
	
	int point;
	point=Func(A,nSize);
	
	for(int i=0;i<nSize;i++){
		cout<<A[i]<<" ";
	}
	cout<<endl;
	cout<<point<<endl;
}