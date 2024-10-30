#include "StdAfx.h"
#include "DefectDumpManager.h"


// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


//#include "MainInctance.h"

//------------------------------------------
// �R���X�g���N�^
// int		TorB		0:TOP 1:BOT	
// LPCSTR	filename	���O�t�@�C���� (NULL�� ���O�o�͂��Ȃ�)
// int		cycleCnt	�ő�T�C�N���b�N�t�@�C������
// int		backCnt		�ő�o�b�N�A�b�v����
//------------------------------------------
DefectDumpManager::DefectDumpManager(int TorB, LPCSTR filename, int cycleCnt, int backCnt) :
m_nMaxCycleCnt(cycleCnt),
m_nMaxBackCnt(backCnt),
m_nNowRow(0),
m_nNowFileNo(0),
gque_Deli(DEFECTDUMPMANAGER_MAX_QUE),
ThreadManager()
{
	CString cWk;
	cWk.Format("DefMgr_%d", TorB);
	my_sThreadName = cWk;
	
	//// ������
	InitializeCriticalSection(&m_csOutPut);
	InitializeCriticalSection(&m_csANC);

	//// �o�͂��邵�Ȃ�����
	m_bFileOut = NULL==filename ? false : true;
	
	//// �o�b�N�A�b�v (�t�@�C���o�͗L�莞�̂�)
	if ( m_bFileOut ) {
		int ii, jj;
		CString cFileName;
		CString cFileNameNew;
		char	cBaseFolder[32];

		// ���O�t�H���_����
		GetPrivateProfileString("TO_DEFECT", "DEF_DUMP_PATH", "C:\\CSV", cBaseFolder, 32, TASKINI_NAME);
		_ASSERT( strlen(cBaseFolder) );

		// �\�ʂ̂�̫��ލ쐬
		if (0 == TorB) _mkdir(cBaseFolder);
			
		// �x�[�X���̌���
		//char cwk[256];
		//GetCurrentDirectory( 256, cwk);
		//m_cBaseFolder += cwk;
		//m_cBaseFolder += "\\Csv\\";
		m_cBaseFolder.Format("%s\\", cBaseFolder);
		m_cBaseFile = filename;

		// bk_xxx_@9_0.log �` bk_xxx_@9_9.log���폜  (�ŌÃt�@�C���폜)
		for( ii=0; ii<=m_nMaxCycleCnt; ii++ ) {
			cFileName.Format("%sbk_%s_@%d_%d.csv", m_cBaseFolder, m_cBaseFile, m_nMaxBackCnt, ii );
			remove(cFileName);
		}

		// bk_xxx_@0_0.csv �` bk_xxx_@0_9.csv �� bk_xxx_@1_0.csv �` bk_xxx_@1_9.csv �ɕύX (�o�b�N�A�b�v�t�@�C�����l�[��)
		for( ii=0; ii<=m_nMaxCycleCnt; ii++ ) {
			for( jj=m_nMaxBackCnt; jj>0; jj-- ) {
				cFileName.Format("%sbk_%s_@%d_%d.csv", m_cBaseFolder, m_cBaseFile, jj-1, ii);	// �ϊ���
				cFileNameNew.Format("%sbk_%s_@%d_%d.csv", m_cBaseFolder, m_cBaseFile, jj, ii);	// �ϊ���
				rename(cFileName, cFileNameNew);
			}
		}

		// xxx_0.log�`xxx_9.csv �� bk_xxx_@0_0.csv �` bk_xxx_@0_9.csv �ɕύX (�ŐV�t�@�C�����o�b�N�A�b�v�t�@�C���Ƀ��l�[��)
		for( ii=0; ii<=m_nMaxCycleCnt; ii++ ) {
			cFileName.Format("%s%s_%d.csv", m_cBaseFolder, m_cBaseFile, ii);					// �ϊ���
			cFileNameNew.Format("%sbk_%s_@0_%d.csv", m_cBaseFolder, m_cBaseFile, ii);			// �ϊ���
			rename(cFileName, cFileNameNew);
		}

		// CSV�t�@�C���I�[�v��
		cFileName.Format("%s%s_%d.csv", m_cBaseFolder, m_cBaseFile, m_nNowFileNo);
		m_stream.open(cFileName, std::ios::out | std::ios::trunc);
		m_cNowFPath = cFileName;
	}

}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
DefectDumpManager::~DefectDumpManager()
{
	// �t�@�C���N���[�Y
	//if ( m_stream != NULL ) {
	if ( m_stream.is_open() ) {
		// �N���[�Y
		m_stream.close();
		//m_stream = NULL; //�����ꂪ�_���B���肪������Ȃ�
	}
	// �J��
	DeleteCriticalSection(&m_csOutPut);
	DeleteCriticalSection(&m_csANC);
	
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int DefectDumpManager::ThreadFirst()
{
	//// �C�x���g�ݒ�
	HANDLE hArray[] = { gque_Deli.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�X���b�h�I�����O�ɃR�[���o�b�N)
//------------------------------------------
int DefectDumpManager::ThreadLast()
{
	//// �S�r���J��		
	AllDeleteQueue();

	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void DefectDumpManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���		
	enum {	EV_QUE = 0,								// �p�����[�^�ύX�ʒm
			EV_END	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE:									// �p�����[�^�ύX�ʒm
		if(true) {
			DEFDUMP_QUE* pDeli = gque_Deli.GetFromHead();

			// �t�@�C���o��
			if( m_bFileOut) {
				if(0 > pDeli->nRecord) FlushFile();
				else				   WriteFile(pDeli->nRecord, pDeli->pStr);
			}
			SAFE_DELETE_ARRAY(pDeli->pStr);
			SAFE_DELETE(pDeli);
		}
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		break;
	}
}


//------------------------------------------
// �񓯊��������݃G���g���[
// int nRecord �R�C����񃌃R�[�hNo
// LPCSTR __cBuffer �ϔz�񃁃b�Z�[�W
//------------------------------------------
void DefectDumpManager::Write( int nRecord, LPCSTR __cBuffer, ... )
{
	char	cMsg[20480];
    SYSTEMTIME time;
    GetLocalTime(&time);

	AutoLock autolock(&m_csANC);

	//// �ψ������A���b�Z�[�W�쐬
	memset( cMsg, 0x00, sizeof(cMsg));
	va_list	vmarker;
	va_start( vmarker, __cBuffer ); 				// �ό̈����̏�����
//	_vsnprintf( (char*)cMsg, _TRUNCATE, __cBuffer, vmarker );
	_vsnprintf( (char*)cMsg, sizeof(cMsg)-1, __cBuffer, vmarker );
	va_end( vmarker );								// �ό̈����̃��Z�b�g
	
	//// �������ݗp�o�b�t�@�쐬 (�g���I�������K���J�����鎖)
	int		nSize;
	nSize = (int)strlen(cMsg) + 256;			// 256�� ���� �� �L���[�������Ƀ��b�Z�[�W ���Ŏg�p�����
	LPSTR pStr = new char [nSize];							// CString �� char*�ɕύX (�A���ŏ������ނ� 0.2ms ���� 0.1ms�ɒZ�k)
	// ���b�Z�[�W����
	sprintf(pStr, "%04d/%02d/%02d %02d:%02d:%02d.%03d%s",
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
		cMsg
		);

	//// �L���[�ɓo�^
	// �L���[�c�e�ʃ`�F�b�N
	if( 0 == gque_Deli.GetFreeCount() ) {			// �󂫂Ȃ�
		// �S���O�J��
		int cnt;
		cnt = AllDeleteQueue();
		// �񓯊��������݃��O�폜
		memset(pStr, 0x00, nSize);
		//sprintf(pStr, "%04d/%02d/%02d %02d:%02d:%02d.%03d%s%d ", 
		//	"�r���L���[�t��!! �r���폜����=", cnt,
		//	time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds
		//	);
		//return;
	}

	DEFDUMP_QUE* pQue = new DEFDUMP_QUE;
	pQue->nRecord = nRecord;
	pQue->pStr = pStr;
	// �L���[�ɓo�^
	gque_Deli.AddToTail( pQue );		// str�͎擾��ɊJ��
}


//------------------------------------------
// ���������� (�ψ����Ή�)
// int nRecord �R�C����񃌃R�[�hNo
// LPCSTR __cBuffer �ϔz�񃁃b�Z�[�W
//------------------------------------------
void DefectDumpManager::WriteLock( int nRecord, LPCSTR __cBuffer, ... )
{
	char	cMsg[20480];
    SYSTEMTIME time;
    GetLocalTime(&time);

	AutoLock autolock(&m_csANC);

	//// �ψ������A���b�Z�[�W�쐬
	memset( cMsg, 0x00, sizeof(cMsg));
	va_list	vmarker;
	va_start( vmarker, __cBuffer ); 				// �ό̈����̏�����
	_vsnprintf( (char*)cMsg, sizeof(cMsg)-1, __cBuffer, vmarker );
	va_end( vmarker );								// �ό̈����̃��Z�b�g
	
	//// �������ݗp�o�b�t�@�쐬 (�g���I�������K���J�����鎖)
	int		nSize;
	nSize = (int)strlen(cMsg) + 256;			// 256�� ���� �� �L���[�������Ƀ��b�Z�[�W ���Ŏg�p�����
	LPSTR pStr = new char [nSize];							// CString �� char*�ɕύX (�A���ŏ������ނ� 0.2ms ���� 0.1ms�ɒZ�k)
	// ���b�Z�[�W����
	sprintf(pStr, "%04d/%02d/%02d_%02d:%02d:%02d.%03d%s",
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
		cMsg
		);


	// �t�@�C���o��
	if( m_bFileOut) WriteFile(nRecord, pStr);


	SAFE_DELETE_ARRAY( pStr );
}


//------------------------------------------
// ���䏑������
// int nRecord �R�C����񃌃R�[�hNo
// LPCSTR __cBuffer �ϔz�񃁃b�Z�[�W
//------------------------------------------
void DefectDumpManager::WriteFile(int nRecord, LPCSTR pStr)
{
	AutoLock autolock(&m_csOutPut);

	if(nRecord != m_nNowFileNo) {
		m_stream.close();

		m_nNowRow = 0;
		m_nNowFileNo = nRecord;
		if( (int)m_nNowFileNo < 0 ) m_nNowFileNo = 0;
		if( (int)m_nNowFileNo > m_nMaxCycleCnt ) m_nNowFileNo = m_nMaxCycleCnt;
		CString cFileName;
		cFileName.Format("%s%s_%d.csv", m_cBaseFolder, m_cBaseFile, m_nNowFileNo);
		m_stream.open(cFileName, std::ios::out | std::ios::trunc);
		m_cNowFPath = cFileName;

	}
	////// �t�@�C���o��
	//// �r���t�@�C���ɏ�������
    m_stream << pStr << std::endl;
	m_nNowRow++;
}


//------------------------------------------
// ���䋭���o��
//------------------------------------------
void DefectDumpManager::FlushFile()
{
	AutoLock autolock(&m_csOutPut);

	m_stream.flush();

}

//------------------------------------------
// �S�L���[�����o��
// �߂�l �o�͌���
//------------------------------------------
int DefectDumpManager::AllGetQueue()
{
	DEFDUMP_QUE*	pDeli;

	int cnt = 0;
	// �S�L���[���o��
	while (true) { 
		pDeli = gque_Deli.GetFromHead(0);
		if(pDeli) {
			// �t�@�C���o��
			if( m_bFileOut) WriteFile(pDeli->nRecord, pDeli->pStr);
			SAFE_DELETE_ARRAY( pDeli ) ;
			cnt++;
		} else {
			break;
		}
	}
	return cnt;
}

//------------------------------------------
// �S�L���[�����폜
// �߂�l �폜����
//------------------------------------------
int DefectDumpManager::AllDeleteQueue()
{
	DEFDUMP_QUE*	pDeli;

	int cnt = 0;
	// �S�L���[���o��
	while (true) { 
		pDeli = gque_Deli.GetFromHead(0);
		if(pDeli) {
			SAFE_DELETE_ARRAY(pDeli->pStr);
			SAFE_DELETE(pDeli);
			cnt++;
		} else {
			break;
		}
	}
	return cnt;
}

