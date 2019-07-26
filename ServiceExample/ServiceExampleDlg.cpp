
// ServiceExampleDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ServiceExample.h"
#include "ServiceExampleDlg.h"
#include "afxdialogex.h"
#include <Winsvc.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CServiceExampleDlg 对话框



CServiceExampleDlg::CServiceExampleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServiceExampleDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServiceExampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}

BEGIN_MESSAGE_MAP(CServiceExampleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_COMMAND(ID_MENU_START,OnStartService)
	ON_COMMAND(ID_MENU_PAUSE,OnPauseService)
	ON_COMMAND(ID_MENU_STOP,OnStopService)
	ON_COMMAND(ID_MENU_REFRESHALL,OnRefreshAll)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, &CServiceExampleDlg::OnLvnColumnclickList1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CServiceExampleDlg::OnNMRClickList1)
END_MESSAGE_MAP()


// CServiceExampleDlg 消息处理程序

BOOL CServiceExampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_list.InsertColumn(0,L"名称",0,100);
	m_list.InsertColumn(1,L"服务名称",0,100);
	m_list.InsertColumn(2,L"描述",0,100);
	m_list.InsertColumn(3,L"状态",0,100);
	m_list.InsertColumn(4,L"服务类型",0,100);
	m_list.InsertColumn(5,L"登录为",0,100);
	m_list.InsertColumn(6,L"启动路径",0,100);
	m_list.InsertColumn(7,L"启动方式",0,100);

	CBitmap bitmap;
	m_toolbarImg.Create(24,24,ILC_COLOR24|ILC_MASK,1,1);
	for (int i = 0; i < 4; i++)
	{
		bitmap.LoadBitmapW(IDB_BITMAP1+i);
		m_toolbarImg.Add(&bitmap,RGB(255,255,255));
		bitmap.DeleteObject();
	}

	const UINT toolbars[] = {ID_MENU_START,ID_MENU_PAUSE,ID_MENU_STOP,ID_MENU_REFRESHALL};
	m_toolbar.CreateEx(this);
	m_toolbar.SetSizes(CSize(60,60),CSize(1,1));
	m_toolbar.SetButtons(toolbars,4);
	m_toolbar.SetButtonText(0,L"Start");
	m_toolbar.SetButtonText(1,L"Pause");
	m_toolbar.SetButtonText(2,L"Stop");
	m_toolbar.SetButtonText(3,L"Refresh");
	m_toolbar.GetToolBarCtrl().SetImageList(&m_toolbarImg);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);
	if(!InitEnumSrv())
		return FALSE;
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CServiceExampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CServiceExampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CServiceExampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


CString GetServiceType(int dwVal)
{
	switch(dwVal)
	{
	case SERVICE_WIN32_OWN_PROCESS:
		return L"SERVICE_WIN32_OWN_PROCESS";
		break;
	case SERVICE_WIN32_SHARE_PROCESS:
		return L"SERVICE_WIN32_SHARE_PROCESS";
		break;
	case SERVICE_KERNEL_DRIVER:
		return L"SERVICE_KERNEL_DRIVER";
		break;
	case SERVICE_FILE_SYSTEM_DRIVER:
		return L"SERVICE_FILE_SYSTEM_DRIVER";
		break;
	case SERVICE_INTERACTIVE_PROCESS:
		return L"SERVICE_INTERACTIVE_PROCESS";
		break;
	default:
		return L"NONE";
		break;
	}
}

CString GetServiceStatus(int dwVal)
{
	switch(dwVal)
	{
	case SERVICE_STOPPED:
		return L"SERVICE_STOPPED";
		break;
	case SERVICE_START_PENDING:
		return L"SERVICE_START_PENDING";
		break;
	case SERVICE_STOP_PENDING:
		return L"SERVICE_STOP_PENDING";
		break;
	case SERVICE_RUNNING:
		return L"SERVICE_RUNNING";
		break;
	case SERVICE_CONTINUE_PENDING:
		return L"SERVICE_CONTINUE_PENDING";
		break;
	case SERVICE_PAUSE_PENDING:
		return L"SERVICE_PAUSE_PENDING";
		break;
	case SERVICE_PAUSED:
		return L"SERVICE_PAUSED";
		break;
	default:
		return L"NONE";
		break;
	}
}

void CServiceExampleDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CListCtrl* pLst = (CListCtrl*)GetDlgItem(IDC_LIST1);
	if(pLst)
	{
		CRect LstCurRect,WinRect;
		pLst->GetWindowRect(&LstCurRect);
		ScreenToClient(&LstCurRect);
		GetWindowRect(&WinRect);
		ScreenToClient(&WinRect);
		LstCurRect.right = WinRect.right - LstCurRect.left-10;
		LstCurRect.bottom = WinRect.bottom -20;
		pLst->MoveWindow(&LstCurRect);
	}
}


