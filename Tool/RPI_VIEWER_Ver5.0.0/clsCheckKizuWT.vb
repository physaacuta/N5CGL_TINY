'*********************************************************************************
'�ʔ����֌W�N���X (���񏈗��Ń`�F�b�N���s��)
'	[Ver]
'		Ver.01    2009/02/20  vs2005 �Ή�
'
'	[����]
'		�E�����L���͈�	���]�̂�
'						ON�C�x���g�F��ʉ��[�{xm ���B�� (�O��`����`�F�b�N�����Ŕ͈͓��ɁA�Ώ��r������ꍇ�j
'						�t���OON���F��ʉ��[�{xm�`��ʏ�[�܂ŁB ������
'		�E��~			���]�̂�
'						ON�C�x���g�F��ʒ����ɗ����Ƃ��B(�O��`����`�F�b�N��������ʒ����ʒu�B���̂Ƃ��ɁA�\���͉�ʒ����ɂ���)
'						�t���OON���F��莞�Ԍo�߂Ŏ���OFF�B		���C�����x���グ��
'*********************************************************************************
Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports System.Threading						' �X���b�h

Public Class clsCheckKizuWT

    '===================================================
    '/ ���J�C�x���g
    Public Event tEv_DspPosChange(ByVal nLen As Integer)				' ��ʒ����\���ʒu�ύX�v�� [mm]


    '===================================================
    '' �����萔
    Private Const mc_nCycleTime As Integer = 100						' ��������s���� [ms]


    '===================================================
    '' �����o�[�N���X
    Private mcls_pParam As clsParamManager								' �p�����[�^�Ǘ��N���X (�|�C���^)
    Private mcls_pRpiView As tClass_ViewRpiThread = Nothing				' RPI�t�@�C���\���N���X (�|�C���^)
    Private mfrm_pPopCont As frmPopControl								' �R���g���[����� (�|�C���^)


    Private mcls_WThread As tClass_WorkThread = Nothing					' ���[�J�[�X���b�h�{��
    Private m_evGo As New Threading.AutoResetEvent(False)				' �������s�C�x���g

    '' �����C�x���g
    Private m_trAutoStop As New Threading.Timer(AddressOf AutoStopEnd)	'���C����~���^�C�}�[



    '''' �K�{�Ǘ����
    Private m_bEnable As Boolean										' ���������L������ (true:�L��)
    Private m_nPosOld As Integer										' �O��̒ʔ��� (�\����[) [mm]
    Private m_nPerSpeed As Integer										' �����A��~�O�̃��C�����x


    '''' �R�C���P�ʂŊǗ����K�v�ȏ��
    Private m_nMen As Integer											' ��

    ' �f�[�^
    Private m_lstDetect As List(Of DATA_DETECT_INF) = Nothing			' �r��񃊃X�g


    ' ����
    Private m_bAutoDown As Boolean										' ������Ԏ� true�B���ŏI���f�́AfrmMain�Ŕ���
    Private m_bAutoStop As Boolean										' ��~���Ԓ� true�B���ŏI���f�́AfrmMain�Ŕ���
    Private m_bAutoChancel As Boolean									' ������Ԓ���STOP�t���� true�B���ŏI���f�́AfrmMain�Ŕ���
    Private m_nRanpGId(MC_RANP_CNT - 1) As Integer						' �r�ڋ߃����v��Gr
    Private m_nRanpGYu(MC_RANP_CNT - 1) As Integer						' �r�ڋ߃����v�̗D��x




    '===================================================
    '' �O���A�N�Z�X
    Public ReadOnly Property IsAutoDown() As Boolean
        Get
            Return m_bAutoDown
        End Get
    End Property

    Public ReadOnly Property IsAutoStop() As Boolean
        Get
            Return m_bAutoStop
        End Get
    End Property

    Public Function RanpStat() As Integer()
        Return m_nRanpGId
    End Function


    ''' <summary>
    ''' ���������L������
    ''' </summary>
    Public Property Enable() As Boolean
        Get
            Return m_bEnable
        End Get
        Set(ByVal value As Boolean)
            m_bEnable = value

            '�������́A�������������������Ă����狭������
            If Not value Then
                Call AutoStopEnd(Nothing)
                Call AutoDownEnd()
            End If

        End Set
    End Property



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����ݒ�
    ''' <summary>
    ''' �p�����[�^�N���X�Z�b�g
    ''' </summary>
    ''' <param name="cls">�v�Z�N���X</param>
    Public Sub InitCls_Param(ByRef cls As clsParamManager)
        mcls_pParam = cls
    End Sub

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

    ''' <summary>
    ''' �r��񃊃X�g
    ''' </summary>
    ''' <param name="lst">�r��񃊃X�g</param>
    ''' <remarks></remarks>
    Friend Sub InitData_LstDetect(ByRef lst As List(Of DATA_DETECT_INF))
        m_lstDetect = lst
    End Sub

    ''' <summary>
    ''' �R�C���ؑ�
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub CoilChange()
        m_nPosOld = 0
        m_nPerSpeed = 0
        '�������́A�������������������Ă����狭������
        Call AutoStopEnd(Nothing)
        Call AutoDownEnd()
    End Sub

    ''' <summary>
    ''' ���������L�����Z��
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub AutoStopCancel()
        m_nPosOld = 0
        m_nPerSpeed = 0
        m_bAutoChancel = True		' ���������L�����Z��
        '�������́A�������������������Ă����狭������
        Call AutoStopEnd(Nothing)
        Call AutoDownEnd()
    End Sub

    ''' <summary>
    ''' �ʕύX
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub MenChange(ByVal nMen As Integer)
        m_nMen = nMen
    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ����

    ''' <summary>
    ''' �X���b�h�X�^�[�g
    ''' </summary>
    Public Function ThreadStart(Optional ByVal priority As Threading.ThreadPriority = Threading.ThreadPriority.Normal) As Boolean
        If mcls_WThread Is Nothing Then
            mcls_WThread = New tClass_WorkThread

            '''' ���C���X���b�h���s
            mcls_WThread.ThreadStart(AddressOf mainThread, priority, "CheckKizuWT", True)
            Return True
        End If
        Return False
    End Function


    ''' <summary>
    ''' �X���b�h�X�g�b�v
    ''' </summary>
    Public Sub ThreadStop()
        '' �C���X�^���X�m�F
        If Not mcls_WThread Is Nothing Then
            '' ���C���X���b�h�I��
            mcls_WThread.ThreadStop(2000)
            mcls_WThread.Dispose()
            mcls_WThread = Nothing
        End If
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �X���b�h���C��

    Private Sub mainThread()
        Dim EventAry(2 - 1) As Threading.WaitHandle		' �C�x���g
        Dim nEvtNo As Integer							' WaitAny ���A���

        '''' �V�O�i���Z�b�g
        EventAry(0) = mcls_WThread.g_evStop				' �X���b�h�I��
        EventAry(1) = m_evGo

        '''' �Ƃ肠�����������s
        m_evGo.Set()

        '''' �V�O�i���҂�
        Try
            While (True)
                '' �V�O�i���҂�
                nEvtNo = Threading.WaitHandle.WaitAny(EventAry, mc_nCycleTime, False)
                Select Case nEvtNo
                    Case 0								' �I���m�F
                        Exit While
                    Case Else							' �������s,�^�C���A�E�g��
                        Call ExecMain()
                End Select
            End While
        Catch ex As Threading.ThreadAbortException
        Catch ex As Exception
        End Try
    End Sub


    ''' <summary>
    ''' ��������s ���X���b�h���Ȃ̂ŁA�R���g���[���ւ̃A�N�Z�X�͋֎~
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ExecMain()
        If Not mcls_pRpiView.IsRpi() Then Return
        If m_lstDetect Is Nothing Then Return

        '''' �K�v�ȏ��擾
        Dim nSpeed As Integer = mfrm_pPopCont.LineSpeed			' ���C�����x
        Dim nDpsAria As Integer = CInt(mcls_pRpiView.GetPos_DpToLen_Y(mcls_pRpiView.GetImg_AriaInf.nAriaHeight))	' ��ʃG���A�̋���
        Dim DspInf As tClass_ViewRpiThread.DSP_ARIA_INF = mcls_pRpiView.GetDspAriaInf
        Dim nPosF As Integer = CInt(mcls_pRpiView.GetPos_DpToLen_Y(DspInf.DspPixelTotalOffset.Y))


        ' ==========================================
        ' �e�͈� �Z�o
        ' ��~ �͈�
        Dim nStop(2 - 1) As Integer		' �J�n,�I�� [mm]
        nStop(0) = m_nPosOld + CInt(nDpsAria / 2)
        nStop(1) = nPosF + CInt(nDpsAria / 2)

        ' ���� �͈�
        Dim nDown(2 - 1) As Integer		' �J�n,�I��
        nDown(0) = nPosF - mfrm_pPopCont.AutoE	' �I��
        nDown(1) = nPosF + nDpsAria + mfrm_pPopCont.AutoS	 ' �J�n

        ' �r�ڋ߃����v
        Dim nRanpGId(MC_RANP_CNT - 1) As Integer						' �r�ڋ߃����v��Gr
        Dim nRanpGYu(MC_RANP_CNT - 1) As Integer						' �r�ڋ߃����v�̗D��x
        Dim dWid As Single = mcls_pRpiView.GetPos_DpToLen_X(mcls_pRpiView.GetImg_AriaInf.nAriaWidth) / MC_RANP_CNT '1��Ԃ̕�
        Dim ranp(2 - 1) As Integer		' �J�n,�I��
        ranp(0) = nPosF - nDpsAria - mfrm_pPopCont.RanpE
        ranp(1) = nPosF + nDpsAria + mfrm_pPopCont.RanpS


        ' ==========================================
        ' �r�`�F�b�N
        Dim bStop As Boolean = False
        Dim bDown As Boolean = False

        ' �ʃX���b�h�Ń��X�g�z��ɃA�N�Z�X����̂ŁA�r�ύX�Ƃ��œǂݒ������ɃG���[�ɂȂ�\����B������Atry���Ă���
        Try
            For Each data As DATA_DETECT_INF In m_lstDetect
                If Not data.bDsp Then Continue For ' ���������\���t�B���^�[�ŏ����Ă����r�͑ΏۊO

                ' �����v
                ' ����̕\���ΏہH
                If data.nY >= ranp(0) AndAlso data.nY <= ranp(1) Then

                    Dim nRpW As Integer = CInt((data.nFrameXmin + data.nFrameXmax) / 2.0) + mcls_pRpiView.GetRpi_THeader.caminfo(data.nCamPos).nBaseLeft	' �r���S�ʒu [RP]
                    Dim nIndex As Integer = CInt(Int(mcls_pRpiView.GetPos_RpToLen_X(nRpW) / dWid))
                    If nIndex < 0 Then nIndex = 0
                    If nIndex >= MC_RANP_CNT Then nIndex = MC_RANP_CNT - 1
                    ' ���E���]�΍�
                    If data.nBaseX <> data.nX Then
                        nIndex = MC_RANP_CNT - nIndex - 1
                    End If

                    If 0 = nRanpGYu(nIndex) OrElse nRanpGYu(nIndex) > data.gYu Then
                        nRanpGId(nIndex) = data.gId
                        nRanpGYu(nIndex) = data.gYu
                    End If
                End If



                '''' �ȍ~�́A���]�̂�
                If Not m_bEnable Or 0 >= nSpeed Then Continue For

                ' ��~ On�̂�
                If 0 <> mfrm_pPopCont.AutoStopTime Then
                    If Not bStop AndAlso data.nY > nStop(0) AndAlso data.nY <= nStop(1) Then
                        If data.gYu <= mfrm_pPopCont.AutoStopGr() Then
                            bStop = True
                        End If
                    End If
                End If

                ' ���� �͈͓�
                If Not bDown AndAlso data.nBaseY > nDown(0) And data.nBaseY <= nDown(1) Then
                    If data.gYu <= mfrm_pPopCont.AutoDownGr() Then
                        bDown = True
                    End If

                    If data.nY <= nStop(1) AndAlso m_bAutoChancel Then
                        bDown = False			' ���������L�����Z�����́A�ʏ�ɖ߂�
                    End If
                End If
            Next
        Catch ex As Exception

        End Try



        ' ==========================================
        '''' ���ʔ��f
        ' ������~
        If bStop Then Call AutoStopStart()
        ' ��������
        If bDown And Not m_bAutoDown Then AutoDownStart() ' �ʏ큨��������
        If Not bDown And m_bAutoDown Then AutoDownEnd() ' �����������ʏ�

        ' �r�ڋ߃����v
        m_nRanpGId = nRanpGId
        m_nRanpGYu = nRanpGYu


        '''' �Ō�`�F�b�N�ʒu
        m_nPosOld = nPosF
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ������~

    ''' <summary>
    ''' ������~�J�n
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub AutoStopStart()

        If Not m_bAutoDown Then m_nPerSpeed = mfrm_pPopCont.LineSpeed ' AutoDown�ł����Ă���B���������O�̒l
        mfrm_pPopCont.LineSpeed = 0
        m_bAutoStop = True

        ' �^�C�}�[���s
        Dim nStopTime As Integer = mfrm_pPopCont.AutoStopTime	   '[ms]
        m_trAutoStop.Change(nStopTime, 0)	' �N��
    End Sub

    ''' <summary>
    ''' ������~���� (�^�C�}�[���s)
    ''' </summary>
    ''' <param name="state"></param>
    ''' <remarks></remarks>
    Private Sub AutoStopEnd(ByVal state As Object)
        If Not m_bAutoStop Then Return ' ������~���͉����B����ȊO�͉������Ȃ��B 2014.06.24 J.Wakazono
        m_bAutoStop = False

        If m_bAutoDown Then
            mfrm_pPopCont.LineSpeed = mfrm_pPopCont.AutoDownMpm
        Else
            mfrm_pPopCont.LineSpeed = m_nPerSpeed
        End If
    End Sub



    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ ��������
    ''' <summary>
    ''' ���������J�n
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub AutoDownStart()
        If m_bAutoStop Then Return ' ������~���͖����B�ʏ킠�肦�Ȃ�

        If Not m_bAutoDown Then m_nPerSpeed = mfrm_pPopCont.LineSpeed
        mfrm_pPopCont.LineSpeed = mfrm_pPopCont.AutoDownMpm
        m_bAutoDown = True
        m_bAutoChancel = False

    End Sub

    ''' <summary>
    ''' ������������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub AutoDownEnd()
        If m_bAutoStop Then Return ' ������~���͖����B�ʏ킠�肦�Ȃ�
        If Not m_bAutoDown Then Return ' �����������͉����B����ȊO�͉������Ȃ��B 2014.06.24 J.Wakazono

        m_bAutoDown = False
        mfrm_pPopCont.LineSpeed = m_nPerSpeed
    End Sub

End Class


'�����Ɠ�����[
'''' <summary>
'''' C++��CreateWaitableTimer, SetWaitableTimer �ɑ���
'''' </summary>
'''' <remarks></remarks>
'Public Class tClass_WaitableTimer
'	Public g_evHandler As New Threading.AutoResetEvent(False)
'	Private m_evTimer As New Threading.Timer(AddressOf TimerProc)


'	''' <summary>
'	''' �^�C�}�[���s�J�n
'	''' </summary>
'	''' <param name="nFastTime">���񎞊�[ms]</param>
'	''' <param name="nCycleTime">�J��Ԃ�����[ms]</param>
'	''' <remarks></remarks>
'	Public Sub SetWaitableTimer(ByVal nFastTime As Integer, Optional ByVal nCycleTime As Integer = 0)
'		m_evTimer.Change(nFastTime, nCycleTime)
'	End Sub

'	''' <summary>
'	''' �^�C���A�b�v
'	''' </summary>
'	''' <param name="state"></param>
'	''' <remarks></remarks>
'	Private Sub TimerProc(ByVal state As Object)
'		g_evHandler.Set()
'	End Sub
'End Class
