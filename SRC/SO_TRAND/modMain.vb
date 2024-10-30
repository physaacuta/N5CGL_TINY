'*********************************************************************************
'�@�X�^�[�g
'	[Ver]
'		Ver.01    2009/12/17  ����
'
'	[����]
'		
'*********************************************************************************

Option Strict On
Imports tClassLibrary
Imports KizuLibCLI
Imports System.Reflection


Module modMain

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���J�萔
	'/ /////////////////////////////////////////////////////////////////////////////////
	Public Const GC_MAX_SELECT_TYPE As Integer = 5								' �ő�I���r��
	Public Const GC_MAX_PROTDATA As Integer = 1000								' �ő�R�C������


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���[�o���ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Public gcls_Log As tClass.tClass_LogManager = Nothing               ' ���O�Ǘ��N���X (�{��)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���[�J���ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' ���C���X���b�h�Ǘ��p (�قڕK�{�̊�{�̌`)
    Private WithEvents mcls_Mod As KizuLibCLI.KizuMod                   ' �����N���X �i�ʃX���b�h�Ȃ̂Œ��Ӂj
    Private m_ap As ApplicationContext									' �R���e�L�X�g

	Private WithEvents m_trmFrmThread As New Timer                      ' ���[���X���b�g�p�X���b�h�����ʃX���b�h�ւ̋��n��
    Private m_MailQue As tClass.tClass_Queue(Of COMMON_QUE_L)           ' ���[���X���b�g�̃L���[

    Private mfrm_Main As frmMain										' �A�N�Z�X�t�H�[��



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ��{��`
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �G���g���[�|�C���g
	''' </summary>
	''' <remarks></remarks>
	Public Function Main() As Integer
        ' ��KizuLibCLI 32bit / 64bit �؂�ւ��p
        AddHandler AppDomain.CurrentDomain.AssemblyResolve, New ResolveEventHandler(AddressOf OnAssemblyResolve)

        Try
            Task_First()
        Catch ex As Exception
            MessageBox.Show("KizuLibCLI.dll �� �m�F���ĉ������B" + vbCrLf + vbCrLf +
                            "---------------------------------------------------------------" + vbCrLf + vbCrLf +
                            ex.ToString, String.Format("�^�X�N���� ���s"), MessageBoxButtons.OK, MessageBoxIcon.Error)

            Exit Function
        End Try

        '''' ��ʕ\��
        Dim showMontor As Integer = 2       ' ���j���[�\���Ώۂ̃��j�^�[No (1�I���W��)
        '' ���j���[��ʂ̕\��
        mfrm_Main = New frmMain
		mfrm_Main.Show(showMontor)


		'' Mail�X���b�h�����ʃX���b�h�ւ̋��n���^�C�}�[�J�n()
		m_trmFrmThread.Interval = 100
		m_trmFrmThread.Start()

		'' ���C���X���b�h����
		m_ap = New ApplicationContext
        Application.Run(m_ap)           'ExitThread���R�[�������܂� �����Ńu���b�N
        m_ap.Dispose()

        Task_Last()

    End Function

    ''' <summary>
    ''' �^�X�N������������
    ''' </summary>
    ''' <remarks>Main����KizuLibCLI�ɃA�N�Z�X�����Ȃ����߂ɕ�������</remarks>
    Private Sub Task_First()

        ' ��ʎ󂯓n���p�L���[������
        m_MailQue = New tClass.tClass_Queue(Of COMMON_QUE_L)

        mcls_Mod = New KizuMod

        '''' �^�X�N������
        KizuLibCLI.KizuMod.SetLineID()
        '' �^�X�N�C�j�V���� (DB�ւ̃A�N�Z�X�͖���)
        If 0 <> mcls_Mod.Task_Init(SO_TRAND) Then
            KizuLibCLI.KizuMod.SysLog(SYSNO_TASK_INIT_ERR, String.Format("[{0}]", SO_TRAND))
            End
        End If

        '' ���O�N���X����
        gcls_Log = New tClassLibrary.tClass.tClass_LogManager(KizuMod.GetMyTaskName(), Nothing, Nothing)
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "�g�����h�\����ʋN��")
    End Sub

    ''' <summary>
    ''' �^�X�N�I������������
    ''' </summary>
    ''' <remarks>Main����KizuLibCLI�ɃA�N�Z�X�����Ȃ����߂ɕ�������</remarks>
    Private Sub Task_Last()

        ' �t�H�[���J��
        mfrm_Main.Close()
        mfrm_Main.Dispose()
        mfrm_Main = Nothing

        ' ���̑��N���X���
        gcls_Log.Dispose()
        gcls_Log = Nothing

        ' �^�X�N�I��
        mcls_Mod.Task_Exit()
    End Sub

    ''' <summary>
    ''' �^�X�N�I������
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub TaskEnd()

        ' �X���b�h����������Ă��Ȃ�������I��
        If m_ap Is Nothing Then
            gcls_Log.Dispose()
            gcls_Log = Nothing
            End
        End If

        m_ap.ExitThread()                                       ' ���C���X���b�h�I��
    End Sub


    ''' <summary>
    ''' ���[���X���b�g��M
    ''' </summary>
    ''' <param name="pData">�f�[�^</param>
    ''' <param name="nEvtNo">�C�x���g�ԍ�</param>
    ''' <remarks></remarks>
    Private Sub mcls_Mod_evRecvMail(ByVal pData As Object, ByVal nEvtNo As Integer) Handles mcls_Mod.evRecvMail
		Dim ii As Integer = 0
		Dim que_l As COMMON_QUE_L = CType(pData, COMMON_QUE_L)

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


		Select Case que_l.nEventNo

			'//-----------------------------------------------------------
			Case FACT_SO_TRAND_01       ' ��ʍX�V�ʒm
                Call SendMailDefMap(FACT_SO_DEFMAP_02)
                Call mfrm_Main.gfrm_Setting_EvDsp()

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

    ''' <summary>
    ''' �r�_�}�b�v��ʍX�V�ʒm
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SendMailDefMap(ByVal nEventNo As Integer)
        Try
            Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
            que.inisialize()

            que.nEventNo = nEventNo
            que.nLineNo = KizuLibCLI.KizuMod.GetLineID()

            KizuLibCLI.KizuMod.Send_Mail(SO_DEFMAP, ".", que)

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�r�_�}�b�v��ʍX�V�ʒm�G���[ [" + ex.Message.ToString + "]")
        Finally

        End Try
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

        frmData.Text = String.Format("[{0}] {1}", SYSTEM_TITLE, frmData.Text)

    End Sub


    ''' <summary>
    ''' KizuLibCLI.DLL�̓Ǎ���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="args"></param>
    ''' <returns></returns>
    ''' <remarks>32bit/64bit�؂�ւ��Ή�</remarks>
    Private Function OnAssemblyResolve(ByVal sender As Object, ByVal args As System.ResolveEventArgs) As Assembly

        If args.Name.ToUpper.StartsWith("KIZULIBCLI") Then

            If IntPtr.Size = 8 Then
                ' 64bit��
                Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx64.dll"))
            Else
                ' 32bit��
                Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx32.dll"))
            End If

        End If

        Return Nothing

    End Function

End Module
