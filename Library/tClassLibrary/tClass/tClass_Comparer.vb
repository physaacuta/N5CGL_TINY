'*********************************************************************************
'�\�[�g���`�����{�N���X
'	[Ver]
'		Ver.01    2006/01/28  vs2005 �Ή�
'		Ver.02    2006/01/28  vs2003 �Ή�
'
'	[����]
'		������� �p�t�H�[�}���X������ ���� �ėp���d���ɍ����(vs2005�ł͖�����)
'*********************************************************************************
'Option Strict On                                ' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Option Strict Off
Imports System.Windows.Forms

#Region "�T���v��"
'''''�K���C���^�[�t�F�[�X�����������鎖
''''Public Class frmMain
''''    Inherits System.Windows.Forms.Form
''''    Implements tClass.tIface_Comparer

''''    ''' �R�[����
''''    Private Sub ListViewCoilSrach_ColumnClick(ByVal sender As Object, ByVal e As System.Windows.Forms.ColumnClickEventArgs) Handles ListViewCoilSrach.ColumnClick
''''        ''' �\�[�g
''''        ListViewCoilSrach.ListViewItemSorter = New tClass.tClass_Comparer(Me, e.Column, tClass.tClass_Comparer.ComparerType.str, oldOrder)
''''    End Sub
''''    '''���������w�� (�\�[�g�����s�����炱�ꂪ�v�f�����R�[�������)
''''    Public Function Comparer_InfSet(ByVal x As Object, ByVal y As Object, ByVal KeyIndex As Integer, ByRef setX As Object, ByRef setY As Object) As Integer Implements tClass.tIface_Comparer.Comparer_InfSet
''''        setX = CType(x, ListViewItem).SubItems(KeyIndex).Text
''''        setY = CType(y, ListViewItem).SubItems(KeyIndex).Text
''''    End Function
''''End Class
#End Region

Namespace tClass

    '///////////////////////////////////////////////////////////////////////////////
    '[Generic�Ή�]
    '///////////////////////////////////////////////////////////////////////////////
#If VBC_VER >= 8.0 Then                         ' Generic �ɑΉ����Ă���o�[�W������

    ''' <summary>
    ''' �����{�� (�����l����������)
    ''' </summary>
    ''' <typeparam name="KEY">�����Ώۂ̃I�u�W�F�N�g</typeparam>
    ''' <typeparam name="SERCH">���ۂɔ�r����l</typeparam>
    ''' <remarks></remarks>
    Public Class tClass_Comparer(Of KEY, SERCH)
        Implements System.Collections.Generic.IComparer(Of KEY)        'IComparer�C���^�[�t�F�C�X����������

        '********************************************************************
        '���J�ϐ���`
        '********************************************************************
        ''' <summary>
        ''' ��r�^��`
        ''' </summary>
        ''' <remarks></remarks>
        Public Enum iComparerType
            ''' <summary>���l</summary>
            int = 0
            ''' <summary>������</summary>
            str
            ''' <summary>���t</summary>
            day
            ''' <summary>�t�@�C���̓��t</summary>
            fileDay
        End Enum

        '********************************************************************
        '�����o�[�ϐ���`
        '********************************************************************
        Private m_comparerMethod As tIface.tIface_Comparer(Of KEY, SERCH)      '�R�[���惁�\�b�h �C���^�[�t�F�[�X

        Private m_Order As System.Windows.Forms.SortOrder       '���ёւ�����@ (�����A�~��)
        Private m_KeyIndex As Integer                           '���ёւ��鍀�� (0�I���W��)
        Private m_Type As iComparerType							'��r�^

        '********************************************************************
        '���\�b�h��`
        '********************************************************************
        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <param name="comparerMethod">�R�[���惁�\�b�h �C���^�[�t�F�[�X</param>
        ''' <param name="keyIndex">���ёւ��鍀��(0�I���W��)</param>
        ''' <param name="type">��r�^</param>
        ''' <param name="order">���ёւ�����@(�����A�~��)</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal comparerMethod As tIface.tIface_Comparer(Of KEY, SERCH), ByVal keyIndex As Integer, ByVal type As iComparerType, Optional ByVal order As SortOrder = SortOrder.Ascending)
            m_comparerMethod = comparerMethod
            m_KeyIndex = keyIndex
            m_Type = Type
            m_Order = order
        End Sub

        ''' <summary>
        ''' ��r���@ (sort�ɂ�����)
        ''' </summary>
        ''' <param name="x">��r�Ώ�</param>
        ''' <param name="y">��r�Ώ�</param>
        ''' <returns>0�F���� 0��菬����:X�̕��������� 0���傫��:X�̕����傫��</returns>
        ''' <remarks></remarks>
        Public Function Compare(ByVal x As KEY, ByVal y As KEY) As Integer Implements System.Collections.Generic.IComparer(Of KEY).Compare
            Dim retc As Double              '�������� x��y��菬�����Ƃ��̓}�C�i�X�̐��A�傫���Ƃ��̓v���X�̐��A�����Ƃ���0��Ԃ�
            Dim setX As SERCH = Nothing
            Dim setY As SERCH = Nothing
            '' �C���^�[�t�F�[�X ��`�� ���\�b�h�Ăяo��
            m_comparerMethod.Comparer_InfSet(x, y, m_KeyIndex, setX, setY)

            '' x��y��菬�����Ƃ��̓}�C�i�X�̐��A�傫���Ƃ��̓v���X�̐��A�����Ƃ���0��Ԃ�
            '''''''' �ǂ��������s�� �Ƃ肠�����A��xObject�ɕϊ����� (�{�b�N�X���A�{�b�N�X����������������̂Ŏ��߂���)
            Select Case m_Type
                Case iComparerType.int
                    If Not IsNumeric(setX) Then setX = CObj(Double.MinValue)
                    If Not IsNumeric(setY) Then setY = CObj(Double.MinValue)
                    retc = CDbl(CObj(setX)) - CDbl(CObj(setY))

                Case iComparerType.str
                    retc = String.Compare(CStr(CObj(setX)), CStr(CObj(setY)))
                Case iComparerType.day
                    retc = Date.Compare(CDate(CObj(setX)), CDate(CObj(setY)))
                Case iComparerType.fileDay
                    retc = DateTime.Compare(CObj(setX), CObj(setY))
            End Select

            '' �~���̂Ƃ���+-���t�]������
            If m_Order = SortOrder.Descending Then retc = -retc

            '' �I�[�o�[�t���[�΍�
            Dim iretc As Integer
            If retc < Integer.MinValue Then
                iretc = Integer.MinValue
            ElseIf retc > Integer.MaxValue Then
                iretc = Integer.MaxValue
            Else
                iretc = CInt(retc)
            End If
            Return iretc

        End Function

    End Class
#End If

    '///////////////////////////////////////////////////////////////////////////////
    '[Generic��Ή�]
    '///////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �����{�� (�����l����������)
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_Comparer
        Implements System.Collections.IComparer        'IComparer�C���^�[�t�F�C�X����������

        '********************************************************************
        '���J�ϐ���`
        '********************************************************************
        ''' <summary>
        ''' ��r�^��`
        ''' </summary>
        ''' <remarks></remarks>
        Public Enum iComparerType
            ''' <summary>���l</summary>
            int = 0
            ''' <summary>������</summary>
            str
            ''' <summary>���t</summary>
            day
            ''' <summary>�t�@�C���̓��t</summary>
            fileDay
        End Enum

        '********************************************************************
        '�����o�[�ϐ���`
        '********************************************************************
        Private m_comparerMethod As tIface.tIface_Comparer      '�R�[���惁�\�b�h �C���^�[�t�F�[�X

        Private m_Order As System.Windows.Forms.SortOrder       '���ёւ�����@ (�����A�~��)
        Private m_KeyIndex As Integer                           '���ёւ��鍀�� (0�I���W��)
        Private m_Type As iComparerType                         '��r�^

        '********************************************************************
        '���\�b�h��`
        '********************************************************************
        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <param name="comparerMethod">�R�[���惁�\�b�h �C���^�[�t�F�[�X</param>
        ''' <param name="keyIndex">���ёւ��鍀��(0�I���W��)</param>
        ''' <param name="type">��r�^</param>
        ''' <param name="order">���ёւ�����@(�����A�~��)</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal comparerMethod As tIface.tIface_Comparer, ByVal keyIndex As Integer, ByVal type As iComparerType, Optional ByVal order As SortOrder = SortOrder.Ascending)
            m_comparerMethod = comparerMethod
            m_KeyIndex = keyIndex
            m_Type = type
            m_Order = order
        End Sub


        ''' <summary>
        ''' ��r���@ (sort�ɂ�����)
        ''' </summary>
        ''' <param name="x">��r�Ώ�</param>
        ''' <param name="y">��r�Ώ�</param>
        ''' <returns>0�F���� 0��菬����:X�̕��������� 0���傫��:X�̕����傫��</returns>
        ''' <remarks></remarks>
        Public Function Compare(ByVal x As Object, ByVal y As Object) As Integer Implements IComparer.Compare
            Dim retc As Double              '�������� x��y��菬�����Ƃ��̓}�C�i�X�̐��A�傫���Ƃ��̓v���X�̐��A�����Ƃ���0��Ԃ�
            Dim setX As Object = Nothing
            Dim setY As Object = Nothing
            '' �C���^�[�t�F�[�X ��`�� ���\�b�h�Ăяo��
            m_comparerMethod.Comparer_InfSet(x, y, m_KeyIndex, setX, setY)

            '' x��y��菬�����Ƃ��̓}�C�i�X�̐��A�傫���Ƃ��̓v���X�̐��A�����Ƃ���0��Ԃ�
            Select Case m_Type
                Case iComparerType.int
                    If Not IsNumeric(setX) Then setX = Double.MinValue
                    If Not IsNumeric(setY) Then setY = Double.MinValue
                    retc = CDbl(setX) - CDbl(setY)
                Case iComparerType.str
                    retc = String.Compare(CStr(setX), CStr(setY))
                Case iComparerType.day
                    retc = Date.Compare(CDate(setX), CDate(setY))
                Case iComparerType.fileDay
                    retc = DateTime.Compare(CType(setX, DateTime), CType(setY, DateTime))
            End Select

            '' �~���̂Ƃ���+-���t�]������
            If m_Order = SortOrder.Descending Then retc = -retc

            '' �I�[�o�[�t���[�΍�
            Dim iretc As Integer
            If retc < Integer.MinValue Then
                iretc = Integer.MinValue
            ElseIf retc > Integer.MaxValue Then
                iretc = Integer.MaxValue
            Else
                iretc = CInt(retc)
            End If
            Return iretc
        End Function
    End Class
End Namespace
