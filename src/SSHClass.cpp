#include "SSHClass.h"


mutex _mutex;

SSHClass::SSHClass(IAddInDefBase* connection)
{
	WSADATA wsadata;
	int32_t err = 0;
	err = WSAStartup(MAKEWORD(2, 0), &wsadata);

	libssh2_init(0);

	pConnection = connection;
}


SSHClass::~SSHClass()
{
	Disconnect();
}

std::string SSHClass::utf8_encode(const wstring& wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

std::wstring SSHClass::utf8_decode(const string& str)
{
	if (str.empty()) return wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], -1, NULL, 0);
	wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], -1, &wstrTo[0], size_needed);
	return wstrTo;
}

void SSHClass::StartAutorization(const wstring login, const wstring password)
{	
	thread connectThread(&SSHClass::Autorization, this, login, password);
	connectThread.detach();
}

void SSHClass::StartConnect(const wstring ip, const int port)
{
	_mutex.lock();
	if (connected)
	{
		connected = false;
		while (autorized)
		{

		}
		if (channel)
		{
			libssh2_channel_close(channel);
			libssh2_channel_free(channel);
		}
		if (session)
		{
			libssh2_session_disconnect(session, "");
			libssh2_session_free(session);
		}
	}
	if (sock)
		closesocket(sock);
	_mutex.unlock();
	thread connectThread(&SSHClass::Connect, this, ip, port);
	connectThread.detach();
}

void SSHClass::Autorization(const wstring login, const wstring password)
{
	_mutex.lock();
	string utf8_login = utf8_encode(login.c_str());
	string utf8_password = utf8_encode(password.c_str());
	int ret = libssh2_userauth_password(session, utf8_login.c_str(), utf8_password.c_str());
	if (ret != 0)
	{
		switch (ret)
		{
		case LIBSSH2_ERROR_SOCKET_SEND:
			pConnection->ExternalEvent(L"Log", L"Error", L"Ошибка передачи данных");
			break;
		case LIBSSH2_ERROR_AUTHENTICATION_FAILED:
			pConnection->ExternalEvent(L"Log", L"Error", L"Неправильные логин или пароль");
			break;
		case LIBSSH2_ERROR_ALLOC:
			pConnection->ExternalEvent(L"Log", L"Error", L"An internal memory allocation call failed");
			break;
		case LIBSSH2_ERROR_PASSWORD_EXPIRED:
			pConnection->ExternalEvent(L"Log", L"Error", L"LIBSSH2_ERROR_PASSWORD_EXPIRED ");
			break;
		case -13:
			pConnection->ExternalEvent(L"Log", L"Error", L"База заблокирована конфигуратором");
			break;
		default:
			wstring error = L"Неизвестная ошибка авторизации " + to_wstring(ret);
			pConnection->ExternalEvent(L"Log", L"Error", (wchar_t*)error.c_str());
			break;
		}
	}

	if (!ret)
	{
		channel = libssh2_channel_open_session(session);
		if (!channel)
		{
			pConnection->ExternalEvent(L"Log", L"Error", L"Unable to open session");
			ret = 1;
		}
	}
	if (!ret)
	{
		if (channel)
		{
			if (libssh2_channel_shell(channel)) {
				pConnection->ExternalEvent(L"Log", L"Error", L"Unable to request shell on allocated pty");
				ret = 1;
			}
		}
	}

	if (!ret)
	{
		libssh2_channel_set_blocking(channel, 0);
		p_readWriteThread = make_shared<thread>(&SSHClass::ReadWriteThread, this);
		p_readWriteThread->detach();
		pConnection->ExternalEvent(L"System", L"Authoriation", L"true");
		autorized = true;
	}
	

	_mutex.unlock();
}

