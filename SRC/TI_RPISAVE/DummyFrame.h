#pragma once

//�_�~�[�t���[���p�L���[�B
// �_�~�[�t���[����Add�����Ƃ��A���ۂɂ�Add����Delete���Ă��܂��B
class CDummyQueueFree : public CInterceptQueue<HT_RPI_FDATA>
{
	// �_�~�[�t���[���̉摜�����BReadOnly�Ȃ̂Œ��ӁI
	static void *GetAllocateMemory(int nSize = 0);
public:
	static void InitMemory(int nSize);
	static void *GetMemory();

	CDummyQueueFree(IQueue<HT_RPI_FDATA> *pQue ) 
		:CInterceptQueue<HT_RPI_FDATA>(pQue)
	{}

	//��������Override
	virtual BOOL AddToTail(HT_RPI_FDATA* pQueData, void *arg = NULL);
};
extern HT_RPI_FDATA *new_dummy_frame();
