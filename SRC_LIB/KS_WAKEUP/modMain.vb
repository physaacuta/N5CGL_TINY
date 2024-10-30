'*********************************************************************************
' �G���g���[
'	[Ver]
'		Ver.01    2007/03/31  �V�K�쐬
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports System.Runtime.InteropServices

Module modMain
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �O���[�o���萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public Enum em_Delivery                                                                ' �ʒm���[�h
        iTO = 0         'TOKATU
        iTI             'TIKUSEKI
    End Enum


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �Q�Ƃł��Ȃ���`
	'/ /////////////////////////////////////////////////////////////////////////////////
	<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_MSL, Pack:=1)> _
	Public Structure COMMON_QUE_L
		Dim nEventNo As Integer
		Dim nLineNo As Integer

		<VBFixedArray(CInt((SIZE_MSL_DATA / 4)) - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=CInt((SIZE_MSL_DATA / 4)))> _
		Dim data() As Integer

		Public Sub inisialize()
			ReDim data(CInt((SIZE_MSL_DATA / 4)) - 1)
		End Sub

	End Structure




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Delegate Sub m_deleSetImgLoad(ByVal pc As Integer, ByVal rec As Integer)        ' �ʃX���b�h����̃R���g���[���A�N�Z�X

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���[�J���ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	'Private WithEvents mcls_Mod As New KizuLibCLI.KizuMod				' �����N���X
	Public WithEvents gcls_Mail As tClass.tClass_MailSlot				' ���[���X���b�g��M�N���X
	Private m_RecvQueBuf As COMMON_QUE_L									' ���[���X���b�g��M�o�b�t�@

	Private m_frm() As frmMain											' �A�N�Z�X�t�H�[�� (PC�̃��j�^���ɂ��ύX)
    Private m_ap As ApplicationContext                                  ' �R���e�L�X�g

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��`
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �G���g���[�|�C���g
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Main()
        Dim ii As Integer

		'' �^�X�N�C�j�V���� (DB�ւ̃A�N�Z�X�͖���)
		'KizuLibCLI.KizuMod.SetLineID()
		'      If 0 <> mcls_Mod.Task_Init(KS_WAKEUP, 0) Then
		'	KizuLibCLI.KizuMod.SysLog(SYSNO_TASK_INIT_ERR, String.Format("[{0}]", KS_WAKEUP))
		'          End
		'      End If
		Try
			gcls_Mail = New tClass.tClass_MailSlot(KS_WAKEUP, 300)
			gcls_Mail.Mail_ReadStart(m_RecvQueBuf, AddressOf tDele_GetMail)
		Catch ex As Exception
			End
		End Try

		'' PC�ɐڑ����Ă��郂�j�^���擾
		Threading.Thread.Sleep(3000)
        Dim monitor() As System.Windows.Forms.Screen
        monitor = System.Windows.Forms.Screen.AllScreens()
        Dim cnt As Integer = monitor.Length
        If cnt <= 0 Then cnt = 1
        ReDim m_frm(cnt - 1)

        '' ��ʕ\��
        For ii = 0 To m_frm.Length - 1
            m_frm(ii) = New frmMain(ii)     ' �t�H�[���𐶐����Ă���X���b�h�����璼�ڃA�N�Z�X�\
            m_frm(ii).Show()
        Next ii

        '' ���C���X���b�h����
        m_ap = New ApplicationContext
        Application.Run(m_ap)           'ExitThread���R�[�������܂� �����Ńu���b�N

        '' �I��
        For ii = 0 To m_frm.Length - 1
            m_frm(ii).Close()
            m_frm(ii).Dispose()
            m_frm(ii) = Nothing
        Next ii
		'mcls_Mod.Task_Exit()
		gcls_Mail.Mail_ReadStop()
		gcls_Mail.Dispose()
		gcls_Mail = Nothing
    End Sub


    ''' <summary>
    ''' ���[���X���b�g��M
    ''' </summary>
    ''' <param name="pData"></param>
    ''' <param name="nEvtNo"></param>
    ''' <remarks></remarks>
	'Private Sub mcls_Mod_evRecvMail(ByVal pData As Object, ByVal nEvtNo As Integer) Handles mcls_Mod.evRecvMail
	'    Dim ii As Integer

	'    If nEvtNo = FACT_KS_END Then                    ' �^�X�N�I��
	'        m_ap.ExitThread()

	'    ElseIf nEvtNo = FACT_KS_WAKEUP_01 Then          ' ����OK
	'        For ii = 0 To m_frm.Length - 1
	'            m_frm(ii).SetImgLoadEntry(em_Delivery.iTO, True)               ' �t�H�[�������X���b�h�ƕʃX���b�h�Ȃ̂Œ���
	'        Next ii

	'    ElseIf nEvtNo = FACT_KS_WAKEUP_02 Then          ' DBOK
	'        For ii = 0 To m_frm.Length - 1
	'            m_frm(ii).SetImgLoadEntry(em_Delivery.iTI, True)
	'        Next ii
	'    End If
	'End Sub
	Public Sub tDele_GetMail(ByVal recvVal As System.ValueType)
		Dim que As COMMON_QUE_L = CType(recvVal, COMMON_QUE_L)
		Dim nEvtNo As Integer = que.nEventNo

		Dim ii As Integer

		If nEvtNo = FACT_KS_END Then					' �^�X�N�I��
			m_ap.ExitThread()

		ElseIf nEvtNo = FACT_KS_WAKEUP_01 Then			' ����OK
			For ii = 0 To m_frm.Length - 1
				m_frm(ii).SetImgLoadEntry(em_Delivery.iTO, True)			   ' �t�H�[�������X���b�h�ƕʃX���b�h�Ȃ̂Œ���
			Next ii

		ElseIf nEvtNo = FACT_KS_WAKEUP_02 Then			' DBOK
			For ii = 0 To m_frm.Length - 1
				m_frm(ii).SetImgLoadEntry(em_Delivery.iTI, True)
			Next ii
		End If
	End Sub

End Module
