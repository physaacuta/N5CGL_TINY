Option Strict On
Imports System.Collections
Imports tClassLibrary

Public Class frmPopRpiPath

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �t�H�[���\��
    ''' </summary>
    Private Sub frmRpiPath_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        SetFormLocationAtOnce = True
        Call LoadIni()
    End Sub


    Private Sub LoadIni()
        For nCnt As Integer = 1 To 999
            ' ini�t�@�C���擾
            Dim sKey As String = String.Format("L_{0}", nCnt)
            Dim sWk As String = tMod.tMod_IniFile.ReadIniStr("LINE", sKey, TASKINI_NAME)

            ' �l����
            Dim sAry() As String = sWk.Split(","c)
            If 2 <> sAry.Length Then Return
            sAry(0).Trim()
            sAry(1).Trim()
            If String.IsNullOrEmpty(sAry(0)) Then Return
            If String.IsNullOrEmpty(sAry(1)) Then Return

            ' �ΏۃZ�N�V�������Q��

            '�\��
            For ou As Integer = 0 To 1
                '�����t�H���_
                For ii As Integer = 0 To 10 - 1
                    Dim sPath As String = tMod.tMod_IniFile.ReadIniStr(sAry(1), String.Format("S_{0}_{1}", ou, ii + 1), TASKINI_NAME)
                    If "" = sPath Then Continue For


                    ' �s����
                    Dim item As New ListViewItem(sAry(0))
                    item.SubItems.Add(CStr(IIf(ou = 0, "�\_", "��_")) & (ii + 1))
                    item.SubItems.Add(sPath)

                    If ou = 1 Then
                        item.BackColor = Color.LightCyan
                    End If

                    ' ���X�g�ɍs�ǉ�
                    lstView.Items.Add(item)

                Next ii
            Next ou
        Next nCnt

    End Sub

    ''' <summary>
    ''' ���X�g�I��
    ''' </summary>
    Private Sub lstView_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles lstView.Click
        If 0 = lstView.SelectedItems.Count Then Exit Sub ' ��������
        Dim sPath As String = lstView.SelectedItems(0).SubItems(2).Text	  ' �I���p�X�擾

        Call ExecExplorer(sPath)
    End Sub

    ''' <summary>
    ''' �G�N�X�v���[���N��
    ''' </summary>
    ''' <param name="sPath">�p�X</param>
    ''' <remarks></remarks>
    Private Sub ExecExplorer(ByVal sPath As String)

        Dim strCommand As String

        '�G�N�X�v���[���N��()
        'strCommand = "/e,"		'�t�H���_�c���[���\��
        strCommand = "/n,"      '�t�H���_�c���[��\��
        strCommand &= sPath

        Process.Start("explorer.exe", strCommand)
    End Sub

End Class