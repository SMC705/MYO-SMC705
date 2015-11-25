#include <iostream>
#include <fstream>

// UDP libraries
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;


const string userID = "01";
const string fileName = "/Users/Paolo/Documents/AAU/2015-2016/Fall\ Semester\ Project/MYO-SMC705/Test\ Recordings/user" + userID + ".txt";


int main() {

    // input file
    ifstream inputFile;
    inputFile.open( fileName );
    
    while ( inputFile.eof() ) {
        
        string myString;
        getline( inputFile, myString );
        cout << myString << endl;
        
    }
    

}
