Imports System.Reflection

'---------------------------------
' 使用する場合は、アプリケーションフレームワークを切って、SubMainに変更
' これで Any CPU 対応OK
'---------------------------------
' アセンブリ読込みで tClass_ViewRpiThread と対になっているからOK
'Dim s_dllImgLibCLI As System.Reflection.Assembly = System.Reflection.Assembly.Load("ImgLibCLI")
'Dim s_clsIppResize As System.Type = s_dllImgLibCLI.GetType("ImgLibCLI.IppResize")
'msmod_pResize_8u_C1R = s_clsIppResize.GetMethod("Resize_8u_C1R")
'---------------------------------

Class Startup
    Private Shared DllLoadPath As String		' DLLパス

    ''' <summary>
    ''' 初期起動
    ''' </summary>
    ''' <remarks></remarks>
    Public Shared Sub Main()

        ' ===>> 坂巻 機密保持 ツール利用制限
        If MC_CHECK_ACCSESS Then
            If Not tNinsho.IsUserCheck(TASK_NAME) Then
                End
            End If
        End If
        ' <<===

        '==========================
        ' 外部連動有無
        g_bOutTrk = 1 <> System.Environment.GetCommandLineArgs.Length

        '''' 外部連動有りの場合のみ、引数解読
        If g_bOutTrk Then
            g_typStartArgs.sArgs = System.Environment.GetCommandLineArgs
            For Each sVal As String In g_typStartArgs.sArgs
                sVal.Trim()

                ' タスク名
                If -1 <> sVal.IndexOf(START_CMD_TASKNAME) Then g_typStartArgs.sTaskName = sVal.Replace(START_CMD_TASKNAME, "").Trim()
                If -1 <> sVal.IndexOf(START_CMD_ID) Then g_typStartArgs.nId = CInt(sVal.Replace(START_CMD_ID, "").Trim())

            Next
        End If

        '''' 実行フォルダのチェック
        Dim sFolder As String = IO.Path.GetDirectoryName(Application.ExecutablePath)
        If IO.Directory.GetCurrentDirectory() <> sFolder Then
            IO.Directory.SetCurrentDirectory(sFolder)
        End If


        '' 連動テスト
        'g_bOutTrk = True
        'g_typStartArgs.sTaskName = "RPI_VIEWER"

        'Dim s As String = IO.Directory.GetCurrentDirectory()
        'Dim sup As String = Application.ExecutablePath
        'MsgBox(s)
        'MsgBox(sup)

        '		' 連動テスト
        '#If DEBUG Then
        '		g_bOutTrk = True
        '		If "" = g_typStartArgs.sTaskName Then

        '			g_typStartArgs.sTaskName = "RPIV0"
        '			g_typStartArgs.nId = 0
        '		End If
        '#End If


        '==========================
        ' Any CPU 対応
        DllLoadPath = Application.StartupPath & "\"
        'AddHandler AppDomain.CurrentDomain.AssemblyLoad, New AssemblyLoadEventHandler(AddressOf OnAssemblyLoad)
        AddHandler AppDomain.CurrentDomain.AssemblyResolve, New ResolveEventHandler(AddressOf OnAssemblyResolve)

        System.Windows.Forms.Application.EnableVisualStyles()
        System.Windows.Forms.Application.Run(frmMain)
    End Sub


    ''' <summary>
    ''' ロードしようとしたDLLがなかった時に呼ばれる。
    ''' </summary>
    ''' <remarks>ImgLibCLI.dllは存在しない必要がある。(CPUタイプ違いはこの方法では拾えないため)
    ''' 代わりに、ImgLibCLIx32.dll,ImgLibCLIx64.dllを読む。
    ''' </remarks>
    Private Shared Function OnAssemblyResolve(ByVal sender As Object, ByVal args As System.ResolveEventArgs) As Assembly

        If args.Name.ToUpper.StartsWith("IMGLIBCLI") Then
            If IntPtr.Size = 8 Then
                '64bit環境
                Return Assembly.LoadFile(DllLoadPath & "\ImgLibCLIx64.dll")
            Else
                '32bit環境
                Return Assembly.LoadFile(DllLoadPath & "\ImgLibCLIx32.dll")
            End If
        Else
            Return Assembly.LoadFile(args.Name)
        End If

        Return Nothing
    End Function
    'Private Shared Sub OnAssemblyLoad(ByVal sender As Object, ByVal args As System.AssemblyLoadEventArgs)
    '	If IntPtr.Size = 8 Then
    '		'64bit環境
    '		Assembly.LoadFile(DllLoadPath & "\ImgLibCLIx64.dll")
    '	Else
    '		'32bit環境
    '		Assembly.LoadFile(DllLoadPath & "\ImgLibCLIx32.dll")
    '	End If
    'End Sub

End Class

