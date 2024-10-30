Option Strict On

Imports tClassLibrary


#Const IsVirtual = False		' 使いやすいようにMustInherit指定にしたら、継承先でデザインが使えなくなるため、切り替えることができるようにした。どちらでも同じ動作


''' <summary>
''' 実態
''' </summary>
''' <remarks></remarks>
#If IsVirtual Then
Public MustInherit Class CntCoilInf_Base
#Else
Public Class CntCoilInf_Base
#End If

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 継承先で実装しねて
	'/ /////////////////////////////////////////////////////////////////////////////////

#If IsVirtual Then
	''' <summary>
	''' コントロールの初期化処理
	''' </summary>
	''' <remarks></remarks>
	Public MustOverride Sub Header_Init()

	''' <summary>
	''' コイル情報表示処理
	''' </summary>
	''' <param name="sKizukenNo">管理No</param>
	''' <param name="ou">表裏(0オリジン)</param>
	''' <remarks></remarks>
	Public MustOverride Sub Header_Dsp(ByVal sKizukenNo as String )

#Else

	Public Overridable Sub Header_Init()

	End Sub
    Public Overridable Sub Header_Dsp(ByVal sKizukenNo As String, ByVal ou As Integer)

    End Sub
#End If



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 共通処理
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' コイル情報取得
	''' </summary>
	''' <param name="strSql">SQL文(1件のみ返すSQL文ね)</param>
	''' <param name="tcls_DB">接続DB(New済み)</param>
	''' <param name="sqlRead">結果(nothing)</param>
	''' <returns>復帰情報</returns>
	''' <remarks></remarks>
	Protected Function GetCoilInf(ByVal strSql As String, ByVal tcls_DB As tClass.tClass_SQLServer, ByRef sqlRead As Data.SqlClient.SqlDataReader) As Boolean
        '''' データ取得
		' Select文実行
		sqlRead = tcls_DB.SelectExecute(strSql)
		If sqlRead Is Nothing Then
			gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB読み込みエラー [" & DB_COIL_INF & "]")
			Return False
		End If

		' データ取得
		Try
			' 一件も無ければ false
			Return sqlRead.Read()

		Catch ex As Exception
			gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "コイル情報取得エラー[" & ex.Message & "]")
			tcls_DB.Close()
			Return False
		End Try
	End Function
End Class


