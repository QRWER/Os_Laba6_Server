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

std::vector<std::string> foodPot = {"���", "����", "������"};

void FormThread()
{
	Application::SetCompatibleTextRenderingDefault(false);
	Application::EnableVisualStyles();
	Laba6Server::Server form;
	Application::Run(% form);
}

void PovarFunction() {
	foodPot = { "���", "����", "������" };
}

[STAThreadAttribute]
int main(array<String^>^ args) {
	srand(time(NULL));
    Thread^ FThread = gcnew Thread(gcnew ThreadStart(&FormThread));	
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

		while (true)
		{
			try {
				// ��������� �������� �����������
				Socket^ clientSocket = serverSocket->Accept();
				Console::WriteLine("������ �����������.");

				array<Byte>^ answerBuffer = gcnew array<Byte>(1024);
				int bytesRead = clientSocket->Receive(answerBuffer);
				String^ answer = System::Text::Encoding::UTF8->GetString(answerBuffer, 0, bytesRead);
				if (!foodPot.size()) {
					Thread^ PovarThread = gcnew Thread(gcnew ThreadStart(&PovarFunction));
					PovarThread->Start();
					PovarThread->Join();
				}
				int random = rand() % foodPot.size();
				String^ food = gcnew String(foodPot[random].c_str());
				foodPot.erase(foodPot.cbegin() + random);
				array<Byte>^ buffer = Encoding::UTF8->GetBytes(food);
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
}