'*********************************************************************************
' �G���g���[
'	[Ver]
'		Ver.01    2007/03/31  �V�K�쐬
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports KizuLibCLI
Imports System.Reflection

Public Module modMain

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���[�o���ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Public gcls_Log As tClassLibrary.tClass.tClass_LogManager           ' ���O�Ǘ��N���X

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���[�J���ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private WithEvents mcls_Mod As KizuLibCLI.KizuMod                   ' �����N���X
    Private m_ap As ApplicationContext                                  ' �R���e�L�X�g

    Private WithEvents m_trmFrmThread As New Timer                      ' ���[���X���b�g�p�X���b�h�����ʃX���b�h�ւ̋��n��
    Private m_MailQue As tClass.tClass_Queue(Of COMMON_QUE_L)           ' ���[���X���b�g�̃L���[

    Private m_frm As frmMain                                  ' �A�N�Z�X�t�H�[��

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��`
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �G���g���[�|�C���g
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Main()
        ' ��KizuLibCLI 32bit / 64bit �؂�ւ��p
        AddHandler AppDomain.CurrentDomain.AssemblyResolve, New ResolveEventHandler(AddressOf OnAssemblyResolve)

        Try
            Task_First()
        Catch ex As Exception
            MessageBox.Show("KizuLibCLI.dll �� �m�F���ĉ������B" + vbCrLf + vbCrLf +
                            "---------------------------------------------------------------" + vbCrLf + vbCrLf +
                            ex.ToString, String.Format("�^�X�N���� ���s"), MessageBoxButtons.OK, MessageBoxIcon.Error)

            Exit Sub
        End Try

        '''' ��ʕ\��
        Dim showMontor As Integer = 2       ' ���j���[�\���Ώۂ̃��j�^�[No (1�I���W��)
        '' ���j���[��ʂ̕\��
        m_frm = New frmMain()
        m_frm.Show(showMontor)

        '' ���C���X���b�h����
        m_ap = New ApplicationContext
        Application.Run(m_ap)               'ExitThread���R�[�������܂� �����Ńu���b�N

        Task_Last()

    End Sub

    ''' <summary>
    ''' �^�X�N�I������
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub TaskEnd()
		m_ap.ExitThread()										' ���C���X���b�h�I��
	End Sub

    ''' <summary>
    ''' �^�X�N������������
    ''' </summary>
    ''' <remarks>Main����KizuLibCLI�ɃA�N�Z�X�����Ȃ����߂ɕ�������</remarks>
    Private Sub Task_First()

        ' ��ʎ󂯓n���p�L���[������
        m_MailQue = New tClass.tClass_Queue(Of COMMON_QUE_L)

        mcls_Mod = New KizuMod

        '''' �^�X�N������        
        KizuLibCLI.KizuMod.SetLineID()  '���C��ID
        '' �^�X�N�C�j�V����
        If 0 <> mcls_Mod.Task_Init(SO_DEFMAP) Then
            KizuLibCLI.KizuMod.SysLog(SYSNO_TASK_INIT_ERR, String.Format("[{0}]", SO_DEFMAP))
            End
        End If

        '' ���O�N���X����
        gcls_Log = New tClassLibrary.tClass.tClass_LogManager(KizuLibCLI.KizuMod.GetMyTaskName(), Nothing, Nothing)
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "���щ�ʋN��")
    End Sub

    ''' <summary>
    ''' �^�X�N�I������������
    ''' </summary>
    ''' <remarks>Main����KizuLibCLI�ɃA�N�Z�X�����Ȃ����߂ɕ�������</remarks>
    Private Sub Task_Last()

        '' �I��
        m_frm.Close()
        m_frm.Dispose()
        m_frm = Nothing

        gcls_Log.Dispose()
        gcls_Log = Nothing
        mcls_Mod.Task_Exit()
    End Sub

    ''' <summary>
    ''' ���[���X���b�g��M
    ''' </summary>
    ''' <param name="pData"></param>
    ''' <param name="nEvtNo"></param>
    ''' <remarks></remarks>
    Private Sub mcls_Mod_evRecvMail(ByVal pData As Object, ByVal nEvtNo As Integer) Handles mcls_Mod.evRecvMail

        Dim que_l As COMMON_QUE_L = CType(pData, COMMON_QUE_L)

        Try

            Select Case nEvtNo

                Case FACT_SO_DEFMAP_01  ' �p�����[�^�ύX�ʒm
                    gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("�p�����[�^�ύX�ʒm��M[{0}]", que_l.data(0)))
                    m_frm.TranParam(que_l.data(0))

                Case FACT_SO_DEFMAP_02  ' ��ʍX�V�ʒm
                    gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("��ʍX�V�ʒm��M"))
                    m_frm.TranDspNew()

                Case FACT_KS_END        ' �^�X�N�I��
                    m_ap.ExitThread()

                Case FACT_KS_PCNAME_CHANGE
					''PC���ǂݒ���
                    'KizuLibCLI.KizuMod.GetPcName()

                Case Else
                    KizuLibCLI.KizuMod.SysLog(SYSNO_MAILNO_ERR, String.Format("[{0}]", "EventNo = [" & nEvtNo & "]"))
                    gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "EventNo = [" & nEvtNo & "]")
            End Select

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "���[����M�G���[ [" & ex.Message & "]")
        End Try

    End Sub

    ''' <summary>
    ''' �t�H�[���^�C�g���ݒ�
    ''' </summary>
    ''' <param name="frmData">�t�H�[��</param>
    ''' <remarks></remarks>
    Public Sub SetTitleName(ByRef frmData As Form)

        frmData.Text = String.Format("[{0}] {1}", SYSTEM_TITLE, frmData.Text)

    End Sub

    ''' <summary>
    ''' �����σt�H���g�T�C�Y�̎擾(���x��)
    ''' </summary>
    ''' <param name="lblLabel">�Ώۃ��x��</param>
    ''' <param name="strString">�\��������</param>
    ''' <param name="nDefaultFontSize">����t�H���g�T�C�Y</param>
    ''' <param name="nMaxWidth">�ő�\����</param>
    ''' <returns>�ő�\�����ɓK�����t�H���g�T�C�Y(1pt�P��)</returns>
    ''' <remarks>
    ''' �t�H���g�T�C�Y��8�����̏ꍇ��8��ԋp����B
    ''' �܂��A����t�H���g�T�C�Y�ōő�\�����𒴂��Ȃ��ꍇ�͊���t�H���g�T�C�Y��Ԃ�
    ''' </remarks>
    Public Function GetLabelFontScaleSize(ByVal lblLabel As Label, ByVal strString As String, ByVal nDefaultFontSize As Single, ByVal nMaxWidth As Integer) As Single
        Dim nFontWidth As Single                    ' �����\����
        Dim nScaleFontSize As Single                ' �����t�H���g�T�C�Y
        Dim fntScale As System.Drawing.Font         ' �����t�H���g
        Dim nMaxDspWidth As Integer                 ' �����\���ő啝
        Dim g As Graphics = lblLabel.CreateGraphics()
        Dim fntDefault As Font = New System.Drawing.Font("�l�r ����", nDefaultFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

        Try

            ' ������
            nFontWidth = g.MeasureString(strString, fntDefault).Width
            nScaleFontSize = nDefaultFontSize
            fntScale = New System.Drawing.Font("�l�r ����", nScaleFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            ' �����\���ő啝�͕\���ő啝�������t�H���g�T�C�Y���p�P�������]�T����������
            nMaxDspWidth = nMaxWidth - CShort(g.MeasureString("@", fntDefault).Width)
            If nMaxDspWidth < 0 Then
                nMaxDspWidth = nMaxWidth
            End If

            ' �\�������Ɏ��܂�ꍇ�͊���t�H���g�T�C�Y��Ԃ�
            If nFontWidth <= nMaxDspWidth Then
                fntScale.Dispose()
                Return nDefaultFontSize
            End If

            ' 1pt���t�H���g�T�C�Y�����������āA�����\���ő啝�������t�H���g�T�C�Y���Z�o
            While nScaleFontSize > 0
                nScaleFontSize = nScaleFontSize - 1.0F
                If nScaleFontSize < 8 Then
                    Exit While
                End If

                fntScale = New System.Drawing.Font("�l�r ����", nScaleFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

                nFontWidth = g.MeasureString(strString, fntScale).Width

                If nFontWidth <= nMaxDspWidth Then
                    Exit While
                End If

            End While

            ' �]�菬�����ƌ����Ȃ��̂Ńt�H���g�T�C�Y��8������8�ɂ���B
            If nScaleFontSize < 8 Then
                nScaleFontSize = 8
            End If

            fntScale.Dispose()

        Catch ex As Exception

        Finally
            If g IsNot Nothing Then
                g.Dispose()
                g = Nothing
            End If

        End Try

        Return nScaleFontSize
    End Function

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
