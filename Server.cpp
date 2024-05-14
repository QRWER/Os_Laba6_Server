#include "Server.h"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
using namespace System;
using namespace System::Windows::Forms;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Threading;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace System::IO;

std::vector<std::string> foodPot = {"���", "����", "������"};

bool formOpen = false;

void FormThread()
{
	formOpen = true;
	Application::SetCompatibleTextRenderingDefault(false);
	Application::EnableVisualStyles();
	Laba6Server::Server form;
	Application::Run(% form);
	formOpen = false;
}

void PovarFunction() {
	foodPot = { "���", "����", "������" };
}

[STAThreadAttribute]
int main(array<String^>^ args) {
	srand(time(NULL));
    Thread^ FThread = gcnew Thread(gcnew ThreadStart(&FormThread));	
	FileStream^ fstream = gcnew FileStream("textfile.txt", FileMode::Append, FileAccess::Write);
	StreamWriter^ sw = gcnew StreamWriter(fstream);
    FThread->Start();
	try
	{
		// ������� IP-����� � ���� ��� �������
		IPAddress^ ipAddress = IPAddress::Parse("127.0.0.1");
		int port = 8080;

		// ������� IPEndPoint ��� �������
		IPEndPoint^ endPoint = gcnew IPEndPoint(ipAddress, port);

		// ������� �����
		Socket^ serverSocket = gcnew Socket(AddressFamily::InterNetwork, SocketType::Stream, ProtocolType::Tcp);

		// ����������� ����� � IPEndPoint
		serverSocket->Bind(endPoint);

		serverSocket->Listen(100);

		Console::WriteLine("������ �������. �������� �����������...");
		while (formOpen)
		{
			try {
				// ��������� �������� �����������
				Socket^ clientSocket = serverSocket->Accept();
				Console::WriteLine("������ �����������.");
				String^ answerPovar = "�����: ";
				array<Byte>^ answerBuffer = gcnew array<Byte>(1024);
				int bytesRead = clientSocket->Receive(answerBuffer);
				String^ answerClient = System::Text::Encoding::UTF8->GetString(answerBuffer, 0, bytesRead);
				if (answerClient == "��" || answerClient == "��") {
					if (!foodPot.size()) {
						Thread^ PovarThread = gcnew Thread(gcnew ThreadStart(&PovarFunction));
						PovarThread->Start();
						PovarThread->Join();
					}
					int random = rand() % foodPot.size();
					answerPovar = "����� ";
					answerPovar += gcnew String(foodPot[random].c_str());
					foodPot.erase(foodPot.cbegin() + random);
				}
				else {
					answerPovar += "� �� ������� ����, ���� �� ���� ������?";
				}
				array<Byte>^ buffer = Encoding::UTF8->GetBytes(answerPovar);
				clientSocket->Send(buffer);
				Console::WriteLine("������ ����������.");
			}
			catch (Exception^ e)
			{
				Console::WriteLine("������: " + e->Message);
			}
		}
	}
	catch (Exception^ e)
	{
		Console::WriteLine("������: " + e->Message);
	}
	sw->Close();
}