#pragma once

// ���Œ�
#include "..\..\Include\LineCommon.h"									// �ӃփC���N���[�h

// �ӃփN���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// �N���e�B�J���Z�N�V��������N���X

// �Ӄփ��C�u���� �C���N���[�h
#include "..\..\Library\KizuODBC\OdbcBase.h"

#include "MainConst.h"

using namespace KizuLib;
using namespace KizuODBC;

class ParamManager :	public ThreadManager
{
public:
	ParamManager(void);
	virtual ~ParamManager(void);

private:
	int		ThreadFirst();												// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int		ThreadLast();												// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void	ThreadEvent(int nEventNo);									// �X���b�h�C�x���g����



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �p�����[�^�p�\����
public:
	//=============================
	// �Ӄ֒萔�@�\����
	//=============================
	struct P_COMMON_MAST
	{
		int		nLongPd_GuidanceAvailable;								// [���������׃K�C�_���X�L��] �T����ʂւ̒������̃K�C�_���X�L��(0:�Ȃ� 1:����)
		int		nLongPd_GuidanceCheckInterval;							// [���������׃K�C�_���X�m�F���{�Ԋu] ���������т̃`�F�b�N�Ԋu [�b]
		int		nLongPd_NearFurnaceRangeFrom;							// [���������טF�`�J����͈�From] �����s�b�`���F�`�J�Ɣ��肳���͈͂�From [mm]
		int		nLongPd_NearFurnaceRangeTo;								// [���������טF�`�J����͈�To] �����s�b�`���F�`�J�Ɣ��肳���͈͂�To [mm]
	};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �C�x���g���J
public:


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �p�����[�^�Q��
public:
	inline const P_COMMON_MAST&				GetCommonMast()		const {return (P_COMMON_MAST&)m_CommonMast;}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �p�����[�^�擾�֘A (����)
public:
	bool ReadParam_All();												// �S�p�����[�^�ǂݍ���
	
	bool Read_CommonMast();												// �萔�p�����[�^�Ǎ�

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �X���b�h�L���[
public:
	ThreadQueue<COMMON_QUE>	gque_Deli;									// �p�����[�^�ύX�ʒm�󂯓n���L���[


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �����o�[�֐�
private:

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �����o�[�ϐ�
private:
	//// �p�����[�^
	P_COMMON_MAST				m_CommonMast;							// �Ӄ֒萔
};
