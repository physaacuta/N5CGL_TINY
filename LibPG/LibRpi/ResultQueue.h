#pragma once
class RpiResultQueue :  public IRpiRueslt {

	mutable CRITICAL_SECTION crs;
	std::vector<BYTE*> m_empty_frames;
	std::deque<BYTE*> m_filled_frames;
	const int m_qsize;
public:
	int framesize;
	RpiResultQueue (int qsize=8) : m_qsize(qsize){
		InitializeCriticalSection( &crs);
	}
	~RpiResultQueue() {
		DeleteCriticalSection(&crs);
	}
    

	virtual void OnRpiOpen(RpiLib::RpiReader *rpi);

	//このフレームは展開が要る？ false:skip
	virtual bool IsNeedDecode(int f) { return true;}


	BYTE *GetFreeQueue();
	BYTE *GetFillQueue();
	void ReturnFree(BYTE *p);

	// 結果がでてきた。
	virtual void OnRpiFrameDecode(int f, BYTE *pImage);
};