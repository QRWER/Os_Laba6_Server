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

void WriteLog(String^ str) {
	FileStream^ fstream = gcnew FileStream("Logs.txt", FileMode::Append, FileAccess::Write);
	StreamWriter^ sw = gcnew StreamWriter(fstream);
	DateTime now = DateTime::Now;
	String^ date = now.ToString("yyyy-MM-dd HH:mm:ss");
	sw->WriteLine("[" + date + "] " + str);
	sw->Close();
}

void FormThread()
{
	formOpen = true;
	Application::SetCompatibleTextRenderingDefault(false);
	Application::EnableVisualStyles();
	Laba6Server::Server form;
	WriteLog("Окно сервера запустилось");
	Application::Run(% form);
	WriteLog("Окно сервера закрылось");
	formOpen = false;
}

void PovarFunction() {
	WriteLog("Повар приготовил еще еды");
	foodPot = { "Суп", "Мясо", "Бульон" };
}

[STAThreadAttribute]
int main(array<String^>^ args) {
	srand(time(NULL));
    Thread^ FThread = gcnew Thread(gcnew ThreadStart(&FormThread));	
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
		Socket^ clientSocket;

		// Привязываем сокет к IPEndPoint
		serverSocket->Bind(endPoint);

		serverSocket->Listen(100);
		WriteLog("Сервер запущен. Ожидание подключений");
		Console::WriteLine("Сервер запущен. Ожидание подключений...");
		while (formOpen)
		{
			bool isConnectionAvailable = serverSocket->Poll(5000, SelectMode::SelectRead);
			if (isConnectionAvailable) {
				clientSocket = serverSocket->Accept();
				WriteLog("Клиент подключился");
				Console::WriteLine("Клиент подключился");
				String^ answerPovar = "Повар: ";
				array<Byte>^ answerBuffer = gcnew array<Byte>(1024);
				int bytesRead = clientSocket->Receive(answerBuffer);
				String^ answerClient = System::Text::Encoding::UTF8->GetString(answerBuffer, 0, bytesRead);
				WriteLog("Сервер получил ответ: " + answerClient);
				if (answerClient == "Да" || answerClient == "да") {
					if (!foodPot.size()) {
						Thread^ PovarThread = gcnew Thread(gcnew ThreadStart(&PovarFunction));
						PovarThread->Start();
						PovarThread->Join();
					}
					int random = rand() % foodPot.size();
					answerPovar += "Держи ";
					answerPovar += gcnew String(foodPot[random].c_str());
					foodPot.erase(foodPot.cbegin() + random);
				}
				else {
					answerPovar += "Я не понимаю тебя, чего ты сюда пришел?";
				}
				array<Byte>^ buffer = Encoding::UTF8->GetBytes(answerPovar);
				clientSocket->Send(buffer);
				WriteLog("Сервер отправил ответ: " + answerPovar);
				clientSocket->Close();
				WriteLog("Клиент отключился");
				Console::WriteLine("Клиент отключился.");
			}
		}
		clientSocket->Close();
		serverSocket->Close();
		WriteLog("Сервер завершил работу\n");
	}
	catch (Exception^ e)
	{
		Console::WriteLine("Ошибка: " + e->Message);
		WriteLog("Ошибка: " + e->Message);
	}
}