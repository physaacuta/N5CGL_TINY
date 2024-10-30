// *********************************************************************************
//	�p�����[�^�Ǘ��N���X
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

using namespace KizuLib;
using namespace KizuODBC;

class ParamManager : public ThreadManager
{
public:

	//=======================================
	// �r�� �\����
	//=======================================
	struct PARAM_TYPE
	{
		int num;									// ����
		struct TYPE_ID
		{
			char cProc[2];							// �O�H���R�[�h
			char cFFName[2];						// FFName
			int	edasysID;							// edasysID

		} type_id[MAX_MAE_TYPE];
	};

	//=======================================
	// �O���[�h �\����
	//=======================================
	struct PARAM_GRADE
	{
		int num;									// ����
		struct GRADE_ID
		{
			char cProc[2];							// �O�H���R�[�h
			char cFFName[2];						// FFName
			int	edasysID;							// edasysID
		} grade_id[MAX_MAE_GRADE];
	};

	//=======================================
	// �O�H���}�X�^�[�e�[�u�� �\����
	//=======================================
	struct PARAM_MASTER
	{
		int num;									// ����
		struct MASTER_ID
		{
			char cProc[2];							// �O�H���R�[�h
			int  nEnable;							// �L������[0:�����A1:�L��]
			int  nMode;								// �O�H���\�����[�h
			int  nImgFmt;							// �摜�`��[0:JPEG�A1:BMP]
            // 2014.07.25 �쌴  ���ԘA�g    >>>>>>>>
			int  nTasyoZai;							// �����ޔ���
            // 2014.07.25 �쌴  ���ԘA�g    <<<<<<<<
		} master_id[MAX_MAE_LINE];
	};



//// ���J�֐�
public:
	ParamManager(void);
	~ParamManager(void);

	void SetReadParam() {	SetEvent(m_evReadParam); }	;				// �p�����[�^�ǂݍ���
	
	bool ReadParam_All();												// �S�ǂݍ���
	bool ReadParam_Type();												// �r��p�����[�^�Ǎ�
	bool ReadParam_Grade();												// �O���[�h�p�����[�^�Ǎ�
	bool ReadParam_Master();											// �O�H���}�X�^�[�e�[�u���Ǎ�
	int  GetTyppeEdasysID(char* cProc, char* cFFName) const;			// �r��EdasysID�擾
	int  GetGradeEdasysID(char* cProc, char* cFFName) const;			// �O���[�hEdasysID�擾
	int  GetDspMode(char* cProc) const;									// �O�H���}�X�^�[�O�H���\�����[�h�擾

    // 2014.07.25 �쌴  ���ԘA�g    >>>>>>>>
	int  GetTasyoZai(CString cProc);									// �����ޔ���擾
    // 2014.07.25 �쌴  ���ԘA�g    <<<<<<<<

//// ���J�ϐ�
public:



//// �����o�[�֐�
private:		
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent( int nEventNo );									// �X���b�h�C�x���g����
	void ParamManager::ExecParam( COMMON_QUE* pQue );					// �p�����[�^�Ǎ�����
	

//// �����o�[�ϐ�
private:
	//// �e�C���X�^���X
	
	// �V�O�i��
	HANDLE m_evReadParam;												// �p�����[�^�ǂݍ��� 


	// �p�����[�^
	PARAM_TYPE				m_Param_Type;								// �r��
	PARAM_GRADE				m_Param_Grade;								// �O���[�h
	PARAM_MASTER			m_Param_Master;								// �O�H���}�X�^�[

};
