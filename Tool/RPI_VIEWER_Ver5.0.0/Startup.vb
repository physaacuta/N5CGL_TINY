Imports System.Reflection

'---------------------------------
' �g�p����ꍇ�́A�A�v���P�[�V�����t���[�����[�N��؂��āASubMain�ɕύX
' ����� Any CPU �Ή�OK
'---------------------------------
' �A�Z���u���Ǎ��݂� tClass_ViewRpiThread �Ƒ΂ɂȂ��Ă��邩��OK
'Dim s_dllImgLibCLI As System.Reflection.Assembly = System.Reflection.Assembly.Load("ImgLibCLI")
'Dim s_clsIppResize As System.Type = s_dllImgLibCLI.GetType("ImgLibCLI.IppResize")
'msmod_pResize_8u_C1R = s_clsIppResize.GetMethod("Resize_8u_C1R")
'---------------------------------

Class Startup
    Private Shared DllLoadPath As String		' DLL�p�X

    ''' <summary>
    ''' �����N��
    ''' </summary>
    ''' <remarks></remarks>
    Public Shared Sub Main()

        ' ===>> �⊪ �@���ێ� �c�[�����p����
        If MC_CHECK_ACCSESS Then
            If Not tNinsho.IsUserCheck(TASK_NAME) Then
                End
            End If
        End If
        ' <<===

        '==========================
        ' �O���A���L��
        g_bOutTrk = 1 <> System.Environment.GetCommandLineArgs.Length

        '''' �O���A���L��̏ꍇ�̂݁A�������
        If g_bOutTrk Then
            g_typStartArgs.sArgs = System.Environment.GetCommandLineArgs
            For Each sVal As String In g_typStartArgs.sArgs
                sVal.Trim()

                ' �^�X�N��
                If -1 <> sVal.IndexOf(START_CMD_TASKNAME) Then g_typStartArgs.sTaskName = sVal.Replace(START_CMD_TASKNAME, "").Trim()
                If -1 <> sVal.IndexOf(START_CMD_ID) Then g_typStartArgs.nId = CInt(sVal.Replace(START_CMD_ID, "").Trim())

            Next
        End If

        '''' ���s�t�H���_�̃`�F�b�N
        Dim sFolder As String = IO.Path.GetDirectoryName(Application.ExecutablePath)
        If IO.Directory.GetCurrentDirectory() <> sFolder Then
            IO.Directory.SetCurrentDirectory(sFolder)
        End If


        '' �A���e�X�g
        'g_bOutTrk = True
        'g_typStartArgs.sTaskName = "RPI_VIEWER"

        'Dim s As String = IO.Directory.GetCurrentDirectory()
        'Dim sup As String = Application.ExecutablePath
        'MsgBox(s)
        'MsgBox(sup)

        '		' �A���e�X�g
        '#If DEBUG Then
        '		g_bOutTrk = True
        '		If "" = g_typStartArgs.sTaskName Then

        '			g_typStartArgs.sTaskName = "RPIV0"
        '			g_typStartArgs.nId = 0
        '		End If
        '#End If


        '==========================
        ' Any CPU �Ή�
        DllLoadPath = Application.StartupPath & "\"
        'AddHandler AppDomain.CurrentDomain.AssemblyLoad, New AssemblyLoadEventHandler(AddressOf OnAssemblyLoad)
        AddHandler AppDomain.CurrentDomain.AssemblyResolve, New ResolveEventHandler(AddressOf OnAssemblyResolve)

        System.Windows.Forms.Application.EnableVisualStyles()
        System.Windows.Forms.Application.Run(frmMain)
    End Sub


    ''' <summary>
    ''' ���[�h���悤�Ƃ���DLL���Ȃ��������ɌĂ΂��B
    ''' </summary>
    ''' <remarks>ImgLibCLI.dll�͑��݂��Ȃ��K�v������B(CPU�^�C�v�Ⴂ�͂��̕��@�ł͏E���Ȃ�����)
    ''' ����ɁAImgLibCLIx32.dll,ImgLibCLIx64.dll��ǂށB
    ''' </remarks>
    Private Shared Function OnAssemblyResolve(ByVal sender As Object, ByVal args As System.ResolveEventArgs) As Assembly

        If args.Name.ToUpper.StartsWith("IMGLIBCLI") Then
            If IntPtr.Size = 8 Then
                '64bit��
                Return Assembly.LoadFile(DllLoadPath & "\ImgLibCLIx64.dll")
            Else
                '32bit��
                Return Assembly.LoadFile(DllLoadPath & "\ImgLibCLIx32.dll")
            End If
        Else
            Return Assembly.LoadFile(args.Name)
        End If

        Return Nothing
    End Function
    'Private Shared Sub OnAssemblyLoad(ByVal sender As Object, ByVal args As System.AssemblyLoadEventArgs)
    '	If IntPtr.Size = 8 Then
    '		'64bit��
    '		Assembly.LoadFile(DllLoadPath & "\ImgLibCLIx64.dll")
    '	Else
    '		'32bit��
    '		Assembly.LoadFile(DllLoadPath & "\ImgLibCLIx32.dll")
    '	End If
    'End Sub

End Class

