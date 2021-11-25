#include "Room3DWebDlg.h"
#include <QWebFrame>
#include <QFileInfo>
#include <QNetworkReply>
#include <QMessageBox>
#include "Common.h"
#include "Room3DApplication.h"

#define DOWNLOAD_PAGE_URL "http://www.myroomdesign.com/"


#define ERROR_NETWORK_CONNECTION_HTML "<html>\
										<head>\
										<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />\
										<title></title>\
										</head>\
										<body>\
										<h1><font color='#003366'> Can not find Server.</font></h1>\
										<hr size='3' width='100%' />\
										<UL>\
										<li>The page you are looking for is currently unavailable.</li>\
										<li>There is a network connection error while downloading homepage.</li>\
										<li>Contact your network administration.</li>\
										</UL>\
										</body>\
										</html>"

CRoom3DProgressDlg::CRoom3DProgressDlg(QWidget* a_pParent) : CRoom3DBaseDlg(a_pParent)
{
	m_pMainLayout = new QVBoxLayout(this);
	m_pLabel = new QLabel(this);
	m_pProgressBar = new QProgressBar(this);
	m_pProgressBar->setRange(0, 100);
	m_pProgressBar->setTextVisible(false);
	m_pMainLayout->addWidget(m_pLabel);
	m_pMainLayout->addWidget(m_pProgressBar);
	setFixedSize(QSize(320, 80));
	setWindowModality(Qt::WindowModal);
}
CRoom3DProgressDlg::~CRoom3DProgressDlg(void)
{
	SAFE_DELETE(m_pMainLayout);
	SAFE_DELETE(m_pLabel);
	SAFE_DELETE(m_pProgressBar);
}


CRoom3DWebDlg::CRoom3DWebDlg(QWidget* a_pParent) : CRoom3DBaseDlg(a_pParent)
{
	m_pWebView = new QWebView(this);
	m_pPage = new QWebPage(m_pWebView);
	m_pProgressDlg = new CRoom3DProgressDlg(this);

	m_pWebView->setPage(m_pPage);
	m_pPage->setView(m_pWebView);
	m_pPage->setForwardUnsupportedContent(true);//It's compulsory.

	connect(m_pPage, SIGNAL(loadFinished(bool)), this, SLOT(LoadFinished(bool)));
	connect(m_pPage, SIGNAL(unsupportedContent(QNetworkReply*)), this, SLOT(handleFileDownload(QNetworkReply*)));

	setWindowModality(Qt::WindowModal);
	this->setMinimumSize(QSize(800, 600));

	m_pPage->mainFrame()->load(QUrl(DOWNLOAD_PAGE_URL));
}

CRoom3DWebDlg::~CRoom3DWebDlg(void)
{
	SAFE_DELETE(m_pPage);
	SAFE_DELETE(m_pWebView);

}

void CRoom3DWebDlg::LoadFinished(bool ok)
{
	int nByte = m_pPage->totalBytes();
	if(ok == false)
	{
		if(nByte == 0)
		{
			m_pWebView->setHtml(ERROR_NETWORK_CONNECTION_HTML);
		}
	}
}

void CRoom3DWebDlg::handleFileDownload(QNetworkReply *reply)
{
	QUrl url=reply->url();
	QFileInfo fileInfo=url.path();
	QString sURL = url.toString();

	int ret = QMessageBox::question(this, tr("Download"), sURL, QMessageBox::Ok, QMessageBox::Cancel);

	if(ret == QMessageBox::Ok)
	{
		CRoom3DItemDownloadMgr* pDownLoadMgr = g_pApp->GetDocument()->GetItemMgr()->GetItemDownloadMgr();
		pDownLoadMgr->SetListner(this);
		pDownLoadMgr->RequestDownloadItemDatas(sURL);

		if(m_pProgressDlg)
		{
			QProgressBar* pProgressBar = m_pProgressDlg->GetProgressBar();
			pProgressBar->reset();
			QLabel* pLabel = m_pProgressDlg->GetLabel();
			pLabel->setText(tr("Downloading 0.0%"));

			m_pProgressDlg->exec();
		}
	}
}

void CRoom3DWebDlg::OnRequestFinish()
{
	if(m_pProgressDlg)
		m_pProgressDlg->accept();

	accept();
}

void CRoom3DWebDlg::SetPercent(float fPercent)
{
	if(m_pProgressDlg)
	{
		QProgressBar* pProgressBar = m_pProgressDlg->GetProgressBar();
		pProgressBar->setValue((int)fPercent);
		QLabel* pLabel = m_pProgressDlg->GetLabel();
		QString strPercent;
		strPercent.setNum(fPercent, 'f', 1);
		QString strLabel = tr("Downloading ");
		strLabel += strPercent;
		strLabel += "%";
		pLabel->setText(strLabel);
	}
}

void CRoom3DWebDlg::resizeEvent(QResizeEvent *event)
{
	QSize size = this->size();
	m_pWebView->resize(size);
}