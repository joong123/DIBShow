// DIBBuffer.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DIBShow.h"
#include "DIBBuffer.h"
#include "afxdialogex.h"


// DIBBuffer �Ի���

IMPLEMENT_DYNAMIC(DIBBuffer, CDialogEx)

DIBBuffer::DIBBuffer(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIBSHOW_DIALOG, pParent)
{

}

DIBBuffer::~DIBBuffer()
{
}

void DIBBuffer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DIBBuffer, CDialogEx)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// DIBBuffer ��Ϣ�������


BOOL DIBBuffer::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return TRUE;

	return CDialogEx::OnEraseBkgnd(pDC);
}
