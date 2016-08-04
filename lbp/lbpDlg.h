
// lbpDlg.h : 头文件
//

#pragma once
#include "afxeditbrowsectrl.h"


// ClbpDlg 对话框
class ClbpDlg : public CDialogEx
{
// 构造
public:
	ClbpDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_LBP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedTrain();
	CMFCEditBrowseCtrl my_filepath;
	afx_msg void OnBnClickedRecognition();
	CMFCEditBrowseCtrl my_picpath;
	afx_msg void OnEnChangeMfceditbrowse2();
	CMFCEditBrowseCtrl my_videopath;
	afx_msg void OnBnClickedCamera();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedVideo();
	afx_msg void OnBnClickedReadRes();
	afx_msg void OnEnChangeMfceditbrowse1();
	afx_msg void OnBnClickedButton2();
};
