
// MyModelDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MyModel.h"
#include "MyModelDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyModelDlg 對話方塊




CMyModelDlg::CMyModelDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyModelDlg::IDD, pParent)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
}

void CMyModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CMD, m_Edit_Cmd);
	DDX_Control(pDX, IDC_REDIT_Report, m_REdit_Report);
	DDX_Control(pDX, IDC_BTN_Click, m_Btn_Click);
}

BEGIN_MESSAGE_MAP(CMyModelDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_Click, &CMyModelDlg::OnBnClickedBtnClick)
	ON_COMMAND(ID_ACCELERATOR_Enter, &CMyModelDlg::OnAcceleratorEnter)
	ON_COMMAND(ID_ACCELERATOR_Tab, &CMyModelDlg::OnAcceleratorTab)
	ON_COMMAND(ID_ACCELERATOR_Down, &CMyModelDlg::OnAcceleratorDown)
	ON_COMMAND(ID_ACCELERATOR_Up, &CMyModelDlg::OnAcceleratorUp)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_TRAY_OPEN, &CMyModelDlg::OnTrayOpen)
	ON_COMMAND(ID_TRAY_CLOSE, &CMyModelDlg::OnTrayClose)
	ON_MESSAGE(WM_TRAY, &CMyModelDlg::OnNotifyTray)
END_MESSAGE_MAP()


// CMyModelDlg 訊息處理常式
CMyModelDlg* CMyModelDlg::p_Dlg = NULL;
BOOL CMyModelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	// var
	m_Is_Closed	= false;
	m_Is_Hide	= false;
	p_Dlg		= this;
	m_hAccel	= NULL;
	m_Handle	= NULL;
	m_LimitTime = TimeOut;

	// 避免程式重複開啟
	HANDLE Mutex = CreateMutex(NULL, true, "MyModel");	
	if(GetLastError() == ERROR_ALREADY_EXISTS)	//系統中已開啟過就不再啟動本程式
	{
		ReleaseMutex(Mutex);
		SendMessage(WM_CLOSE);
		return FALSE;
	}

	// Accelerator
	m_hAccel	= LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR_Table));

	// 取得目前路徑
	char buf[MAX_PATH];
	m_Path = GetCommandLine(); m_Path.Replace("\"", ""); m_Path = m_Path.Mid(0, m_Path.ReverseFind('\\')+1);
	GetSystemDirectory(buf, MAX_PATH);									m_Sys_Path = CString(buf) + "\\";
	SHGetSpecialFolderPath(GetSafeHwnd(), buf, CSIDL_DESKTOP, true);	m_Desktop_Path = CString(buf) + "\\";

	// 取得指令及參數
	CString Str = GetCommandLine(); Str.MakeUpper();
	if(Str.Find("IS_COMPONENT") != -1)	m_Is_Hide		= TRUE;
	if(Str.Find("IS_RUN_MODE") != -1)	Log::m_Is_Log	= FALSE;

	// icon
	m_Icon.Initial(IDR_MENU1, this, WM_TRAY);

	// log
	Log::Initial(m_Path + "Log", "xxxxx.log");

	// timer
	SetTimer(0, 100, 0);

	// TAB
	m_Command_List.Add("SetComport");		m_Command_Format.Add("SetComport\t\t> Comport");

	// title
	SetWindowText("MyModel");



	// regedit	arg2 : path, arg3 : name, arg4 : value
	CString str;
	Device::GetRegComPort(HKEY_CURRENT_USER, "Software\\USI", "regtmp", str);

	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void CMyModelDlg::OnCancel()
{

	//thread
	m_Is_Closed = TRUE;						// to leave thread
	if (m_Handle) while (WaitForSingleObject(m_Handle, 100) == WAIT_TIMEOUT)	AfxGetThread()->PumpMessage();
	m_Handle = NULL;

	// accelerator end
	if(m_hAccel != NULL) DestroyAcceleratorTable(m_hAccel);
	m_hAccel	= NULL;
	
	// 
	KillTimer(0);

	// var
	p_Dlg = NULL;
	m_Command_List.RemoveAll();
	m_Command_Format.RemoveAll();
	m_Command_History.RemoveAll();

	CDialogEx::OnCancel();
}

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，
// 以便繪製圖示。對於使用文件/檢視模式的 MFC 應用程式，
// 框架會自動完成此作業。

