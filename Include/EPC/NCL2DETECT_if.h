// ///////////////////////////////////////////////////////////////////
// �F���G���W��v2�̃C���^�[�t�F�[�X�w�b�_�t�@�C��
//
// 2016 NSSMC all rights reserved.

#pragma once

#include <Windows.h>
#include <tchar.h>

#pragma pack(push, 1)

enum {
	CH_R = 0,
	CH_G,
	CH_B,
	CH_DIFF
};

// ////////////////////
// �T���v���Z�X���
// ////////////////////
enum NCL_ENGINE_STATE
{
	NCL_PREINIT = 1,	// �������O
	NCL_TRANSITION,		// ��ԑJ�ڒ�
	NCL_INITIALIZED,	// �������ς�
	NCL_RUNNING,		// �T����
	NCL_RUNNING_PRELOAD	// �T��������PRELOAD���s��
};

// ////////////////////
// �v���Z�X�̃��^�[���R�[�h (�������邩����j
// ////////////////////
enum NCL_PROCESS_ERR
{
    NCL_PROCESS_NOERR = 0,
    NCL_PROCESS_ERR_STARTUP = -1,
    NCL_PROCESS_ERR_UNKNOWN = -999,
};

// ////////////////////
// ����R�}���h�E�\����
// ////////////////////

enum NCL_CMDCODE
{
    NCL_INIT = 0,			// ������
    NCL_DISPOSE,			// ���������i�I�����j
    NCL_EXIT,				// �I��
    NCL_START,				// �J�n
    NCL_STOP,				// ��~
    NCL_PARAM,				// �p�����[�^�ݒ�
	NCL_COILINFO,			// ��ʏ��ݒ�
	NCL_PRELOAD_PARAM,		// DB���烁�����ւ̃p�����[�^�ǂݍ���
	NCL_SET_POSITION,		// ���������J�E���^�ݒ�
	NCL_QUERY_STATUS,		// ��Ԗ₢���킹
	NCL_CLEAR_QUEUE,		// �����s��PARAM,SET_POSITION������
	NCL_RESUME_START,		// �r���Q���ɂ��J�n(NCL_START�̑���j�B
    NCL_UNKNOWN,


	NCL_SAVE_DDS	= 98,		// DDS���O�ۑ�
	NCL_INIT_RPISRC = 99,		// ������(Input = RPI )
	NCL_ASYNC_ERR = -1,		// �񓯊��G���[�̂Ƃ��̃R�}���h�R�[�h
	NCL_ASYNC_EVENT = -2		// �񓯊��C�x���g�̂Ƃ��̃R�}���h�R�[�h
};

// �e�R�}���h�ɑ΂���G���[�R�[�h
enum NCL_ERRCODE
{
	// �R�}���h�ɋ���
	NCL_SUCCESS = 0,
	NCL_ERR_CMD_UNEXPECTED,		// ���[�h���Ⴄ�Ƃ�
	NCL_ERR_CMD_UNKNOWN,		// �m��Ȃ��R�}���h�R�[�h

	// INIT
	NCL_ERR_INIT_DB = 100,		// DB
	NCL_ERR_INIT_GRABBER,		// APX-3326,3323
	NCL_ERR_INIT_ARBITERNOTFOUND,	// �G�b�W���₪������Ȃ�
	NCL_ERR_INIT_BADSTATE,		// ���łɏ�������
	NCL_ERR_INIT_NOGPU,			// GPU���Ȃ�

	// DISPOSE
	NCL_ERR_DISPOSE_FAILED = 200,

	// EXIT
	NCL_ERR_EXIT_FAILED = 300,

	// START
	NCL_ERR_START_FAILED = 400,
	NCL_ERR_RESET_FAILED,
	NCL_ERR_CONNECT_NCLEDGE_FAILED,
	NCL_ERR_NOT_INITIALIZED,

	// STOP
	NCL_ERR_STOP_FAILED = 500,

	// PARAM
	NCL_ERR_PARAM_NOT_LOADED = 600,
	NCL_ERR_PARAM_DB_OPEN_FAILED,
	NCL_ERR_PARAM_FAILED,

	// COILINFO
	NCL_ERR_COILINFO_PARSE_FAILED = 700,

	// PRELOAD_PARAM
    NCL_ERR_PRELOAD_DB_OPEN_FAILED = 800,
	NCL_ERR_PRELOAD_DB_FETCH_FAILED,

	// PRESET_POSITION
	NCL_ERR_SET_POSITION_FAILED = 1000,

