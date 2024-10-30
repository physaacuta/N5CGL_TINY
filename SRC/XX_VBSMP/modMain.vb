'*********************************************************************************
' エントリー
'	[Ver]
'		Ver.01    2012/10/02  新規作成
'
'	[メモ]
'		・VB雛形 そのままコピペでＯＫ。△部分は、実装時に置き換え
'*********************************************************************************
Option Strict On					' 全疵検、全プログラム必須

Imports KizuLibCLI					' 疵検CLIライブラリ。メールスロット等使用時必須
Imports tClassLibrary				' 疵検VBライブラリ。



Public Module modMain

	Public Const XX_VBSMP As String = "XX_VBSMP"	' △ これは、テストデータ。ホントは定数を使用
	Public Const SYSTEM_TITLE As String = "test"	' △ これは、テストデータ。ホントは定数を使用


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ グローバル変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Public gcls_Log As tClass.tClass_LogManager = Nothing				' ログ管理クラス (本体)
	Public g_NOA As Integer												' NOA接続フラグ (0:オンライン  1:遠隔)


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ローカル変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	' メインスレッド管理用 (ほぼ必須の基本の形)
	Private WithEvents mcls_Mod As New KizuLibCLI.KizuMod				' 総合クラス （別スレッドなので注意）
	Private m_ap As ApplicationContext									' コンテキスト

	Private WithEvents m_trmFrmThread As New Timer						' メールスロット用スレッドから画面スレッドへの橋渡し
	Private m_MailQue As New tClass.tClass_Queue(Of COMMON_QUE_L)		' メールスロットのキュー

	Private mfrm_Menu As frmMenu										' アクセスフォーム

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 基本定義
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' エントリーポイント
	''' </summary>
	''' <remarks></remarks>
	Public Function Main() As Integer


		'//======================================================
		'  前準備
		'' iniファイルより 定数的なグローバール変数読み込み
		g_NOA = tMod.tMod_IniFile.ReadIniInt(INI_COMMON, "NOA", TASKINI_NAME)


		'''' タスク初期化
		KizuLibCLI.KizuMod.SetLineID()
		'' タスクイニシャル (DBへのアクセスは無し)
		If 0 <> mcls_Mod.Task_Init(XX_VBSMP) Then
			If NOA_ONLINE = g_NOA Then
				KizuLibCLI.KizuMod.SysLog(SYSNO_TASK_INIT_ERR, String.Format("[{0}]", XX_VBSMP))
			End If
			End
		End If

		'' ログクラス生成
		gcls_Log = New tClassLibrary.tClass.tClass_LogManager(KizuMod.GetMyTaskName(), Nothing, Nothing)
		gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "オペレータ画面起動")


		'//======================================================
		'  画面実行

		'' 画面表示
		Dim showMontor As Integer = 1		' メニュー表示対象のモニターNo (1オリジン)

		'' △ ここで各画面切り替え(メニュー画面の表示)
		mfrm_Menu = New frmMenu
		mfrm_Menu.Show(showMontor)


		'' Mailスレッドから画面スレッドへの橋渡しタイマー開始()
		m_trmFrmThread.Interval = 100
		m_trmFrmThread.Start()

		'' メインスレッド生成
		m_ap = New ApplicationContext
		Application.Run(m_ap)			'ExitThreadがコールされるまで ここでブロック
		m_ap.Dispose()


		'//======================================================
		'  終了処理
		Try
			'''' ここまで来たら 終了ね
			' フォーム開放
			If Not mfrm_Menu Is Nothing Then
				mfrm_Menu.Close()
				mfrm_Menu.Dispose()
				mfrm_Menu = Nothing
			End If

			' メールスロットキュー削除
			If Not m_MailQue Is Nothing Then
				m_MailQue.Dispose()
				m_MailQue = Nothing
			End If
		Catch ex As Exception
			gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("{0} [{1}]", "メイン処理終了異常", ex.Message))
		End Try

		'' その他クラス解放
		gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "オペレータ画面終了")
		gcls_Log.Dispose()
		gcls_Log = Nothing

		'' タスク終了
		mcls_Mod.Task_Exit()
	End Function

	''' <summary>
	''' タスク終了処理
	''' </summary>
	''' <remarks></remarks>
	Public Sub TaskEnd()
		m_ap.ExitThread()										' メインスレッド終了
	End Sub


	''' <summary>
	''' メールスロット受信
	''' </summary>
	''' <param name="pData">データ</param>
	''' <param name="nEvtNo">イベント番号</param>
	''' <remarks></remarks>
	Private Sub mcls_Mod_evRecvMail(ByVal pData As Object, ByVal nEvtNo As Integer) Handles mcls_Mod.evRecvMail
		Dim ii As Integer = 0
		Dim que_l As COMMON_QUE_L = CType(pData, COMMON_QUE_L)	' 基本一番よく使うため、この形にしておく。

		'''' 画面スレッドへ渡す
		m_MailQue.SetItem(que_l)
	End Sub

	''' <summary>
	''' メールスロット受信
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub m_trmFrmThread_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_trmFrmThread.Tick
		Dim que_l As COMMON_QUE_L = m_MailQue.GetItem(0)
		If que_l.nEventNo = 0 Then Return


		' △ ここはサンプル -------------->>>>
		Dim que_m As COMMON_QUE_STRMIX = CType(mcls_Mod.typConv(que_l, GetType(COMMON_QUE_STRMIX)), COMMON_QUE_STRMIX)
		'Dim quew As COMMON_QUE_MIX = CType(mcls_Mod.typConv(que_l, GetType(COMMON_QUE_MIX)), COMMON_QUE_MIX)	' 
		Dim que_s As COMMON_QUE_S = CType(mcls_Mod.typConv(que_l, GetType(COMMON_QUE_S)), COMMON_QUE_S)
		' <<<------------------------------


		Select Case que_l.nEventNo

			'//-----------------------------------------------------------
			Case FACT_KS_DB_STATEUS		'DB状態変更通知

				'//-----------------------------------------------------------
			Case FACT_KS_PCNAME_CHANGE
				'処理なし

				'//-----------------------------------------------------------
			Case FACT_KS_END		' タスク終了
				gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "タスク終了通知受信")

				m_ap.ExitThread()	' スレッド終了

			Case Else
				KizuLibCLI.KizuMod.SysLog(SYSNO_MAILNO_ERR, String.Format("[{0}]", "err_code = [" & que_l.nEventNo & "]"))
		End Select

	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 処理
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' フォームタイトル設定
	''' </summary>
	''' <param name="frmData">フォーム</param>
	''' <remarks></remarks>
	Public Sub SetTitleName(ByRef frmData As Form)

		frmData.Text = SYSTEM_TITLE & Space(2) & frmData.Text

	End Sub


End Module
