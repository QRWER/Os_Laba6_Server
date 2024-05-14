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

std::vector<std::string> foodPot = {"Суп", "Мясо", "Бульон"};

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
	foodPot = { "Суп", "Мясо", "Бульон" };
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
		// Создаем IP-адрес и порт для сервера
		IPAddress^ ipAddress = IPAddress::Parse("127.0.0.1");
		int port = 8080;

		// Создаем IPEndPoint для сервера
		IPEndPoint^ endPoint = gcnew IPEndPoint(ipAddress, port);

		// Создаем сокет
		Socket^ serverSocket = gcnew Socket(AddressFamily::InterNetwork, SocketType::Stream, ProtocolType::Tcp);

		// Привязываем сокет к IPEndPoint
		serverSocket->Bind(endPoint);

		serverSocket->Listen(100);

		Console::WriteLine("Сервер запущен. Ожидание подключений...");
		while (formOpen)
		{
			try {
				// Принимаем входящее подключение
				Socket^ clientSocket = serverSocket->Accept();
				Console::WriteLine("Клиент подключился.");
				String^ answerPovar = "Повар: ";
				array<Byte>^ answerBuffer = gcnew array<Byte>(1024);
				int bytesRead = clientSocket->Receive(answerBuffer);
				String^ answerClient = System::Text::Encoding::UTF8->GetString(answerBuffer, 0, bytesRead);
				if (answerClient == "Да" || answerClient == "да") {
					if (!foodPot.size()) {
						Thread^ PovarThread = gcnew Thread(gcnew ThreadStart(&PovarFunction));
						PovarThread->Start();
						PovarThread->Join();
					}
					int random = rand() % foodPot.size();
					answerPovar = "Держи ";
					answerPovar += gcnew String(foodPot[random].c_str());
					foodPot.erase(foodPot.cbegin() + random);
				}
				else {
					answerPovar += "Я не понимаю тебя, чего ты сюда пришел?";
				}
				array<Byte>^ buffer = Encoding::UTF8->GetBytes(answerPovar);
				clientSocket->Send(buffer);
				Console::WriteLine("Клиент отключился.");
			}
			catch (Exception^ e)
			{
				Console::WriteLine("Ошибка: " + e->Message);
			}
		}
	}
	catch (Exception^ e)
	{
		Console::WriteLine("Ошибка: " + e->Message);
	}
	sw->Close();
}