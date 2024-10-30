#include "stdafx.h"


#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLibMFC\CLASS\ThreadLoopQueue.h"				// ��]���i�`���̃X���b�h�L���[
#include "..\..\Library\KizuLibMFC\Class\ThreadSyncManager.h"			// ����X���b�h�ԏ����ۏᐧ��N���X
#include "MainConst.h"
#include "DummyFrame.h"




extern HT_RPI_FDATA *new_dummy_frame()
{
	HT_RPI_FDATA *p = new HT_RPI_FDATA();
	p->base_head.addr[0] = (LONG*)CDummyQueueFree::GetMemory();
	return p;
}

void * CDummyQueueFree::GetAllocateMemory(int nSize) {
	static void *p;
	if (p == NULL && nSize > 0)
	{
		p = VirtualAlloc(NULL, nSize, MEM_COMMIT | MEM_TOP_DOWN, PAGE_READONLY);
	}
	return p;
}

 void CDummyQueueFree::InitMemory(int nSize) {
	GetAllocateMemory(nSize);
}

 void * CDummyQueueFree::GetMemory() {
	return GetAllocateMemory();
}

 BOOL CDummyQueueFree::AddToTail(HT_RPI_FDATA * pQueData, void * arg)
 {
	 if( pQueData == NULL )
		 return TRUE;
	 // new_dummy_frame�ō��ꂽ�_�~�[�t���[���B
	 // addr�ւ̎Q�Ƃ𔍂����āAdelete(free�h�~�j�B
	 //���̃L���[�ɂ͓`�����Ȃ�
	 if (pQueData->base_head.addr[0] == GetMemory()) {
		 pQueData->base_head.addr[0] = 0;
		 delete pQueData;
		 return TRUE;
	 }

	 return CInterceptQueue<HT_RPI_FDATA>::AddToTail(pQueData, arg);
 }
