#pragma once
namespace RpiLib { class RpiReaderFile; class RpiReader; };
// RPI�̓ǂݍ��݁ELAC�W�J���}���`�X���b�h�Ŏ��s����.
// �X���b�h�̍\���͉��L�̒ʂ�B
//RunAync
//  Run -> ReadFile -|                      |-> OnRpiFrameDecode(single) ...
//                    -> decode_01(multi) ->
//                    -> decode_02(multi) ->
//                    -> decode_nthread   ->


//�W�J���ʁB�����MultiDecode::Run�Ɠ����X���b�h�Ŏ��s�����B
struct IRpiRueslt {
	virtual void OnRpiOpen(RpiLib::RpiReader *rpi) = 0;

	// ���ʂ��łĂ����B
	virtual void OnRpiFrameDecode(int f, BYTE *pImage) = 0;

	//���̃t���[���͓W�J���v��H false:skip
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
