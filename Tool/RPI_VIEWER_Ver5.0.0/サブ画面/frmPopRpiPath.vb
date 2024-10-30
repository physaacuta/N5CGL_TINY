Option Strict On
Imports System.Collections
Imports tClassLibrary

Public Class frmPopRpiPath

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' フォーム表示
    ''' </summary>
    Private Sub frmRpiPath_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        SetFormLocationAtOnce = True
        Call LoadIni()
    End Sub


    Private Sub LoadIni()
        For nCnt As Integer = 1 To 999
            ' iniファイル取得
            Dim sKey As String = String.Format("L_{0}", nCnt)
            Dim sWk As String = tMod.tMod_IniFile.ReadIniStr("LINE", sKey, TASKINI_NAME)

            ' 値分割
            Dim sAry() As String = sWk.Split(","c)
            If 2 <> sAry.Length Then Return
            sAry(0).Trim()
            sAry(1).Trim()
            If String.IsNullOrEmpty(sAry(0)) Then Return
            If String.IsNullOrEmpty(sAry(1)) Then Return

            ' 対象セクションを参照

            '表裏
            For ou As Integer = 0 To 1
                '分割フォルダ
                For ii As Integer = 0 To 10 - 1
                    Dim sPath As String = tMod.tMod_IniFile.ReadIniStr(sAry(1), String.Format("S_{0}_{1}", ou, ii + 1), TASKINI_NAME)
                    If "" = sPath Then Continue For


                    ' 行生成
                    Dim item As New ListViewItem(sAry(0))
                    item.SubItems.Add(CStr(IIf(ou = 0, "表_", "裏_")) & (ii + 1))
                    item.SubItems.Add(sPath)

                    If ou = 1 Then
                        item.BackColor = Color.LightCyan
                    End If

                    ' リストに行追加
                    lstView.Items.Add(item)

                Next ii
            Next ou
        Next nCnt

    End Sub

    ''' <summary>
    ''' リスト選択
    ''' </summary>
    Private Sub lstView_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles lstView.Click
        If 0 = lstView.SelectedItems.Count Then Exit Sub ' おかしい
        Dim sPath As String = lstView.SelectedItems(0).SubItems(2).Text	  ' 選択パス取得

        Call ExecExplorer(sPath)
    End Sub

    ''' <summary>
    ''' エクスプローラ起動
    ''' </summary>
    ''' <param name="sPath">パス</param>
    ''' <remarks></remarks>
    Private Sub ExecExplorer(ByVal sPath As String)

        Dim strCommand As String

        'エクスプローラ起動()
        'strCommand = "/e,"		'フォルダツリーも表示
        strCommand = "/n,"      'フォルダツリー非表示
        strCommand &= sPath

        Process.Start("explorer.exe", strCommand)
    End Sub

End Class