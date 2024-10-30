#pragma once

#include "..\..\Library\KizuLibMFC\Class\FuncBase.h"
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X

// ���[�J��
#include "MainConst.h"													// ���N���X�萔�w�b�_�[

class ParamManager : public ThreadManager //public FuncBase
{

//// ���J�\����
public:
	//=======================================
	// �r���萔 �\����
	//=======================================
	struct P_COMMON_MAST
	{
		int			nImgCullX;											// �摜�Ԉ������B�ÓI�B �� 1/x ���w��
		int			nImgCullY;
		BOOL		nImgSave[MAX_IMAGE_CNT];							// �S���摜�ۑ��L���B= �S�������ւ̓`���L��
	
		// �����ŎZ�o
		int			nImgSaveNum;										// �S���摜�ۑ����B �������ŎZ�o
	};

//// ���J�֐�
public:
	ParamManager(void);
	virtual ~ParamManager(void);

	void	SetEv_PrmRead() { SetEvent(m_evRead);}						// �񓯊��p�����[�^�Ǎ�


	//=========================================
	// �p�����[�^�Q��
	inline const P_COMMON_MAST&			GetCommonMast()			const {return (P_COMMON_MAST&)m_CommonMast;}


	//=========================================
	// �p�����[�^�擾�֘A (����)
	bool ReadParam_All();												// �S�p�����[�^�ǂݍ���
	

//// �����o�[�֐�
private :
	int				ThreadFirst();										// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int				ThreadLast()	{return 0;};						// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void			ThreadEvent(int nEventNo);							// �X���b�h�C�x���g����

	// DB�Ǎ�
	bool			Read_CommonMast();									// �r���萔�p�����[�^�Ǎ�


//// �����o�[�ϐ�
private :
	//=========================================
	// �V�O�i�� (�O������)
	HANDLE						m_evRead;								// �p�����[�^�ǂݍ���


	//=========================================
	// �p�����[�^
	P_COMMON_MAST				m_CommonMast;							// �r���萔

	// �񓯊��Ǎ�
	P_COMMON_MAST				m_CommonMastTemp;						// �r���萔(�񓯊��ǂݍ��񂾃o�b�t�@)
	
};
