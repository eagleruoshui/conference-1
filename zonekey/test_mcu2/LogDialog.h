#pragma once


// LogDialog �Ի���

class LogDialog : public CDialog
{
	DECLARE_DYNAMIC(LogDialog)

public:
	LogDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~LogDialog();

// �Ի�������
	enum { IDD = IDD_LOGDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_log;

protected:
	afx_msg LRESULT OnLog(WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL OnInitDialog();
};

void _log(const char *fmt, ...);
