// *********************************************************************************
//	FF�ʐM�f�[�^�̎�n��
//	[Ver]
//		Ver.01    2012/08/30
//
//	[����]
//		
// *********************************************************************************

#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\FuncBase.h"					// �t�@���N�V�����p�̃x�[�X�N���X
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// �X�^�e�B�b�N���C�u�����W
#include "..\..\Library\DefectLib\BaseTcpIp.h"							// TCP/IP����N���X
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL�������⏕�N���X

// �����o�[
#include "MainConst.h"

using namespace KizuLib; 
using namespace KizuODBC;

class SockDataExec : public FuncBase
{
public:
	enum FF_SENDKIND_NO {												// �`���敪
		FF_SENDKIND_OK = 0, 											// ���푗�M������
		FF_SENDKIND_NON = 1, 											// �����M
		FF_SENDKIND_ERR = -1, 											// ���M���s�i�đ��M����j
		FF_SENDKIND_NOTSEND = -9 										// �đ��M�Ȃ�
	};

//// ���J�֐�
public:
	SockDataExec(void);
	virtual ~SockDataExec(void);

	//// �O���A�N�Z�X
	// ���M�֌W
	bool GetSendData(Delivery* deli, ExecItem* item);					// ���M�f�[�^�擾


	//// Dump�o�͊֌W
	void SetDumpMode(bool bmode)	{m_bDumpMode = bmode; }				// �_���v�o�̓��[�h�Z�b�g
	bool GetDumpMode() const		{return m_bDumpMode; }				// �_���v�o�̓��[�h�擾

	//// �e�X�g�f�[�^�ǂݍ��݃��[�h�֌W
	void SetReadDataMode(bool bmode)	{m_bReadDataMode = bmode; }				// �e�X�g�f�[�^�ǂݍ��݃��[�h�Z�b�g
	bool GetReadDataMode() const		{return m_bReadDataMode; }				// �e�X�g�f�[�^�ǂݍ��݃��[�h�擾

	bool ReadTestFile( BYTE* cSendData, int nSendSize, char const* cfullname );		// �e�X�g�f�[�^�ǂݍ���

//// ���J�ϐ�
public:


//// �����o�[�ϐ�
private:
	//// �n���h��

	//// �e�C���X�^���X

	//// �X�e�[�^�X
	bool                    m_bDumpMode;								// �_���v�o�̓��[�h(True:�S���o��,False:1���o��)
	bool                    m_bReadDataMode;							// �e�X�g�f�[�^�ǂݍ��݃��[�h(True:DB,False:�t�@�C��)


};
