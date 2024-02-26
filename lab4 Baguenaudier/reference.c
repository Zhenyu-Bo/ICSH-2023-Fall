int REMOVE(int,int);
int PUT(int,int);
int main(){
	int n;/*the value of n in x3100*/ 
	REMOVE(n,0);
	return 0;
}
int REMOVE(int n,int state){
	/*these two args means*/
	/*(1-n rings will be removed,all rings' state at now)*/
	/*the return value means*/
	/*after these operations, what all rings' state are*/
	/*Note that*/
	/*you should store the state of all rings at the specific memory*/
	/*in an appropriate location*/
	if(n==0) return state;/*the state remains*/
	if(n==1){
		/*change the 1st ring's state*/
		return /*all rings' state at now*/;
	}
	/*REMOVE the first n-2 rings*/
	/*REMOVE the n-th ring*/
	/*PUT the first n-2 rings*/
	/*RMOVE the first n-1 rings*/
	return /*all rings' state at now*/;
}
int PUT(int n,int state){
	/*these two args means*/
	/*(1-n rings will be put,all rings' state at now)*/
	/*the return value means*/
	/*after these operations, what all rings' state are*/
	/*you just need to inverse REMOVE*/
}
