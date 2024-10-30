// *********************************************************************************
//	RPI�t�@�C����ǂ݁A���߂��邽�߂̃w�b�_

//	[Ver]
//		Ver.01    2014/09/15  
//
//	[����]
//		�x�[�X�N���X���u����Ă��܂�
// *********************************************************************************
#pragma once
#include "rpi.h"

namespace RpiLib
{
//RPI�t�@�C�������߂��ăw�b�_�Ɗe�t���[���ʒu���Ǘ������{�N���X�B
//��̓I�Ƀt�@�C������ǂ񂾂肷��A���́Arpi_r.h�ɁB
class RpiReader{
protected:
	CString m_sErrorMsg;
	bool setupFramePreLength( int predicate_length, int first_size_if_cb );
public:
	RpiHeader Header;
	RpiFrameLinkArray LinkArray;

	//RPI�t�@�C������A8192�o�C�g���炢�ǂ񂾃o�b�t�@��H�킹�邱�ƁB
	//Header��LinkArray[0]�ɂ͒l������BLinkArray[1]���A�ʒu�����͓���
	bool Parse(const BYTE *header, int length);

	//�t�b�^(�W���C���f�b�N�X)��^���邱�ƂŁALinkArray�̈ʒu�E�T�C�Y������C�ɍX�V����
	int ReadTRPIFooter( const BYTE *footer, int length );

	//�t���[���ǂݍ��݋@�\�B
	//�p���N���X�Ŏ�������Ă���B�g���I�������DELETE���邱�ƁB
	virtual class IRpiFrameEnum *CreateSequencialReader() { return NULL; }

	//�����_���A�N�Z�X���郊�[�_�BRPI�̌`���ɂ���ẮASequencialReader��SkipNext���J��Ԃ��̂ƕς��Ȃ�
	virtual class IRpiFrameEnum *CreateRandomReader() { return CreateSequencialReader(); }

	//�����ǂ��Ȃ����Ƃ��N��������A�G���[��Ԃ���������Ȃ�
	virtual	const char *GetError() const { return m_sErrorMsg; }

};

//�t���[�����i�[�C���^�[�t�F�[�X�B
//�ق������́ApGet~�Ŏ擾����B szGet~�ŁA�T�C�Y���擾�B
class IRpiFrame
{
public:
     int szImg;
     BYTE *pFrame;
	 int nCurrentFrame;
	 RPIFrameSizeInfo RpiInfo;

protected:
     IRpiFrame() : szImg(0), pFrame(NULL), nCurrentFrame(0){
     }
public:
	//ReadNext�Ńt���[����ǂ߂��ꍇ�A���L���\�b�h�Ńt���[�����ɃA�N�Z�X����B

	//�ǂݍ��񂾃t���[���ԍ��B1�I���W��
	int   nFrame() const{ return nCurrentFrame; }

	//�t���[�������N���B�����Ă��͗p����
	const BYTE  *pGetPre() const { return pFrame; }
	BYTE		*pGetPre() { return pFrame; }
	int         szGetPre() const { return RpiInfo.szPreImg; }

	//�摜���
	const BYTE  *pGetImg() const { return &pFrame[RpiInfo.szPreImg]; }
	BYTE		*pGetImg() { return &pFrame[RpiInfo.szPreImg]; }
	int         szGetImg() const { return szImg ;}

	//EPC�t���[�����
	const BYTE  *pGetEpcInf()const { return &pFrame[RpiInfo.szPreImg + szImg ]; }
	BYTE		*pGetEpcInf(){ return &pFrame[RpiInfo.szPreImg + szImg ]; }
	int         szGetEpcInf()const { return RpiInfo.szInfo; }

	//�s�t���[�����
	const BYTE  *pGetTFrame()const { return &pFrame[RpiInfo.szPreImg + szImg + RpiInfo.szInfo]; }
	BYTE		*pGetTFrame(){ return &pFrame[RpiInfo.szPreImg + szImg + RpiInfo.szInfo]; }
	int         szGetTFrame()const { return RpiInfo.szTFrameInf; }
  
};


//�t���[���ǂݍ��݃C���^�[�t�F�[�X�B
//�g���ꍇ�́Arpi_r.h��RpiReaderFile�Ƃ�����CreateSequencialReader()�ł��炤�B
//�N���X��rpi_rf.cpp�Ƃ��Arpi_rm.cpp�Ƃ��Œ�`�B
class IRpiFrameEnum : public IRpiFrame
 {
 protected:
     IRpiFrameEnum(){
     }

 public:
	virtual ~IRpiFrameEnum(){};
     
	//------------------------------------------
	//Read�œǂނƂ��ɁAIRpiFrameEnum�������Ŋm�ۂ���̂ł͂Ȃ��A
	//�w�肵���������A�h���X�ɏ������ނ悤�ɂ���B
	//�@BYTE *pFrameDst	������
	//�@int szFrame		�T�C�Y
	// pGetImg�Ȃǂ��A����Ŏw�肵���������̒����w���悤�ɂȂ�
	//------------------------------------------
	virtual void *SetReadDest( BYTE *pFrameDst, int szFrame ){  return NULL; }


	//------------------------------------------
	//�w�肵���t���[����ǂݍ��ށB
	//RPI�̌`���ɂ���Ă�(���t�b�^������RPI)�A
	//�G���[�ɂȂ�̂Œ���
	//------------------------------------------
	virtual bool ReadFrame( int nFrame ){
		if(	ReadToInternalBuffer(nFrame, true ) ){
			nCurrentFrame = nFrame;
			return true;
		}
		return false;

	}

	//------------------------------------------
	// ���̃t���[����ǂ݂��ށB
	//------------------------------------------
	virtual bool ReadNext()
	{
		if(	ReadToInternalBuffer(nCurrentFrame + 1, true ) ){
			nCurrentFrame++;
			return true;
		}
		return false;
	}

	//------------------------------------------
	// ���̃t���[���͓ǂ܂Ȃ��B�X�L�b�v����B
	// ���s��́AnFrame�ȊO�̏����g���Ă͂����Ȃ�(pGetImg�Ȃ�)
	//------------------------------------------
	virtual bool SkipNext()	
	{
		if(	ReadToInternalBuffer(nCurrentFrame + 1, false ) ){
			nCurrentFrame++;
			return true;
		}
		return false;

	}
 protected:
	//------------------------------------------
	 //���ۂ̓Ǎ��S�����\�b�h�B�p����Ŏ���
	//------------------------------------------
	 virtual bool ReadToInternalBuffer( int nFrame, bool bRead = true) { 
		 return false;
	 }
};
};