	// QUERY_STATUS
	NCL_ERR_QUERY_STATUS_FAILED = 1100,

	// �񓯊��G���[
	NCL_ERR_RECV_EDGE_INFO = -1000,			// �G�b�W�ʒu�ʐM�G���[
	NCL_ERR_SEND_EDGE_INFO,
	NCL_ERR_RECV_EDGE_INFO_TIMEOUT,			// �G�b�W�ʒu�ʐM�^�C���A�E�g
	NCL_ERR_SEND_EDGE_INFO_TIMEOUT,
	NCL_ERR_ASYNC_PARAM_FAILED,				// �t���[���ԍ��w���PARAM�R�}���h�̎��s
	NCL_ERR_ASYNC_SET_POSITION_FAILED,		// �t���[���ԍ��w���SET_POSITION�R�}���h�̎��s
	// �C�x���g
	NCL_ERR_FRAME_SKIPPED = -2000,			// �t���[���X�L�b�v����
	NCL_ERR_ASYNC_PARAM_SUCCESS,			// �t���[���ԍ��w���PARAM�R�}���h�̐���
	NCL_ERR_ASYNC_SET_POSITION_SUCCESS		// �t���[���ԍ��w���SET_POSITION�R�}���h�̐���
};

// NCL_ERRCODE�𕶎���ɕϊ�����
#define ERR_ITEM(x) case x: return _T(#x)
static const TCHAR* GetErrMsg(NCL_ERRCODE err)
{
    switch( err )
    {
        ERR_ITEM(NCL_SUCCESS);
		/*
        ERR_ITEM(ERR_CMD_UNEXPECTED);
        ERR_ITEM(ERR_CMD_UNKNOWN);
        ERR_ITEM(ERR_INIT_SMEM);
        ERR_ITEM(ERR_INIT_CPIPE);
        ERR_ITEM(ERR_INIT_RPIPE);
        ERR_ITEM(ERR_INIT_SOCKET);
        ERR_ITEM(ERR_INIT_MIL);
        ERR_ITEM(ERR_INIT_JPEG);
        ERR_ITEM(ERR_INIT_IMAGEDISP);
        ERR_ITEM(ERR_INIT_DB);
        ERR_ITEM(ERR_INIT_GRABBER);
        ERR_ITEM(ERR_INIT_IPC);
        ERR_ITEM(ERR_INIT_MANAGER);
        ERR_ITEM(ERR_INIT_INVALID_ARGS);
        ERR_ITEM(ERR_EXIT_FAILED);
        ERR_ITEM(ERR_START_FAILED);
        ERR_ITEM(ERR_RESET_FAILED);
        ERR_ITEM(ERR_STOP_FAILED);
        ERR_ITEM(ERR_COILINFO_INVALID);
        ERR_ITEM(ERR_PARAM_DB_OPEN_EXCEPTION);
        ERR_ITEM(ERR_PARAM_RETRIEVE_FAILED);
        ERR_ITEM(ERR_PARAM_RETRIEVE_EXCEPTION);
        ERR_ITEM(ERR_PARAM_EDGE_FAILED);
        ERR_ITEM(ERR_PARAM_DETECT_FAILED);
        ERR_ITEM(ERR_PARAM_POST_FAILED);
        ERR_ITEM(ERR_PARAM_DCD_LOAD_FAILED);
        ERR_ITEM(ERR_PARAM_CLFR_FAILED);
        ERR_ITEM(ERR_PARAM_SYNCADD_FAILED);
        ERR_ITEM(ERR_PARAM_HDRI_FAILED);
        ERR_ITEM(ERR_NOCOILINFO);
        ERR_ITEM(ERR_STARTGRAB_FAILED);
        ERR_ITEM(ERR_NOP_FAILED);
        ERR_ITEM(ERR_ABORT_FAILED);
        ERR_ITEM(ERR_FRAMEIMAGE_TOO_MANY_REQ);
        ERR_ITEM(ERR_FRAMEIMAGE_NOT_FOUND);
        ERR_ITEM(ERR_FRAMEIMAGE_ENCODE_FAILED);
        ERR_ITEM(ERR_FRAMEIMAGE_ENCODE_BUFF_TOO_SMALL);
        ERR_ITEM(ERR_TOP_NOT_DETECTED);
        ERR_ITEM(ERR_BOT_NOT_DETECTED);
        ERR_ITEM(ERR_SMEM_OVERFLOW);
        ERR_ITEM(ERR_TOO_MANY_WAITING_EDGEPOS);
        ERR_ITEM(ERR_NO_FREE_EDGEPOS_LEFT);
		*/
    }

	return _T("Unknown");
}

