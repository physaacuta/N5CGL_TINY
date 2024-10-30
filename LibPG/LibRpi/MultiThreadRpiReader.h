#pragma once
namespace RpiLib { class RpiReaderFile; class RpiReader; };
// RPIの読み込み・LAC展開をマルチスレッドで実行する.
// スレッドの構成は下記の通り。
//RunAync
//  Run -> ReadFile -|                      |-> OnRpiFrameDecode(single) ...
//                    -> decode_01(multi) ->
//                    -> decode_02(multi) ->
//                    -> decode_nthread   ->


//展開結果。これはMultiDecode::Runと同じスレッドで実行される。
struct IRpiRueslt {
	virtual void OnRpiOpen(RpiLib::RpiReader *rpi) = 0;

	// 結果がでてきた。
	virtual void OnRpiFrameDecode(int f, BYTE *pImage) = 0;

	//このフレームは展開が要る？ false:skip
	virtual bool IsNeedDecode(int f) = 0;
protected:
	virtual ~IRpiRueslt() {}
};


class RpiMultiDecode
{
	RpiLib::RpiReaderFile *m_rpi;
	IRpiRueslt *m_pResult;

	int m_nDecodeThread;
	HANDLE m_hStop;
	static unsigned int WINAPI rpi_decode(void *arg);
public:
	RpiMultiDecode()
		: m_rpi(NULL)
		, m_pResult(NULL)
		,m_nDecodeThread(0)
		,m_hStop(0)
	{ }
	bool Open(const char *sRpiPath, IRpiRueslt *result);
	bool IsOpen() const { return m_rpi != NULL; }
	RpiLib::RpiReader &rpi() const;
	bool Run(int nthread, HANDLE hEvStop);
	HANDLE RunAsync(int nthread, HANDLE hEvStop);

};
