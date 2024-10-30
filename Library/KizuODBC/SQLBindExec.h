#pragma once
// *********************************************************************************
//	VB.NET����SQLCommand�݂����ȃN���X��ODBC�ŁB
//	[Ver]
//		Ver.01    2015/09/07  
//
//	[����]
//		�ЂƂ�OdbcManager���畡����SQLBindExec�������ł���B�Ȃ�ׂ��g���܂킷�̂��悢�B
//		�Ȃ��A��{��OdbcManager�̐ؒf���ɂ́A�q����SQLBindExec�͑S�ł���B�����o�ϐ��Ŏ��Ƃ��͒��ӁB
//
//		Bind�̓|�C���^�ɂ���̂ŁAExecute����Ƃ��܂Ŏ��̂����݂��邱�ƁB�i���Ɏ����ϐ��̎����ɒ��Ӂj�B
//		CString��Bind����Ƃ��́ABind�`Execute�Ԃł͒��g��ύX���Ȃ����ƁiCString�����̃A�h���X���ύX������NG�j�B
//	[SQL]
//		INSERT���́A"INSERT INTO [TABLE]"��OK. VALUES (?,?...)�͎����t�^�BBind��ColName�͕s�g�p�Ȃ̂Ŏ��R�B
//		UPDATE���́A"UPDATE [TABLE] WHERE ..." ��OK. SET a=?,b=?...�͎����t�^�B
//			�������ASET����WHERE���̃p�����^��Bind��ColName�ŋ�ʁBSET���͗񖼁AWhere����NULL�ɂ��邱�ƁB
//			SET����SQL�֐����g�������ꍇ�́ABindSQLFunction��"��=�֐���"�ŗ^���邱�ƁB
//
//		SELECT����SQLBindRecvColumn�����������ƁB
// *********************************************************************************

#include "SQLBindBase.h"


namespace KizuODBC
{
	class OdbcManager;
	class OdbcBase;

	typedef  SQLRETURN RequestBlobDataType(class SQLBindExec *that, SQLPOINTER pToken);

	//ODBC�̃X�e�[�g�����g��S������N���X. �p�����^���C�Y�h�N�G���G
	//���x��Execute�����s����Ƃ��p�ɁB
	class SQLBindExec : public SQLBindBase
	{
	protected:
		SQLHSTMT m_hDBC;		//��{�I�ɂ͎Q��
		SQLHSTMT m_hStmt;		//���L��

		const char *m_original_sql;
		char *m_replacedSQL;
		const char *m_sQuery;	//�N�G��������
		std::deque<SQLLEN> m_szHolder;		//Bind�ϐ��̃T�C�Y�z���_�[.[1]������B[0]�͖��g�p�B
		std::deque<const char *>m_sColNames;

		SQLLEN* set_holder(const char *sColName, int len);

		bool m_bPrep;			//�v���y�A�[�h�Ȃ�TRUE.

		SQLCHAR					m_cLastErrMsgMain[256];

		//m_hStmt����G���[���b�Z�[�W���擾����B
		int RecordError();

		//���s�����B��SQL��VALUES(?,?,?)�ȂǑ����Ė{�Ԃɐ؂�ւ���
		virtual void SQLBindExec::CreateSQL();

	public:
		SQLHSTMT GetStmt() const { return m_hStmt; }
		const char *QueryString() const { return m_sQuery; }
		//�Ō�Ɏ��s�����ۂ̃G���[���b�Z�[�W��Ԃ��B
		const char *GetErrMsg() const { return (const char*)m_cLastErrMsgMain; }


		//Manager: �ڑ��ς݂�ODBCManager�B
		// sQeury: �\�z����N�G���BINSERT��UPDATE�ɂ͓��Ⴊ����
		//			INSERT���́A"INSERT INTO [TABLE]"��OK. VALUES (?,?...)�͎����t�^�BBind��ColName�͕s�g�p�Ȃ̂Ŏ��R�B
		//			UPDATE���́A"UPDATE [TABLE] WHERE ..." ��OK. SET a=?,b=?...�͎����t�^�B
		//				�������ASET����WHERE���̃p�����^��Bind��ColName�ŋ�ʁBSET���͗񖼁AWhere����NULL�ɂ��邱�ƁB
		//				SET����SQL�֐����g�������ꍇ�́ABindSQLFunction��"��=�֐���"�ŗ^���邱�ƁB
		//  bPrep: Prepare���邩�B����C���X�^���X�łT��ȏ�Execute����\��Ȃ�True.
		// �@  �@�@�������A�X�g�A�h���Ăяo���ꍇ�́Afalse�̂ق����悢�B
		SQLBindExec(KizuODBC::OdbcManager &manager, const char *sQuery, bool bPrep = false);
		//SQLBindExec(KizuODBC::OdbcBase &manager, const char *sQuery, bool bPrep = false);
		~SQLBindExec(void);

		//BIND�֘A����
			//�_�~�[�p�����[�^�B
		class NoParameter {};
		static NoParameter *NoParam() { return 0; };
		int BindSQLFunction(const char *sColName)
		{
			set_holder(sColName, 0);
			return SQL_SUCCESS;
		}

