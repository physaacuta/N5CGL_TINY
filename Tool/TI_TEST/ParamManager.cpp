#include "StdAfx.h"
#include "ParamManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


//#include "MainInctance.h"

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
ParamManager::ParamManager(void):
ThreadManager("ParamMgr")
{
	m_evRead	= CreateEvent(NULL, FALSE, FALSE, NULL);	
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ParamManager::~ParamManager(void)
{
	CloseHandle(m_evRead);
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int ParamManager::ThreadFirst()
{
	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evRead
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void ParamManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���		
	enum {	EV_READ = 0,								// �񓯊��Ǎ�
			EV_END	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_READ:										// �񓯊��Ǎ�
		LOG(em_MSG), "[%s] DB�p�����[�^ �񓯊��Ǎ��˗�", my_sThreadName);
		ReadParam_All();
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		break;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �p�����[�^�擾�֘A

//------------------------------------------
// �S�p�����[�^�Ǎ���
//------------------------------------------
bool ParamManager::ReadParam_All()
{

	//// �ꎞ�o�b�t�@��
	if( ! Read_CommonMast() )		return false;


	//// �ꎞ�o�b�t�@����{�Ԃ�
	memcpy(&m_CommonMast, &m_CommonMastTemp, sizeof(P_COMMON_MAST));

	LOG(em_MSG), "[%s] �Ǎ��p�����[�^�Ɍ��ʔ��f", my_sThreadName);
	return true;
}

//------------------------------------------
// �r���萔�p�����[�^�Ǎ���
//------------------------------------------
bool ParamManager::Read_CommonMast()
{
	CString	sql;					// SQL��

	//// �p�����[�^������
	CString	cDbName;
	//cDbName = DB_COMMON_MASTER;
	cDbName.Format("%s CROSS JOIN %s",  DB_PARAM_COMMON, DB_COMMON_MASTER) ;
	P_COMMON_MAST & p = m_CommonMastTemp;
	P_COMMON_MAST   buf;
	memset(&buf, 0x00, sizeof(buf));

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME) )
	{
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL���쐬
	//sql = "SELECT";
	//sql += " �S���摜_�Ԉ�������, �S���摜_�Ԉ������c";
	//sql += ", �S���摜_�ۑ�_R, �S���摜_�ۑ�_G, �S���摜_�ۑ�_B, �S���摜_�ۑ�_��";
	sql = "SELECT TOP 1 ";
	sql += " [ShFrameShrinkX], [ShFrameShrinkY]";
	sql += ", �S���摜_�ۑ�_R, �S���摜_�ۑ�_G, �S���摜_�ۑ�_B, �S���摜_�ۑ�_��";

	sql += " FROM ";
	sql += cDbName;

	//// ���s
	if( ! clsDB.ExecuteDirect(sql) )
	{
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// �擾
	while( TRUE )
	{
		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK )					// �f�[�^�L��
		{
			// �f�[�^��
			clsDB.GetData(1, &buf.nImgCullX);
			clsDB.GetData(2, &buf.nImgCullY);

			clsDB.GetData(3, &buf.nImgSave[0]);
			clsDB.GetData(4, &buf.nImgSave[1]);
			clsDB.GetData(5, &buf.nImgSave[2]);
			clsDB.GetData(6, &buf.nImgSave[3]);


			// �����ŎZ�o
			for(int ii=0; ii<MAX_IMAGE_CNT; ii++) {	
				if(TRUE == buf.nImgSave[ii]) buf.nImgSaveNum ++;
			}
		}
		else if( sqlrc == KIZUODBC_FETCH_NODATE )			// �f�[�^����
		{
			break;
		}
		else												// �G���[
		{
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	//// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy(&p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, cDbName);
	return true;
}
