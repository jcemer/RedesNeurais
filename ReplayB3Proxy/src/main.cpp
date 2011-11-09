#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h> //for atoi
#include "Ambiente.h"

using namespace std;

int fuzzy(string& ip, int portClient, int portServer, string logName)
{
  //inicio do laço de jogo
  cout << "Connecting...";
  Ambiente ambiente((char*)ip.c_str(), portClient, portServer);
  cout << "Connected !!" << endl;

  filebuf fb;
  fb.open (logName.c_str(), ios::out);
  ostream os(&fb);

  ambiente.outputLogHeader(os);

  double leftWheel, rightWheel;
  while(1)
  {
    ambiente.sendOutput(leftWheel, rightWheel);
    ambiente.outputLogData(os);
  }

  return 0;
}

int main()
{
  //read stdin
  string ip="127.0.0.1",
         sPort = "3111",
         cPort = "2048",
         logName = "proxyLog.txt";

  //convert port
  int portServer = atoi(sPort.c_str());
  int portClient = atoi(cPort.c_str());

  //return main_tests();
  return fuzzy(ip, portClient, portServer, logName);
}

