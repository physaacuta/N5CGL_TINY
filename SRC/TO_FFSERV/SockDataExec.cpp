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
	case QUE_SEND_ZISEKI:								// �r���ѓ`��
		{
			if( ( 0 == strlen( deli->cOmtFileName ) )
				|| ( GetReadDataMode() ) ) {
				// �ʏ탂�[�h�̏ꍇ ��
				// �e�X�g���[�h�� DB�w��̏ꍇ


				CString cDbName;
				cDbName = DB_BUF_FF_S_ZISEKI;

				//// SQL���쐬
				sql =  "SELECT";
				sql += " TOP 1 ID";
				sql += ", �L�[���, ���`��1, ���`��2, ���`��3, ���`��4";
				sql += " FROM ";
				sql += cDbName;
				sql += " WHERE ";
				sql += " �`���敪 IN (";
				CreateSQL::sqlColumnInsert(&sql, FF_SENDKIND_NON);			// �`���敪(�����M)
				CreateSQL::sqlColumnInsert(&sql, FF_SENDKIND_ERR);			// �`���敪(���M���s�i�đ��M����j)
				CreateSQL::sqlColumnTrim(&sql);								// �Ō�̃J���}����菜��
				sql += ") ";

				cWk.Format(" AND �o�^���t+1>'%d/%d/%d %d:%d:%d' ",STime.wYear, STime.wMonth, STime.wDay, STime.wHour, STime.wMinute, STime.wSecond);
				sql += cWk;
				sql += " ORDER BY �o�^���t ";


				//// ODBC�ڑ��N���X �C���X�^���X����
				OdbcBase clsDB;
				if( ! clsDB.Connect(TASKINI_NAME)) {
					// �f�[�^�x�[�X�ڑ��G���[
					LOG(em_ERR), "[%s] DB�ڑ��G���[[%s]", my_sThreadName, cDbName);
					syslog(SYSNO_DB_CONNECT_ERR, "[%s]", cDbName);
					return false;
				}


				//// ���s
				if( ! clsDB.ExecuteDirect(sql)) {
					LOG(em_ERR), "[%s] DB�ǎ�SQL�����s�G���[[%s]", my_sThreadName, cDbName);
					syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
					return false;
				}

				//// �擾
				int sqlrc = clsDB.Fetch();
				if(sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
					
					//// ���̑��Œ� �f�[�^�Z�b�g
					// �`�����
					item->emStat = deli->eStat;
					// ���Z�b�g���Ă��邩
					item->nSendMaxNo = 2;
					// ���M�`���T�C�Y (�������ѓ`���T�C�Y���Z�b�g)
					item->nSendSize[0] = sizeof(FF_JISSEKI);
					item->nSendSize[1] = sizeof(FF_JISSEKI);
					item->nSendSize[2] = sizeof(FF_STS_JISSEKI);
					item->nSendSize[3] = sizeof(FF_STS_JISSEKI);
					// ����M����ׂ���
					item->nRecvMaxNo = 4;
					//��M�`���T�C�Y (�������� �����`���T�C�Y���Z�b�g)
					item->nRecvSize[0] = sizeof(FF_ANS);
					item->nRecvSize[1] = sizeof(FF_ANS);
					item->nRecvSize[2] = sizeof(FF_STSANS);
					item->nRecvSize[3] = sizeof(FF_STSANS);


					//// DB�������ʂ�� �f�[�^�Z�b�g
					// DB��ID
					clsDB.GetData(1, item->cKeyId, SIZE_SERIAL_FF_NO);
					// �L�[��� ����No
					//clsDB.GetData(2, item->cMeiNo, SIZE_MEI_NO);
					PRO_TYPE_KEY	typKey;
					clsDB.GetData(2, (char*)&typKey, sizeof(typKey));
					memcpy(item->cMeiNo, typKey.cMeiNo, sizeof(typKey.cMeiNo));
					
					// ���M�f�[�^0 �� ���`��1
					clsDB.GetDataImage(3, item->cSendBuf[0], item->nSendSize[0]);
					// ���M�f�[�^1 �� ���`��2
					clsDB.GetDataImage(4, item->cSendBuf[1], item->nSendSize[1]);
					// ���M�f�[�^0 �� ���`��3
					clsDB.GetDataImage(5, item->cSendBuf[2], item->nSendSize[2]);
					// ���M�f�[�^1 �� ���`��4
					clsDB.GetDataImage(6, item->cSendBuf[3], item->nSendSize[3]);

					LOG(em_MSG), "[%s] �r���ѓ`�� �擾���� [ID=%s]", my_sThreadName, item->cKeyId);
					break;

				} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
					LOG(em_INF), "[%s] �r���я�� �����M�f�[�^����", my_sThreadName);
					return false;
				} else {								// �G���[
					LOG(em_ERR), "[%s] DB�ǎ挋�ʎ擾�G���[[%s]", my_sThreadName, cDbName);
					syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
					return false;
				}
			} else {
				// �e�X�g���[�h�� �t�@�C���w��̏ꍇ
				
				LOG(em_INF), "[%s] �f�o�b�O ���ѓ`�����M�v��=====", my_sThreadName);
				LOG(em_INF), "[%s]  �������ѓ`��(�\) [%s]", my_sThreadName, deli->cOmtFileName);
				LOG(em_INF), "[%s]  �������ѓ`��(��) [%s]", my_sThreadName, deli->cUraFileName);
				LOG(em_INF), "[%s]  ������ԓ`��(�\) [%s]", my_sThreadName, deli->cStOmtFileName);
				LOG(em_INF), "[%s]  ������ԓ`��(��) [%s]", my_sThreadName, deli->cStUraFileName);

				//// ���̑��Œ� �f�[�^�Z�b�g
				// �`�����
				item->emStat = deli->eStat;
				// ���Z�b�g���Ă��邩
				item->nSendMaxNo = 2;
				// ���M�`���T�C�Y (�������ѓ`���T�C�Y���Z�b�g)
				item->nSendSize[0] = sizeof(FF_JISSEKI);
				item->nSendSize[1] = sizeof(FF_JISSEKI);
				item->nSendSize[2] = sizeof(FF_STS_JISSEKI);
				item->nSendSize[3] = sizeof(FF_STS_JISSEKI);
				// ����M����ׂ���
				item->nRecvMaxNo = 4;
				//��M�`���T�C�Y (�������� �����`���T�C�Y���Z�b�g)
				item->nRecvSize[0] = sizeof(FF_ANS);
				item->nRecvSize[1] = sizeof(FF_ANS);
				item->nRecvSize[2] = sizeof(FF_STSANS);
				item->nRecvSize[3] = sizeof(FF_STSANS);

				sprintf( item->cKeyId, "%d%d%d %d%d%d", STime.wYear, STime.wMonth, STime.wDay, STime.wHour, STime.wMinute, STime.wSecond );

				// �r���я��e�X�g���M�`���i�\�j�t�@�C���Z�b�g
				if( ! ReadTestFile( item->cSendBuf[0], item->nSendSize[0], deli->cOmtFileName ) )
				{
					item->emStat = (QUE_SEND_STAT)0;
					return false;
				}
				// �r���я��e�X�g���M�`���i���j�t�@�C���Z�b�g
				if( ! ReadTestFile( item->cSendBuf[1], item->nSendSize[1], deli->cUraFileName ) )
				{
					item->emStat = (QUE_SEND_STAT)0;
					return false;
				}
				// �r��ԃe�X�g���M�`���i�\�j�t�@�C���Z�b�g
				if (!ReadTestFile(item->cSendBuf[2], item->nSendSize[2], deli->cStOmtFileName))
				{
					item->emStat = (QUE_SEND_STAT)0;
					return false;
				}
				// �r��ԃe�X�g���M�`���i���j�t�@�C���Z�b�g
				if (!ReadTestFile(item->cSendBuf[3], item->nSendSize[3], deli->cStUraFileName))
				{
					item->emStat = (QUE_SEND_STAT)0;
					return false;
				}
				FF_JISSEKI* pJsk;
				pJsk = (FF_JISSEKI*)&item->cSendBuf[0];
				memcpy( item->cMeiNo, pJsk->Hd.Meino, sizeof(pJsk->Hd.Meino) );
			}
		}
		break;

