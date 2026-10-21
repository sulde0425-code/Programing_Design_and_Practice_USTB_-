#include<iostream>
#include<string>
#include<vector>
#include<utility>
using namespace std;
bool vecfind(vector<int> mem,int target){
	for(int i=0;i<mem.size();i++){
		if(mem[i]==target)
		return true;
	}
	return false;
}

int main(){
	int T,MaxMem;
	cin>>T>>MaxMem;
	
	int top=0;
	int* A;
	A=new int[MaxMem]{0};
	vector<int> mem;
	int counter=0; 
	
	while(T>0){
		string op;     //命令
		int num;
		cin>>op;
			
		//if(num<=0&&op!="def"){//检查输入合法性
		//	cout<<"ILLEGAL_OPERATION"<<endl;
		//	T--;
		//	continue;
		//}
		
		if(op=="new"){
			cin>>num;
			if(top+num>MaxMem){
				cout<<"NULL"<<endl;
			}else{
			    counter++;
			    cout<<counter<<endl;
			    for(int i=0;i<num;i++){
			    	A[top+i]=counter;
				}
			    
			    top=top+num;
		        mem.push_back(counter);
			}
		}
		else if(op=="del"){
			cin>>num;
			if(!vecfind(mem,num)){
				cout<<"ILLEGAL_OPERATION"<<endl;
			}else{
				for(int i=0;i<top;i++){
					if(A[i]==num){
						A[i]=0;
					}
				}
				for(int i=MaxMem-1;i>=0;i--){
					if(A[i]!=0){
						top=i+1;
					}
				}
			}
		}
		else if(op=="def"){
			vector<int> used_block;
			for(int i=0;i<MaxMem;i++){
				if(A[i]!=0){
					used_block.push_back(A[i]);
					A[i]=0;
				}
			}
			for(int i=0;i<used_block.size();i++){
				A[i]=used_block[i];
			}
			top=used_block.size();
			
			//int free_mem = MaxMem - top;
            //cout << free_mem << endl;
		}
		T--;
	}
	delete []A;
}