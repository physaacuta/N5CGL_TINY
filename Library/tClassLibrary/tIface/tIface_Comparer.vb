'*********************************************************************************
'�\�[�g���`�����{�C���^�[�t�F�[�X
'	[Ver]
'		Ver.01    2006/01/28  vs2005 �Ή�
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
    ''' ������� �Ăяo���p �C���^�[�t�F�[�X [Generic�Ή�]
    ''' </summary>
    ''' <typeparam name="KEY">�����Ώۂ̃I�u�W�F�N�g</typeparam>
    ''' <typeparam name="SERCH">���ۂɔ�r����l</typeparam>
    ''' <remarks></remarks>
    Public Interface tIface_Comparer(Of KEY, SERCH)
        ''' <summary>
        ''' �v�f����w��L�[�Ō��� (�\�[�g�����s�����炱�ꂪ�v�f�����R�[�������)
        ''' </summary>
        ''' <param name="x">�����I�u�W�F�N�g</param>
        ''' <param name="y">�����I�u�W�F�N�g</param>
        ''' <param name="KeyIndex">�����L�[�z��ʒu</param>
        ''' <param name="setX">�����L�[�Z�b�g����</param>
        ''' <param name="setY">�����L�[�Z�b�g����</param>
        ''' <returns>0�F���� 0��菬����:X�̕��������� 0���傫��:X�̕����傫��</returns>
        ''' <remarks></remarks>
        Function Comparer_InfSet(ByVal x As KEY, ByVal y As KEY, ByVal KeyIndex As Integer, ByRef setX As SERCH, ByRef setY As SERCH) As Integer

    End Interface
#End If

    '///////////////////////////////////////////////////////////////////////////////
    '[Generic��Ή�]
    '///////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ������� �Ăяo���p �C���^�[�t�F�[�X
    ''' </summary>
    ''' <remarks></remarks>
    Public Interface tIface_Comparer
        ''' <summary>
        ''' �v�f����w��L�[�Ō��� (�\�[�g�����s�����炱�ꂪ�v�f�����R�[�������)
        ''' </summary>
        ''' <param name="x">�����I�u�W�F�N�g</param>
        ''' <param name="y">�����I�u�W�F�N�g</param>
        ''' <param name="KeyIndex">�����L�[�z��ʒu</param>
        ''' <param name="setX">�����L�[�Z�b�g����</param>
        ''' <param name="setY">�����L�[�Z�b�g����</param>
        ''' <returns>0�F���� 0��菬����:X�̕��������� 0���傫��:X�̕����傫��</returns>
        ''' <remarks></remarks>
        Function Comparer_InfSet(ByVal x As Object, ByVal y As Object, ByVal KeyIndex As Integer, ByRef setX As Object, ByRef setY As Object) As Integer

    End Interface
End Namespace