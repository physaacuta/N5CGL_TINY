// *********************************************************************************
//	SQL���쐬�T�|�[�g �N���X
//	[Ver]
//		Ver.01    2007/04/21  vs2005 �Ή�
//
//	[����]
//		OdbcBase�ł�CString���g���Ȃ��ׁA���̃N���X������Ă݂�
//		������Ƃ����z�Ȃ�,OdbcBase��char�^��SQL���쐬�ł����Ȃ�
// *********************************************************************************
#pragma once
#pragma warning(disable : 4996)							// ***���Â��`���Ƃ��Đ錾����܂����B���o���Ȃ��΍�

namespace KizuLib
{
	class CreateSQL
	{
	public:
		CreateSQL(void);
		~CreateSQL(void);
		
		static void sqlColumnInsert(CString *cSql, const char *val, int size);
		static void sqlColumnInsert(CString *cSql, short val);	
		static void sqlColumnInsert(CString *cSql, int val);						
		static void sqlColumnInsert(CString *cSql, long val);
		static void sqlColumnInsert(CString *cSql, DWORD val);
		static void sqlColumnInsert(CString *cSql, float val);
		static void sqlColumnInsert(CString *cSql, double val);
		static void sqlColumnInsert(CString *cSql, const char *val);
		static void sqlColumnInsert(CString *cSql, CString val);

		// update
		static void sqlColumnUpdate(CString *cSql, const char *name, const char *val, int size);
		static void sqlColumnUpdate(CString *cSql, const char *name, short val);
		static void sqlColumnUpdate(CString *cSql, const char *name, int val);
		static void sqlColumnUpdate(CString *cSql, const char *name, long val);
		static void sqlColumnUpdate(CString *cSql, const char *name, DWORD val);
		static void sqlColumnUpdate(CString *cSql, const char *name, float val);
		static void sqlColumnUpdate(CString *cSql, const char *name, double val);
		static void sqlColumnUpdate(CString *cSql, const char *name, CString val);

		// ����ȊO
		static void sqlColumnTrim(CString *cSql);									// sqlColumnInsert,sqlColumnUpdate�ōŌ�ɂ��� ","���������
		static void sqlColumnAddTime(CString *cSql, bool bAddCanma=false);			// ������t�^
		static void sqlColumnAddTime(CString *cSql, CTime t, bool bAddCanma=false);


		static CString StringToTime(char const* pWk);								// ������[yyyymmddhhnnss]�`������[yyyy/mm/dd hh:nn:ss]�`���ɕϊ�
		static CString StringTrim(const char *val, int size);						// �����񂩂�O��̃X�y�[�X����菜��

	};
};