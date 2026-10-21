#include<iostream>
#include<algorithm>
#include<vector>
#include<climits>

using namespace std;
 
struct planet{
	int x,y,z;
};

long long calculator(const vector<planet>&planets,const planet& p,int N ){
	long long TTdistans=0;
	for(int i=0;i<N;i++){
		TTdistans+= abs(planets[i].x-p.x)+abs(planets[i].y-p.y)+abs(planets[i].z-p.z);
	}
	
	return TTdistans;
}

int main(){
	int N;
	cin>>N;
    
	vector<planet> planets(N);
	
	for(int i=0;i<N;i++){
		cin>>planets[i].x>>planets[i].y>>planets[i].z;
	}	
	
	long long totalMin=LLONG_MAX;
	
	for(int i=0;i<N;i++){
		long long distans=calculator(planets,planets[i],N);
		if(distans<totalMin){
			totalMin=distans;
		}
	}
	
	cout<<totalMin<<endl;
} 