		//�p�����^���o�C���h�B
		// ���ӁBODBC�̃o�C���h�́A�u�ϐ��̃A�h���X�v�Ɓu�H�v���q���Â���B
		// �o�C���h����Ă����(���C���X�^���X���������Ă����)�́A�ϐ��͗L���ȃA�h���X�ł���K�v������B
		// �|�C���^��o�^���Ă���̂ŁAmemcpy�ȂǂŎ��̑��̃��������e��ύX����Execute����΁A�p�����^���ς���Ă���B

		//���l
		int BindNum(const char *sColName, const int *nPrepKey);
		int BindNum(const char *sColName, const unsigned int *nPrepKey) { return BindNum(sColName, (const int*)nPrepKey); }
		int BindNum(const char *sColName, const long *nPrepKey) { return BindNum(sColName, (const int*)nPrepKey); }
		int BindNum(const char *sColName, const unsigned long *nPrepKey) { return BindNum(sColName, (const int*)nPrepKey); }
		int BindNum(const char *sColName, const float *nPrepKey);
		int BindNum(const char *sColName, const double *nPrepKey);

		//������B�inMaxColumnWidth�́ADB���̕��B�N�G���̑g����(Cache�̓�����)�Ɍ����Ă���B�j
		int BindStr(const char *sColName, const char *pPrepKey, int nMaxColumnWidth = -1);
		//�Œ蒷������iC��������NUL�I�[�����A���nColumnWidth�����̃T�C�Y��o�^����BNUL�����������DB�ɂ�NUL����������̂Œ��Ӂj
		int BindFix(const char *sColName, const char *pPrepKey, int nMaxColumnWidth);
		//UUID�BFile Stream�̂Ƃ��ɏo�Ă��邩���B
		int Bind(const char *sColName, const UUID *pPrepKey);
		//SQL�����^�B
		int Bind(const char *sColName, const SQL_TIMESTAMP_STRUCT *pTimeStamp);

		//blob( Image�Ƃ�VarBinary MAX�Ƃ�) �̃o�C���h�B
		// ���ۂ̃f�[�^�͌�񂵁B
		//  �񋟂́ABlobDataRequested�Ɋ֐������ăR�[���o�b�N��҂�
		//  ���̃N���X���p������OnRequestBlobData���I�[�o�[���C�h���邩�B
		//�@Execute�̒���ɃR�[���o�b�N���Ă΂��B
		// pToken : �g�[�N���B�u���ۂ̃f�[�^�v�ɕR�Â��邽�߂�ID�B 
		//�@�@�@�@�@���Ƃ��΁AOnBlobData_SendFile�Ȃ�A�t�@�C�����ւ̃|�C���^������B
		//nMaxSize: �قƂ�ǃ_�~�[�B���f�[�^���傫���Ė��Ȃ����A�������ƃG���[�ɂȂ�B

		// image, varbinary(max) 
		int BindBlobBin(const char *sColName, const void *pToken, int nMaxSize = (1 << 30));

		// varchar(max)
		int BindBlobChar(const char *sColName, const void *pToken, int nMaxSize = (1 << 30));

		//���[�e�B���e�B�I���\�b�h�B


		//�P���Ɏ��s����B
		bool Execute();

		//���s����B���s��AHSTMT��OdbcManager�ɓ]������B
		//�Ȃ̂ŁAOdbcManager����Fetch�Ƃ�GetData�������g����
		SQLRETURN SelectExecute(KizuODBC::OdbcManager &manager);

		//Blob�f�[�^���M����
			//Execute�̍ہAbindBlob����������
		virtual SQLRETURN OnRequestBlobData(SQLPOINTER pToken);
		//�f�t�H���g�ł́AOnRequestBlobData�� BlobDataRequested���Ă�
		RequestBlobDataType *BlobDataRequested;

		//Blob�Ƃ��ăt�@�C���𑗐M�BpToken���t�@�C���p�X��������Ӗ�����Ƃ��āA���̃t�@�C���𑗐M����B
		static SQLRETURN OnBlobData_SendFile(SQLBindExec *that, SQLPOINTER pToken);

		//�t�@�C���T�C�Y���Ƃ�B�{���I�ɂ͕s�v����SendFile����Ƃ��Ƀt�@�C���T�C�Y���ق����Ȃ�̂͂悭����B
		static long long GetFileSize(const char *filename);

		struct SendMemoryType {
			long long size;
			const void *data;
		};

		//Blob�Ƃ��ă������𑗐M�BpToken��SendMemoryType�^���Ӗ�����Ƃ��āA���̃������𑗐M����B
		static SQLRETURN OnBlobData_SendMemory(SQLBindExec *that, SQLPOINTER pToken);

		// Callback�̍ہAbidnBlob���ŗv�����ꂽ�f�[�^�𑗏o���镔���B
		SQLRETURN PutData(SQLPOINTER pData, SQLINTEGER nLen);
		SQLRETURN PutNULL() { return PutData(0, 0); }
		SQLRETURN PutEmpty() { return PutData("", 0); }

	};

};