//  ///////////////////////////////////
// �萔
static const int NCL_ncam_max = 8;			// ����P�䂠����̃J�����䐔
static const long NCL_nattr_common = 256;	// ���ʓ�����
static const long NCL_nattr_angle  = 256;	// �p�x���̌�
static const int NCL_nperiod_hist = 8;		// �P���ȈՎ������o�̌�

// ///////////////////////////////////
// �R�}���h�̍\���� (�ǉ��f�[�^�Ȃ�)
// DISPOSE, EXIT, START, STOP, QUERY_STATUS
struct NCL_CMD
{
	NCL_CMDCODE	cmd;
};


struct NCL_CMD_RESUME_START : public NCL_CMD {
	DWORD	start_frame_num;		//�ŏ���Grab�ł���t���[���̔ԍ��B
};

// �R�}���h�̍\����(�������R�}���h)
struct NCL_CMD_INIT
{
	NCL_CMDCODE	cmd;			// NCL_INIT
	DWORD	    camnum;			// �J�����䐔
	int         camid[NCL_ncam_max];			// �}1�`, ���̒l�́A�G�b�W����M�݂̂ł��邱�Ƃ������B�J�����ԍ��Ƃ��Ă͐�Βl���g���B
	long		initial_scode;	// scode�����l
	TCHAR       dbconnstr[256];
};

// �R�}���h�̍\����(�������R�}���h�BRPI����)
struct NCL_CMD_INIT_RPISRC : public NCL_CMD_INIT
{
	long		frame_start;	// skip frame.
	long		frame_end;		// end frame.
	TCHAR		rpi_filename[256];//camera�Q�� 256*1~, 3��256*2~�ɏ������ƁB�����́A�錾��rpi_filename[1]�Ɠ����B
};

struct NCL_CMD_SAVE_DDS : public NCL_CMD
{
	long long free_space;	//�e�ʊǗ����s���ꍇ�A�����Ă��Ăق����T�C�Y(byte�P��)�B0�͗e�ʊǗ����Ȃ��B
	TCHAR	dds_path[256];	//�ۑ���p�X�B���̃R�C���ύX�ŗ����B�J���������n���ƁADDS�ۑ���~�B
};

// �R�}���h�̍\����(�p�����[�^�ݒ�)
struct NCL_CMD_PARAM
{
	NCL_CMDCODE	cmd;			// NCL_PARAM
	DWORD       scode;			// �\�ʏ�ԃR�[�h
	DWORD		framenum;		// ���s�t���[���ԍ�
};

// �R�}���h�̍\����(��ʏ��ݒ�)
struct NCL_CMD_COILINFO
{
	NCL_CMDCODE	cmd;			// NCL_COILINFO
	DWORD		info_size;
	BYTE		coil_info[1];
};

// �R�}���h�̍\����(�p�����[�^��ǂ�)
struct NCL_CMD_PRELOAD_PARAM
{
	NCL_CMDCODE	cmd;			// NCL_PARAM
	DWORD       scode;			// �\�ʏ�ԃR�[�h
};

// �R�}���h�̍\����(�����J�E���^�Z�b�g)
enum NCL_SET_POSITION_MASK
{
	FRONT =	0x1,
	TAIL  = 0x2,
	SHEAR = 0x4,
	UNIV_UP0 = 0x8,
	UNIV_UP1 = 0x10,
	UNIV_UP2 = 0x20,
	UNIV_UP3 = 0x40,
	UNIV_DN0 = 0x80,
	UNIV_DN1 = 0x100,
	UNIV_DN2 = 0x200,
	UNIV_DN3 = 0x400,
	ALL = 0xfff
};

struct NCL_CMD_SET_POSITION
{
	NCL_CMDCODE		cmd;			// NCL_SET_POSITION
	DWORD			update_mask;	// �X�V����J�E���^���w�肷��r�b�g�p�^�[��
	DWORD			framenum;		// ���s�t���[���ԍ�
	float			front_pos;		// ���Z�J�E���^
	float			tail_pos;		// ���Z�J�E���^
	float			shear_cut_pos;	// ���Z�J�E���^
	float			univ_cnt_up_pos[4];		// �ėp���Z�J�E���^
	float			univ_cnt_dn_pos[4];		// �ėp���Z�J�E���^
};


