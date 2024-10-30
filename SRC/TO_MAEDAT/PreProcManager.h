// *********************************************************************************
//	�O�H�����C���Ǘ��N���X
//	[Ver]
//		Ver.01    2012/09/06
//
//	[����]
//		
// *********************************************************************************

#pragma once


// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL�������⏕�N���X
#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB����N���X
#include "..\..\include\ToTbl.h"										// �R�C�����

// ���[�J���̃N���X
#include "MainConst.h"
#include "FFSendManager.h"
#include "PreDefectManager.h"
#include "ParamManager.h"

using namespace KizuLib;
using namespace KizuODBC;

class PreProcManager : public ThreadManager
{
private :
	//=============================
	// �{�N���X�󂯓n���p�N���X
	//=============================
	//// ���[�����󂯓n���L���[
	struct DELIVERY_MAIL {
		ENUM_MAEDATOPERATION	emNo;									// �C�x���gNo
		COMMON_QUE				que;									// �t�����
	};



//// ���J�֐�
public:
	PreProcManager(void);
	~PreProcManager(void);

	void Alloc();																		// �֐�������
	
	// �O���A�N�Z�X�p
	ThreadQueue<COMMON_QUE>	gque_Deli;													// �󂯓n���L���[

	// �O���A�N�Z�X
	void SetFFSendManager(FFSendManager* pCls) {mcls_pFFSend = pCls;}					// FF���M�Ǘ��N���X�Z�b�g
	void SetParamManager(ParamManager* pCls) {mcls_pParam = pCls;}						// �p�����[�^�Ǘ��N���X�Z�b�g
	void SetPreDefectManager(PreDefectManager* pCls) {mcls_pPreDefect = pCls;}			// �O�H���ҏW�N���X�Z�b�g
	void SetPreDBManager(PreDBManager* pCls) {mcls_pPreDefect->SetPreDBManager(pCls);}	// �O�H�����я��DB�o�^�Ǘ��N���X�Z�b�g


//// ���J�ϐ�
public:
	// �e���[�J�[�X���b�h



//// �����o�[�֐�
private:		
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent( int nEventNo );									// �X���b�h�C�x���g����

	// �V�[�P���X����
	bool MakeMaeCoilInf( COMMON_QUE* pQue );																				// �O�H���R�C����񃁃C������
	bool SelectCountMaeCoilInf( OdbcBase* clspDB, PRO_TYPE_KEY* keyinf, int* nDataCnt );									// �O�H���R�C�����c�a �����擾
	bool SelectMaeCoilInf( OdbcBase* clspDB, PRO_TYPE_KEY* keyinf, char* cKizuKenNo, int* nFFFlg,int* nKariFlg );			// �O�H���R�C�����擾
	bool InsertCoilOrder( OdbcBase* clspDB, char* cKizukenNo, int nTujun, PRO_TYPE_DATA_COILINF::NEXT_COIL*	pnext_coil );	// �R�C���ʔ��ʃe�[�u���o�^
	bool InsertMaeCoilInf( OdbcBase* clspDB, char* cKizukenNo, int nTujun, PRO_TYPE_DATA_COILINF::NEXT_COIL* pnext_coil );					// �O�H���R�C�����o�^
	void MakeMaeDefectInf( COMMON_QUE* pQue );															// �O�H���r���o�^���C������
	bool SelectBufFfRMaeinf( OdbcBase* clspDB, char* cId, PRO_TYPE_KEY* keyinf,  FF_MAE_INF* typData );	// FF�O�H���r��� �擾
	bool UpdateCoilInf( OdbcBase* clsInsDB,char* cKizukenNo, char* cId,  FF_MAE_INF* typData );			// �O�H���R�C�����X�V
	bool UpdateMaeCoilInf( OdbcBase* clspDB, char* cCoilKey, int nFrontLen );							// �����t�����g�[���� �X�V
	bool InsertMaeCoilLine( OdbcBase* clsInsDB, char* cKizuKenNO, int nOuDiv, int nProcKind, int nDspMode, FF_MAE_TYPE_INF* pKizuHead );	// �O�H�����C���ʃR�C�����o�^
	bool InsertMaeFfInf( OdbcBase* clsInsDB, char* cKizuKenNO, int nKizuNo, int nProcKind, int nOuDiv, int nDspMode, FF_MAE_TYPE_INF* pKizuHead, FF_MAE_TYPE_DATA* pKizuBody );	// �O�H��FF�r���e�[�u���o�^
	bool InsertDefectInfMain( OdbcBase* clsInsDB, char* cKizuKenNO, FF_MAE_INF* typData );				// �O�H���r���o�^���C������
	

//// �����o�[�ϐ�
	//// �e�C���X�^���X
	FFSendManager*			mcls_pFFSend;								// FF���M�Ǘ�
	PreDefectManager*       mcls_pPreDefect;							// �O�H���ҏW�N���X
	ParamManager*			mcls_pParam;								// �p�����[�^�Ǘ��N���X

	//// ���L���������
	HANDLE					m_hCoil;									// �R�C����� �n���h��
	TO_COIL_TBL*			mtbl_pCoil;									// �R�C����� �f�[�^
};
