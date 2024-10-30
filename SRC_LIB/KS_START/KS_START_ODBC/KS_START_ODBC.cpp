// KS_START_ODBC.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "KS_START_ODBC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// �B��̃A�v���P�[�V���� �I�u�W�F�N�g�ł��B

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// MFC �����������āA�G���[�̏ꍇ�͌��ʂ�������܂��B
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: �K�v�ɉ����ăG���[ �R�[�h��ύX���Ă��������B
		_tprintf(_T("�v���I�ȃG���[: MFC �̏��������ł��܂���ł����B\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: �A�v���P�[�V�����̓�����L�q����R�[�h�������ɑ}�����Ă��������B

		//// �������ɓ������Z�b�V�������g�p
		char const * Session = argv[1];
//MessageBox(NULL, Session, "", 0);

		// ===>> �⊪ 2018.10.21 ODBC�h���C�o�敪�ʎw��@�\�ǉ�
		//if( 2 != argc ) return -9;
		if( 3 != argc ) return -9;
		// <<===
		if( NULL == Session ) return -8;

		// ��ini�t�@�C���ɐݒ肳��Ă���DB�Z�b�V������ODBC��ݒ肷��
		char wk_addr[128];
		char wk_odbc[128];
		char wk_dsna[128];
		GetPrivateProfileString(Session, "DBADDLESS", "", (char *)wk_addr, sizeof(wk_addr), TASKINI_NAME);
		GetPrivateProfileString(Session, "ODBCNAME",  "", (char *)wk_odbc, sizeof(wk_odbc), TASKINI_NAME);
		GetPrivateProfileString(Session, "DSNAME",    "", (char *)wk_dsna, sizeof(wk_dsna), TASKINI_NAME);
		
		// �f�[�^���S�����邩�`�F�b�N
		if( 0 == strlen(wk_addr) ||
			0 == strlen(wk_odbc) ||
			0 == strlen(wk_dsna) ) {
			//LOG(em_MSG), "[CheckDB] DB Session��񖳂�");
			return -1;
		}
		
		// �h���C�o
		int driverid = GetPrivateProfileInt("DB", "DRIVERID", -1, PCINI_NAME);
		// ===>> �⊪ 2018.10.21 ODBC�h���C�o�敪�ʎw��@�\�ǉ�
		int const nId = atoi(argv[2]);
		CString sWk;
		sWk.Format("DRIVERID_%d", nId);
		driverid = GetPrivateProfileInt("DB", sWk, driverid, PCINI_NAME);
		// <<===
		if(0 > driverid ) {
			//LOG(em_ERR), "ODBC�̃h���C�o�敪�ُ� [%d]", driverid);
			//_ASSERT(0);
			return -2;
		}

		// ��������
		if( ODBC_Setting(driverid, wk_odbc, wk_addr, wk_dsna) ) {
			return 0;
		} else {
			return -3;	
		}
	}

	return nRetCode;
}
