#include<iostream>
#include<thread>
using namespace std;

void worker{
  cout<<"Worker thread is running\n";
}

int main(){
  cout<<"Main thread is running\n";
  thread t(worker);
  t.join();
  cout<<"Main thread is running\n";

  return 0;
}
