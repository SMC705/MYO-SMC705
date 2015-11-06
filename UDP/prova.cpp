#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

using namespace std;

void change(int * n_addr)
{
    *n_addr = 5;
}


int main() {

    int n=4;
    int * n_addr = &n; // pointer definition
    
    change(n_addr);
    
    cout << n << endl;
    
}
