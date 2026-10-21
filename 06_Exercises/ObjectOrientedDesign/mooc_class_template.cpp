#include<iostream>
#include<string>
#include<initializer_list> 
using namespace std;
template<class T,int size>
class Student{
private:
	string m_name;
	T m_scores[size];
	int m_scoresNum;
public:
	Student(string name,initializer_list<T> scores)
		:m_name(name), m_scoresNum(scores.size()), m_scores{0} {
		int i=0;
		for(auto s:scores){
			m_scores[i] =s;
			++i;
		}
	}	
	friend ostream& operator<< (ostream& out,const Student<T,size>& stu){
		cout<<stu.m_name<<" ";
		for(int i=0;i<stu.m_scoresNum;i++){
			cout<<stu.m_scores[i]<<" ";
		}
		return out;
	}
	
};

typedef Student<double,5> Undergraduate;
typedef Student<char,3 > Master;

int main(){
	Undergraduate s1("Alice",{90.5,98.4,76.5,89.7});
	Master s2("Bob",{'A','B','C'});
	cout<<s1<<endl;
	cout<<s2<<endl;
	return 0; 
}
