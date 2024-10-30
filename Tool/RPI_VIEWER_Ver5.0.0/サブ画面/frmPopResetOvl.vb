Imports tClassLibrary

Public Class frmPopResetOvl


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 公開イベント
    Public Event tEv_ReSetKizuOvl()										' オーバーレイ再セット



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数

    ' データ
    Private m_lstDetect As List(Of DATA_DETECT_INF) = Nothing			' 疵情報リスト


    ' コイル単位
    Private m_nStartCamSet As Integer				' 最初のカメラセット
    Private m_nOffSetVal() As Integer				' カメラ数分



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ プロパティ

    ''' <summary>
    ''' 疵情報リスト
    ''' </summary>
    ''' <param name="lst">疵情報リスト</param>
    ''' <remarks></remarks>
    Friend Sub SetData_LstDetect(ByRef lst As List(Of DATA_DETECT_INF))
        m_lstDetect = lst
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 基本

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    Private Sub frmPopResetOvl_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        SetFormLocationAtOnce = True
    End Sub


    ''' <summary>
    ''' コイル変更
    ''' </summary>
    ''' <param name="nStartCamSet">最初のカメラセット</param>
    ''' <param name="nCamSetNum">カメラセット数</param>
    ''' <remarks></remarks>
    Public Sub ChangeCoil(ByVal nStartCamSet As Integer, ByVal nCamSetNum As Integer)
        ' メンバー初期化
        m_nStartCamSet = nStartCamSet
        ReDim m_nOffSetVal(nCamSetNum - 1)
        lblNowOffset.Text = ""




        Call SetCamItem(nStartCamSet, nCamSetNum)
        Call ClearOffSet()
    End Sub





    ''' <summary>
    ''' オフセット解除
    ''' </summary>
    Private Sub btnClearOffSert_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnClearOffSert.Click
        If -1 = cmbCam.SelectedIndex Then Return

        Call ClearOffSet()

        Call DspNowOffset()
        RaiseEvent tEv_ReSetKizuOvl()
    End Sub

    ''' <summary>
    ''' 指定反映
    ''' </summary>
    Private Sub btnSetOffSert_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSetOffSert.Click
        If -1 = cmbCam.SelectedIndex Then Return

        Dim nCamPos As Integer = CType(cmbCam.SelectedItem, tClass_CmbItem).g_nKey
        Dim nVal As Integer = CInt(nudOffset.Value)

        Call SetOffSet(nCamPos, 0, nVal)

        Call DspNowOffset()
        RaiseEvent tEv_ReSetKizuOvl()
    End Sub
    ''' <summary>
    ''' 加算反映
    ''' </summary>
    Private Sub btnAddOffSert_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAddOffSert.Click
        If -1 = cmbCam.SelectedIndex Then Return

        Dim nCamPos As Integer = CType(cmbCam.SelectedItem, tClass_CmbItem).g_nKey
        Dim nVal As Integer = CInt(nudOffset.Value)

        Call SetOffSet(nCamPos, 1, nVal)

        Call DspNowOffset()
        RaiseEvent tEv_ReSetKizuOvl()
    End Sub


    ''' <summary>
    ''' 現在オフセット量表示
    ''' </summary>
    Private Sub cmbCam_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbCam.SelectedIndexChanged
        Call DspNowOffset()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 処理

    ''' <summary>
    ''' 現在オフセット量表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspNowOffset()
        Dim nCamPos As Integer = CType(cmbCam.SelectedItem, tClass_CmbItem).g_nKey
        If -1 = nCamPos Then
            lblNowOffset.Text = ""
        Else
            lblNowOffset.Text = m_nOffSetVal(nCamPos).ToString

        End If
    End Sub



    ''' <summary>
    ''' カメラ数セット
    ''' </summary>
    ''' <param name="nStartCamSet">最初のカメラセット</param>
    ''' <param name="nCamSetNum">カメラセット数</param>
    ''' <remarks></remarks>
    Private Sub SetCamItem(ByVal nStartCamSet As Integer, ByVal nCamSetNum As Integer)
        cmbCam.Items.Clear()

        For ii As Integer = -1 To nCamSetNum - 1
            Dim sName As String


            If -1 = ii Then
                sName = "全体"
            Else
                sName = (nStartCamSet + ii).ToString
            End If

            ' リストに行追加
            Dim item As New tClass_CmbItem(ii, sName)
            Call cmbCam.Items.Add(item)
        Next
        cmbCam.SelectedIndex = 0
    End Sub



    ''' <summary>
    ''' オフセット量初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ClearOffSet()
        For ii As Integer = 0 To m_lstDetect.Count - 1		' 値を書き換えるため、コレクションではNG
            Dim data As DATA_DETECT_INF = m_lstDetect(ii)

            data.nFrameNo = data.nFrameNoBase

            m_lstDetect(ii) = data			' 更新
        Next

        ' 現状セット
        For ii As Integer = 0 To m_nOffSetVal.Length - 1
            m_nOffSetVal(ii) = 0
        Next
    End Sub

    ''' <summary>
    ''' オフセット量セット
    ''' </summary>
    ''' <param name="nCamPos">カメラ位置(-1:全部 0～:指定のみ)</param>
    ''' <param name="nMode">(0:絶対値 1:相対値)</param>
    ''' <param name="nVal">オフセット量</param>
    ''' <remarks></remarks>
    Private Sub SetOffSet(ByVal nCamPos As Integer, ByVal nMode As Integer, ByVal nVal As Integer)
        Dim nS As Integer
        Dim nE As Integer

        If -1 = nCamPos Then
            nS = 0
            nE = m_nOffSetVal.Length - 1
        Else
            nS = nCamPos
            nE = nCamPos
        End If

        ' 値反映
        For ii As Integer = 0 To m_lstDetect.Count - 1		' 値を書き換えるため、コレクションではNG
            Dim data As DATA_DETECT_INF = m_lstDetect(ii)


            If nS <= data.nCamPos And nE >= data.nCamPos Then
                If 0 = nMode Then
                    ' 絶対
                    data.nFrameNo = data.nFrameNoBase + nVal
                Else
                    ' 相対
                    data.nFrameNo += nVal
                End If
            End If


            m_lstDetect(ii) = data			' 更新
        Next


        ' 現状セット
        For ii As Integer = nS To nE
            If 0 = nMode Then
                ' 絶対
                m_nOffSetVal(ii) = nVal
            Else
                ' 相対
                m_nOffSetVal(ii) += nVal
            End If
        Next
    End Sub


End Class