//-----------------------------------------------------------------------------------------------
	case QUE_SEND_MAE:									// �O�H�����їv��
		{
			// �Œ�f�[�^
			// �`�����
			item->emStat = deli->eStat;
			// ���Z�b�g���Ă��邩
			item->nSendMaxNo = 1;
			// ���M�`���T�C�Y (�O�H���r���їv���`���T�C�Y���Z�b�g)
			item->nSendSize[0] = sizeof(FF_MAE_REQUEST);
			// ����M����ׂ���
			item->nRecvMaxNo = 2;
			// ��M�`���T�C�Y (�O�H���r���`���T�C�Y���Z�b�g)
			item->nRecvSize[0] = sizeof(FF_MAE_JISSEKI);				
			item->nRecvSize[1] = sizeof(FF_MAE_JISSEKI);


			//// �N��������� �f�[�^�Z�b�g
			// ����No
			cWk.Format("%04s", deli->cMeiNo);
			memcpy(item->cMeiNo, cWk, SIZE_MEI_NO);
			// �R�C��No
			memcpy(item->cCoilNo, deli->cCoilNo, sizeof(deli->cCoilNo));
			// �R�C�����肷�邽�߂̃L�[���
			memcpy(&item->keyinf, &deli->keyinf, sizeof(deli->keyinf));

			// �`������
			FF_MAE_REQUEST& pBuf = (FF_MAE_REQUEST&)item->cSendBuf[0];
			memset(&pBuf, 0x20, sizeof(FF_MAE_REQUEST));


			// �`���R�[�h
			memcpy( pBuf.HeaderCode, FF_TC_CODE_MAEREQ, sizeof(pBuf.HeaderCode));
			// �A��
			memcpy( pBuf.Rec_Code, "00", sizeof(pBuf.Rec_Code));
			// �`����������
			cWk.Format("%04d%02d%02d%02d%02d%02d", STime.wYear, STime.wMonth, STime.wDay, STime.wHour, STime.wMinute, STime.wSecond); 
			memcpy( pBuf.DateTime, cWk, sizeof(pBuf.DateTime));
			// �H���R�[�h
			memcpy( pBuf.Line_Code, deli->cLine_Code, SIZE_KOUTEI_CODE);
			// ���ߔԍ�
			memcpy( pBuf.MeireiNo, item->cMeiNo, SIZE_MEI_NO);
			
			LOG(em_MSG), "[%s] �O�H�����їv���`�� �������� [�R�C��No=%s][����No=%s]", my_sThreadName, item->cCoilNo, item->cMeiNo);
		}
		break;

