'*********************************************************************************
' DB�ڑ��m���ҋ@����ʃN���X
'	[Ver]
'		Ver.01    2007/07/31  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

Public Class frmPop_DbCheck
	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���C�x���g
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �t�@���N�V�����{�^�������C�x���g (�x�[�X�̒u������)
	''' </summary>
	''' <param name="dbMode">DB�̏��</param>
	''' <remarks></remarks>
	Public Shadows Event tEv_FunctionButton(ByVal dbMode As Boolean)


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���[�J���ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_strSession As String						' ini�t�@�C����DB�Z�b�V����
	Private m_nNoa As Integer							' NOA�ݒ���
	Private m_bDBOK As Boolean = False					' DB�ڑ����
	Private m_wThread As tClass.tClass_WorkThread = Nothing		  ' DB�`�F�b�N�p�X���b�h


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���J���\�b�h
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' �R���X�g���N�^
	''' </summary>
	''' <param name="strDbSession">DB�Z�b�V����</param>
	''' <param name="nNoa">�I�����C�������u��</param>
	''' <remarks></remarks>
	Public Sub New(ByVal strDbSession As String, ByVal nNoa As Integer)

		' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
		InitializeComponent()

		' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B
		m_strSession = strDbSession
        m_nNoa = nNoa
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �C�x���g
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �t�H�[�����[�h
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmPop_MsgBox_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

		' ������
		Me.TopLevel = True						 ' �őO��
        btnF01.Enabled = CBool(IIf(m_nNoa = NOA_ONLINE, False, True))

		'''' ���C���X���b�h���s
		m_wThread = New tClass.tClass_WorkThread
		m_wThread.ThreadStart(AddressOf mainThread, Threading.ThreadPriority.Lowest, "DBcheck", True)
	End Sub

	''' <summary>
	''' �t�@���N�V�����{�^���N���b�N
	''' </summary>
	''' <param name="FncNo"></param>
	''' <param name="onKey"></param>
	''' <remarks></remarks>
	Private Sub frmPop_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles MyBase.tEv_FunctionButton
		' ��ʂ̏I�� (�C���X�^���X�͂��̂܂܂������)
		Me.Close()
		' ���[�J�[�X���b�h��~
		m_wThread.ThreadStop()
		m_wThread.Dispose()
		m_wThread = Nothing

		' �Ăь��֒ʒm
		RaiseEvent tEv_FunctionButton(m_bDBOK)
	End Sub


	''' <summary>
	''' �^�C�}�[
	''' </summary>
	Private Sub trmCycle_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trmCycle.Tick
		' �v���O���X�o�[�Z�b�g
		If ProgressBar1.Value = ProgressBar1.Maximum Then
			ProgressBar1.Value = 0
		End If
		ProgressBar1.Increment(1)

		' DB�ڑ��m�F
		If m_bDBOK Then
			' DB�ɂȂ�������A����ʂ��I�����āA�Ăяo�����ɃC�x���g�ʒm
			frmPop_tEv_FunctionButton(Nothing, Nothing)
		End If
	End Sub


	''' <summary>
	''' ���C���X���b�h
	''' </summary>
	''' <remarks></remarks>
	Private Sub mainThread()
		Dim EventAry(0) As Threading.WaitHandle		'�C�x���g
		Dim iRetc As Integer						'WaitAny ���A���

		'''' �V�O�i���Z�b�g
		EventAry(0) = m_wThread.g_evStop			' �X���b�h�I��

		' �܂�DB�̐ڑ��m�F
		m_bDBOK = tClass.tClass_SQLServer.OpenCheck(TASKINI_NAME, m_strSession)

		Try
			While (True)
				'' �V�O�i���҂�
				iRetc = Threading.WaitHandle.WaitAny(EventAry, 3000, False)

				'' �܂��I���m�F
				Select Case iRetc
					Case 0									'�I��
						Exit While

					Case Threading.WaitHandle.WaitTimeout	'�^�C���A�E�g
						' DB�ڑ��m�F
						m_bDBOK = tClass.tClass_SQLServer.OpenCheck(TASKINI_NAME, m_strSession)
					Case Else
						'���肦�Ȃ�
				End Select
			End While
		Catch ex As Threading.ThreadAbortException
		Catch ex As Exception
		End Try
	End Sub

End Class