// ////////////////////////////////////
// �߂�l(�񓯊��G���[)�\����
struct NCL_ANSWER
{
	NCL_ERRCODE			retcode;
	NCL_CMDCODE			cmd;		// ���̃R�}���h�ɑ΂���G���[�R�[�h���H
};

// QUERY_STATUS�̖߂�\����
struct NCL_ANSWER_QUERY_STATE
{
	NCL_ERRCODE			retcode;
	NCL_CMDCODE			cmd;			// NCL_QUERY_STATUS
	NCL_ENGINE_STATE	state;
	DWORD				camnum;						// �J�����䐔
	DWORD				camid[NCL_ncam_max];		// �J�����ԍ�
	float				exptime[3][NCL_ncam_max];	// �F��
	float				gain[3][NCL_ncam_max];		// �F��
	float				temp[NCL_ncam_max];			// �J�������x
	float				volt[NCL_ncam_max];			// �J�����d��
};

// {467D5741-9583-45FA-92A1-E824704482B2}
static const GUID ID_FIRST_SHMEM_INFO_IDv1 =
{ 0x467d5741, 0x9583, 0x45fa,{ 0x92, 0xa1, 0xe8, 0x24, 0x70, 0x44, 0x82, 0xb2 } };

static const int FIRST_SHMEM_SIZE = 4096;

static const char SerialResultTimeout[] = "<Timeout>";

struct SerialResult 
{
	DWORD Tick;
	char SendTo[32];
	char Result[32];
	DWORD TickGcp;
	char Gcp[2048];
};

struct FIRST_SHMEM_INFOv1 {
	GUID first_smem_req_id;
	long long			UpdateTime;						// ���ڍX�V�V���A��(GetTickCount())
	DWORD				grab_frame;						// ���ݎB�����̂����ŐV�t���[��No
	DWORD				camnum;							// �J�����䐔
	DWORD				camid[NCL_ncam_max];			// �J�����ԍ�

	long				exptime[NCL_ncam_max][3];		// �F��
	float				gain[NCL_ncam_max][3];			// �F��
	float				matrix[NCL_ncam_max][9];	
	float				temp[NCL_ncam_max];				// �J�������x
	float				volt[NCL_ncam_max];				// �J�����d��
	SerialResult		serial_result[NCL_ncam_max];	// GCP�R�}���h�̃��^�[��������
};

// ////////////////////
// �G�b�W�͈͐���UDP�p�P�b�g
// ////////////////////
struct NCL_EDGE_LIMIT
{
	BOOL	use_limit;
	float	left_outer;
	float	left_inner;
	float	right_inner;
	float	right_outer;
};

struct NCL_EDGE_RESTRICT_DATA
{
	DWORD	frame_num;
	NCL_EDGE_LIMIT	limit[2];	// 0:top, 1:bot
};


// ////////////////////
// �p�C�v�f�[�^
// ////////////////////

struct NCL_SMEM_PIPE_DATA
{
	DWORD    frame_num;		// �T���J�n����̘A��
	DWORD    camid;			// �J�����ԍ�
	long     smem_offset;
	long     smem_size;
};

// NCL_SMEM_PIPE_DATA��size��0�̂Ƃ���offset�Ɋi�[����l
// �t���[���摜����ɕԂ����߂ɁA���L�������������ς��̏ꍇ�����G���[�͖����B
// ���̃G���[�̓w�b�_�ɏ������
enum SMEM_PIPEDATA_OFFSET
{
    NCL_SMEMPIPE_SMEMFULL = -1 // ���L�������������ς�������
};

// ////////////////////
// ���L������
// ////////////////////

// //////////////////////
// �X�e�[�^�X�G���A�͖���


// ���������C�A�E�g  ////////////////////////////////////////////////
// �t���[���w�b�_
// ��΃G�b�W�ʒu[mm] float left[height], right[height]
// �t���[���摜	width*height*4
// ���� NCL_DEFECT_RESULT * defect_num
// ������ double[NCL_nattr_common+4*NCL_nattr_angle] * defect_num
// �؂�o���摜	NCL_IMAGE_HEADER * image_num


// �t���[���w�b�_�ł̒T����Ԓ�`
enum	NCL_FRAME_STATUS
{
	STATUS_VALID = 0,		// ���̃t���[���ɔ�����
	STATUS_INVALID,			// ������
	STATUS_EDGELOST,		// �������X�L�b�v����(�G�b�W���o�Ɏ��s��������)
    STATUS_PROCSKIP,		// �������X�L�b�v����(���炩�̗��R��)
    STATUS_TOOMANY,			// �������X�L�b�v����(�r���̐�������𒴂�������)
    STATUS_QFULL,			// �������X�L�b�v����(�����҂���������𒴂���������������)
	STATUS_UNKNOWN
};

