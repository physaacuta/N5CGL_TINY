#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X

#include "..\..\Include\KizuSmyu.h"										// GOT�C���N���[�h
#include "..\..\Include\ToTbl.h"										// �������L�������p�w�b�_�[
#include "..\..\Include\OutInterface.h"									// �O��IF�p�w�b�_�[

#include "PlgControlManager.h"



class PioControlManager :	public ThreadManager
{
private:
	static const int	TIMER_CYCLE = 50;					// �`�F�b�N����
//// ���J�֐�
public:
	PioControlManager(void);
	virtual ~PioControlManager(void);


	//=========================================
	// ��ʌ��J�p
	int	GetMaxBood() const { return m_nMaxBood;}
	char const* GetSmyuName(int nIndex) { return m_sSmyuName[nIndex];} 
	char const* GetBitName(int bood, int port, int bit, int pio) const { return m_cBitName[bood][port][bit][pio]; }


#ifndef SEQ_PIO
	void SetValPi(int bood, int port, BYTE onoff) { mtbl_pSmyu[bood]->nDi[port] = onoff; }
	void SetValPi(int bood, int port, int bit, bool onoff) { if (onoff) { mtbl_pSmyu[bood]->nDi[port] |= GetIndexToBit(bit); } else { mtbl_pSmyu[bood]->nDi[port] &= ~GetIndexToBit(bit); } }
	void SetValPo(int bood, int port, int bit, bool onoff) { if (onoff) { mtbl_pSmyu[bood]->nDo[port] |= GetIndexToBit(bit); } else { mtbl_pSmyu[bood]->nDo[port] &= ~GetIndexToBit(bit); } }
	bool GetValPi(int bood, int port, int bit) const { return (mtbl_pSmyu[bood]->nDi[port] & GetIndexToBit(bit)) ? true : false; }
	bool GetValPo(int bood, int port, int bit) const { return (mtbl_pSmyu[bood]->nDo[port] & GetIndexToBit(bit)) ? true : false; }
	//BYTE GetValPo(int bood, int port) const			 { return mtbl_pSmyu[bood]->nDo[port]; }
#else
	void SetValPi(int bood, int port, BYTE onoff);
	void SetValPi(int bood, int port, int bit, bool onoff);
	void SetValPo(int bood, int port, int bit, bool onoff);
	bool GetValPi(int bood, int port, int bit) const;
	bool GetValPo(int bood, int port, int bit) const;
	//BYTE GetValPo(int bood, int port) const			 { return mtbl_pSmyu[bood]->nDo[port]; }
#endif
	

	void SetDebug_Pi(bool flg)	{ m_bDegub[0] = flg; }
	void SetDebug_Po(bool flg)	{ m_bDegub[1] = flg; }
	bool GetDebug_Pi() const	{ return m_bDegub[0]; }
	bool GetDebug_Po() const	{ return m_bDegub[1]; }

	void SetPlgCls(PlgControlManager* pCls) { m_clsPlgCMgr = pCls; };

//// �����o�[�֐�
private:
	//int ThreadFirst() {return 0;};										// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	//int ThreadLast() {return 0;};										// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
 //   void ThreadEvent(int nEventNo) {return;};							// �X���b�h�C�x���g����
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
	void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����	

	//=========================================
	// ����
	void GetIniParam();													// Ini�t�@�C���擾
	static int	GetBitToIndex(BYTE addr);								// �w��ʒu�r�b�g �� No �ɕϊ�
	static BYTE GetIndexToBit(int index);								// �w��ʒuNo �� �r�b�g �ɕϊ�
#ifdef SEQ_PIO
	void SetSeqToSmyu(int bood, int port, int bit, bool onoff, int Id) {
		int nWk = m_cBitAddr[bood][port][bit][Id];
		if (-1 == nWk) return;
		mtbl_pSeq->fb_dm[Id].Data[nWk] = onoff ? 1 : 0;
		if(onoff && 0 == Id)WpdCheck(nWk);
	}
	void SetSeqToSmyu(int bood, int port, BYTE onoff, int Id) {
		for (int ii = 0; ii < 8; ii++) {
			int nWk = m_cBitAddr[bood][port][ii][Id];
			if (-1 == nWk) continue;
			mtbl_pSeq->fb_dm[Id].Data[nWk] = (onoff &  (1 << ii)) != 0;
			if ((onoff &  (1 << ii)) != 0 && 0 == Id)WpdCheck(nWk);
		}
	}
	void WpdCheck(int nId) {
		if (-1 != m_nIdPioKen && nId == m_nIdPioKen) m_clsPlgCMgr->Set_WpdKen();
		if (-1 != m_nIdPioSub && nId == m_nIdPioSub) m_clsPlgCMgr->Set_WpdSub();
		if (-1 != m_nIdPioDsp && nId == m_nIdPioDsp) m_clsPlgCMgr->Set_WpdDsp();
	}
	void ResetCheck() {
		if (mtbl_pSeq->fb_dm[DM_SEND_REC].Data[m_nIdPioReset] == m_nStPioReset) return;

		if (1 == mtbl_pSeq->fb_dm[DM_SEND_REC].Data[m_nIdPioReset] && 1 != m_nStPioReset) m_clsPlgCMgr->Set_ResetSignal();
		m_nStPioReset = mtbl_pSeq->fb_dm[DM_SEND_REC].Data[m_nIdPioReset];
	}
#endif

//// �����o�[�ϐ�
private:

	//=========================================
	HANDLE					m_evTimerCycle;								// �`�F�b�N�����C�x���g
	//=========================================
	// ��������
	//CRITICAL_SECTION		m_csLock;									// �N���e�B�J���Z�N�V����



	//=========================================
	// ���̑��������
	bool					m_bDegub[2];								// �f�o�b�N�� true [0:PI 1:PO]

	//=========================================
	// ���L������
	int						m_nMaxBood;									// �L����
	CString					m_sSmyuName[SMYU_PIO_MAX_BOOD];				// PIO��� ����
	HANDLE					m_hTblSmyu[SMYU_PIO_MAX_BOOD];				// PIO��� �n���h��
	SMYU_TBL_PIO*			mtbl_pSmyu[SMYU_PIO_MAX_BOOD];				// PIO��� �f�[�^

	char					m_cBitName[SMYU_PIO_MAX_BOOD][SMYU_PIO_MAX_PORT][8][2][32];	// �X�̃{�^������
	int						m_cBitAddr[SMYU_PIO_MAX_BOOD][SMYU_PIO_MAX_PORT][8][2];		// �X�̃{�^���ɕR�Â��A�h���X

	CString					m_sSeqName;									// �V�[�P���T��� ����
	HANDLE					m_hTblSeq;									// �V�[�P���T��� �n���h��
	TO_SEQMEM_TBL*			mtbl_pSeq;									// �V�[�P���T��� �f�[�^

	PlgControlManager*		m_clsPlgCMgr;

	int						m_nIdPioKen;								// �����pWPD�C���f�b�N�X
	int						m_nIdPioSub;								// ������r�pWPD�C���f�b�N�X
	int						m_nIdPioDsp;								// �\���pWPD�C���f�b�N�X
	int						m_nIdPioReset;								// ���Z�b�g�M��
	bool					m_nStPioReset;								// ���Z�b�g�M���O��l
};
