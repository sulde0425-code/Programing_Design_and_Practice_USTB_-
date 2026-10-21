#include<iostream>

using namespace std;

class MyStack{
private:
	int m_arry[10];
	int m_size;
public:
	MyStack(){
		m_size=0;
	}
	bool push(int n){
		if(m_size>10){
			return false;
		}else{
		    m_arry[m_size]=n;
		    m_size++;
		    return true;
	    }
	}
	bool pop(){
		if(m_size>0&&m_size<=10){
			m_size--;
			m_arry[m_size]=0;
			return true;
		}else{
			return false;
		}
	}
	void print(){
		if(m_size>0&&m_size<=10){
			cout<<"( ";
			for(int i=0;i<m_size;i++){
				cout<<m_arry[i]<<" ";
			}
			cout<<")";
		}
	}
};

int main(){
	MyStack s;
	char op;
	while(cin>>op){
		if(op=='i'){
			int n;
			cin>>n;
			s.push(n);
		}else if(op=='o'){
			if(!s.pop()){
				cout<<"Can not pop empty stack"<<endl;
				break;
			}
		}else if(op=='s'){
			s.print();
			break;
		}
	}
	return 0;
}