'*********************************************************************************
'�����p�C���^�[�t�F�[�X
'	[Ver]
'		Ver.01    2005/12/24  vs2005 �Ή�
'		Ver.02    2006/01/28  vs2003 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On                                ' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Namespace tIface
    '///////////////////////////////////////////////////////////////////////////////
    '[Generic�Ή�]
    '///////////////////////////////////////////////////////////////////////////////
#If VBC_VER >= 8.0 Then                         ' Generic �ɑΉ����Ă���o�[�W������
    ''' <summary>
    ''' Find�̃C���^�[�t�F�[�X [Generic�Ή�]
    ''' </summary>
    ''' <typeparam name="KEY">�����L�[</typeparam>
    ''' <remarks>tClass_Array �̌Ăь��N���X�� Implements tIface_Find ���`���鎖</remarks>
    Public Interface tIface_Find(Of KEY)
        ''' <summary>
        ''' �v�f���X�g����w��L�[�Ō������s���A�������ʂ̔z��ʒu��߂� ��������������
        ''' </summary>
        ''' <param name="gnrKey">�����L�[</param>
        ''' <param name="aryList">�v�f���X�g</param>
        ''' <returns>�������ʂ�Ԃ� 0�I���W���̔z��ʒu -1:�Y������</returns>
        ''' <remarks></remarks>
        Function Find(ByVal gnrKey As KEY, ByVal aryList As ArrayList) As Integer

    End Interface
#End If

    '///////////////////////////////////////////////////////////////////////////////
    '[Generic��Ή�]
    '///////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' Find�̃C���^�[�t�F�[�X
    ''' </summary>
    ''' <remarks>tClass_Array �̌Ăь��N���X�� Implements tIface_Find ���`���鎖</remarks>
    Public Interface tIface_Find
        ''' <summary>
        ''' �v�f���X�g����w��L�[�Ō������s���A�������ʂ̔z��ʒu��߂� ��������������
        ''' </summary>
        ''' <param name="gnrKey">�����L�[</param>
        ''' <param name="aryList">�v�f���X�g</param>
        ''' <returns>�������ʂ�Ԃ� 0�I���W���̔z��ʒu -1:�Y������</returns>
        ''' <remarks></remarks>
        Function Find(ByVal gnrKey As Object, ByVal aryList As ArrayList) As Integer

    End Interface
End Namespace