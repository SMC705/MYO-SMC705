#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include <cmath>


using namespace std;

void change(int * n_addr)
{
    *n_addr = 5;
}


int main() {

    double n=153;
    cout << static_cast<int>(rint(n/5)) << endl;
    
    
}