void SSHClass::Connect(const wstring ip, const int port)
{
	_mutex.lock();
	sock = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in sockaddr;
	ZeroMemory(&sockaddr, sizeof(sockaddr));

	string init_addr_string = utf8_encode(ip);

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr.s_addr = inet_addr(init_addr_string.c_str());


	int ret = connect(sock, (struct sockaddr*)&sockaddr, sizeof(struct sockaddr_in));
	wstring result;
	result.append(L"Подключение к " + ip + L":" + to_wstring(port) + L" выполнено с кодом (" + to_wstring(ret) + L")");

	pConnection->ExternalEvent(L"Log", L"Debug", (wchar_t*)result.c_str());
	pConnection->ExternalEvent(L"System", L"Connection", (wchar_t*)to_wstring(ret).c_str());

	session = libssh2_session_init();
	if (session == nullptr)
	{
		pConnection->ExternalEvent(L"Debug", L"Error", L"Ошибка создания сессии");
		_mutex.unlock();
		return;
	}

	if (ret == 0)
	{
		ret = libssh2_session_handshake(session, sock);
		if (ret != 0)
		{
			switch (ret)
			{
			case LIBSSH2_ERROR_SOCKET_NONE:
				pConnection->ExternalEvent(L"Log", L"Error", L"LIBSSH2_ERROR_SOCKET_NONE");
			case LIBSSH2_ERROR_BANNER_SEND:
				pConnection->ExternalEvent(L"Log", L"Error", L"LIBSSH2_ERROR_BANNER_SEND");
			case LIBSSH2_ERROR_KEX_FAILURE:
				pConnection->ExternalEvent(L"Log", L"Error", L"LIBSSH2_ERROR_KEX_FAILURE");
			case LIBSSH2_ERROR_SOCKET_SEND:
				pConnection->ExternalEvent(L"Log", L"Error", L"LIBSSH2_ERROR_SOCKET_SEND");
			case LIBSSH2_ERROR_SOCKET_DISCONNECT:
				pConnection->ExternalEvent(L"Log", L"Error", L"LIBSSH2_ERROR_SOCKET_DISCONNECT");
			case LIBSSH2_ERROR_PROTO:
				pConnection->ExternalEvent(L"Log", L"Error", L"LIBSSH2_ERROR_PROTO");
			case LIBSSH2_ERROR_EAGAIN:
				pConnection->ExternalEvent(L"Log", L"Error", L"LIBSSH2_ERROR_EAGAIN");
			}
		} else
		{
			connected = true;
		}
	}

	_mutex.unlock();

}

void SSHClass::SendMessageSSH(const wstring message)
{
	string utf8_message = utf8_encode(message);
	int n = libssh2_channel_write(channel, utf8_message.c_str(), utf8_message.length());
	/*if (n > 0)
		pConnection->ExternalEvent(L"Log", L"Error", L"Успешно отправлено");
	else*/ if (n == LIBSSH2_ERROR_EAGAIN)
		pConnection->ExternalEvent(L"Log", L"Error", L"Send LIBSSH2_ERROR_EAGAIN");
	else if (n < 0)
		pConnection->ExternalEvent(L"Log", L"Error", L"Ошибка отправки");
	
}

void SSHClass::Disconnect()
{
	_mutex.lock();
	if (connected)
	{
		connected = false;
		while (autorized)
		{

		}
		if (channel)
		{

			libssh2_channel_close(channel);
			libssh2_channel_free(channel);
		}
		if (session)
		{
			libssh2_session_disconnect(session, "");
			libssh2_session_free(session);
		}

	}
	if (sock)
		closesocket(sock);
	libssh2_exit();
	_mutex.unlock();
}

void SSHClass::ReadWriteThread()
{
	int rc = 0;
	char buffer[1000];
	
	do
	{
		_mutex.lock();
		if (!connected)
			break;

		rc = libssh2_channel_read(channel, buffer, sizeof(buffer));
		_mutex.unlock();
		if (rc > 0)
		{

			string utf8_string(buffer, rc);
			wstring message = utf8_decode(utf8_string);
			pConnection->ExternalEvent(L"Log", L"Message", (wchar_t*)message.c_str());
		}
		else if (rc == LIBSSH2_ERROR_EAGAIN)
			Sleep(100);
		else //if (rc != LIBSSH2_ERROR_EAGAIN)
			pConnection->ExternalEvent(L"Log", L"Error", L"Error read ");
	} while (rc > 0 || connected);

	autorized = false;
}

