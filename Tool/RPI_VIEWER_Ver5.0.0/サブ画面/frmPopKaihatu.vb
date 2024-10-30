Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class frmPopKaihatu
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 公開イベント
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public Event tEv_ReCreate()																			' 画像再生成

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_pRpiView As tClass_ViewRpiThread = Nothing				' RPIファイル表示クラス (ポインタ)


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 公開関数
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' RPIクラス
    ''' </summary>
    ''' <param name="cls">RPIクラス</param>
    ''' <remarks></remarks>
    Public Sub SetCls_RpiView(ByRef cls As tClass_ViewRpiThread)
        mcls_pRpiView = cls
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 基本処理
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' フォームロード
    ''' </summary>
    Private Sub frmPop_Control_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        Dim bIsUseImgLibCLI As Boolean = CBool(tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "DRAW_MODE", TASKINI_NAME))
        Dim nQuarity As Integer = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "DRAW_QUARITY", TASKINI_NAME)

        '''' コントロール初期設定
        If 0 = nQuarity Then
            radQuarity1.Checked = True
        Else
            radQuarity2.Checked = True
        End If
        If Not bIsUseImgLibCLI Then
            radDraw1.Checked = True
        Else
            radDraw2.Checked = True
        End If

    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 画質
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' モード変更イベント	' イベントはそれぞれ一個だけ登録でよい。
    ''' </summary>
    Private Sub radQuarity_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles radQuarity1.CheckedChanged, radDraw1.CheckedChanged
        Call SetQuarity()
    End Sub
    ''' <summary>
    ''' モード変更
    ''' </summary>
    ''' <remarks></remarks>
    Sub SetQuarity()
        Dim bIsUseImgLibCLI As Boolean = CBool(IIf(radDraw1.Checked, False, True))
        Dim nQuarity As Integer = CInt(IIf(radQuarity1.Checked, 0, 1))

        '''' ライブラリに設定
        mcls_pRpiView.SetParam_Quarity(nQuarity, bIsUseImgLibCLI)

        '''' 再描画
        RaiseEvent tEv_ReCreate()
    End Sub



End Class