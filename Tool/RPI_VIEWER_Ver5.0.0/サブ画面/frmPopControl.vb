
Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class frmPopControl
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 公開イベント

    ''' <param name="mode">Yモード (0:無視 1:指定値(先頭) 2:指定値(中央) 3:現在から差分)</param>
    ''' <param name="len">[mm]</param>
    Public Event tEv_JumpNagate(ByVal mode As Integer, ByVal len As Single)

    ''' <param name="nMode">-1:先頭 1:尾端 0:任意位置</param>
    ''' <param name="nVal">nMode0のときのみ。差分距離</param>
    Public Event tEv_MapJumpNagate(ByVal nMode As Integer, ByVal nVal As Integer)



    ''' <param name="nVal">ライン速度 [mpm]</param>
    Public Event tEv_LineSpeed_ValueChange(ByVal nVal As Integer) ' ライン速度
    Public Event tEv_AutoDownUmu()	' 自動減速関係 設定変更
    Public Event tEv_AutoDownCancel() ' 自動減速解除 (ライン速度0ボタン押したときのみ)


    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ メンバー変数

    ' クラス
    Private mcls_pParam As clsParamManager = Nothing					' パラメータ管理クラス (ポインタ)

    ' もろもろ
    Private m_nDefVal As Integer = 0										' デフォルト値 [mm] ※ピッチ指定時はピッチ

    ' 他スレッドからのアクセス用として、内部変数として情報保持
    Private m_nSpeed As Integer = 0												' ライン速度
    Private m_emDivSpeed As EM_DIV_SPEED = EM_DIV_SPEED.emDivSpeed_Up			' 正転、逆転（0mpmの時は、前回のまま）




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ プロパティ

    ''' <summary>
    ''' パラメータクラスセット
    ''' </summary>
    ''' <param name="cls">計算クラス</param>
    Public Sub SetCls_Param(ByRef cls As clsParamManager)
        mcls_pParam = cls
    End Sub

    ''' <summary>
    '''  計算結果セット
    ''' </summary>
    ''' <param name="dVal">測定結果mm</param>
    ''' <remarks></remarks>
    Public Sub SetVal_Calc(ByVal dVal As Double)
        Dim target As Decimal = CDec(dVal / 1000)
        target = Math.Min(target, nudCalc.Maximum)
        target = Math.Max(target, nudCalc.Minimum)
        nudCalc.Value = target
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    ' 基本
    ''' <summary>
    ''' フォームロード
    ''' </summary>
    Private Sub frmPop_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        SetFormLocationAtOnce = True
        '自動減速用疵反映
        Call SetListGr()

        '設定読み込み

    End Sub



    ''' <summary>
    ''' 減速部分の無効化
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Disable_AutoDown()
        Me.FormBorderStyle = Windows.Forms.FormBorderStyle.FixedDialog

        Dim s As Size = Me.ClientSize
        s.Height = Me.grpAutoDown.Location.Y
        Me.ClientSize = s

        Me.AutoScroll = False
    End Sub



    ''/ /////////////////////////////////////////////////////////////////////////////////
    '' コイルチェンジ
    ''' <summary>
    ''' コイル初期値
    ''' </summary>
    ''' <param name="nJumpVal">ジャンプ値 [mm]</param>
    ''' <remarks></remarks>
    Public Sub ChangeDefVal(ByVal nJumpVal As Integer)
        If 0 >= nJumpVal Then nJumpVal = 100

        m_nDefVal = nJumpVal
        nudY.Value = CDec(nJumpVal / 1000)

        ' 自動通板
        Me.LineSpeed = 0


    End Sub

    ''' <summary>
    ''' ライン変更
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ChangeLine()
        ' 自動減速
        Call SetListGr()
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    ' ジャンプ

    ''' <summary>
    ''' 指定距離
    ''' </summary>
    Private Sub btnSubY_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSubY.Click
        Dim nVal As Integer = CInt(nudY.Value * 1000 * -1)

        RaiseEvent tEv_JumpNagate(3, nVal)
    End Sub
    Private Sub btnAddY_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAddY.Click
        Dim nVal As Integer = CInt(nudY.Value * 1000)

        RaiseEvent tEv_JumpNagate(3, nVal)
    End Sub


    ''' <summary>
    ''' 指定位置移動
    ''' </summary>
    Private Sub btnSkip_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSkip.Click
        Dim nVal As Integer = CInt(nudSkip.Value * 1000)

        RaiseEvent tEv_JumpNagate(2, nVal)
    End Sub

    ''' <summary>
    ''' 指定距離
    ''' </summary>
    Private Sub btnSubCalc_Click(sender As System.Object, e As System.EventArgs) Handles btnSubCalc.Click
        Dim nVal As Integer = CInt(nudCalc.Value * 1000 * -1)

        RaiseEvent tEv_JumpNagate(3, nVal)
    End Sub
    Private Sub btnAddCalc_Click(sender As System.Object, e As System.EventArgs) Handles btnAddCalc.Click
        Dim nVal As Integer = CInt(nudCalc.Value * 1000)

        RaiseEvent tEv_JumpNagate(3, nVal)
    End Sub



    ''' <summary>
    ''' 固定距離
    ''' </summary>
    Private Sub btnJump_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSub05.Click, btnSub01.Click, btnAdd01.Click, btnAdd05.Click, btnSub50.Click, btnAdd50.Click, btnAdd10.Click, btnSub10.Click, btnSub100.Click, btnSub1.Click, btnAdd100.Click, btnAdd1.Click
        Dim nVal As Integer = CInt(CDbl(CType(sender, Control).Tag) * 1000)

        RaiseEvent tEv_JumpNagate(3, nVal)
    End Sub

    ''' <summary>
    ''' 先端
    ''' </summary>
    Private Sub btnF_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnF.Click
        RaiseEvent tEv_MapJumpNagate(-1, 0)
    End Sub
    ''' <summary>
    ''' 尾端
    ''' </summary>
    Private Sub btnT_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnT.Click
        RaiseEvent tEv_MapJumpNagate(1, 0)
    End Sub


    ''' <summary>
    ''' 初期値戻し
    ''' </summary>
    Private Sub btnReLoad_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnReLoad.Click
        Try	' 初期画面時に押すと 0割になるため
            ' 設定
            nudY.Value = CDec(m_nDefVal / 1000)
        Catch ex As Exception

        End Try
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    ' 自動通板


    ''' <summary>
    ''' 正転逆転
    ''' </summary>
    Friend Property DivSpeed() As EM_DIV_SPEED
        Get
            Return m_emDivSpeed
        End Get
        Set(ByVal value As EM_DIV_SPEED)
            Me.LineSpeed = m_nSpeed
            Dim nSpeed As Integer = m_nSpeed

            m_emDivSpeed = value
            If value = EM_DIV_SPEED.emDivSpeed_Up Then
                nSpeed = Math.Abs(nSpeed)
            ElseIf value = EM_DIV_SPEED.emDivSpeed_Down Then
                nSpeed = Math.Abs(nSpeed) * -1
            End If

            ' 結果反映
            Me.LineSpeed = nSpeed

        End Set
    End Property

    ''' <summary>
    ''' ライン速度  ※別スレッドからアクセス可能性有りなので、ちゃんとしたつくり
    ''' </summary>
    Public Property LineSpeed() As Integer
        Get
            Return m_nSpeed	'trbSpeed.Value
        End Get
        Set(ByVal value As Integer)
            '''' 別スレッドアクセス対応
            If Not InvokeRequired Then
                ' 自スレッド
                Call SetLeinSpeedChange(value)
            Else
                ' 別スレッド
                Invoke(New SetLeinSpeedChangeDelegate(AddressOf SetLeinSpeedChange), value)
            End If
        End Set
    End Property
    Private Delegate Sub SetLeinSpeedChangeDelegate(ByVal nSpeed As Integer)		' 別スレッドからのアクセス用
    Private Sub SetLeinSpeedChange(ByVal value As Integer)
        ' 上下限
        If value < trbSpeed.Minimum Then trbSpeed.Value = trbSpeed.Minimum
        If value > trbSpeed.Maximum Then trbSpeed.Value = trbSpeed.Maximum
        If value >= trbSpeed.Minimum And value <= trbSpeed.Maximum Then
            trbSpeed.Value = value
        End If

        ' セット
        m_nSpeed = value
        lblSpeedNow.Text = value.ToString



        ' 正転、逆転 色替
        Dim nDiv As Integer
        If 0 < m_nSpeed Then
            nDiv = 1
        ElseIf 0 > m_nSpeed Then
            nDiv = -1
        Else
            ' 0のときは、前回値
            If m_emDivSpeed = EM_DIV_SPEED.emDivSpeed_Up Then
                nDiv = 1
            Else
                nDiv = -1
            End If
        End If

        If 1 = nDiv Then
            m_emDivSpeed = EM_DIV_SPEED.emDivSpeed_Up
            btnDivSpeedUp.BackColor = Color.Blue
            btnDivSpeedUp.ForeColor = Color.Yellow
            btnDivSpeedDown.BackColor = SystemColors.Control
        ElseIf -1 = nDiv Then
            m_emDivSpeed = EM_DIV_SPEED.emDivSpeed_Down
            btnDivSpeedUp.BackColor = SystemColors.Control
            btnDivSpeedUp.ForeColor = Color.Black
            btnDivSpeedDown.BackColor = Color.LawnGreen
        End If

        '' ---------->>>>> 操作BOXと同じように速度色替え
        'If True Then
        '	Dim nSpeed As Integer
        '	nSpeed = Math.Abs(m_nSpeed)
        '	btnSpeed1.BackColor = SystemColors.Control
        '	btnSpeed2.BackColor = SystemColors.Control
        '	btnSpeed3.BackColor = SystemColors.Control
        '	btnSpeed4.BackColor = SystemColors.Control
        '	btnSpeed5.BackColor = SystemColors.Control
        '	btnSpeed6.BackColor = SystemColors.Control
        '	btnSpeed7.BackColor = SystemColors.Control
        '	btnSpeed8.BackColor = SystemColors.Control
        '	If 5 = nSpeed Then
        '		btnSpeed1.BackColor = Color.White
        '	ElseIf 10 = nSpeed Then
        '		btnSpeed2.BackColor = Color.White
        '	ElseIf 20 = nSpeed Then
        '		btnSpeed3.BackColor = Color.White
        '	ElseIf 30 = nSpeed Then
        '		btnSpeed4.BackColor = Color.White
        '	ElseIf 60 = nSpeed Then
        '		btnSpeed5.BackColor = Color.White
        '	ElseIf 80 = nSpeed Then
        '		btnSpeed6.BackColor = Color.White
        '	ElseIf 100 = nSpeed Then
        '		btnSpeed7.BackColor = Color.White
        '	ElseIf 150 = nSpeed Then
        '		btnSpeed8.BackColor = Color.White
        '	End If
        'End If
        '' ----------<<<<< 操作BOXと同じように速度色替え

        ' 変更通知
        RaiseEvent tEv_LineSpeed_ValueChange(value)
    End Sub
    ''' <summary>
    ''' 正転、逆転
    ''' </summary>
    Private Sub btnDivSpeedDiv_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnDivSpeedUp.Click, btnDivSpeedDown.Click
        Dim nDiv As Integer = CInt(CType(sender, Control).Tag)

        If 0 < nDiv Then
            Me.DivSpeed = EM_DIV_SPEED.emDivSpeed_Up
        ElseIf 0 > nDiv Then
            Me.DivSpeed = EM_DIV_SPEED.emDivSpeed_Down
        End If
    End Sub

    ''' <summary>
    ''' 速度変更ボタン
    ''' </summary>
    Private Sub btnSpeed_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSpeed0.Click, btnSpeed1.Click, btnSpeed2.Click, btnSpeed3.Click, btnSpeed4.Click, btnSpeed5.Click, btnSpeed6.Click, btnSpeed7.Click, btnSpeed8.Click
        Dim nSpeed As Integer = CInt(CType(sender, Control).Tag)
        If m_emDivSpeed = EM_DIV_SPEED.emDivSpeed_Down Then nSpeed *= -1 ' 逆転

        Me.LineSpeed = nSpeed

        If 0 = nSpeed Then
            RaiseEvent tEv_AutoDownCancel()
        End If

    End Sub
    ''' <summary>
    ''' ライン速度変更
    ''' </summary>
    Private Sub trbSpeed_ValueChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles trbSpeed.ValueChanged
        Me.LineSpeed = trbSpeed.Value
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    ' 自動減速
    Private Structure TYP_ITEM
        Dim sString As String
        Dim nKey As Integer


        Public Overrides Function ToString() As String
            Return String.Format("{0}", sString)
        End Function
    End Structure


    Private m_nAuto(2 - 1) As Integer ' 減速範囲
    Private m_AutoDownMpm As Integer ' 減速速度
    Private m_AutoStopTime As Integer	' 停止時間
    Private m_AutoDownGr As Integer
    Private m_AutoStopGr As Integer

    Public ReadOnly Property AutoS() As Integer
        Get
            Return m_nAuto(0)
        End Get
    End Property
    Public ReadOnly Property AutoE() As Integer
        Get
            Return m_nAuto(1)
        End Get
    End Property
    Public ReadOnly Property AutoDownMpm() As Integer
        Get
            Return m_AutoDownMpm
        End Get
    End Property
    Public ReadOnly Property AutoStopTime() As Integer
        Get
            Return m_AutoStopTime
        End Get
    End Property
    Public ReadOnly Property AutoDownGr() As Integer
        Get
            Return m_AutoDownGr
        End Get
    End Property
    Public ReadOnly Property AutoStopGr() As Integer
        Get
            Return m_AutoStopGr
        End Get
    End Property
    Public ReadOnly Property AutoDownUmu() As Boolean
        Get
            Return chkDownUmu.Checked
        End Get
    End Property


    ''' <summary>
    ''' グレード一覧セット
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetListGr()
        Dim item As TYP_ITEM
        cmbAutoDown.Items.Clear()
        cmbAutoStop.Items.Clear()

        For Each wk As clsParamManager.PARAM_GRADE In mcls_pParam.GetParamGrade()
            item.sString = wk.sName
            item.nKey = wk.nYuusen
            cmbAutoDown.Items.Add(item)
            cmbAutoStop.Items.Add(item)
        Next

        If 0 = mcls_pParam.GetParamGrade().Count Then
            item.sString = "無効"
            item.nKey = 0
            cmbAutoDown.Items.Add(item)
            cmbAutoStop.Items.Add(item)
        End If


        cmbAutoDown.SelectedIndex = 0
        cmbAutoStop.SelectedIndex = 0
        If cmbAutoDown.Items.Count > 1 Then		' デフォルトは無害以上にしておく
            cmbAutoDown.SelectedIndex = cmbAutoDown.Items.Count - 1
            cmbAutoStop.SelectedIndex = cmbAutoDown.Items.Count - 1
        End If
    End Sub


    ''' <summary>
    ''' 自動関係反映
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnDownExec_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAutoExec.Click
        ' 自動減速
        m_nAuto(0) = CInt(CSng(nudAutoS.Value) * 1000)
        m_nAuto(1) = CInt(CSng(nudAutoE.Value) * 1000)
        m_AutoDownMpm = CInt(nudAutoDownMpm.Value)
        m_AutoStopTime = CInt(CSng(nudAutoStopTime.Value) * 1000)

        Dim item As TYP_ITEM
        item = CType(cmbAutoDown.SelectedItem, Global.RPI_VIEWER.frmPopControl.TYP_ITEM)
        m_AutoDownGr = item.nKey
        item = CType(cmbAutoStop.SelectedItem, Global.RPI_VIEWER.frmPopControl.TYP_ITEM)
        m_AutoStopGr = item.nKey

        '接近ランプ
        m_nRanp(0) = CInt(CSng(nudRanpS.Value) * 1000)
        m_nRanp(1) = CInt(CSng(nudRanpE.Value) * 1000)

        RaiseEvent tEv_AutoDownUmu()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    ' 自動ランプ
    Private m_nRanp(2 - 1) As Integer	  ' ランプ範囲

    Public ReadOnly Property RanpS() As Integer
        Get
            Return m_nRanp(0)
        End Get
    End Property
    Public ReadOnly Property RanpE() As Integer
        Get
            Return m_nRanp(1)
        End Get
    End Property

End Class