#include <iostream>
#include <vector>

using namespace std;

class PoweredDevice{
	static int PDcounter;
	int id;
public:
	PoweredDevice(){
		id=PDcounter;
		PDcounter++;
		cout<<"A new PoweredDevice is constructed."<<endl;
		cout<<"There are "<<PDcounter<<" PoweredDevices in total."<<endl;
	}
	
		
};

int PoweredDevice::PDcounter=0;

class Scanner:virtual public PoweredDevice{
	static int Scounter;
	int Sid;
public:
	Scanner(){
		Sid=Scounter;
		Scounter++;
		cout<<"A new Scanner is constructed."<<endl;
		cout<<"There are "<<Scounter<<" Scanner in total."<<endl;
	}
};

int Scanner::Scounter=0;

class Printer:virtual public PoweredDevice{
	static int Pcounter;
	int Pid;
public: 
    Printer(){
    	Pid=Pcounter;
    	Pcounter++;
    	cout<<"A new Printer is constructed."<<endl;
    	cout<<"There are "<<Pcounter<<" Printer in total."<<endl;
	}
};

int Printer::Pcounter=0;

class Copier:public Scanner,public Printer{
	static int Ccounter;
	int Cid;
public:
	Copier(){
		Cid=Ccounter;
		Ccounter++;
		cout<<"A new Copier is constructed."<<endl;
		cout<<"There are "<<Ccounter<<" Copiers in total."<<endl;
	}
};

int Copier::Ccounter=0;

int main(){
	char op;
	vector<string> devices;
	while(cin>>op){
		vector<PoweredDevice*> PD;
		if(op=='s'){
			PD.push_back(new Scanner());
			devices.push_back("Scanner");
		}else if(op=='p'){
			PD.push_back(new Printer());
			devices.push_back("Printer");
		}else if(op=='c'){
			PD.push_back(new Copier());
			devices.push_back("Copier");
		}else if(op=='e'){
			for(auto device:PD){
				delete device;
			}
			PD.clear();
			cout<<"The devices are:"<<endl;
			for(auto& name:devices){
				cout<<name<<endl;
			}
			break;
		}
	}
}