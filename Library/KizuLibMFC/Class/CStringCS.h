#pragma once

// �r���L���CString ��킹�N���X (singleton)
// �����݂ƎQ�Ƃ��ʂ̃X���b�h�̏ꍇ�̎�n�p
//
//--------------
//�z��P�[�X�P
//  CoilManager 
//		CString  m_KanriNo;
//		CString  GetKanriNo() { return m_KanriNo;}
//
//  CoilMgr ��
//		m_KanriNo = sVal;
//
//  Dlg ��
//		CoilMgr->GetKanriNo()
//  �̏ꍇ�A 
//  CoilMgr ) �Â�m_KanriNo ���폜
//  Dlg ) GetKanriNo() �o�R�� m_KanriNo �̒��g���R�s�[
//  CoilMgr ) m_KanriNo �̒��g��V�������B
// �ƂȂ��āADlg�Ŏ���
//
//  ���̂��߁A
//		CString  m_KanriNo;  ��  CStringCS m_KanriNo�ɕύX
//
//
//--------------
//�z��P�[�X�Q
//  ��L�ُ̈�̂Ƃ��ɁACStringCS�ɒu������������ȏꍇ�B
//		CString* �ł����Ȋ֐��ɓn���Ă���
//		�z���g�ɐG�肽���Ȃ��^�X�N�Ȃ�
//	���̏ꍇ�A�ȉ��Ŏ��{����ƁA�v���Z�X�ԂŔr������ł��邽�߁A
//  �ǂ݁A���� �����̂ݔ�킹���邱�ƂŁA������̂�h�~����
//
//  CStringCS::LockCpy(&cstringto, &cstringfrom);
//  sWk = CStringCS::LockCpy(&cstringfrom);
//	
namespace KizuLib
{
	// �{��
	class CStringCS
	{

		//----------
		// singleton
		//    LockCpy�p
		class CStringEnv
		{
		//// �d�v�Bexe�ŗB��̃C���X�^���X��N���X��
		private:
			static CStringEnv				m_inst;											// exe�ŗB��̃C���X�^���X�B ����������new����āA�I������delete�����.
			mutable CRITICAL_SECTION		m_csLock;										// ���N���X�̑��C���X�^���X�Ԃ̔r������
			//CRITICAL_SECTION		m_csLock;	

		//// ���J�͂��ׂ�static
		public:
			static CStringEnv* GetIns() { 
				static CStringEnv ins; 
				return &ins;
			}
			//static CStringEnv* GetIns() { return &m_inst;}
			static inline CRITICAL_SECTION* GetCS() { 
				return &GetIns()->m_csLock;
			}

		//// ����
		private:

			//// �R���X�g���N�^(singleton)
			CStringEnv(void)																	// private�R���X�g���N�^
			{
				InitializeCriticalSection(&m_csLock);
			}

			//// �R�s�[�R���X�g���N�^
			CStringEnv(const CStringEnv &ob) {}
			// ���
			CStringEnv& operator=(const CStringEnv& ob) {}

		public:
			virtual ~CStringEnv(void)
			{ 
				DeleteCriticalSection(&m_csLock); 
			}
		};




	//// �����o�[�萔
	protected:
		CString						m_sVal;								// ����
		mutable CRITICAL_SECTION	m_csLock;							// ���N���X���̔r������

	public:
		CStringCS() 
		{
			InitializeCriticalSection(&m_csLock);
		}
		virtual ~CStringCS(void)
		{ 
			DeleteCriticalSection(&m_csLock); 
		}

		CStringCS(const char* c) 
		{
			AutoLock lock(&m_csLock);
			m_sVal = c;
		}

		//// �R�s�[�R���X�g���N�^
		CStringCS(CStringCS const& ob)
		{
			AutoLock lock(&m_csLock);
			this->m_sVal = ob.m_sVal;
		}


