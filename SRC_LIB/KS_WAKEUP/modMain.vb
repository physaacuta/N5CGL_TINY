'*********************************************************************************
' エントリー
'	[Ver]
'		Ver.01    2007/03/31  新規作成
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports System.Runtime.InteropServices

Module modMain
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ グローバル定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public Enum em_Delivery                                                                ' 通知モード
        iTO = 0         'TOKATU
        iTI             'TIKUSEKI
    End Enum


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 参照できない定義
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
    '/ メンバー定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Delegate Sub m_deleSetImgLoad(ByVal pc As Integer, ByVal rec As Integer)        ' 別スレッドからのコントロールアクセス

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ローカル変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	'Private WithEvents mcls_Mod As New KizuLibCLI.KizuMod				' 総合クラス
	Public WithEvents gcls_Mail As tClass.tClass_MailSlot				' メールスロット受信クラス
	Private m_RecvQueBuf As COMMON_QUE_L									' メールスロット受信バッファ

	Private m_frm() As frmMain											' アクセスフォーム (PCのモニタ数により変更)
    Private m_ap As ApplicationContext                                  ' コンテキスト

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' エントリーポイント
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Main()
        Dim ii As Integer

		'' タスクイニシャル (DBへのアクセスは無し)
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

		'' PCに接続しているモニタを取得
		Threading.Thread.Sleep(3000)
        Dim monitor() As System.Windows.Forms.Screen
        monitor = System.Windows.Forms.Screen.AllScreens()
        Dim cnt As Integer = monitor.Length
        If cnt <= 0 Then cnt = 1
        ReDim m_frm(cnt - 1)

        '' 画面表示
        For ii = 0 To m_frm.Length - 1
            m_frm(ii) = New frmMain(ii)     ' フォームを生成しているスレッドだから直接アクセス可能
            m_frm(ii).Show()
        Next ii

        '' メインスレッド生成
        m_ap = New ApplicationContext
        Application.Run(m_ap)           'ExitThreadがコールされるまで ここでブロック

        '' 終了
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
    ''' メールスロット受信
    ''' </summary>
    ''' <param name="pData"></param>
    ''' <param name="nEvtNo"></param>
    ''' <remarks></remarks>
	'Private Sub mcls_Mod_evRecvMail(ByVal pData As Object, ByVal nEvtNo As Integer) Handles mcls_Mod.evRecvMail
	'    Dim ii As Integer

	'    If nEvtNo = FACT_KS_END Then                    ' タスク終了
	'        m_ap.ExitThread()

	'    ElseIf nEvtNo = FACT_KS_WAKEUP_01 Then          ' 統括OK
	'        For ii = 0 To m_frm.Length - 1
	'            m_frm(ii).SetImgLoadEntry(em_Delivery.iTO, True)               ' フォーム生成スレッドと別スレッドなので注意
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

		If nEvtNo = FACT_KS_END Then					' タスク終了
			m_ap.ExitThread()

		ElseIf nEvtNo = FACT_KS_WAKEUP_01 Then			' 統括OK
			For ii = 0 To m_frm.Length - 1
				m_frm(ii).SetImgLoadEntry(em_Delivery.iTO, True)			   ' フォーム生成スレッドと別スレッドなので注意
			Next ii

		ElseIf nEvtNo = FACT_KS_WAKEUP_02 Then			' DBOK
			For ii = 0 To m_frm.Length - 1
				m_frm(ii).SetImgLoadEntry(em_Delivery.iTI, True)
			Next ii
		End If
	End Sub

End Module
