#include <iostream>
#include<vector>
#include<sstream>
#include<string>
using namespace std;
void move(int* arry,int n,int m){
	if(n<=0||m<=0){
		return;
	}
	m=m%n;
	
	vector<int> temp;
	
	for(int i=n-m;i<n;i++){
		temp.push_back(arry[i]);
	}
	for(int i=n-1;i>=m;i--){
		arry[i]=arry[i-m];
	}
	for(int i=0;i<m;i++){
		arry[i]=temp[i];
	}
}
vector<int> getinput(){
	vector<int>input;
	string input_line;
	
	getline(cin,input_line);
	stringstream ss(input_line);
	int num;
	while (ss>>num){
		input.push_back(num);
	}
	return input;
	
}
int main(){
	vector<int> get;
	get=getinput();
	
	int* arry;
	int n;
	int m=3;
	
	arry=get.data();
	n=get.size();
	move(arry,n,m);
	
	for(int i=0;i<n;i++){
		cout<<*(arry+i)<<" ";
	}
	
}