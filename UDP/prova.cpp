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

    double prova[5];
    prova[0] = 4;
    prova[1] = 10;
    prova[2] = 1;
    prova[3] = -3;
    prova[4] = -1;
    
    for (int i=0; i<5; i++) {
        for (int j=i+1; j<5; j++) {
            if (prova[i] > prova[j]) {
                int temp = prova[i];
                prova[i] = prova[j];
                prova[j] = temp;
                
            }
        }
    }
    
    for (int i=0; i<5; i++) {
        cout << prova[i] << endl;
    }
    
}
