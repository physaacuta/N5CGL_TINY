// *********************************************************************************
//	[����] RPI�t�@�C������������A�ǂ񂾂肷��N���X�ɕK�v�ȃw�b�_�B

//	[Ver]
//		Ver.01    2014/09/15  
//
//	[����]
//		�����p�̃w�b�_�ł��B�g�����ɂ͕s�v
// *********************************************************************************

#pragma once

namespace RpiLib
{
const int SIZE_RAW_nreso  = 44;    //�񈳏k�E������\ �w�b�_�T�C�Y
const int SIZE_RAW_RESO   = 52;    //�񈳏k�E�L����\ �w�b�_�T�C�Y
const int SIZE_COMP_nreso = 40;    //�@���k�E������\ �w�b�_�T�C�Y
const int SIZE_COMP_RESO  = 48;    //�@���k�E�L����\ �w�b�_�T�C�Y
    
enum RpiHeaderType{
    TYPE_RWFL = RPIFOURCC('R', 'W', 'F', 'L'),    //�t���[����񖳂�
    TYPE_RWFT = RPIFOURCC('R', 'W', 'F', 'T'),    //�t���[����񂠂�
};


//������<=>�����̃A�N�Z�X�p�Breinterpret_cast���g�����͏����₷���͂��B
//Intel�n�̓o�X�G���[���Ȃ����炢���Ƃ����΂�������

//------------------------------------------
//����������64bit�l��ǂݍ���
// const void *buf ������
// int nPosInFile  ��������̈ʒu
//�߂�l�@buf���� �ǂ�64bit�l
//------------------------------------------
static int64_t read_int64( const void *buf, int nPosInFile ){
	int64_t m;
	memcpy( &m, &static_cast<const char*>( buf )[nPosInFile], 8);
	return m;
}

//------------------------------------------
//����������32bit�l��ǂݍ���
// const void *buf ������
// int nPosInFile  ��������̈ʒu
//�߂�l�@buf���� �ǂ�32bit�l
//------------------------------------------
static int read_int32( const void *buf, int nPosInFile ){
	int m;
	memcpy( &m, &static_cast<const char*>( buf )[nPosInFile], 4);
	return m;
}

//------------------------------------------
//��������64bit�l����������
// void *buf	������
// int nValue	�������ޒl
//------------------------------------------
static void put_int64(  void *buf, int64_t nValue){
	memcpy( buf, &nValue, 8);
}

//------------------------------------------
//��������32bit�l����������
// void *buf	 ������
// int nValue	�������ޒl
//------------------------------------------
static void put_int32(  void *buf, int nValue ){
	memcpy( buf, &nValue, 4);
}


//32bit�P�ʂŁA�\���̂ƃt�@�C��(����ǂݍ��񂾃�����)�̓]��������B
struct CopyInt32 {
	int nStructure; //�\���̑��������I�t�Z�b�g
	int nFile;		//�t�@�C�����I�t�Z�b�g
	
	//------------------------------------------
	//�t�@�C�����̃���������A�\���̑��̃������փf�[�^���R�s�[����
	//         void *st	�\���̑�������
	// const void *file	�t�@�C����������
	//------------------------------------------
	void StructFromFile( void *st, const void *file )const{
		if( nStructure < 0 || nFile < 0 )
			return;

		static_cast<int*>(st)[ nStructure/4 ] = static_cast<const int*>(file)[ nFile/4 ]; 
	}

	//------------------------------------------
	//�\���̑��̃���������A�t�@�C�����̃������փf�[�^���R�s�[����
	//       void *file	�t�@�C����������
	//   const void *st	�\���̑�������
	//------------------------------------------
	void FileFromStruct(void *file, const  void *st )const{
		if( nStructure < 0 || nFile < 0 )
			return;
		static_cast< int*>(file)[ nFile/4 ] = 		static_cast<const int*>(st)[ nStructure/4 ] ;

	}
};

struct RpiHeaderCopier
{
	static const CopyInt32 Parse_Rf[];	//���E���t���[���p�R�s�[�\����
	static const CopyInt32 Parse_Cf[];	//���k�E���t���[���p�R�s�[�\����
	static const CopyInt32 Parse_RF[];  //���E�L�t���[���p�R�s�[�\����
	static const CopyInt32 Parse_CF[];	//���k�E�L�t���[���p�R�s�[�\����
};
};