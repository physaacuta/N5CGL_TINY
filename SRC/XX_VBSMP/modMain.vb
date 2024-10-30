'*********************************************************************************
' �G���g���[
'	[Ver]
'		Ver.01    2012/10/02  �V�K�쐬
'
'	[����]
'		�EVB���` ���̂܂܃R�s�y�łn�j�B�������́A�������ɒu������
'*********************************************************************************
Option Strict On					' �S�r���A�S�v���O�����K�{

Imports KizuLibCLI					' �r��CLI���C�u�����B���[���X���b�g���g�p���K�{
Imports tClassLibrary				' �r��VB���C�u�����B



Public Module modMain

	Public Const XX_VBSMP As String = "XX_VBSMP"	' �� ����́A�e�X�g�f�[�^�B�z���g�͒萔���g�p
	Public Const SYSTEM_TITLE As String = "test"	' �� ����́A�e�X�g�f�[�^�B�z���g�͒萔���g�p


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���[�o���ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Public gcls_Log As tClass.tClass_LogManager = Nothing				' ���O�Ǘ��N���X (�{��)
	Public g_NOA As Integer												' NOA�ڑ��t���O (0:�I�����C��  1:���u)


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���[�J���ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	' ���C���X���b�h�Ǘ��p (�قڕK�{�̊�{�̌`)
	Private WithEvents mcls_Mod As New KizuLibCLI.KizuMod				' �����N���X �i�ʃX���b�h�Ȃ̂Œ��Ӂj
	Private m_ap As ApplicationContext									' �R���e�L�X�g

	Private WithEvents m_trmFrmThread As New Timer						' ���[���X���b�g�p�X���b�h�����ʃX���b�h�ւ̋��n��
	Private m_MailQue As New tClass.tClass_Queue(Of COMMON_QUE_L)		' ���[���X���b�g�̃L���[

	Private mfrm_Menu As frmMenu										' �A�N�Z�X�t�H�[��

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ��{��`
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �G���g���[�|�C���g
	''' </summary>
	''' <remarks></remarks>
	Public Function Main() As Integer


		'//======================================================
		'  �O����
		'' ini�t�@�C����� �萔�I�ȃO���[�o�[���ϐ��ǂݍ���
		g_NOA = tMod.tMod_IniFile.ReadIniInt(INI_COMMON, "NOA", TASKINI_NAME)


		'''' �^�X�N������
		KizuLibCLI.KizuMod.SetLineID()
		'' �^�X�N�C�j�V���� (DB�ւ̃A�N�Z�X�͖���)
		If 0 <> mcls_Mod.Task_Init(XX_VBSMP) Then
			If NOA_ONLINE = g_NOA Then
				KizuLibCLI.KizuMod.SysLog(SYSNO_TASK_INIT_ERR, String.Format("[{0}]", XX_VBSMP))
			End If
			End
		End If

		'' ���O�N���X����
		gcls_Log = New tClassLibrary.tClass.tClass_LogManager(KizuMod.GetMyTaskName(), Nothing, Nothing)
		gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "�I�y���[�^��ʋN��")


		'//======================================================
		'  ��ʎ��s

		'' ��ʕ\��
		Dim showMontor As Integer = 1		' ���j���[�\���Ώۂ̃��j�^�[No (1�I���W��)

		'' �� �����Ŋe��ʐ؂�ւ�(���j���[��ʂ̕\��)
		mfrm_Menu = New frmMenu
		mfrm_Menu.Show(showMontor)


		'' Mail�X���b�h�����ʃX���b�h�ւ̋��n���^�C�}�[�J�n()
		m_trmFrmThread.Interval = 100
		m_trmFrmThread.Start()

		'' ���C���X���b�h����
		m_ap = New ApplicationContext
		Application.Run(m_ap)			'ExitThread���R�[�������܂� �����Ńu���b�N
		m_ap.Dispose()


		'//======================================================
		'  �I������
		Try
			'''' �����܂ŗ����� �I����
			' �t�H�[���J��
			If Not mfrm_Menu Is Nothing Then
				mfrm_Menu.Close()
				mfrm_Menu.Dispose()
				mfrm_Menu = Nothing
			End If

			' ���[���X���b�g�L���[�폜
			If Not m_MailQue Is Nothing Then
				m_MailQue.Dispose()
				m_MailQue = Nothing
			End If
		Catch ex As Exception
			gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("{0} [{1}]", "���C�������I���ُ�", ex.Message))
		End Try

		'' ���̑��N���X���
		gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "�I�y���[�^��ʏI��")
		gcls_Log.Dispose()
		gcls_Log = Nothing

		'' �^�X�N�I��
		mcls_Mod.Task_Exit()
	End Function

	''' <summary>
	''' �^�X�N�I������
	''' </summary>
	''' <remarks></remarks>
	Public Sub TaskEnd()
		m_ap.ExitThread()										' ���C���X���b�h�I��
	End Sub


	''' <summary>
	''' ���[���X���b�g��M
	''' </summary>
	''' <param name="pData">�f�[�^</param>
	''' <param name="nEvtNo">�C�x���g�ԍ�</param>
	''' <remarks></remarks>
	Private Sub mcls_Mod_evRecvMail(ByVal pData As Object, ByVal nEvtNo As Integer) Handles mcls_Mod.evRecvMail
		Dim ii As Integer = 0
		Dim que_l As COMMON_QUE_L = CType(pData, COMMON_QUE_L)	' ��{��Ԃ悭�g�����߁A���̌`�ɂ��Ă����B

		'''' ��ʃX���b�h�֓n��
		m_MailQue.SetItem(que_l)
	End Sub

	''' <summary>
	''' ���[���X���b�g��M
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub m_trmFrmThread_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_trmFrmThread.Tick
		Dim que_l As COMMON_QUE_L = m_MailQue.GetItem(0)
		If que_l.nEventNo = 0 Then Return


		' �� �����̓T���v�� -------------->>>>
		Dim que_m As COMMON_QUE_STRMIX = CType(mcls_Mod.typConv(que_l, GetType(COMMON_QUE_STRMIX)), COMMON_QUE_STRMIX)
		'Dim quew As COMMON_QUE_MIX = CType(mcls_Mod.typConv(que_l, GetType(COMMON_QUE_MIX)), COMMON_QUE_MIX)	' 
		Dim que_s As COMMON_QUE_S = CType(mcls_Mod.typConv(que_l, GetType(COMMON_QUE_S)), COMMON_QUE_S)
		' <<<------------------------------


		Select Case que_l.nEventNo

			'//-----------------------------------------------------------
			Case FACT_KS_DB_STATEUS		'DB��ԕύX�ʒm

				'//-----------------------------------------------------------
			Case FACT_KS_PCNAME_CHANGE
				'�����Ȃ�

				'//-----------------------------------------------------------
			Case FACT_KS_END		' �^�X�N�I��
				gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "�^�X�N�I���ʒm��M")

				m_ap.ExitThread()	' �X���b�h�I��

			Case Else
				KizuLibCLI.KizuMod.SysLog(SYSNO_MAILNO_ERR, String.Format("[{0}]", "err_code = [" & que_l.nEventNo & "]"))
		End Select

	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ����
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' �t�H�[���^�C�g���ݒ�
	''' </summary>
	''' <param name="frmData">�t�H�[��</param>
	''' <remarks></remarks>
	Public Sub SetTitleName(ByRef frmData As Form)

		frmData.Text = SYSTEM_TITLE & Space(2) & frmData.Text

	End Sub


End Module
