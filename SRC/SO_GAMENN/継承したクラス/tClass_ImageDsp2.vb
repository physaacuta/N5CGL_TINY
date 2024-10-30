Option Strict On

Imports tClassLibrary.tClass

Public Class tClass_ImageDsp2
	Inherits tClass_ImageDsp

	Private m_ScaleInterval As Integer = 0	' スケール間詳細分割数 1以下の場合はスケール間詳細線表示処理が行われない
	Private mcls_Log As tClass_LogManager

	''' <summary>
	''' スケール詳細線分割数
	''' </summary>
	''' <value>1以下:非表示 2以上:分割数</value>
	''' <returns></returns>
	''' <remarks></remarks>
	Public Property ScaleInterval() As Integer
		Get
			Return m_ScaleInterval
		End Get
		Set(ByVal value As Integer)
			m_ScaleInterval = value
		End Set
	End Property

	''' <summary>
	''' ログ管理クラスのセット
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	Public Property SetLogManager() As tClass_LogManager
		Get
			Return mcls_Log
		End Get
		Set(ByVal value As tClass_LogManager)
			mcls_Log = value
		End Set
	End Property


	''' <summary>
	''' ログ書き込み処理
	''' </summary>
	''' <param name="div"></param>
	''' <param name="fmt"></param>
	''' <param name="params"></param>
	''' <remarks></remarks>
	Private Sub LOG(ByVal div As tClass_LogManager.LNO, ByVal fmt As String, ByVal ParamArray params() As Object)
		If mcls_Log Is Nothing Then Exit Sub

		mcls_Log.Write(div, String.Format(fmt, params))
	End Sub

	''' <summary>
	''' 詳細目盛り描画処理
	''' </summary>
	''' <param name="e">グラフィックオブジェクト</param>
	''' <remarks></remarks>
	Private Sub PaintScaleDetail(ByVal e As System.Windows.Forms.PaintEventArgs)

		' 補助線の分割区分が1以下なら処理しない
		If m_ScaleInterval <= 1 Then Exit Sub

		Dim ii As Integer
		Dim nPosLineTop As Integer
		Dim nLineWidth As Integer

		Try

			nPosLineTop = m_AriaInf.nAriaHeight - m_Memori.nPosLineBot

			' 詳細目盛りの線太さを決める
			nLineWidth = CInt(m_Memori.LineWidth / 2)
			If nLineWidth <= 0 Then
				nLineWidth = 1
			End If

			Dim clsPen As Pen = m_memoriPen

			'Ｘ軸表示
			For ii = 1 To m_Memori.nLoopCntX * tProp_MemoriInf.nIntervalX
				If ii Mod (m_Memori.nIntervalX \ m_ScaleInterval) = 0 Then
					Dim nPos As Integer = m_Memori.nPosLineLeft + CInt(ii * m_CalcAns.RateRes.dW)
					'区切り線
					e.Graphics.DrawLine(clsPen, nPos, nPosLineTop, nPos, nPosLineTop - CInt(m_Memori.HojoLineLen / 2))
				End If
			Next ii

			'Ｙ軸表示
			For ii = 1 To m_Memori.nLoopCntY * m_Memori.nIntervalY
				If ii Mod (m_Memori.nIntervalY \ m_ScaleInterval) = 0 Then
					Dim nPos As Integer = nPosLineTop - CInt(ii * m_CalcAns.RateRes.dH)
					'区切り線
					e.Graphics.DrawLine(clsPen, m_Memori.nPosLineLeft, nPos, m_Memori.nPosLineLeft + CInt(m_Memori.HojoLineLen / 2), nPos)
				End If
			Next ii

		Catch ex As Exception
			LOG(tClass_LogManager.LNO.ERR, String.Format("詳細目盛描画異常[{0}]", ex.Message))
		End Try
	End Sub

	''' <summary>
	''' メモリ描画
	''' </summary>
	''' <remarks></remarks>
	Public Overrides Sub Paint_Memori(ByVal e As System.Windows.Forms.PaintEventArgs)
		' 目盛り非表示の場合は処理不要
		If Not m_Memori.bIsDsp Then Exit Sub

		' ベースクラスの目盛り描画処理を呼び出す
		MyBase.Paint_Memori(e)

		' 詳細目盛り表示処理呼び出し
		PaintScaleDetail(e)
	End Sub

    ''' <summary>
    ''' 最終描画処理を行う
    ''' </summary>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Protected Overrides Sub Paint_End(ByVal e As System.Windows.Forms.PaintEventArgs)
        ' ベースクラスのオーバーレイ描画を呼び出す
        MyBase.Paint_OverLay(e)
    End Sub

End Class
