Option Strict On                    ' 全疵検、全プログラム必須

Imports tClassLibrary               ' 疵検VBライブラリ。
Imports tClassLibrary.tClass
Namespace tClass

    Public Class SQLServerEx
        Inherits tClassLibrary.tClass.tClass_SQLServer

        Public Shared gbTraceDefault As Boolean = True
        Public bTrace As Boolean = gbTraceDefault

        Public m_sTraceLog As String
        Protected m_lastException As Exception = Nothing


        Public ReadOnly Property LastException As Exception
            Get
                Return m_lastException
            End Get
        End Property
        Public ReadOnly Property LastSQL As String
            Get
                Return m_sTraceLog
            End Get
        End Property


        Sub New(ByVal ini As String, ByVal section As String)
            MyBase.New(ini, section)
        End Sub

        Sub New(ByVal ini As String, ByVal section As String, ByVal user As String, ByVal pass As String)
            MyBase.New(ini, section, user, pass)
        End Sub

        Protected Function PrepareCommand(strSQL As String, oParams() As Common.DbParameter) As String
            Dim sbTraceLog As New System.Text.StringBuilder
            m_Command.CommandText = strSQL

            If oParams IsNot Nothing Then
                m_Command.Parameters.Clear()
                For Each param As System.Data.SqlClient.SqlParameter In oParams
                    m_Command.Parameters.Add(param)
                Next
                If bTrace Then
                    sbTraceLog.Append(SimpleSQLParameter.ToParamString(oParams))
                End If


                If bTrace Then
                    sbTraceLog.AppendLine()
                End If
            End If

            If bTrace Then
                sbTraceLog.AppendLine(strSQL)
                m_sTraceLog = sbTraceLog.ToString
            End If

            Return m_Command.ToString
        End Function
        ''例外を吐く連中
        Public Shadows Function SelectExecuteEx(ByVal strSQL As String, ByVal ParamArray oParams() As System.Data.Common.DbParameter) As SqlClient.SqlDataReader
            m_sTraceLog = PrepareCommand(strSQL, oParams)
            Return m_Command.ExecuteReader(Data.CommandBehavior.Default)
        End Function

        Public Shadows Function SelectAdapterEx(ByVal strSQL As String, ByVal ParamArray oParams() As System.Data.Common.DbParameter) As SqlClient.SqlDataAdapter
            m_sTraceLog = PrepareCommand(strSQL, oParams)
            Return New SqlClient.SqlDataAdapter(m_Command)
        End Function

        Public Shadows Function DMLExecuteEx(ByVal strSQL As String, ByVal ParamArray oParams() As SqlClient.SqlParameter) As Integer
            m_sTraceLog = PrepareCommand(strSQL, oParams)
            Return DMLExecute(strSQL)
        End Function


        ''例外を握りつぶす連中。エラー処理は、都度都度If LastException IsNot Nothing Then ...を呼ぶ
        Public Shadows Function SelectExecute(ByVal strSQL As String, ByVal ParamArray oParams() As System.Data.Common.DbParameter) As SqlClient.SqlDataReader
            Try
                m_lastException = Nothing
                Return SelectExecuteEx(strSQL, oParams)
            Catch ex As Exception
                m_lastException = ex
                Return Nothing
            End Try
        End Function

        Public Shadows Function SelectAdapter(ByVal strSQL As String, ByVal ParamArray oParams() As System.Data.Common.DbParameter) As SqlClient.SqlDataAdapter
            Try
                m_lastException = Nothing
                Return SelectAdapterEx(strSQL, oParams)
            Catch ex As Exception
                m_lastException = ex
                Return Nothing
            End Try
        End Function

        Public Shadows Function DMLExecute(ByVal strSQL As String, ByVal ParamArray oParams() As SqlClient.SqlParameter) As Integer
            Try
                m_lastException = Nothing
                Return DMLExecuteEx(strSQL, oParams)
            Catch ex As Exception
                m_lastException = ex
                Return -1
            End Try
        End Function

    End Class

    Public Class SimpleSQLParameter
        Private params As New List(Of SqlClient.SqlParameter)
        Private sSQl As New System.Text.StringBuilder

        Public Sub AppendFormat(ByVal s As String, ByVal arg0 As Object)
            sSQl.AppendFormat(s, arg0)
        End Sub
        Public Sub AppendFormat(ByVal s As String, ByVal ParamArray args() As Object)
            sSQl.AppendFormat(s, args)
        End Sub
        Public Sub Append(ByVal s As String)
            sSQl.Append(s)
        End Sub

        Public Overrides Function ToString() As String
            Return sSQl.ToString
        End Function

        Public Sub AddParam(ByVal name As String, ByVal value As Object)
            Dim s As String = TryCast(value, String)
            If s IsNot Nothing Then
                Dim param As New SqlClient.SqlParameter(name, SqlDbType.VarChar, s.Length, name)
                param.Value = s
                params.Add(param)
                Return
            End If

            params.Add(New SqlClient.SqlParameter(name, value))
        End Sub


        Public Function ToParamArray() As SqlClient.SqlParameter()
            Return params.ToArray
        End Function

        Public Shared Function ToParamString(params As IEnumerable(Of Common.DbParameter)) As String
            Dim sb As New System.Text.StringBuilder
            For Each param As SqlClient.SqlParameter In params
                Dim objP As Object = param.Value
                Dim typP As Type = objP.GetType

                Dim dbType As String = ""
                Dim dbValue As String = ""

                If typP Is GetType(String) Then
                    dbType = String.Format("varchar({1})", objP.ToString().Length)
                    dbValue = String.Format("'{0}'", objP)
                ElseIf typP Is GetType(Integer) Then
                    dbType = "Integer"
                    dbValue = param.Value.ToString
                End If

                ' default handler
                If String.IsNullOrEmpty(dbType) Then
                    dbType = typP.ToString
                    If typP.IsArray Then
                        dbValue = "0x00"
                    Else
                        dbValue = String.Format("'{0}'", objP)
                    End If
                End If

                'write sql.
                sb.AppendFormat("declare {0} as {1}", param.ParameterName, dbType)
                sb.AppendLine()
                sb.AppendFormat("set {0} = {1}", param.ParameterName, dbValue)
                sb.AppendLine()
            Next

            Return sb.ToString
        End Function


        Public Shared Function One(ByVal name As String, ByVal value As Object) As SqlClient.SqlParameter()
            Dim s As String = TryCast(value, String)
            If s IsNot Nothing Then
                Dim param As New SqlClient.SqlParameter(name, SqlDbType.VarChar, s.Length, name)
                param.Value = s
                Return New SqlClient.SqlParameter() {param}
            End If
            Return New SqlClient.SqlParameter() {New SqlClient.SqlParameter(name, value)}
        End Function

        ''' <summary>
        ''' 主キー１つだけをパラメタ化して返す。 = @PK
        ''' </summary>
        Public Shared Function PK(ByVal value As Object) As SqlClient.SqlParameter()
            Return One("@PK", value)
        End Function

        ''' <summary>
        ''' 検査管理NOと疵NOをパラメタ化して戻す。
        ''' </summary>
        ''' <param name="KanriNO">検査管理番号 =@KanriNO</param>
        ''' <param name="KizuNo">疵NO =@KizuNO</param>
        Public Shared Function KK_NO(ByVal KanriNO As String, ByVal KizuNo As Integer) As SqlClient.SqlParameter()
            Dim params As SqlClient.SqlParameter() = New SqlClient.SqlParameter() {
          New SqlClient.SqlParameter("@KanriNO", SqlDbType.VarChar, 20),
          New SqlClient.SqlParameter("@KizuNO", SqlDbType.Int, 4)}
            params(0).Value = KanriNO
            params(1).Value = KizuNo
            Return params
        End Function

        'Public Const KKNO_Where As String = " 疵検管理No=@KanriNO AND 疵No=@KizuNO"
    End Class
End Namespace