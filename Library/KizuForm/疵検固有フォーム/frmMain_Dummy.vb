'*********************************************************************************
' マルチモニタ時のダミー画面出力クラス
'	[Ver]
'		Ver.01    2007/03/31  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On



''' <summary>
''' ダミーフォーム （直接参照禁止）
''' </summary>
''' <remarks></remarks>
Friend Class frmMain_Dummy


End Class


''' <summary>
''' ダミーフォーム管理
''' </summary>
''' <remarks></remarks>
Public Class frmMain_DummyManager

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メンバ 変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_nNotMonitorNo() As Integer									' 表示対象外のモニターNo (1オリジン) (これ以外の全モニターに本画面を表示する)
	Private mfrm_Dummy() As frmMain_Dummy									' 配下のダミーフォーム (2画面以上でダミーフォームを表示する場合に有効)


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 公開 メソッド
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 自画面を指定モニタ位置に表示
	''' </summary>
	''' <param name="nNotMonitorNo">表示対象外のモニターNo (1オリジン)</param>
	''' <remarks></remarks>
	Public Overloads Sub Show(ByVal nNotMonitorNo As Integer)
		'''' メンバー変数セット
		ReDim m_nNotMonitorNo(0)
		m_nNotMonitorNo(0) = nNotMonitorNo

		' 画面表示メイン処理
		Call DspForm()
	End Sub
	''' <summary>
	''' 自画面を指定モニタ位置に表示
	''' </summary>
	''' <param name="nNotMonitorNo">表示対象外のモニターNo (1オリジン) </param>
	''' <remarks>引数指定時は、「Dim a As New KizuForm.frmMain_Dummy(New Integer() {1, 1})」 でもいいよ</remarks>
	Public Overloads Sub Show(ByVal nNotMonitorNo() As Integer)
		'''' メンバー変数セット
		'm_nNotMonitorNo = nNotMonitorNo		参照になるからヤメ
		ReDim m_nNotMonitorNo(nNotMonitorNo.Length - 1)
		For ii As Integer = 0 To nNotMonitorNo.Length - 1
			m_nNotMonitorNo(ii) = nNotMonitorNo(ii)
		Next ii

		' 画面表示メイン処理
		Call DspForm()
	End Sub


	''' <summary>
	''' 全画面を閉じる
	''' </summary>
	''' <remarks></remarks>
	Public Sub Close()
		If mfrm_Dummy Is Nothing Then Return

		'' 全画面閉じる
		For ii As Integer = 0 To mfrm_Dummy.Length - 1
			mfrm_Dummy(ii).Close()
			mfrm_Dummy(ii).Dispose()
			mfrm_Dummy(ii) = Nothing
		Next ii
		mfrm_Dummy = Nothing
	End Sub



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 処理
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 画面表示メイン処理
	''' </summary>
	''' <remarks></remarks>
	Private Sub DspForm()
		Dim ii, jj As Integer

		'' 表示したいモニタの情報
		Dim monitor() As System.Windows.Forms.Screen
		monitor = System.Windows.Forms.Screen.AllScreens()

		Dim nDummyFromCnt As Integer					' ダミーフォーム数
		nDummyFromCnt = monitor.Length - m_nNotMonitorNo.Length
		If nDummyFromCnt <= 0 Then Return ' 表示ダミーフォーム無し

		'' ダミーフォームを生成
		ReDim mfrm_Dummy(nDummyFromCnt - 1)
		For ii = 0 To nDummyFromCnt - 1
			mfrm_Dummy(ii) = New frmMain_Dummy
		Next ii


		'' ダミーフォームを表示
		Dim bDspFlg As Boolean
		Dim nCnt As Integer = 0
		For ii = 0 To monitor.Length - 1
			' 表示対象位置かチェック
			bDspFlg = True
			For jj = 0 To m_nNotMonitorNo.Length - 1
				If m_nNotMonitorNo(jj) = ii + 1 Then
					bDspFlg = False
					Exit For
				End If
			Next jj
			If Not bDspFlg Then Continue For

			' 表示
			mfrm_Dummy(nCnt).Show(ii + 1, True)
			nCnt += 1
		Next ii
	End Sub


End Class
