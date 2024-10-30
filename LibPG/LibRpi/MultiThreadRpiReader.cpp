#include "stdafx.h"
#include "ipp.h"

#include "rpi.h"
#include "RpiReaderFile.h"
#include "../lac/LacManager.h"
#include "MultiThreadRpiReader.h"

#include "../../Library/KizuLib/Class/AutoLock.h"
void ReverseBitmap(BYTE *pImg, const int w, const int h)
{
	IppiSize roi = { w,h };
	ippiMirror_8u_C1IR(pImg, w, roi, ippAxsHorizontal);
}


class LacDecoderFrame
{
	LacManager lacdec;
public:
	int mfn;
	int mw, mh;

	int insize;
	BYTE *pInImg;

	int bmpsize;
	BYTE *pBmp;
	BYTE *pBmpImage;

	void(*OnComplete)(void *, LacDecoderFrame *);
	void *objectOnComplete;

	LacDecoderFrame(int w, int h)
	{
		mw = w;
		mh = h;
		bmpsize = mw * mh + 4096;
		pBmp = (BYTE*)VirtualAlloc(0, bmpsize, MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE);

		pInImg = (BYTE*)VirtualAlloc(0, mw*mh, MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE);

	}
	~LacDecoderFrame()
	{
		if (pBmp) {
			VirtualFree(pBmp, 0, MEM_RELEASE);
			pBmp = NULL;
		}
		if (pInImg) {
			VirtualFree(pInImg, 0, MEM_RELEASE);
			pInImg = NULL;
		}
	}

	static DWORD CALLBACK Run(
		LPVOID                 Context
	) {
		((LacDecoderFrame*)Context)->doDeocde();
		return 0;
	}

	int doDeocde()
	{
		int nWritten = 0;
		lacdec.LacDecode(insize, &pInImg[RpiLib::RPIFrameSizeInfo::LINK_SIZE_Cf], bmpsize, pBmp, &nWritten);

		pBmpImage = &pBmp[((BITMAPFILEHEADER*)pBmp)->bfOffBits];
		ReverseBitmap(pBmpImage, mw, mh);

		OnComplete(objectOnComplete, this);
		return 0;
	}

};

class MultiLacDecoderImpl
{
	mutable CRITICAL_SECTION m;

	std::vector< LacDecoderFrame * >::iterator FindFrameInCompl(int f)
	{
		std::vector< LacDecoderFrame * >::iterator it;
		for (it = comp_frames.begin(); it != comp_frames.end(); ++it) {
			if ((*it)->mfn == f) {
				break;
			}
		}
		return it;

	}

public:
	MultiLacDecoderImpl()
	{
		InitializeCriticalSection(&m);
	}
	~MultiLacDecoderImpl()
	{
		Dispose();
		DeleteCriticalSection(&m);
	}


	int qsize;
	std::vector< LacDecoderFrame *> free_frames;
	std::vector< LacDecoderFrame *> comp_frames;
	std::deque< int > m_queue_frames;

	bool IsAllComplete()const
	{
		KizuLib::AutoLock lock(&m);
		return free_frames.size() == qsize;
	}


	int Init(int w, int h, int q)
	{
		qsize = q;
		for (int i = 0; i < q; i++) {
			free_frames.push_back(new LacDecoderFrame(w,h));
		}
		return 0;
	}

	void Dispose()
	{
		KizuLib::AutoLock lock(&m);
		for (int ii = 0; ii < (int)free_frames.size(); ii++) {
			delete free_frames[ii];
		}
		free_frames.clear();

		//理屈上、Disposeするときは、全部Freeにあってほしい。
	}

	LacDecoderFrame *PeekFree()
	{
		KizuLib::AutoLock lock(&m);
		if (free_frames.empty()) return NULL;
		return free_frames.back();
	}

	LacDecoderFrame *GetComplete()
	{
		KizuLib::AutoLock lock(&m);
		if (comp_frames.empty()) return NULL;

		//IN/Outの順序を守る。
		std::vector< LacDecoderFrame * >::iterator it = FindFrameInCompl(m_queue_frames[0] );
		if (it != comp_frames.end()) {
			LacDecoderFrame *f = *it;

			//SingleThreadモデルであるという前提において、freeに追加してもよい。
			//（＝次のPeekFreeまでに、このメモリを使うコードは終わる。）
			free_frames.push_back(f);
			comp_frames.erase(it);
			m_queue_frames.pop_front();
			return f;
		}
		return NULL;
	}



