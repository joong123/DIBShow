#pragma once


// DIBBuffer 对话框

class DIBBuffer : public CDialogEx
{
	DECLARE_DYNAMIC(DIBBuffer)

public:
	DIBBuffer(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DIBBuffer();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIBSHOW_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
