#include <iostream>
#include <vector>
#include <string>
using namespace std;
class Point2D{
	int x , y ;
public:
	Point2D(){
		x = 0 ; y = 0 ;
	}
	Point2D(int X,int Y):x(X),y(Y){
	}
	void change(int X,int Y){
		x=X;
		y=Y;
	}
	int getX()const{
		return x;
	}
	int getY()const{
		return y;
	}
};

class Creature{
	string name;
	int id;
	Point2D adress;
	static int count;
public:
	Creature(string Name,int X ,int Y){
		name=Name;
		adress.change(X,Y);
		id=count;
		count++;
	}
	void changeP(int X ,int Y){
		adress.change(X,Y);
	}
	int getId() const{
		return id;
	}
	string getName()const{
		return name;
	}
	int getX()const{
		int x;
		x=adress.getX();
		return x;
	}
	int getY()const{
		int y;
		y=adress.getY();
		return y;
	}
	
};

int Creature::count=0;

int main(){
	char op;
	vector<Creature>  creatures;
	  
	while(cin>>op){
		if(op=='n'){
			string name;
			int x;
			int y;
			cin>>name>>x>>y;
			
			creatures.emplace_back(name,x,y);
			
		}else if(op=='a'){
			for(auto& c:creatures){
				cout<<"No. "<<c.getId()<<" creature "<<c.getName()<<" is at ("<<c.getX()<<", "<<c.getY()<<")"<<endl;
			}
		}else if(op=='m'){
			int i,x,y;
			cin>>i>>x>>y;
			creatures[i].changeP(x,y);
		}else if(op=='s'){
			int i;
			cin>>i;
			cout<<"No. "<<creatures[i].getId()<<" creature "
			<<creatures[i].getName()<<" is at ("<<creatures[i].getX()<<", "<<creatures[i].getY()<<")"<<endl;
		}else if(op=='e'){
			break;
		}
	}
	return 0;
}