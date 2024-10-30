'*********************************************************************************
' �����ݒ��ʂ̐e�N���X
'	[Ver]
'		Ver.01    2007/03/31  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports System.Runtime.InteropServices

''' <summary>
''' �����ݒ��ʂ̐e�N���X
''' </summary>
''' <remarks></remarks>
Public Class frmPop_TimeSet

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����ݒ�p�\����
    '/ /////////////////////////////////////////////////////////////////////////////////
    <StructLayout(LayoutKind.Sequential, Size:=SIZE_MSL, pack:=1)> _
    Structure COMMON_QUE_TIME
        Dim nEventNo As Integer
        Dim nLineNo As Integer

        Dim wYear As Short
        Dim wMonth As Short
        Dim wDayOfWeek As Short
        Dim wDay As Short
        Dim wHour As Short
        Dim wMinute As Short
        Dim wSecond As Short
        Dim wMilliseconds As Short

        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=CInt(SIZE_MSL_DATA / 2) - 16)> _
        Dim dataL() As Short

        Public Sub Inisialize()
            ReDim dataL(CInt(SIZE_MSL_DATA / 2) - 16 - 1)
        End Sub
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �O���C�x���g
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �t�@���N�V�����{�^�������C�x���g
    ''' </summary>
    ''' <param name="FncNo">0�I���W��</param>
    ''' <param name="selectDay">�ݒ莞��</param>
    ''' <remarks></remarks>
    Public Shadows Event tEv_FunctionButton(ByVal FncNo As Integer, ByVal selectDay As Date)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �C�x���g
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmPop_TimeSet_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        ' �����\��
        Call timClock_Tick(Nothing, Nothing)
    End Sub
    ''' <summary>
    ''' �����\��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub timClock_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timClock.Tick
        lblClock.Text = Now.ToString("yyyy/MM/dd HH:mm:ss")
    End Sub
    ''' <summary>
    ''' ���t�R���g���[���ɑ΂��� �z�C�[���ŉ��Z���Z���\�Ƃ���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpTime.MouseWheel
        ' ���݂�datetimepicker �Ɂ����L�[�������ꂽ���Ƃ���
        SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
    End Sub
    ''' <summary>
    ''' �J�����_�[���t��ݒ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub mclMonth_DateChanged(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DateRangeEventArgs) Handles mclMonth.DateChanged
        '' �J�����_�[�̔N��ύX����� �ςȑI����ԂɂȂ�� (�ǂ����R���g���[�����o�N���Ă���炵�����)
        mclMonth.SelectionStart = e.Start
        mclMonth.SelectionEnd = e.Start
    End Sub
    ''' <summary>
    ''' �t�@���N�V�����{�^���N���b�N
    ''' </summary>
    ''' <param name="FncNo"></param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmPop_TimeSet_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles MyBase.tEv_FunctionButton
        Dim day As Date = CDate(mclMonth.SelectionStart.ToString("yyyy/MM/dd ") & dtpTime.Value.ToString("HH:mm:ss"))

        ' �I��������n��
        RaiseEvent tEv_FunctionButton(FncNo, day)
    End Sub

End Class
