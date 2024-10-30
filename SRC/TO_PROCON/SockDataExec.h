// *********************************************************************************
//	�v���R���ʐM �� ����M�f�[�^�󂯓n��
//	[Ver]
//		Ver.01    2007/08/03  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

// ���Œ�
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u����
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// �}�N���W
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// �X�^�e�B�b�N���C�u�����W
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ���O�Ǘ��N���X

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB����N���X
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL�������⏕�N���X
#include "..\..\Library\DefectLib\BaseTcpIp.h"							// TCP/IP����N���X
#include "..\..\Library\DefectLib\DBSaveManager.h"						// DB�����݊�{�N���X

// �����o�[
#include "MainConst.h"
#include "SockManager.h"
#include "..\..\Include\ToTbl.h"										// �������L�������e�[�u��

using namespace KizuLib;
using namespace KizuODBC;

class SockDataExec : public SockManager
{
//// ���J�֐�
public:
	SockDataExec(void);
	virtual ~SockDataExec(void);

	void Alloc();														// �֐�������
	void Free();														// �֐��J��

	void SetLogMgr(LogFileManager* pCls) { mcls_pLog = pCls; SockManager::SetLogMgr(pCls); };	// ���O�t�@�C���Z�b�g 


	//// �O���A�N�Z�X
	// ���M�֌W
	bool SetSendDataAnser(Delivery* deli, bool bAns);					// ���M�f�[�^ ���㏈��
	bool SetRecvData(int tc, int size, BYTE * data);					// ��M�f�[�^ ���㏈��
	bool AddQue_Ziseki(COMMON_QUE const* pQue);							// �r�������ѓ`�� ���M�˗� [��Dlg]

	// �O������
	bool ReadBufFile(int nTc, BYTE * data, char* cFileName, int nMode = 0);		// �o�b�t�@�t�@�C����Ǎ�


//// �����o�[�֐�
private:

	//// �΃v���R��I/F�֘A
	void	ByteHantenProDataRecv(int nTc, BYTE * data);				// �v���R����M�`�� �o�C�g���]
	void	ByteHantenProDataSend(int nTc, BYTE * data);				// �v���R�����M�`�� �o�C�g���]

	//// ���̑�
	bool	InsertBufProRData(int nTc, BYTE * data);					// ��M�`����BUF�e�[�u���o�^



//// �����o�[�ϐ�
private:
	//// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	DBSaveManager*			mcls_pDbs;									// DB�����݃N���X	
	ThreadQueue<DBSaveManager::DeliverySQL> mque_AddDB;					// �󂯓n���f�[�^�p�L���[ (�����ɓ������L���[�͑S�ēo�^����)


	// ���L���������
	HANDLE					m_hCoil;									// �R�C����� �n���h��
	TO_COIL_TBL*			mtbl_pCoil;									// �R�C����� �f�[�^
	
	// �f�o�b�N�p
	PRO_TYPE_CUT            mtyp_Cut;									// �J�b�g���ۑ�
};