void CMyModelDlg::OnPaint()
{
	if (m_Is_Hide)
	{
		ShowWindow(SW_HIDE);			// 當視窗執行在桌面上，會持續呼叫此函式重建視窗
		OnSize(1, 0, 0);				// 當視窗大小改變，要呼叫此函數
		return;
	}

	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

int CMyModelDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	AfxInitRichEdit2();

	return 0;
}

// 當視窗size改變時，會呼叫此函數
void CMyModelDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (nType == SIZE_MINIMIZED)	// SIZE_MINIMIZED = 1  
	{
		m_Is_Hide = true;
		ShowWindow(SW_HIDE);			// 隱藏視窗
		//m_Icon.SetTrayIcon(IDI_ICON1, "Double Click to Open Tool");
		m_Icon.SetTrayIcon(IDI_ICON2, "Double Click to Open Tool");
	}
}


void CMyModelDlg::OnTrayOpen()
{
	m_Is_Hide = false;
	m_Icon.RemoveTrayIcon();	// 縮小的icon刪除，亦可不刪
	ShowWindow(SW_NORMAL);		// 顯示視窗 SW_NORMALs
	SetForegroundWindow();		// 要顯示視窗最後都必須呼叫此函數
}


void CMyModelDlg::OnTrayClose()
{
	OnCancel();
}


afx_msg LRESULT CMyModelDlg::OnNotifyTray(WPARAM wParam, LPARAM lParam)
{
	if (lParam == WM_RBUTTONUP)
	{
		CMenu Menu;
		CMenu* SubMenu;
		CPoint Mouse;
		Menu.LoadMenu(IDR_MENU1);
		SubMenu = Menu.GetSubMenu(0);
		GetCursorPos(&Mouse);
		SetForegroundWindow();
		SubMenu->TrackPopupMenu(TPM_LEFTALIGN, Mouse.x, Mouse.y, this, NULL);
	}
	else if (lParam == WM_LBUTTONDBLCLK)
	{
		OnTrayOpen();
	}

	return 0;
}

// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CMyModelDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMyModelDlg::Report( CString data )
{
	data += "\r\n";
	
	// report
	m_REdit_Report.SendMessage(EM_SETSEL, m_REdit_Report.GetWindowTextLength(), m_REdit_Report.GetWindowTextLength());		// 移到最後一個字元後面
	m_REdit_Report.ReplaceSel(data);
	m_REdit_Report.SendMessage(WM_VSCROLL, SB_BOTTOM, 0);

	if (Log::m_Is_Log)	Log::Write(data);
}


void CMyModelDlg::OnAcceleratorEnter()
{
	if(!m_Handle)	m_Handle = AfxBeginThread(Run_Loop, this)->m_hThread;
}


void CMyModelDlg::OnAcceleratorTab()
{
	CString Str, Command;
	m_Edit_Cmd.GetWindowText(Command);
	if(Command.IsEmpty())	
	{
		Report("========================================================");
		for(int i = 0;i < m_Command_Format.GetSize();i++) Report(m_Command_Format[i]);
		Report("========================================================");
		m_Edit_Cmd.SetFocus();
		return;
	}

	Report(">" + Command);
	CStringArray List;
	Command.MakeUpper();
	for(int i = 0;i < m_Command_List.GetSize();i++)
	{
		Str = m_Command_List[i]; Str.MakeUpper();
		if(Str.Find(Command) == 0) List.Add(m_Command_List[i]);
	}

	if(List.GetSize() == 1)	m_Edit_Cmd.SetWindowText(List[0]);
	else if(List.GetSize() > 1)
	{		
		Report("========================================================");
		for(int i = 0;i < List.GetSize();i++) Report(List[i]);
		Report("========================================================");
	}
	List.RemoveAll();
	::SendMessage(m_Edit_Cmd.m_hWnd, WM_KEYDOWN, VK_END, 0); // 移到字串最後面
}

