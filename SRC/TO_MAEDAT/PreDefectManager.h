// *********************************************************************************
//	�O�H���ҏW�N���X
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
#include "..\..\Library\DefectLib\DBSaveManager.h"						// DB�����݊�{�N���X
#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB����N���X

// ���[�J���̃N���X
#include "MainConst.h"
#include "PreDBManager.h"

using namespace KizuLib;
using namespace KizuODBC;
class PreDefectManager : public ThreadManager
{
public:
	PreDefectManager();
	~PreDefectManager(void);

	void SetGetData() {	SetEvent(m_evGetData); }	;					// �O�H�����ѕҏW�����v��
	void SetPreDBManager(PreDBManager* pCls) {mcls_pPreDB = pCls;}		// �O�H�����я��DB�o�^�Ǘ��N���X

private:

	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent( int nEventNo );									// �X���b�h�C�x���g����
	
	static const int PREPROC_TIMER = 60*60*1000;						// ����������^�C�}�[�C���^�[�o��[ms]

	// �V�[�P���X����
	void PreDefectMain();												// �O�H�����ѕҏW���C������

	//// �e�C���X�^���X
	PreDBManager*			mcls_pPreDB;								// �O�H�����я��DB�o�^�Ǘ�

	// �V�O�i��
	HANDLE m_evGetData;													// �O�H�����擾 
	HANDLE m_evTimeCycle;												// �w���V�[

	PreDBManager::DeliveryPreDB  m_PreDeliWk;							// �O��X���b�h���M

};
