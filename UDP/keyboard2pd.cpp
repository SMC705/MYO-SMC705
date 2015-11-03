#include <string>
#include <iostream>

// UDP libraries
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

int main() {

  //==========================================================================
  // UDP CONNECTION TO LOCALHOST
        
  int clientSocket, portNum, nBytes;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
        
  /*Create UDP socket*/
  clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
        
  /*Configure settings in address struct*/
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(7890);      // port
  serverAddr.sin_addr.s_addr = 16777343;  // local address
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
        
  /*Initialize size variable to be used later on*/
  addr_size = sizeof serverAddr;



  //==========================================================================
  // WHILE LOOP FOR INPUT COMMANDS
  
  string inputStr;
  char * buffer = new char[4];
  buffer[0] = '/';
  
  while ( cin >> inputStr ) {

    // volume up
    if ( inputStr.compare("w") == 0 ) {
      buffer[1] = 'v';
      buffer[2] = 'u';
    }
    // volume down
    else if ( inputStr.compare("s") == 0 ) {
      buffer[1] = 'v';
      buffer[2] = 'd';
    }
    // volume mute
    else if ( inputStr.compare("x") == 0 ) {
      buffer[1] = 'v';
      buffer[2] = 'm';
    }
    // one-song backward
    else if ( inputStr.compare("q") == 0 ) {
      buffer[1] = 'o';
      buffer[2] = 'b';
    }
    // one-song forward
    else if ( inputStr.compare("e") == 0 ) {
      buffer[1] = 'o';
      buffer[2] = 'f';
    }
    // scroll backward
    else if ( inputStr.compare("a") == 0 ) {
      buffer[1] = 's';
      buffer[2] = 'b';
    }
    // scroll forward
    else if ( inputStr.compare("d") == 0 ) {
      buffer[1] = 's';
      buffer[2] = 'f';
    }
    // lock
    else if ( inputStr.compare("l") == 0 ) {
      buffer[1] = 'l';
      buffer[2] = 'l';
    }
    // unlock
    else if ( inputStr.compare("u") == 0 ) {
      buffer[1] = 'u';
      buffer[2] = 'l';
    }
    else {
      cout << "Unknown command" << endl;
      buffer[1] = 'n';
      buffer[2] = 'n';
    }

    // Send UDP package
    
    nBytes = strlen(buffer) + 1;
    /*Send message to server*/
    sendto(clientSocket,buffer,nBytes,0,(struct sockaddr *)&serverAddr,addr_size);

    cout << "Command sent: " << buffer << endl;
    
  }

  delete[] buffer;
  
}