void CMyModelDlg::OnAcceleratorDown()
{
	m_Command_Index = max(-1, m_Command_Index-1);
	if((m_Command_Index < 0) || (m_Command_Index >= m_Command_History.GetSize()))	m_Edit_Cmd.SetWindowText("");
	else																			m_Edit_Cmd.SetWindowText(m_Command_History[m_Command_Index]);
	::SendMessage(m_Edit_Cmd.GetSafeHwnd(), WM_KEYDOWN, VK_END, 0);		// 將focus移到command最後面
}


void CMyModelDlg::OnAcceleratorUp()
{
	m_Command_Index = min(m_Command_History.GetSize(), m_Command_Index+1);
	if((m_Command_Index < 0) || (m_Command_Index >= m_Command_History.GetSize()))	m_Edit_Cmd.SetWindowText("");
	else																			m_Edit_Cmd.SetWindowText(m_Command_History[m_Command_Index]);
	::SendMessage(m_Edit_Cmd.GetSafeHwnd(), WM_KEYDOWN, VK_END, 0);		// 將focus移到command最後面
}

BOOL CMyModelDlg::PreTranslateMessage(MSG* pMsg)
{
	if (TranslateAccelerator(AfxGetMainWnd()->GetSafeHwnd(), m_hAccel, pMsg))
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

UINT CMyModelDlg::Run_Callback()
{
	while (!m_Is_Closed)
	{
		Sleep(100);
		Report("kyle");
	}
	Report("End");
	m_Handle = NULL;
	return 10;

	CString Str;
	m_Edit_Cmd.GetWindowText(Str);
	if (!Str.IsEmpty())
	{
		Send_Command(Str);
		m_Command_History.InsertAt(0, Str);
		m_Command_Index = -1;
	}

	m_Edit_Cmd.SetWindowText("");
	m_Edit_Cmd.SetFocus();
	m_Handle = NULL;

	return 100;
}


void CMyModelDlg::OnBnClickedBtnClick()
{
	//if (!m_Handle)	m_Handle = AfxBeginThread(Run_Loop, this)->m_hThread;
}

bool CMyModelDlg::Send_Command(CString Cmd)
{
	Report(Cmd);
	/*
	ZeroMemory(CStressTest::p_Msg, 4096);
	if(!CStressTest::Do_Command(Cmd)) return false;
	if(memcmp(CStressTest::p_Status, "OK", 2) == 0) return true;
	return false;
	*/
	return true;
}

void CMyModelDlg::Response_Callback(char *pBuffer, UINT Size)
{

}

void CMyModelDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 0)
	{
		//Report("123");
	}

	CDialogEx::OnTimer(nIDEvent);
}



//	major topic 
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

UINT CMyModelDlg::File()
{
	// 開啟要轉換成excel的文字檔
	CFileDialog dlg(TRUE, _T("*.log"), "", OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		"log Files (*.log)|*.log|All Files (*.*)| *.*||", NULL);
	if (dlg.DoModal() == IDOK)
	{
		m_Btn_Click.EnableWindow(FALSE);
		Implement(dlg.GetPathName());
	}

	return 100;
}

UINT CMyModelDlg::Forder()
{
	return 100;
}


BOOL CMyModelDlg::Implement(CString fileroute)
{

	// 取得檔案內所有資料，存在data內
 	// Method 1
	/*
	long End = 0;
 	char* data = NULL;
 	FILE* RFile = NULL; RFile = fopen(fileroute, "r");
  	fseek(RFile, 0, SEEK_END);
 	End = ftell(RFile);
 	fseek(RFile, 0, SEEK_SET);
 	data = new char[End+1]; ZeroMemory(data, End+1);
  	fread(data, 1, End, RFile);
 	fclose(RFile);
	*/





	return TRUE;
}