bool CServiceExampleDlg::InitEnumSrv(void)
{
	
	SC_HANDLE hSrv = OpenSCManager(NULL,NULL,SC_MANAGER_ENUMERATE_SERVICE);
	if(NULL == hSrv)
	{
		CString tmp;
		tmp.Format(L"打开服务控制器失败!错误代码:%d",GetLastError());
		MessageBox(tmp,L"Noties",MB_OK|MB_ICONEXCLAMATION|MB_ICONWARNING);
		return FALSE;
	}
	ENUM_SERVICE_STATUS ess[1024];
	LPQUERY_SERVICE_CONFIG qsc = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR,8*1024);

	DWORD dwReceivesSize,dwReceivesNum,dwSrvPt=0;
	CString strft;
	wchar_t buf[1024]={0};
	int s = sizeof(SERVICE_STATUS);
	int result = EnumServicesStatus(hSrv,SERVICE_WIN32,SERVICE_STATE_ALL,ess,sizeof(ENUM_SERVICE_STATUS)*1024,&dwReceivesSize,&dwReceivesNum,&dwSrvPt);
	
	if(result)
	{
		SC_HANDLE sTmp =NULL;
		for (int i = 0; i < dwReceivesNum; i++)
		{
			m_list.InsertItem(0,ess[i].lpDisplayName);
			m_list.SetItemText(0,1,ess[i].lpServiceName);
			m_list.SetItemText(0,4,GetServiceType(ess[i].ServiceStatus.dwServiceType));
			m_list.SetItemText(0,3,GetServiceStatus(ess[i].ServiceStatus.dwCurrentState));
			sTmp = NULL;
			sTmp = OpenService(hSrv,ess[i].lpServiceName,SERVICE_ALL_ACCESS);
			if(sTmp)
			{
				dwReceivesSize=0;
				if(QueryServiceConfig(sTmp,qsc, 8*1024,&dwReceivesSize))
				{
					m_list.SetItemText(0,6,qsc->lpBinaryPathName);
					if(2 == qsc->dwStartType)
						strft = L"自动";
					else if(3 == qsc->dwStartType)
						strft = L"手动";
					else if(4 == qsc->dwStartType)
						strft = L"禁止";
					m_list.SetItemText(0,7,strft);
					
					dwReceivesSize=0;
					memset(buf,0,sizeof(buf));
					if(QueryServiceConfig2(sTmp,SERVICE_CONFIG_DESCRIPTION,(BYTE*)buf,sizeof(buf),&dwReceivesSize))
					{
						strft.Format(L"%s",buf);
						//MessageBox(strft.Right(strft.GetLength()-2));
						m_list.SetItemText(0,2,strft.Right(strft.GetLength()-2));

					}
				}
				else
				{
					int err = GetLastError();
					strft.Format(L"获取服务信息失败,错误代码:[%d]",err);
					MessageBox(strft);
					return FALSE;
				}
			}
			CloseServiceHandle(sTmp);
		}
		return TRUE;
	}
	else
		CloseServiceHandle(hSrv);
	return FALSE;

	
}


void CServiceExampleDlg::OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CServiceExampleDlg::OnStartService()
{

	POSITION pts = m_list.GetFirstSelectedItemPosition();
	int CurNum=0;
	if(!pts)
	{
		MessageBox(L"Its not selected a line");
		return;
	}

	CurNum = (int)pts-1;
	
	CString Tmp;
	wchar_t buf[256]={0};
	m_list.GetItemText(CurNum,1,buf,256);

	SC_HANDLE sch;
	sch = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(!sch)
	{
		int err;
		err = GetLastError();
		Tmp.Format(L"OpenSCManager Err code:%d",err);
		MessageBox(Tmp);
		return;
	}
	SC_HANDLE scml = OpenService(sch,buf,SERVICE_ALL_ACCESS);
	if(scml == NULL)
	{
		int err;
		err = GetLastError();
		Tmp.Format(L"OpenService Err code:%d",err);
		MessageBox(Tmp);
		return;
	}

	SERVICE_STATUS ss;
	if(!QueryServiceStatus(scml,&ss))
	{
		int err;
		err = GetLastError();
		Tmp.Format(L"QueryServiceStatus Err code:%d",err);
		MessageBox(Tmp);
		return;
	}
	
	if(ss.dwCurrentState ==SERVICE_STOPPED)
	{
		if(!StartService(scml,0,NULL))
		{
			int err;
			err = GetLastError();
			Tmp.Format(L"SetServiceStatus Err code:%d",err);
			MessageBox(Tmp);
			return;
		}
		m_list.SetItemText(CurNum,3,L"SERVICE_RUNNING");
		MessageBox(L"已启动服务！");

	}
	else if(ss.dwCurrentState == SERVICE_PAUSED)
	{
		if(!ControlService(scml,SERVICE_CONTROL_CONTINUE,&ss))
		{
			int err;
			err = GetLastError();
			Tmp.Format(L"SetServiceStatus Err code:%d",err);
			MessageBox(Tmp);
			return;
		}
		m_list.SetItemText(CurNum,3,L"SERVICE_RUNNING");
		MessageBox(L"已恢复服务！");
	}
	return;
}

