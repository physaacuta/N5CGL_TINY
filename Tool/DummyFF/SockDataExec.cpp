#include "StdAfx.h"
#include "SockDataExec.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
SockDataExec::SockDataExec(void):
FuncBase("SockDataExec"),
m_bDumpMode(false),
m_bReadDataMode(true)
{
}
//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
SockDataExec::~SockDataExec(void)
{
}

//------------------------------------------
// ���M�f�[�^�擾
// Delivery* deli ���M�v��
// ExecItem* item ����M�f�[�^
//------------------------------------------
bool SockDataExec::GetSendData(Delivery* deli, ExecItem* item)
{
	CString	sql;
	CString	cWk;
	SYSTEMTIME STime;
	GetLocalTime(&STime);

	//// �Z�b�g�G���A������
	memset(item, 0x00, sizeof(ExecItem));

	//// �e�`������
	switch(deli->eStat) {

//-----------------------------------------------------------------------------------------------
	case QUE_SEND_ZISEKI_T:								// �r���ѓ`��
	case QUE_SEND_ZISEKI_B:
	{
			//// ���̑��Œ� �f�[�^�Z�b�g
			// �`�����
			item->emStat = deli->eStat;
			// ���M�`���T�C�Y (�������ѓ`���T�C�Y���Z�b�g)
			item->nSendSize = sizeof(FF_ANS);
			// ����No
			memcpy(item->cMeiNo, deli->cMeiNo, sizeof(item->cMeiNo));
			// �R�C��No
			memcpy(item->cCoilNo, deli->cCoilNo, sizeof(item->cCoilNo));

			// �`���R�[�h
			FF_ANS* pBuf;
			pBuf = (FF_ANS*)&item->cSendBuf;
			if (QUE_SEND_ZISEKI_T == deli->eStat) {
				memcpy(pBuf->Hd.Header_Code, FF_TC_CODE_ANS_O, sizeof(pBuf->Hd.Header_Code));
			}
			else {
				memcpy(pBuf->Hd.Header_Code, FF_TC_CODE_ANS_U, sizeof(pBuf->Hd.Header_Code));

			}
			// �`����������
			cWk.Format("%04d%02d%02d%02d%02d%02d", STime.wYear, STime.wMonth, STime.wDay, STime.wHour, STime.wMinute, STime.wSecond);
			memcpy(pBuf->Hd.Date, cWk, sizeof(pBuf->Hd.Date));

			// �A��
			memcpy(pBuf->Hd.Rec_Code, "00", sizeof(pBuf->Hd.Rec_Code));

			// ����No
			memcpy(pBuf->Hd.Meino, deli->cMeiNo, sizeof(pBuf->Hd.Meino));

			// �R�C��No
			memcpy(pBuf->Hd.Coilno, deli->cCoilNo, sizeof(pBuf->Hd.Coilno));

			// �X�e�[�^�X
			 memcpy(pBuf->Data.Status, "00", sizeof(pBuf->Data.Status));
	}
	break;

//-----------------------------------------------------------------------------------------------
	case QUE_SEND_STAT_T:								// ������ԉ����v��
	case QUE_SEND_STAT_B:
	{
		//// ���̑��Œ� �f�[�^�Z�b�g
		// �`�����
		item->emStat = deli->eStat;
		// ���M�`���T�C�Y (�������ѓ`���T�C�Y���Z�b�g)
		item->nSendSize = sizeof(FF_STSANS);
		// ����No
		memcpy(item->cMeiNo, deli->cMeiNo, sizeof(item->cMeiNo));
		// �R�C��No
		memcpy(item->cCoilNo, deli->cCoilNo, sizeof(item->cCoilNo));

		// �`���R�[�h
		FF_STSANS* pBuf;
		pBuf = (FF_STSANS*)&item->cSendBuf;
		if (QUE_SEND_STAT_T == deli->eStat) {
			memcpy(pBuf->Hd.Header_Code, FF_TC_CODE_STSANS_O, sizeof(pBuf->Hd.Header_Code));
		}
		else {
			memcpy(pBuf->Hd.Header_Code, FF_TC_CODE_STSANS_U, sizeof(pBuf->Hd.Header_Code));

		}
		// �`����������
		cWk.Format("%04d%02d%02d%02d%02d%02d", STime.wYear, STime.wMonth, STime.wDay, STime.wHour, STime.wMinute, STime.wSecond);
		memcpy(pBuf->Hd.Date, cWk, sizeof(pBuf->Hd.Date));

		// �A��
		memcpy(pBuf->Hd.Rec_Code, "00", sizeof(pBuf->Hd.Rec_Code));

		// ����No
		memcpy(pBuf->Hd.Meino, deli->cMeiNo, sizeof(pBuf->Hd.Meino));

		// �R�C��No
		memcpy(pBuf->Hd.Coilno, deli->cCoilNo, sizeof(pBuf->Hd.Coilno));

		// �X�e�[�^�X
		memcpy(pBuf->Data.Status, "00", sizeof(pBuf->Data.Status));
	}
	break;

//-----------------------------------------------------------------------------------------------
	case QUE_SEND_MAE_T:									// �O�H�����їv��
	case QUE_SEND_MAE_B:
	{
		// �`�����
		item->emStat = deli->eStat;
		// ����No
		memcpy(item->cMeiNo, deli->cMeiNo, sizeof(item->cMeiNo));
		// �R�C��No
		memcpy(item->cCoilNo, deli->cCoilNo, sizeof(item->cCoilNo));

		// ���M�`���T�C�Y (�������ѓ`���T�C�Y���Z�b�g)
		item->nSendSize = sizeof(FF_MAE_JISSEKI);
		// ���M�`���t�@�C���Z�b�g
		if (!ReadTestFile(item->cSendBuf, item->nSendSize, deli->cFileName))
		{
			item->emStat = (QUE_SEND_STAT)0;
			return false;
		}
	}
	break;

//-----------------------------------------------------------------------------------------------
	case QUE_SEND_HELCY:								// �w���V�[�`�����M�v��
		{
			// �Œ�f�[�^
			// �`�����
			item->emStat = deli->eStat;
			// ���M�`���T�C�Y (�w���V�[�`���T�C�Y���Z�b�g)
			item->nSendSize = sizeof(FF_HELCY);			
			

			// �`������
			FF_HELCY& pBuf = (FF_HELCY&)item->cSendBuf[0];
			memset(&pBuf, 0x20, sizeof(FF_HELCY));


			// �`���R�[�h
			memcpy( pBuf.Header_Code, FF_TC_CODE_HELCY, sizeof(pBuf.Header_Code));
			// �A��
			memcpy( pBuf.Rec_Code, "00", sizeof(pBuf.Rec_Code));
			// �`����������
			cWk.Format("%04d%02d%02d%02d%02d%02d", STime.wYear, STime.wMonth, STime.wDay, STime.wHour, STime.wMinute, STime.wSecond); 
			memcpy( pBuf.DateTime, cWk, sizeof(pBuf.DateTime));
			// �H���R�[�h
			memcpy(pBuf.Line_Code, KOUTEI_CODE, sizeof(pBuf.Line_Code));
			// �`�ǃR�}���hNo
			memcpy(pBuf.Denkan_No, FF_DENKAN_NO, sizeof(pBuf.Denkan_No));
		}
		break;
	}
	return true;
}
//------------------------------------------
// �e�X�g�f�[�^�ǂݍ���
// void* cSendData		�`���o�b�t�@�̈�
// int iSendSize		�`���o�b�t�@�T�C�Y
// char const* fullname	�t�@�C������
//------------------------------------------
bool SockDataExec::ReadTestFile( BYTE* cSendData, int nSendSize, char const* cfullname )
{
	HANDLE		fp;
	DWORD		nNumByte = 0;
	bool 		bRet = false;
	char		cfullpath[MAX_PATH];

	sprintf( cfullpath, ".\\%s", cfullname );						// �t�@�C��PATH����

	LOG(em_MSG), "[%s] %s�Ǎ��݊J�n", my_sThreadName, cfullname );	// �J�n���O�o��

	fp = CreateFile( cfullpath,										// �t�@�C��PATH
					 GENERIC_READ,									// �A�N�Z�X���[�h�i�Ǎ��݁j
					 FILE_SHARE_READ,								// ���L���[�h�i�Ǎ���OK�j
					 NULL,											// �Z�L�����e�B�L�q�q�i�w��Ȃ��j
					 OPEN_EXISTING,									// �쐬���@
					 FILE_ATTRIBUTE_NORMAL,							// �t�@�C�������i���ʁj
					 NULL );										// �e���v���[�g�t�@�C���̃n���h���i�w��Ȃ��j

	if( fp != INVALID_HANDLE_VALUE ) {
		if( ReadFile( fp, cSendData, nSendSize, &nNumByte, NULL) ) {									// �Ǎ��ݎ��s
			LOG(em_MSG), "[%s] %s�Ǎ��݊���", my_sThreadName, cfullname );								// �Ǎ��݊������O�o��
			bRet = true;
		} else {
			LOG(em_ERR), "[%s] %s�Ǎ��ݎ��s (retc=%d)", my_sThreadName, cfullname, GetLastError() );	// �Ǎ��ݎ��s���O�o��
			bRet = false;
		}
		CloseHandle( fp );
	} else {
		LOG(em_ERR), "[%s] %s�I�[�v�����s (retc=%d)", my_sThreadName, cfullname, GetLastError() );		// �t�@�C���I�[�v�����s���O�o��
		bRet = false;
	}
	return bRet;
}