// �t���[���w�b�_
struct NCL_SMEM_FRAME_HEADER
{
	DWORD	frame_num;
	DWORD   camid;
	NCL_FRAME_STATUS	status;

	long     defect_num;
	long     image_num;

	DWORD	 edge_offset;
	DWORD    frame_offset;
	DWORD    result_offset;
	DWORD    attr_offset;
	DWORD    image_offset;

	BYTE	frame_brightness[4];	// RGB

	// ���J�E���^�l
	float			front_pos;		// ���Z�J�E���^
	float			tail_pos;		// ���Z�J�E���^
	float			shear_cut_pos;	// ���Z�J�E���^
	float			univ_cnt_up_pos[4];		// �ėp���Z�J�E���^
	float			univ_cnt_dn_pos[4];		// �ėp���Z�J�E���^

	DWORD executed_position_mask;	  //NCL_SET_POSITION_MASK ���̃t���[���ŏ��������ꂽ�J�E���^�B

	//PD�̂݁B���Z�摜�ւ̃I�t�Z�b�g
	DWORD pd_frame_offset;
};

enum NCL_DEFECT_FLAGS
{
	NORMAL = 0,		// �ʏ��r
	PERIODIC = 1,		// �ȈՎ����ł̎����r
	SERIES = 2,			// �ȈՎ����ł̘A���r
	START = 4,			// �ȈՎ����ł̍ŏ����r�t���O
	SYNC_ADD = 8,		// �摜���Z�ł̎����r
	OTHER
};

struct NCL_DEFECT_RESULT
{
	long    type;
	long    grade;
	long    group;
	long    flags;				// NCL_DEFECT_FLAGS�̒l
	long    im_idx;
};

struct NCL_IMAGE_HEADER			// �؂�o���摜
{
	DWORD	left;				// �؂�o��������W�B�i�k�����W�n�j
	DWORD	top;				// ����
	DWORD   width;
	DWORD   height;
	float	shrinkratio;			//�؂�o���摜�k����
	DWORD	res[3];

	BYTE    image[1];			// width*height*(RGB,DIFF)4��
};

// frame_offset�̎w�����������B
//���摜�i�k������j
struct NCL_FRAME_IMAGE{
    DWORD width;
    DWORD height;
    DWORD stride;
    DWORD channel;
    BYTE image[1];              // stride*height*channel��
};

// pd_frame_offset�̎w��������
// �������Z�摜
struct NCL_PERIOD_IMAGE {
	DWORD   nAddIndex;      //���Z�ԍ��i���Ԗڂ̃��[�����j
	DWORD	nFrameStart;    //���Z�J�n�t���[��NO
	DWORD   width;			//�����d�˂��킹�摜�̕�
	DWORD   height;			//�����d�˂��킹�摜�̍�
	BYTE    image[1];		// width*height*(RGB,DIFF)4��
};
// UDP�ŃJ�����̃V���A�����x�؂�ւ��𓝊��Ɉ˗�����
static const WORD cam_ser_udp_port = 8999;
static const int cam_ser_packet_id = 1111;
struct CAM_SERIAL_SPEED
{
	int packetid;
	DWORD	camid;		// �J�����ԍ�(1�`)
	DWORD	speed;		// 9600 or 115200
};

#pragma pack(pop)

#ifndef _DEBUG
// ////////////////////
// ���L�������̖��O
// ////////////////////
static const char* name_shmem = "shmem_ncldetect";

// ////////////////////
// �R�}���h�p�C�v�̖��O
// ////////////////////
static const char* name_cpipe = "\\\\.\\pipe\\NCL_cpipe";

// ////////////////////
// �T�����ʃp�C�v�̖��O
// ////////////////////
static const char* name_rpipe = "\\\\.\\pipe\\NCL_rpipe";
#else
static const char* name_shmem = "shmem_ncldetect_debug";
static const char* name_cpipe = "\\\\.\\pipe\\NCL_cpipe_debug";
static const char* name_rpipe = "\\\\.\\pipe\\NCL_rpipe_debug";
#endif

// DB��
static const char* CAMNAME = "[CAMSET]";		// �f�[�^�x�[�X�̌����L�[��
static const char* SCODENAME = "[SCODE]";		// �f�[�^�x�[�X�̌����L�[��
static const char* ANGLENAME = "[CAMANGLE]";	// �f�[�^�x�[�X�̌����L�[��