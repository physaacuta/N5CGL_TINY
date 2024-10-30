// *********************************************************************************
//	�񓯊��ǂݏ����w���p
//	[Ver]
//		Ver.01    2014/09/15  
//
//	[����]
//		WIN32�� OVERLAPPED�\���̂̎g���₷�����b�p��񋟂���
//
// *********************************************************************************
#pragma once

//------------------------------------------
//��{�N���X
//�@�ǂݏ����p�̃��������̂́A���̃N���X�ł͊Ǘ�(�m�ہE���)���Ȃ�
//  �P�ɃR���X�g���N�^�ŗ^�����邾���B
//------------------------------------------
class COVData : private OVERLAPPED
{
public:
	BYTE* pBuffer;
	int   szCapacity;

	//Offset��OffsetHigh�ɋ����ʂ�Ă���̂ɑΏ�
	int64_t &offset() { return *reinterpret_cast<int64_t *>(&this->Offset); }
	HANDLE GetEvent() { return hEvent; }



	//------------------------------------------
	//�R���X�g���N�^�B
	// abuffer, acapacity : �o�b�t�@�������Ƃ��̃T�C�Y
	// bCreateEvent       : �ҋ@�C�x���g�쐬���邩(IOCP�Ȃ�C�x���g�͗v��Ȃ�)
	//------------------------------------------
	COVData(BYTE* abuffer = NULL, int acapacity = 0,  bool bCreateEvent = true)
		:pBuffer(abuffer), szCapacity(acapacity)
	{
		if ( bCreateEvent)
			hEvent = CreateEvent( NULL, TRUE, FALSE, NULL);
		else
			hEvent = NULL;
	}


	//------------------------------------------
	//�f�X�g���N�^�B
	//------------------------------------------
	virtual ~COVData()
	{
		if( 0 != hEvent ){
			CloseHandle( hEvent );
		}
	}

	//------------------------------------------
	//�o�b�t�@���`�F�b�N����
	//	const BYTE *ptr, DWORD size����Ώۂɂ��郁�����͈̔�
	//�߂�l�@���������������Atrue
	//�E�Z�N�^�A���C�������g�v���𖞂����Ă��邩
	//�E�o�b�t�@�͈͓����B
	//------------------------------------------
	bool check_alignment(const BYTE *ptr, DWORD size) 
	{
		ASSERT( 0 == (int64_t(ptr) & 0x1ff) );
		ASSERT( int(&ptr[size] - pBuffer) <= szCapacity);
		return true;
	}

	//------------------------------------------
	//�ǂݍ��݊J�n�B�i�񓯊��j
	//�@�t�@�C��hFile��aoffset�ʒu���炩��size�o�C�g�f�[�^��ǂݍ��݂͂��߂�B
	//  bufstartpos �ǂݍ��ސ�̃o�b�t�@�ʒu
	//------------------------------------------
	bool IssureRead( HANDLE hFile, int64_t aoffset,  DWORD size, int bufstartpos = 0 ){
		check_alignment(  &pBuffer[bufstartpos], size ) ;
		this->offset() = aoffset;
		int r = ReadFile( hFile, &pBuffer[bufstartpos], size, NULL, this );
		if( r ) 
			return true;
		if( 0 == r && GetLastError() == ERROR_IO_PENDING )
			return true;
		return false;

	}
	
	//------------------------------------------
	//�������݊J�n�B�i�񓯊��j
	//�@�t�@�C��hFile��aoffset�ʒu���炩��size�o�C�g�f�[�^���󂫍��݂͂��߂�B
	//  bufstartpos�@�������݂Ɏg�p����o�b�t�@�̈ʒu
	//------------------------------------------
	bool IssureWrite( HANDLE hFile, int64_t aoffset, DWORD size, int bufstartpos = 0 ){
		check_alignment(  &pBuffer[bufstartpos], size) ;
		this->offset() = aoffset;
		int r = WriteFile( hFile, &pBuffer[bufstartpos], size, NULL, this );
		if( r ) 
			return true;
		if( 0 == r && GetLastError() == ERROR_IO_PENDING )
			return true;
		return false;
	}

	//------------------------------------------
	//�񓯊����ʂ��擾
	//�߂�l�@��������or�ǂݍ��݃o�C�g���B�O�ُ͈�B
	//------------------------------------------
	int WaitResult( HANDLE hFile )
	{
		DWORD read = 0;
		if (GetOverlappedResult( hFile, this, &read, TRUE ))
			return read;
		return 0;
	}
};

//------------------------------------------
//�o�b�t�@��VirtualAlloc�Ŋm�ۂ��A���L����N���X�B
//��FILE_FLAG_NO_BUFFERING�ŃI�[�v������ƁA�o�b�t�@�|�C���^�ɃZ�N�^�A���C�������g�̗v��������
//���������p�r�ɁB�I�u�W�F�N�g�j���Ńo�b�t�@�͉��
//------------------------------------------
class COVDataVMAlloc : public COVData
{
public:
	static int PageAlignOf( int sz){ return (sz+4095) & 0x7ffff000; }

	COVDataVMAlloc(int buszFrame, bool createEvent = true)
		:COVData( NULL,  buszFrame, createEvent )
	{
		pBuffer  = (BYTE*)VirtualAlloc( NULL, buszFrame, MEM_COMMIT, PAGE_READWRITE);
	}
	~COVDataVMAlloc()
	{
		if( pBuffer ){
			VirtualFree( pBuffer, 0, MEM_RELEASE);
			pBuffer = 0;
		}
	}
};