		//// �Z�b�g (�悭�g�����̂̂�)
		//		������ȊO��Format��Append �Ȃǂ́ACSring�ł���Ă���{�N���X�ɃZ�b�g
		CStringCS& operator = (const CStringCS& ob)
		{
			AutoLock lock(&m_csLock);
			m_sVal = ob.m_sVal;
			return *this;
		}
		CStringCS& operator = ( const CString& ob )
		{
			AutoLock lock(&m_csLock);
			m_sVal = ob;
			return *this;
		}
		CStringCS& operator = (char* ob)
		{
			AutoLock lock(&m_csLock);
			m_sVal = ob;
			return *this;
		}


		//// �|�C���^�̃I�[�o�[���[�h�͐�΂Ƀ_���B�Ӗ��𐬂��Ȃ�����
		//CString* operator & ()
		//{
		//	_ASSERT(false);
		//	//AutoLock lock(&m_csLock);
		//	return NULL;
		//}

		//// �ÖٓI�^�ϊ�
		operator CString () const {
			AutoLock lock(&m_csLock);
			return m_sVal;		// CString �� �R�s�[�R���X�g���N�^�����邩��A�n�j�B����d�v
		}


	//// �O������p
	// ���݉��������Ȃ��Ȃ�悤�ɂ��邽�߂̏��u
	// ��{�͏���g���Ăق����B
	// �v���Z�X�ԂŁA���b�N�\
	public:
		static void LockCpy(CString* to, CString* from) {
			AutoLock lock(CStringEnv::GetCS());
			*to = *from;
		}

		static void LockCpy(CString* to, char const* from) {
			AutoLock lock(CStringEnv::GetCS());
			//Sleep(20000);  // �e�X�g
			*to = from;
		}

		static CString LockCpy(CString* from)
		{
			AutoLock lock(CStringEnv::GetCS());
			return *from;	// CString �̃R�s�[�R���X�g���N�^������͂�
		}
		static CString LockCpy(CString& from)
		{
			AutoLock lock(CStringEnv::GetCS());
			return from;	// CString �̃R�s�[�R���X�g���N�^������͂�
		}

	};


/*
	class CStringCS_back
	{
		
	//// �����o�[�萔
	protected:
			mutable	CRITICAL_SECTION	m_csLock;							// �N���e�B�J���Z�N�V����
			CString						m_sVal;								// ����

	//// ����
	public:
		CStringCS(void) 
		{ 
			InitializeCriticalSection(&m_csLock);
		}
		CStringCS(const char* c) 
		{ 
			InitializeCriticalSection(&m_csLock);

			AutoLock lock(&m_csLock);
			m_sVal = c;
		}
		virtual ~CStringCS(void)
		{ 
			DeleteCriticalSection(&m_csLock); 
		}

		//// �R�s�[�R���X�g���N�^
		CStringCS(CStringCS const& ob)
		{
			InitializeCriticalSection(&this->m_csLock);
			this->m_sVal = ob.m_sVal;
		}


		//// �Z�b�g (�悭�g�����̂̂�)
		//		������ȊO��Format��Append �Ȃǂ́ACSring�ł���Ă���{�N���X�ɃZ�b�g
		CStringCS& operator = (const CStringCS& ob)
		{
			AutoLock lock(&m_csLock);
			m_sVal = ob.m_sVal;
			return *this;
		}
		CStringCS& operator = ( const CString& ob )
		{
			AutoLock lock(&m_csLock);
			m_sVal = ob;
			return *this;
		}

		CStringCS& operator = (char* ob)
		{
			AutoLock lock(&m_csLock);
			m_sVal = ob;
			return *this;
		}



		//CStringCS& operator * ()
		//{
		//	AutoLock lock(&m_csLock);
		//	m_sVal = ob;
		//	return *this;
		//}

		// �|�C���^�̃I�[�o�[���[�h�͐�΂Ƀ_���B�Ӗ��𐬂��Ȃ�����
		//CString* operator & ()
		//{
		//	AutoLock lock(&m_csLock);
		//	return &m_sVal;
		//}

		//// �ÖٓI�^�ϊ�
		operator CString () const {
			AutoLock lock(&m_csLock);
			return m_sVal;		// CString �� �R�s�[�R���X�g���N�^������
		}
	};
*/
};