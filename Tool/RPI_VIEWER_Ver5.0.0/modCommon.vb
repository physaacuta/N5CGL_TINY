
Module modCommon

    ' ===>> 坂巻 機密保持 ツール利用制限
    Public Const MC_CHECK_ACCSESS As Boolean = True	' 認証処理有無（TRUE:認証処理あり,FALSE:認証処理なし）
    ' <<===

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 超重要
    Public g_nLockMode As Integer = 0			' 拡張機能挙動 (0:表示禁止 1:サンプル公開 9:全公開)
    Public g_bOutTrk As Boolean					' 外部連動有無 (false:従来=単独動作  true:連動動作 )
    Public g_typStartArgs As START_ARGS			' 起動引数

    Public gcls_Log As tClassLibrary.tClass.tClass_LogManager = Nothing							' ログクラス


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数 構造体定義
    Public Const TASK_NAME As String = "RPI_VIEWER"
    Public Const TASKINI_NAME As String = ".\RPI_VIEWER.ini"
    Public Const LOCALINI_NAME As String = ".\RPI_VIEWER_LOCAL.ini"
    Public Const MC_MAX_IMG As Integer = 4
    Public Const MC_RANP_CNT As Integer = 9			' 疵接近ランプ数
    ' ===>> 坂巻 機密保持 ツール利用制限
    Public Const MC_LINE_CNT As Integer = 100       ' 最大ライン数（ループで使用）
    ' <<===


    ''' <summary>
    ''' 通板方向
    ''' </summary>
    Public Enum EM_DIV_SPEED
        'emDivSpeed_Stop = 0			' 停止
        emDivSpeed_Up = 1			' 正転
        emDivSpeed_Down = -1		' 逆転
    End Enum


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数 構造体定義
    ''' <summary>
    ''' 検査対象Key情報
    ''' </summary>
    Public Structure SELECT_KEY
        Dim sKizukenNo As String				' 疵検管理No
        Dim nMen As Integer						' 現在の表裏状態
    End Structure

    ''' <summary>
    ''' コイル情報
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure DATA_COIL
        Dim sKizukenNo As String				' 疵検管理No
        Dim sCoilNo As String		' コイルNo
        Dim nLen As Integer			' 疵検実測長[mm]
        Dim nWid As Integer			' コイル幅 [mm]
    End Structure
    ''' <summary>
    ''' 個々のカット 構造体
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure DATA_CUT_INF
        ''''' 反転値
        ''' <summary>長手位置[mm]</summary>
        Dim nY As Integer

        ''''' 反転前
        Dim nBaseY As Integer
    End Structure

    ''' <summary>
    ''' 個々の疵情報 構造体
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure DATA_DETECT_INF
        ''' <summary>表示有無</summary>
        Dim bDsp As Boolean


        ''' <summary>疵No</summary>
        Dim nNo As Integer
        ''' <summary>疵種EdasysID</summary>
        Dim tId As Integer
        ''' <summary>グレードEdasysID</summary>
        Dim gId As Integer
        ''' <summary>疵種優先度(1最強)</summary>
        Dim tYu As Integer
        ''' <summary>グレード優先度(1最強)</summary>
        Dim gYu As Integer

        ''' <summary>カメラ番号(左端カメラを0として何番目)</summary>
        Dim nCamPos As Integer
        ''' <summary>フレームNo(コイル先頭を1) (1オリジン)</summary>
        Dim nFrameNo As Integer
        ''' <summary>フレームNo(コイル先頭を1) (1オリジン) の補正前の値</summary>
        Dim nFrameNoBase As Integer


        ''' <summary>フレームNo(オリジナル)</summary>
        Dim nFrameNum As Integer

        ''' <summary>フレーム画像左上原点の外接長方形</summary>
        Dim nFrameXmin As Integer
        Dim nFrameXmax As Integer
        Dim nFrameYmin As Integer
        Dim nFrameYmax As Integer
        Dim nFrameXc As Integer		' C基準(DS:-, WS:+)(散布図描画用)[mm]
        Dim nFrameBaseXc As Integer	' 反転前

        ''' <summary>切り出し画像左上原点の外接長方形</summary>
        Dim nKiriXmin As Integer
        Dim nKiriXmax As Integer
        Dim nKiriYmin As Integer
        Dim nKiriYmax As Integer


        ''''' 反転値
        ''' <summary>長手位置[mm]</summary>
        Dim nY As Integer
        ''' <summary>DS幅位置[mm]</summary>
        Dim nX As Integer
        ''' <summary>WS幅位置[mm]</summary>
        Dim nXws As Integer
        ''' <summary>C基準幅位置[mm](DS:-, WS:+)</summary>
        Dim nXc As Integer

        ''''' 反転前
        Dim nBaseY As Integer
        Dim nBaseX As Integer
        Dim nBaseXws As Integer
        Dim nBaseXc As Integer

    End Structure

    ''' <summary>
    ''' 切り出し画像情報 構造体
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure DETECT_KIRI_DATA
        ''' <summary>基本情報</summary>
        Dim data As DATA_DETECT_INF

        ''' <summary>イメージ</summary>
        Dim imgAngle() As Bitmap
        Dim nImgSize() As Integer				' 画像サイズ
        Dim nImgCnt As Integer					' 画像枚数


        Dim dResX As Double						' 分解能
        Dim dResY As Double						' 
        Dim nImgCenterX As Integer				' 疵中心位置
        Dim nImgCenterY As Integer
        Dim nImgSizeX As Integer				' 画像サイズ
        Dim nImgSizeY As Integer


        ' 初期化
        Public Sub initialize()
            ReDim imgAngle(MC_MAX_IMG - 1)
            ReDim nImgSize(MC_MAX_IMG - 1)
        End Sub
    End Structure


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 内部処理用
    Public Structure TYP_ITEM
        Dim sString As String
        Dim nKey As Integer

        Public Overrides Function ToString() As String
            Return String.Format("{0}	:{1}", nKey, sString)
        End Function
    End Structure

    ''' <summary>
    ''' サブ画面の位置をメインフォームの中央へ
    ''' </summary>
    ''' <param name="myfrm"></param>
    ''' <param name="own"></param>
    ''' <remarks></remarks>
    Public Sub SubFormLocation(ByVal myfrm As Form, ByVal own As Control)
        If own Is Nothing Then Return
        'myfrm.Location = New Point(own.Location.X + (own.Width - myfrm.Width) \ 2, _
        'own.Location.Y + (own.Height - myfrm.Height) \ 2)
        myfrm.Location = New Point(own.Location.X + 10, _
         own.Location.Y + 20)


        '''' ＋サイズ変更 4.5.3から
        Dim sWk As String = tClassLibrary.tMod.ReadIniStr("FORM_SIZE", myfrm.Name, TASKINI_NAME)
        If 0 <> sWk.Length Then
            Dim sSize() As String = sWk.Split(CChar(","))
            Dim nSizeW As Integer = CInt(sSize(0))
            Dim nSizeH As Integer = CInt(sSize(1))

            ' 元サイズを取得
            Dim size As Size = myfrm.Size
            ' iniファイル分を反映
            size.Width = CInt(IIf(0 < nSizeW, nSizeW, size.Width))
            size.Height = CInt(IIf(0 < nSizeH, nSizeH, size.Height))
            ' 再セット
            myfrm.Size = size
        End If
    End Sub
    ''' <summary>
    ''' 疵検管理Noからフォルダ名を取得する
    ''' </summary>
    ''' <param name="sKizukenNo">疵検管理No文字列</param>
    ''' <returns>フォルダ名</returns>
    ''' <remarks></remarks>
    Public Function GetFolderName(ByRef sKizukenNo As String) As String
        Dim sFileName As String = sKizukenNo.Split(" "c)(0)
        ' 先頭から一つ目のスペースまでの文字数が"疵検ID+日付文字列（YYYYMMDD）"の10文字であれば疵検ID分の文字数2文字を除外する
        If sFileName.Length = 10 Then sFileName = sFileName.Remove(0, 2)
        Return sFileName
    End Function

    Public ReadOnly m_reParen As New System.Text.RegularExpressions.Regex("\{[^0-9].*?\}")
 
End Module
