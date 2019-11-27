// DIBBuffer.cpp : 实现文件
//

#include "stdafx.h"
#include "DIBShow.h"
#include "DIBBuffer.h"
#include "afxdialogex.h"


// DIBBuffer 对话框

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


// DIBBuffer 消息处理程序


BOOL DIBBuffer::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;

	return CDialogEx::OnEraseBkgnd(pDC);
}
