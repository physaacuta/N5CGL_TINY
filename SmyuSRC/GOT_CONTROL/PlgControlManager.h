#pragma once

#include "resource.h"		// ��ʃR���g���[���ɃA�N�Z�X�p
#include <winsock2.h>													// UDP���M�p

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X

#include "..\..\Include\KizuSmyu.h"										// GOT�C���N���[�h
#include "..\..\Include\ToTbl.h"										// �������L�������p�w�b�_�[
#include "..\..\Include\OutInterface.h"									// �O��IF�p�w�b�_�[


class PlgControlManager :	public ThreadManager
{
//// �����o�[�\����
public:

	//=======================================
	// 臒l �\����
	//=======================================
	struct TYP_SETTING {										// �ݒ�f�[�^
		int		nSamplingEnable;										// 1:�ʏ���r��  0:PLG�̂�

		int		nFrameSizeY;											// �c�t���[���� [pixel]

		int		nChNum;													// �L��Ch��
		double	dResY[SMYU_PLG_MAX_CHANNEL];							// �c����\ [mm/pixel]
		double	dPlsY[SMYU_PLG_MAX_CHANNEL];							// �p���X���[�g [mm/pls]
	};
	struct TYP_SAVEDATA {										// �ێ��f�[�^
		int		nFrameCycle;											// �t���[���o�͎��� [ms]
		int		nSpeed[SMYU_PLG_MAX_CHANNEL];							// ���C�����x [mpm]
	};

//// �����o�[�萔
private :
	static const int	TIME_CNT_INTERVAL	= 1000;					// ���C�u�J�E���^���s����
	static const int	TIME_SPEED_INTERVAL = 100;					// �ʔ������x�������s����
	static const int	TIME_HELCY_INTERVAL	= 5000;					// �w���V�[���M����


//// ���J�֐�
public:
	PlgControlManager(void);
	virtual ~PlgControlManager(void);

	void SmyuStart();													// �V�~�����[�^�J�n


	//=========================================
	// ����p
	TYP_SAVEDATA&		SetSaveData() const { return (TYP_SAVEDATA&)m_SaveData; }
	//void	SetTimer_FrameCycle(bool bMode);
	void	SetTimer_SpeedCycle(int nIndex, bool bMode);
	
	// �ʔ����p
	double	GetLength(int nIndex)	const	{ return mtbl_pSmyu->dLen[nIndex]; }
	void	ResetLength(int nIndex)			{ mtbl_pSmyu->dLen[nIndex] = 0.0; }
	DWORD	GetFrameNo()	const
	{
		SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
		return typWk->ken.nFrameNo;
	}


	//=========================================
	// ��ʌ��J�p
	TYP_SAVEDATA const&		GetSaveData() const { return (TYP_SAVEDATA const&) m_SaveData; }
	TYP_SETTING const&		GetSetting() const  { return (TYP_SETTING const&) m_Setting; }

	void	Set_WpdKen(void);											// �����pWPD�M��ON
	void	Set_WpdSub(void);											// ������r�pWPD�M��ON
	void	Set_WpdDsp(void);											// �\���pWPD�M��ON
	void	Set_ResetSignal(void);										// ���Z�b�g�M��ON

	void	Clear_SeqMem(void);											// �V�[�P���T���L�������N���A

//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	

	//=========================================
	// ���L����������
	void SetMem_Sampling();												// �T���v�����O�N���b�N�p
	void SetMem_Cnt();													// ���C�u�J�E���^�p
	void SetMem_Speed(int nIndex);										// �ʔ����p

	void SetSeqMem_Clear(TO_SEQMEM_TBL* tbl);							// �V�[�P���T���L�������N���A


	//=========================================
	// ����
	void GetIniParam();													// Ini�t�@�C���擾
	void SendUpd();														// UDP���M
	void SendHelcy();													// �w���V�[���M
	

	//=========================================
	// UDP���M
	BOOL SetUdpSocket();												// UDP SOCKET����
	void ReSetUdpSocket();												// UDP SOCKET�j��

//// �����o�[�ϐ�
private:

	//=========================================
	// �n���h��
	HANDLE					m_evTimerFrame;								// �t���[���o�͒�������s�C�x���g (nFrameCycle ��0�ȏ�̏ꍇ�̂�)
	HANDLE					m_evTimerCnt;								// ���C�u�J�E���^�C�x���g
	HANDLE					m_evTimerSpeed[SMYU_PLG_MAX_CHANNEL];		// ���̑��ʔ�����������s�C�x���g
	HANDLE					m_evTimerHelcy;								// �w���V�[���M�C�x���g


	//=========================================
	// �ێ��f�[�^
	TYP_SETTING				m_Setting;									// �����ݒ�萔 (�����グ���̂ݓǂݒ���)
	TYP_SAVEDATA			m_SaveData;									// �ێ��f�[�^


	//=========================================
	// ���L������
	CString					m_sSmyuName;								// PLG��� ����
	HANDLE					m_hTblSmyu;									// PLG��� �n���h��
	SMYU_TBL_PLG*			mtbl_pSmyu;									// PLG��� �f�[�^

	CString					m_sSeqName;									// �V�[�P���T��� ����
	HANDLE					m_hTblSeq;									// �V�[�P���T��� �n���h��
	TO_SEQMEM_TBL*			mtbl_pSeq;									// �V�[�P���T��� �f�[�^
	int						m_nIdPlgKen;									// �����pPLG�C���f�b�N�X
	int						m_nIdPlgSub;									// ������r�pPLG�C���f�b�N�X
	int						m_nIdPlgDsp;									// �\���pPLG�C���f�b�N�X

	double					m_dKenPlgDiff;								// PLG����
	double					m_dKenPlgAdd;								// PLG�ώZ
	double					m_dKenExsync;								// ExSync�ώZ�l
	double					m_dDspPlgAdd;								// PLG�ώZ

	//=========================================
	// UDP���M
	SOCKET					m_SeqUdpsock;								// UDP�p �\�P�b�g�{��
	sockaddr_in				m_SeqUdpaddr;								// UDP�p sockaddr_in�\����
	CString					m_SeqUdpIp;									// UDP�p IP�A�h���X
	WORD					m_SeqUdpPort;								// UDP�p Port�ԍ�
};
