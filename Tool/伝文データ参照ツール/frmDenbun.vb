Option Strict Off

Imports tClassLibrary

Public Class frmDenbun

    ''' <summary>
    ''' データ取得
    ''' </summary>
    ''' <param name="typZiseki">疵検査実績データ</param>
    ''' <remarks></remarks>
    Private Function GetData(ByRef typZiseki As PRO_TYPE_ZISEKI) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文
        Dim bytData() As Byte        '生伝文バイナリデータ
        Dim ii As Integer
        Dim jj As Integer

        Dim para(0) As Data.SqlClient.SqlParameter
        para(0) = New Data.SqlClient.SqlParameter("@Key", SqlDbType.Image, tMod.SizeOf(typZiseki))

        '疵検管理Noなし
        strSQL = ""
        strSQL &= " SELECT 生伝文"
        strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
        strSQL &= " WHERE ID = 'test'"

        'DBオープン
        If Not tcls_DB.Open() Then
            Return False
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Return False

        Try

            '' データ取得
            sqlRead.Read()

            ''生伝文
            bytData = CType(sqlRead.GetValue(0), Byte())

            tMod.MemSet(bytData, 0, typZiseki, tMod.SizeOf(typZiseki))

            typZiseki.data.nKikiMode = 0
            typZiseki.data.nZiMode = 1

            For jj = 0 To 5
                typZiseki.data.grade_inf(jj).nMixedRate = jj

                For ii = 0 To 9
                    typZiseki.data.grade_inf(jj).nTypeCnt(ii) = jj * 10 + ii + 1
                Next ii

            Next jj

            'typZiseki.data.grade_inf(0).nTypeCnt(0) = 10
            'typZiseki.data.grade_inf(5).nTypeCnt(9) = 1000

            sqlRead.Close()
            sqlRead = Nothing

            tMod.MemSet(typZiseki, bytData, 0, tMod.SizeOf(typZiseki))

            '疵検管理Noなし
            strSQL = ""
            strSQL &= " UPDATE " & DB_BUF_PRO_S_ZISEKI & " SET "
            strSQL &= "  生伝文 =@Key "
            strSQL &= " WHERE ID = 'test' "

            para(0).Value = bytData

            ' Select文実行
            tcls_DB.DMLExecute(strSQL, para)

        Catch ex As Exception
        Finally

            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

        Return True

    End Function


   

   
    ''' <summary>
    ''' 検査実績プロコンクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnPro_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnPro.Click

        Dim frm As New frmMixedRate

        frm.ShowDialog()
        frm.Dispose()
        frm = Nothing

    End Sub

    ''' <summary>
    ''' 検査実績FFクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnFF_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnFF.Click

        Dim frm As New frmFF

        frm.ShowDialog()
        frm.Dispose()
        frm = Nothing


    End Sub
    ''' <summary>
    ''' 採寸実績クリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSaisun_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSaisun.Click


        Dim frm As New frmSaisun

        frm.ShowDialog()
        frm.Dispose()
        frm = Nothing

    End Sub
    ''' <summary>
    ''' カット実績クリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnCut_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCut.Click

        Dim frm As New frmCut


        frm.ShowDialog()
        frm.Dispose()
        frm = Nothing


    End Sub

    ''' <summary>
    ''' 前工程実績情報クリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnFFRecv_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnFFRecv.Click


        Dim frm As New frmFFRecv


        frm.ShowDialog()
        frm.Dispose()
        frm = Nothing


    End Sub

    ''' <summary>
    ''' プロコンテスト実績伝文作成
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnProZi_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnProZi.Click


        Dim typZiseki As PRO_TYPE_ZISEKI = Nothing  '疵検査実績データ

        Call GetData(typZiseki)



    End Sub



    ''' <summary>
    ''' データ取得
    ''' </summary>
    ''' <remarks></remarks>
    Private Function GetFFData() As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文
        Dim bytData1() As Byte        '生伝文バイナリデータ
        Dim bytData2() As Byte
        Dim typFF_O As FF_JISSEKI = Nothing
        Dim typFF_U As FF_JISSEKI = Nothing
        Dim ii As Integer


        Dim para(1) As Data.SqlClient.SqlParameter
        para(0) = New Data.SqlClient.SqlParameter("@Key1", SqlDbType.Image, tMod.SizeOf(typFF_O))
        para(1) = New Data.SqlClient.SqlParameter("@Key2", SqlDbType.Image, tMod.SizeOf(typFF_U))

        '疵検管理Noなし
        strSQL = ""
        strSQL &= " SELECT 生伝文1,生伝文2"
        strSQL &= " FROM " & DB_BUF_FF_S_ZISEKI
        strSQL &= " WHERE ID = 'test'"

        'DBオープン
        If Not tcls_DB.Open() Then
            Return False
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Return False

        Try

            '' データ取得
            sqlRead.Read()

            ''生伝文
            bytData1 = CType(sqlRead.GetValue(0), Byte())
            bytData2 = CType(sqlRead.GetValue(1), Byte())

            tMod.MemSet(bytData1, 0, typFF_O, tMod.SizeOf(typFF_O))
            tMod.MemSet(bytData2, 0, typFF_U, tMod.SizeOf(typFF_U))
         
            typFF_O.Hd.Line_Code = tMod.StringToByte("54")
            typFF_U.Hd.Line_Code = tMod.StringToByte("54")
            typFF_O.Hd.bytDate = tMod.StringToByte("20080708090000")
            typFF_U.Hd.bytDate = tMod.StringToByte("20080708090000")
            typFF_O.Id = tMod.StringToByte("1520080708 090000   ")
            typFF_U.Id = tMod.StringToByte("1520080708 090000   ")
            typFF_O.CutCount = tMod.StringToByte("02")
            typFF_U.CutCount = tMod.StringToByte("02")
            typFF_O.DataDefault = tMod.StringToByte("0004")
            typFF_U.DataDefault = tMod.StringToByte("0004")

            sqlRead.Close()
            sqlRead = Nothing

            For ii = 4 To 1799
                typFF_O.Dt(ii).Kizukbn = "32"
                typFF_O.Dt(ii).Kind = tMod.StringToByte("  ")
                typFF_O.Dt(ii).Grade = tMod.StringToByte("  ")
                typFF_O.Dt(ii).WidPos = tMod.StringToByte("    ")
                typFF_O.Dt(ii).Len = tMod.StringToByte("    ")
                typFF_O.Dt(ii).DefectPos = tMod.StringToByte("     ")
                typFF_O.Dt(ii).kbn = "32"
                typFF_O.Dt(ii).No = tMod.StringToByte("    ")
                typFF_O.Dt(ii).DspOpe = "32"
                typFF_U.Dt(ii).Kizukbn = "32"
                typFF_U.Dt(ii).Kind = tMod.StringToByte("  ")
                typFF_U.Dt(ii).Grade = tMod.StringToByte("  ")
                typFF_U.Dt(ii).WidPos = tMod.StringToByte("    ")
                typFF_U.Dt(ii).Len = tMod.StringToByte("    ")
                typFF_U.Dt(ii).DefectPos = tMod.StringToByte("     ")
                typFF_U.Dt(ii).kbn = "32"
                typFF_U.Dt(ii).No = tMod.StringToByte("    ")
                typFF_U.Dt(ii).DspOpe = "32"
            Next ii





            tMod.MemSet(typFF_O, bytData1, 0, tMod.SizeOf(typFF_O))
            tMod.MemSet(typFF_U, bytData2, 0, tMod.SizeOf(typFF_U))

            '疵検管理Noなし
            strSQL = ""
            strSQL &= " UPDATE " & DB_BUF_FF_S_ZISEKI & " SET "
            strSQL &= "  生伝文1 =@Key1, "
            strSQL &= "  生伝文2 =@Key2 "
            strSQL &= " WHERE ID = 'test' "

            para(0).Value = bytData1
            para(1).Value = bytData2

            ' Select文実行
            tcls_DB.DMLExecute(strSQL, para)

        Catch ex As Exception
        Finally

            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

        Return True

    End Function


    Private Sub UpDateData()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文



        '疵検管理Noなし
        strSQL = ""
        strSQL &= " UPDATE " & DB_BUF_PRO_S_ZISEKI & " SET "
        strSQL &= "  生伝文 =@ "
        strSQL &= " WHERE ID = 'test' "

    End Sub



    ''' <summary>
    ''' FFテスト伝文作成
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnFFMake_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnFFMake.Click

        Call GetFFData()

    End Sub


End Class