//-----------------------------------------------------------------------------------------------
	case QUE_SEND_HELCY:								// �w���V�[�`�����M�v��
		{
			// �Œ�f�[�^
			// �`�����
			item->emStat = deli->eStat;
			// ���Z�b�g���Ă��邩
			item->nSendMaxNo = 1;
			// ���M�`���T�C�Y (�w���V�[�`���T�C�Y���Z�b�g)
			item->nSendSize[0] = sizeof(FF_HELCY);			
			item->nRecvMaxNo = 1;
			// ��M�`���T�C�Y (�w���V�[�����`���T�C�Y���Z�b�g)
			item->nRecvSize[0] = sizeof(FF_HELCY);	
			

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
// ���ѓ`�����M����
// char const* cKey ��L�[
// FF_SENDKIND_NO emKind �`���敪�̐ݒ�l
//------------------------------------------
bool SockDataExec::SetZisekiSendAnser(char const* cKey, FF_SENDKIND_NO emKind)
{
	//// �`�F�b�N
	if(0 == strlen(cKey)) {
		LOG(em_ERR), "[%s] ���ѓ`�����M���ʔ��f�ُ�B��L�[���� [%s]", my_sThreadName, cKey);
	}

	//// SQL����
	CString	sql;
	CString cDbName;
	cDbName = DB_BUF_FF_S_ZISEKI;

	sql = "UPDATE ";
	sql += cDbName;
	sql += " SET ";
	CreateSQL::sqlColumnUpdate(&sql, "�`���敪", emKind); 
	sql += "���M���t=";	CreateSQL::sqlColumnAddTime(&sql);
	sql += " WHERE ID=";
	CreateSQL::sqlColumnInsert(&sql, cKey, SIZE_SERIAL_FF_NO);		// ID
	CreateSQL::sqlColumnTrim(&sql);									// �Ō�̃J���}����菜��

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[[%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_CONNECT_ERR, "[%s]", cDbName);
		return false;
	}


	//// ���s
	if( ! clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] ���ѓ`�����M���� UPDATE�G���[ [%s]", my_sThreadName, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	return true;
}
//------------------------------------------
// �O�H�����уo�b�t�@�o�^
// ExecItem const* item ����M�f�[�^
//------------------------------------------
bool SockDataExec::InsertMaeBuf(ExecItem const* item)
{
	char cId[SIZE_SERIAL_FF_NO];
	KizuFunction::GetSerialNoFF(cId);		// ID�쐬

	//// SQL����
	CString	sql;
	CString cDbName;
	cDbName = DB_BUF_FF_R_MAEINF;

	//======================================================	
	//// FF�O�H���r����M�o�b�t�@�e�[�u��
	// �w�b�_�[��
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	


	// �f�[�^��
	CreateSQL::sqlColumnInsert(&sql, cId, SIZE_SERIAL_FF_NO);						// ID
	CreateSQL::sqlColumnAddTime(&sql, true);										// �o�^���t
	CreateSQL::sqlColumnInsert(&sql, (char*)&item->keyinf, sizeof(item->keyinf));	// �L�[���
	sql += "?, ?";																	// ���`��
	sql += ")";

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[[%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_CONNECT_ERR, "[%s]", cDbName);
		return false;
	}
	if( ! clsDB.SqlBinalyWrite(sql, item->nRecvSize[0], item->nRecvSize[1], (LPVOID)item->cRecvBuf[0], (LPVOID)item->cRecvBuf[1] )) {
		LOG(em_ERR), "[%s] �O�H���`������ INSERT�G���[ [%s]", my_sThreadName, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}


	//// �O�H�����o�^�ł����ׁATO_MAEDAT�ɒʒm
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	que.nEventNo = FACT_TO_MAEDAT_03;
	que.nLineNo = getlineid();
	memcpy(que.fc.data , cId, SIZE_SERIAL_FF_NO);
	send_mail( TO_MAEDAT, ".", &que);

	LOG(em_MSG), "[%s] �O�H�����ѓo�^���� [ID=%s]", my_sThreadName, cId);

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

