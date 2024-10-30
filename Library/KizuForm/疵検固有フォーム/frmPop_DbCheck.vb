'*********************************************************************************
' DB接続確立待機中画面クラス
'	[Ver]
'		Ver.01    2007/07/31  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

Public Class frmPop_DbCheck
	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 外部イベント
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ファンクションボタン押下イベント (ベースの置き換え)
	''' </summary>
	''' <param name="dbMode">DBの状態</param>
	''' <remarks></remarks>
	Public Shadows Event tEv_FunctionButton(ByVal dbMode As Boolean)


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ローカル変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_strSession As String						' iniファイルのDBセッション
	Private m_nNoa As Integer							' NOA設定状態
	Private m_bDBOK As Boolean = False					' DB接続状態
	Private m_wThread As tClass.tClass_WorkThread = Nothing		  ' DBチェック用スレッド


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 公開メソッド
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' コンストラクタ
	''' </summary>
	''' <param name="strDbSession">DBセッション</param>
	''' <param name="nNoa">オンラインか遠隔か</param>
	''' <remarks></remarks>
	Public Sub New(ByVal strDbSession As String, ByVal nNoa As Integer)

		' この呼び出しは、Windows フォーム デザイナで必要です。
		InitializeComponent()

		' InitializeComponent() 呼び出しの後で初期化を追加します。
		m_strSession = strDbSession
        m_nNoa = nNoa
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ イベント
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' フォームロード
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmPop_MsgBox_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

		' 初期化
		Me.TopLevel = True						 ' 最前面
        btnF01.Enabled = CBool(IIf(m_nNoa = NOA_ONLINE, False, True))

		'''' メインスレッド実行
		m_wThread = New tClass.tClass_WorkThread
		m_wThread.ThreadStart(AddressOf mainThread, Threading.ThreadPriority.Lowest, "DBcheck", True)
	End Sub

	''' <summary>
	''' ファンクションボタンクリック
	''' </summary>
	''' <param name="FncNo"></param>
	''' <param name="onKey"></param>
	''' <remarks></remarks>
	Private Sub frmPop_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles MyBase.tEv_FunctionButton
		' 画面の終了 (インスタンスはそのままだからね)
		Me.Close()
		' ワーカースレッド停止
		m_wThread.ThreadStop()
		m_wThread.Dispose()
		m_wThread = Nothing

		' 呼び元へ通知
		RaiseEvent tEv_FunctionButton(m_bDBOK)
	End Sub


	''' <summary>
	''' タイマー
	''' </summary>
	Private Sub trmCycle_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trmCycle.Tick
		' プログレスバーセット
		If ProgressBar1.Value = ProgressBar1.Maximum Then
			ProgressBar1.Value = 0
		End If
		ProgressBar1.Increment(1)

		' DB接続確認
		If m_bDBOK Then
			' DBにつながったら、自画面を終了して、呼び出し元にイベント通知
			frmPop_tEv_FunctionButton(Nothing, Nothing)
		End If
	End Sub


	''' <summary>
	''' メインスレッド
	''' </summary>
	''' <remarks></remarks>
	Private Sub mainThread()
		Dim EventAry(0) As Threading.WaitHandle		'イベント
		Dim iRetc As Integer						'WaitAny 復帰情報

		'''' シグナルセット
		EventAry(0) = m_wThread.g_evStop			' スレッド終了

		' まずDBの接続確認
		m_bDBOK = tClass.tClass_SQLServer.OpenCheck(TASKINI_NAME, m_strSession)

		Try
			While (True)
				'' シグナル待ち
				iRetc = Threading.WaitHandle.WaitAny(EventAry, 3000, False)

				'' まず終了確認
				Select Case iRetc
					Case 0									'終了
						Exit While

					Case Threading.WaitHandle.WaitTimeout	'タイムアウト
						' DB接続確認
						m_bDBOK = tClass.tClass_SQLServer.OpenCheck(TASKINI_NAME, m_strSession)
					Case Else
						'ありえない
				End Select
			End While
		Catch ex As Threading.ThreadAbortException
		Catch ex As Exception
		End Try
	End Sub

End Class
