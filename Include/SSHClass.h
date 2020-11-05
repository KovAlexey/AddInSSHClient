#include "ComponentBase.h"
#include <libssh2.h>
#include "AddInDefBase.h"
#include <thread>
#include <string>
#include <mutex>

using namespace std;

class SSHClass
{
public:
	SSHClass(IAddInDefBase* connection);
	~SSHClass();
	std::string utf8WideCharToMultyByte(const std::wstring& wstr);
	std::wstring utf8MultiByteToWideChar(const string& str);
	void StartAutorization(const wstring login, const wstring password);
	void Autorization(const wstring login, const wstring password);
	void StartConnect(const wstring ip, const int port);
	void Connect(const wstring ip, const int port);
	void SendMessageSSH(const wstring message);
	void Disconnect();
	bool isConnected() { return connected; };
	void request_pty(wstring pty_name);
private:
	bool autorized = false;
	bool connected = false;
	IAddInDefBase* pConnection;
	int sock = 0;
	LIBSSH2_SESSION* session = nullptr;
	LIBSSH2_CHANNEL* channel = nullptr;
	shared_ptr<thread> p_readWriteThread;

	void ReadWriteThread();
	
	
	//void Authorization(wchar_t* login, wchar_t* password);
	
};

