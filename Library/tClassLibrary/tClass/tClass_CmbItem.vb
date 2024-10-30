'*********************************************************************************
'�R���{�{�b�N�X�ɕ\������ׂ�ToString�����������N���X
'	[Ver]
'		Ver.01    2009/01/26  vs2005 �Ή�
'
'	[����]
'		���܂Ŏg�����ŏ���ɐ錾���Ă������A�v�������ǂ��g�p����ׁA�N���X�Ɋi�グ
'*********************************************************************************
Option Strict On

''' <summary>
''' �R���{�{�b�N�X�ɕ\������f�[�^ (�����ȃf�[�^�ɑΉ��ł���悤�ɍ\�����Ă���)
''' </summary>
Public Class tClass_CmbItem
    ''' <summary>Int��L�[</summary>
    Public g_nKey As Integer							' Int��L�[
    ''' <summary>String��L�[</summary>
    Public g_sKey As String								' String��L�[

    ''' <summary>�R�t������(ToString�ŕԋp����镶����)</summary>
    Public g_sName As String							' �R�t������

    ''' <summary>Int�A�C�e��</summary>
    Public g_nItem As Integer							' Int�A�C�e��
    ''' <summary>String�A�C�e��</summary>
    Public g_sItem As String							' String�A�C�e��


    ''' <summary>
    ''' �R���X�g���N�^ (Int��L�[)
    ''' </summary>
    ''' <param name="nKey">��L�[</param>
    ''' <param name="strName">�R�t������</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal nKey As Integer, ByVal strName As String)
        g_nKey = nKey
        g_sKey = ""
        g_sName = strName

        g_nItem = 0
        g_sItem = ""
    End Sub
    Public Sub New(ByVal nKey As Integer, ByVal strName As String, ByVal nItem As Integer)
        g_nKey = nKey
        g_sKey = ""
        g_sName = strName

        g_nItem = nItem
        g_sItem = ""
    End Sub
    Public Sub New(ByVal nKey As Integer, ByVal strName As String, ByVal sItem As String)
        g_nKey = nKey
        g_sKey = ""
        g_sName = strName

        g_nItem = 0
        g_sItem = sItem
    End Sub
    Public Sub New(ByVal nKey As Integer, ByVal strName As String, ByVal nItem As Integer, ByVal sItem As String)
        g_nKey = nKey
        g_sKey = ""
        g_sName = strName

        g_nItem = nItem
        g_sItem = sItem
    End Sub


    ''' <summary>
    ''' �R���X�g���N�^ (String��L�[)
    ''' </summary>
    ''' <param name="sKey">��L�[</param>
    ''' <param name="strName">�R�t������</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal sKey As String, ByVal strName As String)
        g_nKey = 0
        g_sKey = sKey
        g_sName = strName

        g_nItem = 0
        g_sItem = ""
    End Sub
    Public Sub New(ByVal sKey As String, ByVal strName As String, ByVal nItem As Integer)
        g_nKey = 0
        g_sKey = sKey
        g_sName = strName

        g_nItem = nItem
        g_sItem = ""
    End Sub
    Public Sub New(ByVal sKey As String, ByVal strName As String, ByVal sItem As String)
        g_nKey = 0
        g_sKey = sKey
        g_sName = strName

        g_nItem = 0
        g_sItem = sItem
    End Sub
    Public Sub New(ByVal sKey As String, ByVal strName As String, ByVal nItem As Integer, ByVal sItem As String)
        g_nKey = 0
        g_sKey = sKey
        g_sName = strName

        g_nItem = nItem
        g_sItem = sItem
    End Sub


    ''' <summary>
    ''' �\���̂̕\������
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Overrides Function ToString() As String
        Return g_sName
    End Function
End Class
