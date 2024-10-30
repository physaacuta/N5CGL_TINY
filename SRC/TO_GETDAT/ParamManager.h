#pragma once
// ���Œ�
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h

// �r���N���X �C���N���[�h
//#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLibMFC\CLASS\FuncBase.h"

// �r�����C�u���� �C���N���[�h
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X

// ���[�J��
#include "MainConst.h"

using namespace KizuLib;
using namespace KizuODBC;

// �t�@���N�V�����̎g���������AThreadManager���p�����Ă����B���[�J�[�X���b�h�͋N�����Ȃ�
class ParamManager : public FuncBase
{

//// ���J�\����
public:
	//=======================================
	// �r���萔 �\����
	struct P_COMMON_MAST
	{
		int nWalm;									// �E�H�[�}�ނ̌����p�^�[�����R�[�h�ʒu (1�I���W��)
	};


	//=======================================
	// �����\�ʏ�� �\����
	struct P_SCODE_OUTPUT
	{
		// �����ݒ�
		struct JUDGE
		{
			BOOL	bUmu;				// �L������ (true:�L�� false:����)
			int		nSpm;				// SPM�����L�� (-1:���� 0:�~(��:����) 1:��(�J:�J��))
			char	cMeiNo[32];			// ����No
			char	cCoilNo[32];		// ���YNo
			int		nAtuSei[2];			// ������[��m] 0:���� 1:���
			char	cKenKijun[32];		// �����
			char	cMuke[32];			// ����
			char	cYouto[32];			// �p�r
			char	cSyuZai[32];		// ��ގ�
			char	cOmoteSyori1[32];	// �\�ʏ���1����
			char	cHinCode[32];		// �i��
		};

		// ����
		struct INF
		{
			// �}�X�^�[
			BOOL	bUmu;							// �L������ (true:�g�p false:���g�p)				

			// �����ݒ�
			JUDGE	typJudge[MAX_CODE_JUDGE];			// �����ݒ�
		} rec[MAX_SCODE];							// �ŏI�́A���̑����ڌŒ�

		// �D��x�Ɣz��ʒu�̕R�t��
		int		nPriority[MAX_SCODE];				// �D��x����scode(=�z��Index)�������Ă���
		int		nMasterNum;							// �L���ȃ}�X�^�[��
	};


	//=======================================
	// �����p�^�[���� �\����
	struct P_PCODE_OUTPUT
	{
		// �����ݒ�
		struct JUDGE
		{
			BOOL	bUmu;				// �L������ (true:�L�� false:����)
			char	cMeiNo[32];			// ����No
			char	cCoilNo[32];		// ���YNo
			int		nAtuSei[2];			// ������[��m] 0:���� 1:���
			char	cKenKijun[32];		// �����
			char	cMuke[32];			// ����
			char	cYouto[32];			// �p�r
			char	cSyuZai[32];		// ��ގ�
			char	cOmoteSyori1[32];	// �\�ʏ���1����
			char	cHinCode[32];		// �i��
		};

		// ����
		struct INF
		{
			// �}�X�^�[
			BOOL	bUmu;							// �L������ (true:�g�p false:���g�p)				

			// �����ݒ�
			JUDGE	typJudge[MAX_CODE_JUDGE];		// �����ݒ�
		} rec[MAX_PCODE];							// �ŏI�́A���̑����ڌŒ�

		// �D��x�Ɣz��ʒu�̕R�t��
		int		nPriority[MAX_PCODE];				// �D��x����pcode(=�z��Index)�������Ă���
		int		nMasterNum;							// �L���ȃ}�X�^�[��
	};

//// ���J�֐�
public:
	ParamManager(void);
	virtual ~ParamManager(void);

	//=========================================
	// �p�����[�^�Q��
	inline const P_COMMON_MAST&		GetCommonMast()			const {return (P_COMMON_MAST&)m_CommonMast;}
	inline const P_SCODE_OUTPUT&	GetScodeOutput()		const {return (P_SCODE_OUTPUT&)m_ScodeOutput;}
	inline const P_PCODE_OUTPUT&	GetPcodeOutput()		const {return (P_PCODE_OUTPUT&)m_PcodeOutput;}


	//=========================================
	// �p�����[�^�擾�֘A (����)
	bool ReadParam_All();												// �S�p�����[�^�ǂݍ���
	bool Read_CommonMast();												// �r���萔�p�����[�^�ǂݍ���
	bool Read_ScodeOutput();											// �����\�ʏ�ԃp�����[�^�Ǎ�
	bool Read_PcodeOutput();											// �����p�^�[���ʃp�����[�^�Ǎ�


//// �����o�[�֐�
private:
	//// ���[�J�[�X���b�h�Ƃ��ē��삳���Ȃ����߁A�֌W�֐������J�ɕύX
	//int						ThreadFirst()				{return 0;}		// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	//int						ThreadLast()				{return 0;}		// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
	//void					ThreadEvent(int nEventNo)	{;}				// �X���b�h�C�x���g����

	//=========================================
	// �p�����[�^
	P_COMMON_MAST			m_CommonMast;								// �r���萔
	P_SCODE_OUTPUT			m_ScodeOutput;								// �����\�ʏ��	
	P_PCODE_OUTPUT			m_PcodeOutput;								// �����p�^�[����
};
