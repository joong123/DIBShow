#pragma once


// DIBBuffer �Ի���

class DIBBuffer : public CDialogEx
{
	DECLARE_DYNAMIC(DIBBuffer)

public:
	DIBBuffer(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DIBBuffer();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIBSHOW_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