	void decode(int frame)
	{
		KizuLib::AutoLock lock(&m);

		LacDecoderFrame  *f = free_frames.back();
		f->mfn = frame;
		f->OnComplete = OnCompleteDecode;
		f->objectOnComplete = this;
		m_queue_frames.push_back(frame);
		QueueUserWorkItem(LacDecoderFrame::Run, f, WT_EXECUTELONGFUNCTION);
		free_frames.pop_back();
	}

	static void OnCompleteDecode(void *o, LacDecoderFrame*f) {
		MultiLacDecoderImpl *that = (MultiLacDecoderImpl*)o;
		KizuLib::AutoLock lock(&that->m);

		that->comp_frames.push_back( f);

	}

};

bool RpiMultiDecode::Open(const char *sRpiPath, IRpiRueslt *result)
{
	m_pResult = result;
	m_rpi = new RpiLib::RpiReaderFile();
	return m_rpi->Open(sRpiPath);
}

#define STOP_READING(x)  { \
					delete fReader; fReader = NULL;\
					bRet  = x;\
					continue;\
				}


bool RpiMultiDecode::Run(int nthread, HANDLE hEvStop)
{
	bool bRet = false;
	MultiLacDecoderImpl mr;
	mr.Init(m_rpi->Header.width, m_rpi->Header.height, nthread);
	RpiLib::IRpiFrameEnum *fReader = m_rpi->CreateSequencialReader();
	m_pResult->OnRpiOpen(m_rpi);
	while (1)
	{
		//終わったかな？
		if (LacDecoderFrame *c = mr.GetComplete())
		{
			m_pResult->OnRpiFrameDecode(c->mfn, c->pBmpImage);
		}
		if (fReader) {
			if (hEvStop) {
				if (WAIT_OBJECT_0 == WaitForSingleObject(hEvStop,0))
				{
					// 終了要求された。けど、
					//非同期展開を待つ必要があるのでここでReturnしてはダメ。
					STOP_READING( false );
					hEvStop = NULL;
				}
			}
		}
		if (fReader) {
			LacDecoderFrame *f = mr.PeekFree();
			if (!f) {
				Sleep(10);
				continue;
			}

			if (!m_pResult->IsNeedDecode(fReader->nCurrentFrame + 1)) {
				if( !fReader->SkipNext() ){
					STOP_READING( true );
				}
				continue;
			}

			//実際によむ。
			fReader->SetReadDest(f->pInImg, f->bmpsize);
			if (!fReader->ReadNext()) {
				STOP_READING( true );
			}

			f->insize = fReader->szGetImg();
			//decode threadに投げる。
			mr.decode(fReader->nCurrentFrame);
		}
		else {
			//全部のRPIを読み終わっても、非同期展開がまだかも。
			if (mr.IsAllComplete())
				break;
			else
				Sleep(10);

		}
	}
	return bRet;
}

unsigned int WINAPI RpiMultiDecode::rpi_decode(void *arg)
{
	RpiMultiDecode *that = static_cast<RpiMultiDecode *>(arg);
	if (that->m_nDecodeThread == 0) {
		SYSTEM_INFO si = {};
		GetSystemInfo(&si);

		if (si.dwNumberOfProcessors > 16) that->m_nDecodeThread = 8;
		else  if (si.dwNumberOfProcessors < 4)  that->m_nDecodeThread = si.dwNumberOfProcessors;
		else that->m_nDecodeThread = si.dwNumberOfProcessors/2;
	}



	that->Run(that->m_nDecodeThread, that->m_hStop);
	return 0;
}



HANDLE RpiMultiDecode::RunAsync(int nthread, HANDLE hEvStop)
{
	m_hStop = hEvStop;
	m_nDecodeThread = nthread;
	unsigned int  dwThreadId = 0;
	HANDLE m_hThread= (HANDLE)_beginthreadex(NULL, 0, &rpi_decode, this, 0, &dwThreadId);

	return m_hThread;
}


RpiLib::RpiReader &RpiMultiDecode::rpi() const {
	return *m_rpi;
}


