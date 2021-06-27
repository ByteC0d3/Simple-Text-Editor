//libs: -s -lgdi32 -lcomdlg32 -o
#include <windows.h>
#include <stdio.h>

#define IDM_ABOUT 805		//id File -> About
#define ID_EDITCHILD 303	//id Edit
#define IDM_FILE_SAVE 801	//id File -> Save
#define IDM_CLEAN 804 		//id Format -> Clean
#define IDM_FILE_OPEN 802	//id File -> Open
#define IDM_FILE_QUIT 803	//id File -> Quit

char *APP_NAME = "Notepad";
static HWND hwndEdit;

//function set window to center of screen
void CenterWindow(HWND hwnd_self)
{
	HWND hwnd_parent;
	RECT rw_self, rc_parent, rw_parent;
	int xpos, ypos;
	hwnd_parent = GetParent(hwnd_self);
	if (NULL == hwnd_parent)
		hwnd_parent = GetDesktopWindow();
	GetWindowRect(hwnd_parent, &rw_parent);
	GetClientRect(hwnd_parent, &rc_parent);
	GetWindowRect(hwnd_self, &rw_self);
	xpos = rw_parent.left + (rc_parent.right + rw_self.left - rw_self.right) / 2;
	ypos = rw_parent.top + (rc_parent.bottom + rw_self.top - rw_self.bottom) / 2;
	SetWindowPos(hwnd_self, NULL,xpos, ypos, 0, 0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
}

void ShowDialog(HWND hwnd,int mode)
{
	char filename[1024];
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "txt";

	if(mode == 0)
	{
		if(GetOpenFileName(&ofn))
		ReadTextFile(filename);
	}
	else if(mode == 1)
	{
		if(GetSaveFileName(&ofn))
		WriteTxtFile(filename);
	}
	else
	return;

	ZeroMemory(&ofn, sizeof(ofn));
}

//function of reading file; path - full file name
void ReadTextFile(char* path)
{
	FILE *file;
	file = fopen(path,"rb");
	fseek(file,0,SEEK_END);
	int _fsize = ftell(file);
	rewind(file);
	char* data[_fsize + 1];
	fread(data,_fsize,1,file);
	//data[_fsize] = "\0";
	SetWindowText(hwndEdit, data);
}

//function of writing file
void WriteTxtFile(char *filename)
{
	int textsize =  GetWindowTextLength(hwndEdit); //take text symbols (size of text)
	char txt[textsize+1];
	GetWindowText(hwndEdit, txt, textsize+1);

	/* Clean code on C
	FILE *fileforsave;
	fileforsave=fopen(filename,"w");
	fwrite(txt, textsize+1,1,fileforsave);
	fclose(fileforsave);
	*/

	//Code on WinAPI
	HANDLE cf = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, 0);
	DWORD dwBytesWritten = 0;
	WriteFile(cf, txt, (DWORD)textsize+1, &dwBytesWritten, NULL);
	CloseHandle(cf);

	char str1[1024];
	sprintf(str1,"File saved in %s",filename);
	MessageBox(NULL,str1,"Notepad",MB_OK | MB_ICONINFORMATION);
}

//create main menu
void CreateMainMenu(HWND hwnd)
{
	HMENU hMenubar=CreateMenu(); 
	HMENU hMenu = CreateMenu();
	HMENU hFormatMenu = CreateMenu();
	AppendMenuW(hMenu, MF_STRING, IDM_FILE_SAVE,L"&Save");
	AppendMenuW(hMenu, MF_STRING, IDM_FILE_OPEN,L"&Open");

	AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);

	AppendMenuW(hMenu, MF_STRING, IDM_ABOUT,L"&About");
	AppendMenuW(hMenu, MF_STRING, IDM_FILE_QUIT,L"&Quit");
	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenu,L"&File");
	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hFormatMenu,L"&Format");
	AppendMenuW(hFormatMenu, MF_STRING, IDM_CLEAN,L"&Clean");
	SetMenu(hwnd, hMenubar);	
}

/*void CreatePpupMenu(HWND hwnd, LPARAM lParam)
{
	POINT point;
	point.x = LOWORD(lParam);
	point.y = HIWORD(lParam);
	HMENU hMenu = CreatePopupMenu();
	AppendMenuW(hMenu, MF_STRING, IDM_COPY, L"&Copy");
	AppendMenuW(hMenu, MF_STRING, IDM_PASTE, L"&Paste");
	TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);
	DestroyMenu(hMenu);
}*/

LRESULT CALLBACK NotepadWnd(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_CREATE:
			CenterWindow(hwnd);
			CreateMainMenu(hwnd);
			hwndEdit = CreateWindowEx(0, "EDIT",NULL,WS_CHILD | WS_VISIBLE | WS_VSCROLL| WS_HSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,0, 0, 0, 0,hwnd,(HMENU)ID_EDITCHILD,(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),NULL);
			HFONT myhFont=CreateFont(0,8,0,0,0,0,0,0,0,0,0,0,0,TEXT("Arial")); //create font for edit
			SendMessage(hwndEdit,WM_SETFONT,(WPARAM)myhFont,0);
		break;

		case WM_COMMAND: //отвечает за команды
		{
			switch(LOWORD(wParam))
			{
				case IDM_CLEAN:
				SendMessage(hwndEdit, WM_SETTEXT, 0, (LPARAM)"");
				break;
				
				case IDM_ABOUT:
				MessageBox(NULL,"Created by ByteC0d3 (vk.com/bytec0d3)","About app",MB_OK);
				break;
				
				case IDM_FILE_SAVE:
				ShowDialog(hwnd,1);
				break;

				case IDM_FILE_OPEN:
				ShowDialog(hwnd,0);
				break;

				case IDM_FILE_QUIT:
				DestroyWindow(hwnd);
				break;
			}
		}
		break;

		case WM_SIZE:
		//do full-screen editcontrol
			MoveWindow(hwndEdit,0, 0,LOWORD(lParam),HIWORD(lParam),TRUE);
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
		break;

		break;

		case WM_KEYDOWN: //событие вызывается при нажатии клавиш
			if(VK_ESCAPE == wParam) //если мы нажали ESCAPE
				DestroyWindow(hwnd); //закрываем форму
		break;

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rc;
			hdc = BeginPaint(hwnd, &ps);
			GetClientRect(hwnd, &rc);
			EndPaint(hwnd, &ps);
		}
		break;

		/*case WM_RBUTTONUP:
		CreatePpupMenu(hwnd,lParam);
		break;*/

		default:
            	return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInst, LPTSTR lpCmdLine, int nShowCmd) 
{
	MSG msg;
	WNDCLASS wc;
	HWND hwnd;
	FreeConsole();
	ZeroMemory(&wc, sizeof wc);
	wc.hInstance = hInstance; //create thread
	wc.lpszClassName = "CLEANWINDOWSFORM"; //set class name
	wc.lpfnWndProc = (WNDPROC)NotepadWnd; //set callback
	wc.style = CS_DBLCLKS|CS_VREDRAW|CS_HREDRAW; //form style
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //create black background
	wc.hIcon = LoadIcon(NULL,IDI_APPLICATION); //icon app
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); //default cursor
	if (RegisterClass(&wc) == FALSE)
		return 0;
	hwnd = CreateWindow(wc.lpszClassName,APP_NAME,WS_OVERLAPPEDWINDOW|WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,700/*width*/,500/*height*/,0,0,hInstance,0);
	if (hwnd == NULL)
		return 0;
	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}