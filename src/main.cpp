#include<iostream>
#include<thread>
using namespace std;

void worker(int id){
  cout<<"Worker" <<id<<" is running\n";
}

int main(){
  cout<<"Main thread is running\n";
  thread t1(worker,1);
  thread t2(worker,2);
  thread t3(worker,3);

  t1.join();
  t2.join();
  t3.join();

  cout<<"All thread finished running\n";

  return 0;
}
