Option Strict Off

Imports tClassLibrary

Public Class frmDenbun

    ''' <summary>
    ''' �f�[�^�擾
    ''' </summary>
    ''' <param name="typZiseki">�r�������уf�[�^</param>
    ''' <remarks></remarks>
    Private Function GetData(ByRef typZiseki As PRO_TYPE_ZISEKI) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��
        Dim bytData() As Byte        '���`���o�C�i���f�[�^
        Dim ii As Integer
        Dim jj As Integer

        Dim para(0) As Data.SqlClient.SqlParameter
        para(0) = New Data.SqlClient.SqlParameter("@Key", SqlDbType.Image, tMod.SizeOf(typZiseki))

        '�r���Ǘ�No�Ȃ�
        strSQL = ""
        strSQL &= " SELECT ���`��"
        strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
        strSQL &= " WHERE ID = 'test'"

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            Return False
        End If

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Return False

        Try

            '' �f�[�^�擾
            sqlRead.Read()

            ''���`��
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

            '�r���Ǘ�No�Ȃ�
            strSQL = ""
            strSQL &= " UPDATE " & DB_BUF_PRO_S_ZISEKI & " SET "
            strSQL &= "  ���`�� =@Key "
            strSQL &= " WHERE ID = 'test' "

            para(0).Value = bytData

            ' Select�����s
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
    ''' �������уv���R���N���b�N
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
    ''' ��������FF�N���b�N
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
    ''' �̐����уN���b�N
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
    ''' �J�b�g���уN���b�N
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
    ''' �O�H�����я��N���b�N
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
    ''' �v���R���e�X�g���ѓ`���쐬
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnProZi_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnProZi.Click


        Dim typZiseki As PRO_TYPE_ZISEKI = Nothing  '�r�������уf�[�^

        Call GetData(typZiseki)



    End Sub



    ''' <summary>
    ''' �f�[�^�擾
    ''' </summary>
    ''' <remarks></remarks>
    Private Function GetFFData() As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��
        Dim bytData1() As Byte        '���`���o�C�i���f�[�^
        Dim bytData2() As Byte
        Dim typFF_O As FF_JISSEKI = Nothing
        Dim typFF_U As FF_JISSEKI = Nothing
        Dim ii As Integer


        Dim para(1) As Data.SqlClient.SqlParameter
        para(0) = New Data.SqlClient.SqlParameter("@Key1", SqlDbType.Image, tMod.SizeOf(typFF_O))
        para(1) = New Data.SqlClient.SqlParameter("@Key2", SqlDbType.Image, tMod.SizeOf(typFF_U))

        '�r���Ǘ�No�Ȃ�
        strSQL = ""
        strSQL &= " SELECT ���`��1,���`��2"
        strSQL &= " FROM " & DB_BUF_FF_S_ZISEKI
        strSQL &= " WHERE ID = 'test'"

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            Return False
        End If

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Return False

        Try

            '' �f�[�^�擾
            sqlRead.Read()

            ''���`��
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

            '�r���Ǘ�No�Ȃ�
            strSQL = ""
            strSQL &= " UPDATE " & DB_BUF_FF_S_ZISEKI & " SET "
            strSQL &= "  ���`��1 =@Key1, "
            strSQL &= "  ���`��2 =@Key2 "
            strSQL &= " WHERE ID = 'test' "

            para(0).Value = bytData1
            para(1).Value = bytData2

            ' Select�����s
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
        Dim strSQL As String    'SQL��



        '�r���Ǘ�No�Ȃ�
        strSQL = ""
        strSQL &= " UPDATE " & DB_BUF_PRO_S_ZISEKI & " SET "
        strSQL &= "  ���`�� =@ "
        strSQL &= " WHERE ID = 'test' "

    End Sub



    ''' <summary>
    ''' FF�e�X�g�`���쐬
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnFFMake_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnFFMake.Click

        Call GetFFData()

    End Sub


End Class
