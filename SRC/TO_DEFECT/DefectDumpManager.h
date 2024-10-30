// *********************************************************************************
//	�r���񓯊������� �N���X
//
//	[����]
//		
// *********************************************************************************
#pragma once

#pragma warning(disable : 4996)							// ***���Â��`���Ƃ��Đ錾����܂����B���o���Ȃ��΍�

#include <fstream>	
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// �}�N���W
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// �X�^�e�B�b�N���C�u�����W
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL�������⏕�N���X


// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[


// EPC�C���N���[�h
//#include "..\..\Include\EPC\NCattr.h"
#include "..\..\Include\EPC\NCL2attr.h"									// �����ʒ�`


// ���[�J���̃N���X
#include "WarpMatchImage.h"												// ����Y���␳����
#include "MainConst.h"
#include "ParamManager.h"												// �p�����[�^�Ǘ��N���X

#include <math.h>
#include <fstream>
#include <direct.h>

using namespace KizuLib;


class DefectDumpManager : public ThreadManager
{
public:
	DefectDumpManager(int TorB, LPCSTR filename="DEF_", int cycleCnt=5, int backCnt=4);
	virtual ~DefectDumpManager();


private:
	void	ThreadEvent(int nEventNo);									// �X���b�h�C�x���g����
	int		ThreadFirst();												// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int		ThreadLast();												// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���[�J���萔
private:
	static const int DEFECTDUMPMANAGER_MAX_QUE		= 512;				// ���O�t�@�C���ő吔


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X���b�h�L���[
public:

	// �X���b�h�L���[ �Z�b�g�p�\����
	struct	DEFDUMP_QUE {
		int		nRecord;												// �o�^���R�[�hNo
		LPSTR	pStr;													// �r���
	};

	ThreadQueue<DEFDUMP_QUE>	gque_Deli;								// �X���b�h�L���[
	

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���J�֐�
public:

	// ���O�o��
	void	Write( int nRecord, LPCSTR __cBuffer, ... );				// �񓯊��������݃G���g���[ (�ψ����Ή�)[��HtRecv]
	void	WriteLock( int nRecord, LPCSTR __cBuffer, ... );			// ���������� (�ψ����Ή�)[���N���g���ĂȂ�]
	int		AllGetQueue();												// �S�L���[�����o��[���N���g���ĂȂ�]
	int		AllDeleteQueue();											// �S�L���[�����폜[��DefectDumpMgr]

	// ���̑��������
	void	SetFileOut(bool umu) {m_bFileOut = umu;}					// �t�@�C���o�͗L��[���N���g���ĂȂ�]
	void	FileNoClear() {m_nNowFileNo = -1;}							// �t�@�C��No�̏�����(�����J�n���Ɏ��s)[��HtRecv]
	CString	GetNowLogPath() {return m_cNowFPath;}						// ���݂̃��O�t�@�C���̐�΃p�X��Ԃ�[���N���g���ĂȂ�]
	void	WriteFile(int nRecord, LPCSTR msg);							// �t�@�C���ɏ�������[��DefectDumpMgr]
	void	FlushFile();												// �t�@�C���ɋ����o��[��DefectDumpMgr]
	int		Get_NowFileNo() {return m_nNowFileNo;}						// ����̧��No���o��[��HtRecv, RealCntMgr]


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// �����o�[�ϐ�
private:

	// ���O�o�͐���
	//ofstream				m_stream;									// �X�g���[��	// VS6
	std::ofstream			m_stream;									// �X�g���[��	// VS2003�ȍ~

	CRITICAL_SECTION		m_csOutPut;									// �N���e�B�J���Z�N�V����
	CRITICAL_SECTION		m_csANC;									// �񓯊��p�N���e�B�J���Z�N�V����

	bool					m_bFileOut;									// �t�@�C���o�� (true:�t�@�C���o�͗L��)

	// ���O�t�@�C���֘A
	CString					m_cBaseFolder;								// �x�[�X�̃t�H���_
	CString					m_cBaseFile;								// �x�[�X�̃t�@�C����
	CString					m_cNowFPath;								// ���ݏo�͒��̃t�@�C���t���p�X
	int						m_nNowFileNo;								// �t�@�C����
	DWORD					m_nNowRow;									// �s��
	int						m_nMaxCycleCnt;								// �ő�T�C�N���b�N�t�@�C������
	int						m_nMaxBackCnt;								// �ő�o�b�N�A�b�v����
};
