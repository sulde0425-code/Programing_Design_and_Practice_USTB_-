#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class Student{
private:
	string name;
	int id;
	static int counter;
public:
	Student(){
		
	}
	Student(string Name){
	    name=Name;
	    id=counter;
	    counter++;
	}
	string getName(){
		return name;
	}
	int getId(){
		return id;
	}
	void print(){
		cout<<id<<"-"<<name;
	}
	
};

int Student::counter=0;

class Major{
private:
	string name;
	int id;
	static int m_counter;
	vector<int> students;
public:
	Major(){
		
	}
	Major(string Name){
		name = Name;
		id=m_counter;
		m_counter++;
	}
	void addStudent(int i){
		students.push_back(i);
	}
	void removeStudent(int i){
		auto it=find(students.begin(),students.end(),i);
		if(it==students.end()){
			cout<<"删除失败"<<endl;
			return ;
		}
		students.erase(it);
	}
	int getId(){
		return id;
	}
	string getName(){
		return name;
	}
	const vector<int>& getStudents()const&{
		return students;
	}
	void print(){
		cout<<"Major "<<id<<"-"<<name<<":";
	}
};

int Major::m_counter=0;

int main(){
	char op;
	vector<Student> s;
	vector<Major> m;
	while(cin>>op){
		if(op=='s'){
			string Name;
			cin>>Name;
			s.emplace_back(Name);
		}else if(op=='m'){
			string name;
			cin>>name;
			m.emplace_back(name);
		}else if(op=='n'){
			if(!s.empty()){
				for(auto& S: s){
					S.print();
					cout<<" ";
				}
				cout<<endl;
			}
		}else if(op=='a'){
			int sid,mid;
			cin>>sid>>mid;
			m[mid].addStudent(sid);
		}else if(op=='l'){
			if(!m.empty()){
				for(auto& M:m){
					M.print();
					vector<int> students=M.getStudents();
					if(!students.empty()){
						for(auto& i:students){
							cout<<" ";
							s[i].print();
						}
						
					}
					cout<<endl;
				}
				
			}
		}else if(op=='d'){
			int sid,mid;
			cin>>sid>>mid;
			m[mid].removeStudent(sid);
		}else if(op=='v'){
			int fMid,sid,sMid;
			cin>>fMid>>sid>>sMid;
			m[fMid].removeStudent(sid);
			m[sMid].addStudent(sid);
		}else if(op=='e'){
			break;
		}
		
	}
}