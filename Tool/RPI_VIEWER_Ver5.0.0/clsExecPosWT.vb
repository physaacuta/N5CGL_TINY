'*********************************************************************************
'�ʔ����֌W�N���X (�t�H�[���^�C�}�[���g�p�Ȃ̂ŁA�X���b�h�ł͖����̂Œ���)
'	[Ver]
'		Ver.01    2009/02/20  vs2005 �Ή�
'
'	[����]
'		�E�ʔ��������Z����
'*********************************************************************************
Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports System.Threading						' �X���b�h

Public Class clsExecPosWT
    '===================================================
    '' �C�x���g
    Public Event tEv_AutoRunning(ByVal nSpeed As Integer, ByVal nDp As Integer)	   ' �����X�V�ʒm

    '===================================================
    '' �����萔
    Private Const mc_nCycleTime As Integer = 60							' ��������s���� [ms]

    '===================================================
    '' �����o�[�N���X
    Private mcls_pRpiView As tClass_ViewRpiThread = Nothing				' RPI�t�@�C���\���N���X (�|�C���^)
    Private mfrm_pPopCont As frmPopControl								' �R���g���[����� (�|�C���^)
    Private mcnt_VBar As tCnt.tCnt_VScrollBar							' �X�N���[���o�[ (�|�C���^)

    '''' �K�{�Ǘ����
    Private WithEvents m_tmDspInterval As New Windows.Forms.Timer()		' ������^�C�}�[
    Private m_tmCycleTime As New Stopwatch								' ���C�����x����p���Ԍv��



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����ݒ�
    ''' <summary>
    ''' RPI�N���X
    ''' </summary>
    ''' <param name="cls">RPI�N���X</param>
    ''' <remarks></remarks>
    Public Sub InitCls_RpiView(ByRef cls As tClass_ViewRpiThread)
        mcls_pRpiView = cls
    End Sub
    Public Sub InitFrm_Control(ByRef frm As frmPopControl)
        mfrm_pPopCont = frm
    End Sub
    Public Sub InitCnt_Bar(ByRef cnt As tCnt.tCnt_VScrollBar)
        mcnt_VBar = cnt
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ����

    ''' <summary>
    ''' �X���b�h�X�^�[�g
    ''' </summary>
    Public Sub ThreadStart()
        m_tmDspInterval.Interval = 60
        m_tmDspInterval.Start()
        m_tmCycleTime.Start()
    End Sub

    ''' <summary>
    ''' �X���b�h�X�g�b�v
    ''' </summary>
    Public Sub ThreadStop()
        m_tmDspInterval.Stop()
        m_tmCycleTime.Stop()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �X���b�h���C��

    ''' <summary>
    ''' �^�C�}�[
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub m_trDspInterval_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_tmDspInterval.Tick
        If Not mcls_pRpiView.IsRpi() Then Return

        '''' ����̈ʒu����
        Dim nSpeed As Integer = mfrm_pPopCont.LineSpeed
        Dim nDp As Integer = mcnt_VBar.Value	   ' �\���ʒu [�\��pixel]
        If 0 <> nSpeed Then
            nDp = ExecPos(nSpeed, nDp)
        End If

        '''' �C�x���g�ʒm
        RaiseEvent tEv_AutoRunning(nSpeed, nDp)

        ' ����ʒu
    End Sub

    ''' <summary>
    ''' ����̈ʒu����
    ''' </summary>
    ''' <remarks></remarks>
    Private Function ExecPos(ByVal nSpeed As Integer, ByVal nDP As Integer) As Integer

        '''' ����̎��Ԃ��Z�o
        Dim tms As TimeSpan = m_tmCycleTime.Elapsed
        m_tmCycleTime.Reset()
        m_tmCycleTime.Start()


        '''' ����̒ʔ������Z�o
        Dim dwk As Double = nSpeed / 60.0		' [m/msec]
        dwk = dwk * tms.Milliseconds			' �^�C�}�[���ԕӂ� ���ꂾ���i�ދ��� [mm]
        Dim nWk As Integer = mcls_pRpiView.GetPos_LenToDp_Y(dwk)	' �\�����W�n��pixel�ʒu
        ' �Œ�1pixel�͕K���ړ�������
        If 0 < nSpeed AndAlso nWk <= 0 Then nWk = 1
        If 0 > nSpeed AndAlso nWk >= 0 Then nWk = -1

        ' ����̕\���ʒu
        nDP += nWk
        'If nDP > mcnt_VBar.MaximumVirtual Then nDP = mcnt_VBar.Minimum
        If nDP > mcnt_VBar.MaximumVirtual Then nDP = mcnt_VBar.MaximumVirtual
        If nDP < mcnt_VBar.Minimum Then nDP = mcnt_VBar.Minimum


        'm_Output.WriteLine("{0},{1},{2}", m_nSpeed, tms.Milliseconds, nWk)

        Return nDP
    End Function
End Class
