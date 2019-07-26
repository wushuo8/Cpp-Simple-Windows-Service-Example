
// ServiceExampleDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"


// CServiceExampleDlg 对话框
class CServiceExampleDlg : public CDialogEx
{
// 构造
public:
	CServiceExampleDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SERVICEEXAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	bool InitEnumSrv(void);
	CListCtrl m_list;
	CToolBar m_toolbar;
	CImageList m_toolbarImg;
public:
	afx_msg void OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnStartService();
	afx_msg void OnPauseService();
	afx_msg void OnStopService();
	afx_msg void OnRefreshAll();
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
};
