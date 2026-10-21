#include<iostream>
#include<string>
#include<vector>
using namespace std;

class Curriclum{
private:
	string name;
	int id;
	static int count;
	vector<string> pre;
public:
	Curriclum(string nm){
		name=nm;
		id=count;
		count++;
	}

	Curriclum(string nm,Curriclum &C){
		name=nm;
		id=count;
		count++;
		pre.push_back(C.getName());
		vector<string>vec=C.getPre();
		if(vec.size()>0){
		
		pre.insert(pre.end(),vec.begin(),vec.end());
        }
    }
	string getName() const{
		return name;
	}
	int getId() const{
		return id;
	}
	const vector<string>& getPre() const{
		return pre;
	}
	static int getCount(){
		return count;
	}
};

int Curriclum::count=0;

int main(){
	char op;
	vector<Curriclum> courses;
	while(cin>>op){
		if(op=='c'){
			string CourseName;
			int PreId;
			cin>>CourseName>>PreId;
			if(PreId==-1){
				courses.emplace_back(CourseName);
				
			}else if(PreId<Curriclum::getCount()){
				courses.emplace_back(CourseName,courses[PreId]);
			}
		}else if(op=='s'){
			for(auto& c: courses){
				cout<<"Course "<<c.getId()<<"-"<<c.getName()<<" has prerequisites: ";
				const vector<string>& preList=c.getPre();
				if(preList.empty()){
					cout<<"none"<<endl;
				}else{
					for(auto Name:preList){
						cout<<Name<<" ";
					}
					cout<<endl;
				}
			}
		}else if(op=='e'){
			break;
		}
		
	}
	return 0;
}