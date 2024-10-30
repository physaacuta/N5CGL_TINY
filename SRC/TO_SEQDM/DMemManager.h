#pragma once

// ���Œ�
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u����
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// �}�N���W
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// �X�^�e�B�b�N���C�u�����W
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ���O�Ǘ��N���X
#include "..\..\Library\KizuLib\Class\LockManager.h"					// �X���b�h�ԓ�������N���X

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ���[�J�[�X���b�h�N���X

#include "..\..\Include\ToTbl.h"										// �������L�������e�[�u�� �C���N���[�h

// ���[�J��
#include "actqj71e71udp1.h"

using namespace KizuLib;

class DMemManager :	public WorkThread
{
private:
	//=======================================
	// �萔��`
	//=======================================
	static const int TIME_INTERVAL_EXEC			= 30;					// ������������s �^�C�}���� [ms]


public:
	DMemManager(void);
	virtual ~DMemManager(void);

	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(); };								// �X���b�h�I��


	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; };			// ���O�t�@�C���Z�b�g
	void SetMxComponent(CActqj71e71udp1* pCom) { m_pCom = pCom; };		// MX�R���|�[�l�g �ʐM�N���X


	//// �O���A�N�Z�X
	void SetConnect(bool flg) { m_bIsConnect = flg; };					// �ڑ���Ԑݒ�
	bool IsConnect() const { return m_bIsConnect; };					// �ڑ���Ԏ擾 
	bool IsStatus() const  { return m_bIsStatus; }						// �ǂݏ������


private :
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h

	void Exec_ReadWrite();												// �f�[�^�̓Ǎ��Ə����ݏ���
	bool MxRead();														// �V�[�P���T�f�[�^�擾
	bool MxWrite();														// �f�[�^������ �f�[�^����



	//// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X

	//// MX�R���|�[�l�g
	CActqj71e71udp1*		m_pCom;										// MX�R���|�[�l�g �ʐM�N���X
	char					m_cAdderssRead[16];							// MX�R���|�[�l���g�ł� �Ǎ��݊J�n�A�h���X
	char					m_cAdderssWrite[16];						// MX�R���|�[�l���g�ł� �����݊J�n�A�h���X
	BYTE					m_cDMBuf[DM_AREA_MAX][DM_DATA_MAX];			// ����M�p�o�b�t�@ [0:��M�o�b�t�@ 1:���M�o�b�t�@]
	

	//// ���L���������
	HANDLE					m_hDataMem;									// �f�[�^��������� �n���h��
	TO_SEQMEM_TBL*			mtbl_pDM;									// �f�[�^��������� �f�[�^

	//// ���̑��������
	bool					m_bIsConnect;								// �ڑ���� (true:�ڑ�)
	bool					m_bIsStatus;								// �ǂݏ������

};
