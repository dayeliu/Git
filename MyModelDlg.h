
// MyModelDlg.h : 標頭檔
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "Include/Log.h"
#include "Include/Device.h"
#include "Include/Strings.h"
#include "Include/TrayIcon.h"

#define WM_TRAY		WM_USER+0
#define TimeOut		5000

// CMyModelDlg 對話方塊
class CMyModelDlg : public CDialogEx,
					private Device,
					private Log
{
// 建構
public:
	CMyModelDlg(CWnd* pParent = NULL);	// 標準建構函式

// 對話方塊資料
	enum { IDD = IDD_MYMODEL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();					// initial
	virtual void OnCancel();						// final function
	virtual BOOL PreTranslateMessage(MSG* pMsg);	// acerlerator
	afx_msg void OnPaint();
	
	// accelerater
	afx_msg void OnAcceleratorEnter();
	afx_msg void OnAcceleratorTab();
	afx_msg void OnAcceleratorDown();
	afx_msg void OnAcceleratorUp();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnClick();
	afx_msg void OnTimer(UINT_PTR nIDEvent);		// timer
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
	// tray
	afx_msg LRESULT OnNotifyTray(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTrayOpen();
	afx_msg void OnTrayClose();
	DECLARE_MESSAGE_MAP()

private:
	CEdit			m_Edit_Cmd;
	CRichEditCtrl	m_REdit_Report;
	CButton			m_Btn_Click;
	int				m_LimitTime;
	
	CStringArray				m_Command_List;
	CStringArray				m_Command_Format;
	CStringArray				m_Command_History;
	int							m_Command_Index;

	CString			m_Path;
	CString			m_Sys_Path;
	CString			m_Desktop_Path;
	
	// thread 
	BOOL			m_Is_Closed;				// to close application
	HANDLE			m_Handle;

	// accerlerator
	HACCEL			m_hAccel;

	// tray app
	bool						m_Is_Hide;
	CTrayIcon					m_Icon;

	// static
	static CMyModelDlg*			p_Dlg;


private:
	void				Report(CString data);	
	bool				Send_Command(CString Cmd);

	static UINT			Run_Loop(LPVOID pParam) { return ((CMyModelDlg *)pParam)->Run_Callback();}	
	UINT				Run_Callback();

	static void			Response_Callback_Loop(char* pBuffer, UINT Size) { p_Dlg->Response_Callback(pBuffer, Size);}	
	void				Response_Callback(char *pBuffer, UINT Size);


	// create 2 thread to get files and folders.
	static UINT				File_Loop(LPVOID param) { return ((CMyModelDlg*)param)->File(); };
	UINT					File();
	static UINT				Forder_Loop(LPVOID param) { return ((CMyModelDlg*)param)->Forder(); };
	UINT					Forder();
	BOOL					Implement(CString fileroute);

};
