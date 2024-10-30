#include "stdafx.h"
#include "CheckLogic.h"

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
CheckLogic::CheckLogic() :
ThreadManager("CheckLogic"),
m_bLogicUmu(DIV_BOOL_NON)
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
CheckLogic::~CheckLogic()
{
}


//------------------------------------------
// scode �̃`�F�b�N����
// int scode	�\�ʏ�� (0:�S�� 1�`:�Y��scode�̂�)
//------------------------------------------
void CheckLogic::Check(int scode)
{
	TYP_DELEV* p = new TYP_DELEV;
	p->scode = scode;
	if( ! gque_Check.AddToTailFreeCheck(p) ) {
		delete p;
		LOG(em_ERR), "[%s] gque_Check �L���[�t��!!", my_sThreadName);
	}
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int CheckLogic::ThreadFirst()
{
	//// �C�x���g�ݒ�
	AddEventNum(1, &gque_Check.g_evSem );
	return 0;
}


//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void CheckLogic::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���		
	enum {	EV_QUE = 0,									// �L���[
			EV_END	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE:										// �`�F�b�N
		{
			TYP_DELEV* pType = gque_Check.GetFromHead();
			LOG(em_MSG), "[%s] Logic�`�F�b�N�˗� <%d>", my_sThreadName, pType->scode );
			m_bLogicUmu = ExecLogic(pType->scode);
			delete pType;
		}
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		break;
	}
}

//------------------------------------------
// ���W�b�N�t�@�C���L���`�F�b�N
// int scode	�\�ʏ�� (0:�S�� 1�`:�Y��scode�̂�)
//------------------------------------------
EM_DIV_BOOL CheckLogic::ExecLogic(int scode)
{
	CString	sql;					// SQL��
	EM_DIV_BOOL	ans = DIV_BOOL_TRUE;
	
	int  val;
	char path[512];

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME) )	{
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return DIV_BOOL_NON;
	}


	//// SQL���쐬
	CString sWhere = "";
	if( 0 != scode ) { sWhere.Format("AND s.scode=%d", scode); }

	sql.Format("SELECT MIN(s.scode), s.DCDname FROM %s AS s INNER JOIN %s AS m ON s.scode=m.scode WHERE m.�L������=1 %s GROUP BY s.DCDname",
		DB_PARAM_SCODE, DB_SCODE_MASTER, sWhere);

	//// ���s
	if( ! clsDB.ExecuteDirect(sql) )
	{
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sql);
		return DIV_BOOL_NON;
	}

	//// �擾
	while( TRUE ) {
		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK )					// �f�[�^�L��
		{
			clsDB.GetData(1, &val);
			clsDB.GetData(2, path, sizeof(path));

			// �t�@�C���`�F�b�N
			{
				CFileFind fFind;
				if( !fFind.FindFile(path, 0) ) {
					ans = DIV_BOOL_FALSE;
					LOG(em_ERR), "[%s] ���W�b�N�t�@�C������ (%d)(%s)", my_sThreadName, val, path);
					syslog(120, "(%d)(%s)", val, path);
				} else {
					LOG(em_MSG), "[%s] ���W�b�N�t�@�C��OK (%d)(%s)", my_sThreadName, val, path);
				}
				fFind.Close();
			}

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) { break; }		// �f�[�^����
		else {											// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", sql);
			return DIV_BOOL_NON;
		}
	}

	return ans;
}