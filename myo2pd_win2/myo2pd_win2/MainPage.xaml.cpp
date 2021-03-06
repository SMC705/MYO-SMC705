﻿//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace myo2pd_win2;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace std;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();
}

/************* UDP CLIENT CODE *******************/
#include <iostream>
//#ifdef __WIN32__
# include <winsock2.h>
//#else
//# include <sys/socket.h>
//#endif
//#ifdef __WIN32__
# include <WS2tcpip.h>
//#else
//#include <netinet/in.h>
//#endif
#include <string>

using namespace std;

int main() {
	cout << "Try" << endl;

	int clientSocket, portNum, nBytes;
	char buffer[1024];
	struct sockaddr_in serverAddr;
	socklen_t addr_size;

	/*Create UDP socket*/
	clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

	/*Configure settings in address struct*/
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(7890);
	serverAddr.sin_addr.s_addr = 16777343;
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	/*Initialize size variable to be used later on*/
	addr_size = sizeof serverAddr;

	while (1) {
		printf("Type a sentence to send to server:\n");
		cin.getline(buffer, 1024);
		//    fgets(buffer,1024,stdin);
		printf("You typed: %s", buffer);

		nBytes = strlen(buffer) + 1;

		/*Send message to server*/
		sendto(clientSocket, buffer, nBytes, 0, (struct sockaddr *)&serverAddr, addr_size);

		/*Receive message from server*/
		//                nBytes = recvfrom(clientSocket,buffer,1024,0,NULL, NULL);

		//    printf("Received from server: %s\n",buffer);

	}

	return 0;
}