void CServiceExampleDlg::OnPauseService()
{
	POSITION pts = m_list.GetFirstSelectedItemPosition();
	int CurNum=0;
	if(!pts)
	{
		MessageBox(L"Its not selected a line");
		return;
	}

	CurNum = (int)pts-1;
	
	CString Tmp;
	wchar_t buf[256]={0};
	m_list.GetItemText(CurNum,1,buf,256);

	SC_HANDLE sch;
	sch = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(!sch)
	{
		int err;
		err = GetLastError();
		Tmp.Format(L"OpenSCManager Err code:%d",err);
		MessageBox(Tmp);
		return;
	}
	SC_HANDLE scml = OpenService(sch,buf,SERVICE_ALL_ACCESS);
	if(scml == NULL)
	{
		int err;
		err = GetLastError();
		Tmp.Format(L"OpenService Err code:%d",err);
		MessageBox(Tmp);
		return;
	}

	SERVICE_STATUS ss;
	if(!QueryServiceStatus(scml,&ss))
	{
		int err;
		err = GetLastError();
		Tmp.Format(L"QueryServiceStatus Err code:%d",err);
		MessageBox(Tmp);
		return;
	}
	
	if(ss.dwCurrentState ==SERVICE_RUNNING)
	{
		if(!ControlService(scml,SERVICE_CONTROL_PAUSE,&ss))
		{
			int err;
			err = GetLastError();
			Tmp.Format(L"SetServiceStatus Err code:%d",err);
			MessageBox(Tmp);
			return;
		}
		m_list.SetItemText(CurNum,3,L"SERVICE_PAUSE");
		MessageBox(L"已暂停服务！");

	}
}

void CServiceExampleDlg::OnStopService()
{
	POSITION pts = m_list.GetFirstSelectedItemPosition();
	int CurNum=0;
	if(!pts)
	{
		MessageBox(L"Its not selected a line");
		return;
	}

	CurNum = (int)pts-1;
	
	CString Tmp;
	wchar_t buf[256]={0};
	m_list.GetItemText(CurNum,1,buf,256);

	SC_HANDLE sch;
	sch = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(!sch)
	{
		int err;
		err = GetLastError();
		Tmp.Format(L"OpenSCManager Err code:%d",err);
		MessageBox(Tmp);
		return;
	}
	SC_HANDLE scml = OpenService(sch,buf,SERVICE_ALL_ACCESS);
	if(scml == NULL)
	{
		int err;
		err = GetLastError();
		Tmp.Format(L"OpenService Err code:%d",err);
		MessageBox(Tmp);
		return;
	}

	SERVICE_STATUS ss;
	if(!QueryServiceStatus(scml,&ss))
	{
		int err;
		err = GetLastError();
		Tmp.Format(L"QueryServiceStatus Err code:%d",err);
		MessageBox(Tmp);
		return;
	}
	
	if(ss.dwCurrentState ==SERVICE_RUNNING || ss.dwCurrentState == SERVICE_PAUSED)
	{
		if(!ControlService(scml,SERVICE_CONTROL_STOP,&ss))
		{
			int err;
			err = GetLastError();
			Tmp.Format(L"SetServiceStatus Err code:%d",err);
			MessageBox(Tmp);
			return;
		}
		m_list.SetItemText(CurNum,3,L"SERVICE_STOPED");
		MessageBox(L"已停止服务！");

	}
}


void CServiceExampleDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	CMenu menu,*pMenu=NULL;
	menu.LoadMenuW(IDR_MENU1);
	pMenu = menu.GetSubMenu(0);

	POINT pt;
	GetCursorPos(&pt);

	pMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
}


void CServiceExampleDlg::OnRefreshAll()
{
	m_list.DeleteAllItems();
	if(!InitEnumSrv())
		MessageBox(L"刷新服务状态失败");
}