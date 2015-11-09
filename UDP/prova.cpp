#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <vector>


using namespace std;

void change(int * n_addr)
{
    *n_addr = 5;
}


int main() {

    int i=0;
    while(1) {
        if (cin.get()) break;
        else {
            cout << i << endl;
            i++;
        }
    }
    
}
