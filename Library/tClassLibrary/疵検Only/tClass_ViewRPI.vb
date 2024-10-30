'*********************************************************************************
'RPI画像表示に特化した基底クラス
'	[Ver]
'		Ver.01    2009/02/20  vs2005 対応
'
'	[メモ]
'		・横1フレーム 縦?フレーム 形式でPixel単位でずらして表示させる専用クラス
'		・基本的にこのクラスの中で描画はしない。(= 位置変更とかされても使うほうで描画をコールする事。それが繰り返し描画の減少へとつながる)
'*********************************************************************************
Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Imports System.Windows.Forms
Imports System.Drawing

Imports tClassLibrary


Namespace tClass
    Public Class tClass_ViewRPI
        Implements IDisposable															' デストラクタ定義


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 外部イベント
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' 生画像 に対する画像処理要求 (自クラス内で生成)
        ''' </summary>
        ''' <param name="fno">今回対象のフレームNo(1オリジン)</param>
        ''' <param name="img">(in/out)修正対象の画像クラス</param>
        ''' <remarks></remarks>
        Public Event tEv_CreatingImg(ByVal fno As Integer, ByRef img As Bitmap)

        ''' <summary>
        ''' 表示先頭フレームNoが変更、又は、表示範囲が変更 (自クラス内で生成)
        ''' </summary>
        ''' <param name="nFrameNoStart">今回の表示先頭フレームNo(1オリジン)</param>
        ''' <param name="nFrameNoEnd">今回の表示後端フレームNo(1オリジン)</param>
        ''' <remarks></remarks>
        Public Event tEv_ChangeDspFrameNo(ByVal nFrameNoStart As Integer, ByVal nFrameNoEnd As Integer)

        ''' <summary>
        ''' ユーザー初期描画 (tClass.tClass_ImageDspBaseからの受け渡し)
        ''' </summary>
        ''' <param name="e">グラフィックオブジェクト</param>
        ''' <remarks></remarks>
        Public Event tEv_Paint_Start(ByVal e As System.Windows.Forms.PaintEventArgs)
        ''' <summary>
        ''' ユーザー最終描画 (tClass.tClass_ImageDspBaseからの受け渡し)
        ''' </summary>
        ''' <param name="e">グラフィックオブジェクト</param>
        ''' <remarks></remarks>
        Public Event tEv_Paint_End(ByVal e As System.Windows.Forms.PaintEventArgs)

        ''' <summary>
        ''' オーバーレイ内クリック (tClass.tClass_ImageDspからの受け渡し)
        ''' </summary>
        ''' <param name="sender">オーバーレイ情報</param>
        ''' <param name="e">表示されているオーバーレイ位置情報</param>
        ''' <remarks></remarks>
        Public Event tEv_OverLay_Click(ByVal sender As tClass_ImageDsp.OVERLAY_INF, ByVal e As Rectangle)


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ プライベート定数
        '/ /////////////////////////////////////////////////////////////////////////////////
        Private Const mcIMGBUF_SP As Integer = 2										' パフォーマンス対策用の内部画像保持数初期値

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 公開構造体
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' 使用コントロール 受け渡し構造体
        ''' </summary>
        ''' <remarks></remarks>
        Public Structure CNTL_ITEM
            ''' <summary>画像表示ピクチャー</summary>
            Dim picDspImg As PictureBox													' 画像表示ピクチャー
            ''' <summary>縦スクロールバー (NULL許可)</summary>
            Dim vsBar As tCnt.tCnt_VScrollBar											' 縦スクロールバー (NULL許可)
            ''' <summary>横スクロールバー (NULL許可)</summary>
            Dim hsBar As tCnt.tCnt_HScrollBar											' 横スクロールバー (NULL許可)
        End Structure

        ''' <summary>
        ''' 画像表示情報
        ''' </summary>
        ''' <remarks></remarks>
        Public Structure DSP_ARIA_INF
            Dim DspPixelTotalOffset As Point											' 表示座標系での表示左上までのトータルオフセットpixel値
            Dim nDspFno As Integer														' 表示先頭のフレームNo (0オリジン)
            Dim nDspFrameCnt As Integer													' 画面に何フレーム表示可能か？(切り上げ)

            Dim ImgNonAriaSize As Size													' 表示エリアに表示されていない部分の画像サイズ (表示倍率に変換した後のPixel数) = スクロールバーの量となる

            Dim nImgBufFno As Integer													' バッファ内の先頭フレームNo (0オリジン)
            Dim ImgBuf() As Bitmap														' パフォーマンス対策用の内部画像 (シェーディング補正等の変換後の画像)
        End Structure



        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ メンバー変数
        '/ /////////////////////////////////////////////////////////////////////////////////
        ' 外部セットクラス
        Private mcls_pLog As tClass.tClass_LogManager									' ログ管理クラス (ポインタ)

        ' 外部セットオブジェクト
        Private mcntl_Item As CNTL_ITEM													' 外部セットオブジェクト

        ' 自生成クラス
        Private mcls_Rpi As New tClass.tClass_ReadRPIFile									' RPIファイル読込クラス
        Private mcls_Img As New tClass.tClass_ImageDsp									' 画像表示クラス 

        ' 内部状態管理
        'Private m_bIsPaint As Boolean = False											' Paint描画可能か
        Private m_bIsRpi As Boolean = False												' ファイルアクセス準備完了か
        Private m_DspAriaInf As DSP_ARIA_INF											' 画像表示情報
        Private m_nImgBufSp(1) As Integer												' パフォーマンス対策用の内部画像保持数(0:前 1:後) (この数+表示可能なフレーム数 が 合計となる)


        ' その他もろもろ


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 内部クラス の 情報公開用 関係
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' RPIヘッダ取得
        ''' </summary>
        Public ReadOnly Property GetRpi_Header() As tClass.tClass_ReadRPI.RPI_HEADER
            Get
                Return mcls_Rpi.GetRpiHeader
            End Get
        End Property
        ''' <summary>
        ''' RPIフレームアクセス
        ''' </summary>
        ''' <param name="fno">フレームNo (1オリジン)</param>
        ''' <returns>RPIフレーム情報</returns>
        Public ReadOnly Property GetRpi_FrameInf(ByVal fno As Integer) As tClass.tClass_ReadRPI.RPI_FRAME_INF
            Get
                Return mcls_Rpi.GetRpiFrameInf(fno)
            End Get
        End Property

        ''' <summary>
        ''' 画像情報情報取得
        ''' </summary>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public ReadOnly Property GetImg_ImgInf() As tClass.tClass_ImageDsp.IMG_INF
            Get
                Return mcls_Img.tProp_ImgInf
            End Get
        End Property
        ''' <summary>
        ''' 表示形式情報取得
        ''' </summary>
        Public ReadOnly Property GetImg_ImgKind() As tClass.tClass_ImageDsp.IMG_KIND
            Get
                Return mcls_Img.tProp_ImgKind
            End Get
        End Property
        ''' <summary>
        ''' 表示領域情報取得
        ''' </summary>
        Public ReadOnly Property GetImg_AriaInf() As tClass.tClass_ImageDsp.ARIA_INF
            Get
                Return mcls_Img.tProp_AriaInf
            End Get
        End Property
        ''' <summary>
        ''' 座標最終結果取得
        ''' </summary>
        Public ReadOnly Property GetImg_CalcAns() As tClass.tClass_ImageDsp.CALC_ANS
            Get
                Return mcls_Img.tProp_CalcAns
            End Get
        End Property




        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ メンバークラスへの情報セット
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' オーバーレイ情報登録
        ''' </summary>
        ''' <param name="ovl">オーバーレイ情報</param>
        ''' <remarks></remarks>
        Public Sub SetOverLayInf(ByVal ovl As tClass_ImageDsp.OVERLAY_INF)
            Call mcls_Img.SetOverLayInf(ovl)
        End Sub
        ''' <summary>
        ''' オーバーレイ情報開放
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ResetOverLayInf()
            Call mcls_Img.ResetOverLayInf()
        End Sub

        ''' <summary>
        ''' 画面左側のDS/WS区分
        ''' </summary>
        ''' <param name="nMode">DS:0 WS:1</param>
        ''' <remarks></remarks>
        Public Sub SetProp_DSWS(ByVal nMode As Integer)
            'm_nDsWs = nMode

            ' デフォルトでは、画面左側は、DS
            If 0 = nMode Then mcls_Img.tProp_ImgKind_IsTurnLR = False
            If 1 = nMode Then mcls_Img.tProp_ImgKind_IsTurnLR = True
        End Sub


        ''' <summary>
        ''' 画像拡大
        ''' </summary>
        ''' <param name="dRate">拡大倍率</param>
        ''' <remarks></remarks>
        Public Sub SetProp_Zoom(ByVal dRate As Double)
            mcls_Img.tProp_AriaInf_Zoom = New tClass.tClass_ImageDsp_Calc.RATE(dRate, dRate)
        End Sub

        ''' <summary>
        ''' 画像拡大
        ''' </summary>
        ''' <param name="dRateX">拡大倍率(x)</param>
        ''' <param name="dRateY">拡大倍率(x)</param>
        ''' <remarks></remarks>
        Public Sub SetProp_Zoom(ByVal dRateX As Double, ByVal dRateY As Double)
            mcls_Img.tProp_AriaInf_Zoom = New tClass.tClass_ImageDsp_Calc.RATE(dRateX, dRateY)
        End Sub

        ''' <summary>
        ''' 画像表示状態
        ''' </summary>
        Public Sub SetProp_ImgKind_StrechType(Optional ByVal em As tClass.tClass_ImageDsp_Calc.EM_IMG_STRECH = tClass.tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechWidth)
            mcls_Img.tProp_ImgKind_StrechType = em
        End Sub



        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ プロパティ関係
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' 画像表示OK
        ''' </summary>
        Public ReadOnly Property IsRpi() As Boolean
            Get
                Return m_bIsRpi
            End Get
        End Property
        ''' <summary>
        ''' 画像表示情報
        ''' </summary>
        Public ReadOnly Property GetDspAriaInf() As DSP_ARIA_INF
            Get
                Return m_DspAriaInf
            End Get
        End Property
        ''' <summary>
        ''' パフォーマンス対策用の内部画像保持数(0:前 1:後)
        ''' </summary>
        Public Property PropImgBufSp(ByVal Index As Integer) As Integer
            Get
                Return m_nImgBufSp(Index)
            End Get
            Set(ByVal value As Integer)
                m_nImgBufSp(Index) = value
                ' △
                ' いきなり変えていい？
            End Set
        End Property

        ''' <summary>
        ''' フレーム付加情報取得 (フレーム単位のエッジ位置)
        ''' </summary>
        ''' <param name="fno">フレームNo (1オリジン)</param>
        ''' <returns>フレーム単位のエッジ位置</returns>
        ''' <remarks></remarks>
        Public Function ReadFile_FrameInfo_EdgePerFrame(ByVal fno As Integer) As tClass_ReadRPI.EDGE_PER_FRAME
            Return mcls_Rpi.ReadFile_FrameInfo_EdgePerFrame(fno)
        End Function

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 共通公開処理関係
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' 表示座標系の全体先頭オフセット量から該当するフレームNo、フレーム先頭からのオフセットを算出
        ''' </summary>
        ''' <param name="nTotalOffsetY">0フレーム先頭からのオフセット量(表示座標系)</param>
        ''' <param name="nFrameNo">算出したフレームNo(0オリジン)</param>
        ''' <param name="nOneOffsetY">算出したオフセット値</param>
        ''' <remarks></remarks>
        Public Sub GetOffset_TotalToOne(ByVal nTotalOffsetY As Integer, ByRef nFrameNo As Integer, ByRef nOneOffsetY As Integer)
            nFrameNo = GetOffset_TotalToOne_FrameNo(nTotalOffsetY)
            nOneOffsetY = GetOffset_TotalToOne_OffsetY(nTotalOffsetY) 'nTotalOffsetY - CInt(Math.Floor(nFrameNo * OneImgPixel))
        End Sub
        Public Function GetOffset_TotalToOne_FrameNo(ByVal nTotalOffsetY As Integer) As Integer
            Dim OneImgPixel As Double = mcls_Img.tProp_ImgInf.nOneImgH * mcls_Img.tProp_CalcAns.RateDsp.dH
            Return CInt(Math.Floor(nTotalOffsetY / OneImgPixel))
        End Function
        Public Function GetOffset_TotalToOne_OffsetY(ByVal nTotalOffsetY As Integer) As Integer
            Dim OneImgPixel As Double = mcls_Img.tProp_ImgInf.nOneImgH * mcls_Img.tProp_CalcAns.RateDsp.dH
            Return CInt(Math.Floor(nTotalOffsetY Mod OneImgPixel))
        End Function

        ''' <summary>
        ''' 該当フレームNo、フレーム先頭からのオフセットから表示座標先頭から全体のオフセット量を算出
        ''' </summary>
        ''' <param name="nFrameNo">指定したフレームNo(0オリジン)</param>
        ''' <param name="nOneOffsetY">そのフレーム内でのオフセット量</param>
        ''' <returns>表示座標系 先頭からのトータルオフセット量</returns>
        ''' <remarks></remarks>
        Public Function GetOffsetY_OneToTotal(ByVal nFrameNo As Integer, ByVal nOneOffsetY As Integer) As Integer
            Return CInt(Math.Floor(nFrameNo * mcls_Rpi.GetRpiHeader().height * mcls_Img.tProp_CalcAns.RateDsp.dH)) + nOneOffsetY
        End Function
        Public Function GetOffsetX_OneToTotal(ByVal nOneOffsetX As Integer) As Integer
            Return CInt(Math.Floor(mcls_Rpi.GetRpiHeader().width * mcls_Img.tProp_CalcAns.RateDsp.dW)) + nOneOffsetX
        End Function

        ''' <summary>
        ''' 元の画像座標から表示用の座標に変換
        ''' </summary>
        Public Overloads Function GetSimplePoint_ImgToDsp(ByVal x As Integer, ByVal y As Integer) As Point
            Return mcls_Img.GetSimplePoint_ImgToDsp(x, y)
        End Function

        ''' <summary>
        ''' 表示用の座標から元の画像座標に変換
        ''' </summary>
        Public Overloads Function GetSimplePoint_DspToImg(ByVal x As Integer, ByVal y As Integer) As Point
            Return mcls_Img.GetSimplePoint_DspToImg(x, y)
        End Function


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 初期セット
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' ログクラスセット
        ''' </summary>
        Public Sub SetCls_Log(ByRef cls As tClass.tClass_LogManager)
            mcls_pLog = cls

            ' 配下のクラスにもセット
        End Sub

        ''' <summary>
        ''' 疵無し画像をセット
        ''' </summary>
        ''' <param name="DefImg">疵無し画像</param>
        Public Sub SetItem_DefImg(ByVal DefImg As Bitmap)
            Call mcls_Img.SetDefImage(CType(DefImg, Image))
        End Sub


        ''' <summary>
        ''' 初期セット (Set～が終わった後で、生成時に一回のみ)
        ''' </summary>
        ''' <param name="cntl">本クラスで使用するコントロール群</param>
        ''' <remarks></remarks>
        Public Sub Alloc(ByRef cntl As CNTL_ITEM)
            '''' コントロールを登録
            mcntl_Item = cntl


            ''''' 画像表示クラス の 準備 2
            ' 描画コントロールセット
            Call mcls_Img.AddHandler_PaintEvent(mcntl_Item.picDspImg)							' ペイントイベント登録 & ピクチャー登録
            Call mcls_Img.AddHandler_OverLayClick()												' ピクチャークリックでオーバーレイをクリックしたか判断 登録


            '''' 内部イベントと紐付け
            ' コントロールのイベントと紐付け
            AddHandler mcntl_Item.picDspImg.Resize, AddressOf ResizePaintAria					' ペイントサイズ変更イベント登録
            ' 内部クラスとのイベント紐付け
            AddHandler mcls_Img.tEv_Paint_Start, AddressOf Ev_Paint_Start						' フレーム画像に独自描画
            AddHandler mcls_Img.tEv_Paint_End, AddressOf Ev_Paint_End							' フレーム画像に独自描画
            AddHandler mcls_Img.tEv_OverLay_Click, AddressOf Ev_OverLay_Click					' オーバーレイクリック


            '''' 自クラス初期化
            Call Init(True)

            '''' サイズ変更処理でデータを登録
            Call ResizePaintAria(Nothing, Nothing)

            '''' 再描画
            Call RePaint()
        End Sub

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 初期化関連
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        Public Sub New()
            '''' もろもろ初期化
            m_nImgBufSp(0) = mcIMGBUF_SP
            m_nImgBufSp(1) = mcIMGBUF_SP

            '''' 画像表示クラス の 準備 1
            mcls_Img.tProp_ImgKind_StrechType = tClass.tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechWidth	' 表示エリアの幅に画像幅を合わせる
            mcls_Img.tProp_AriaInf_IsCenter = False												' 疵中心位置移動無し
            mcls_Img.SetImgInf(1024, 1024, 1, 1, 0, 0)											' とりあえず初期情報としてセット
            mcls_Img.tProp_Paint_IsDefImgDspn = True											' デフォルト画像表示にON
            mcls_Img.tProp_Paint_IsPaint = True													' 画像表示をON

            ' とりあえず画像データの器を作っておく (違ったら画像表示時にリサイズ)
            m_DspAriaInf.nImgBufFno = -1
            ReDim m_DspAriaInf.ImgBuf(0)
        End Sub

        ''' <summary>
        ''' デストラクタ
        ''' </summary>
#Region " IDisposable Support "
        Private disposedValue As Boolean = False		' 重複する呼び出しを検出するには

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: 明示的に呼び出されたときにマネージ リソースを解放します
                End If

                ' TODO: 共有のアンマネージ リソースを解放します

                '''' クラス解放
                mcls_Rpi.RpiClose()
                mcls_Rpi.Dispose()
                mcls_Rpi = Nothing

                Call mcls_Img.AddHandler_OverLayClick()		' 動的ハンドル開放
                mcls_Img.Dispose()
                mcls_Img = Nothing

                ' 内部画像バッファクリア
                Dispose_ImgBuf()
            End If
            Me.disposedValue = True
        End Sub


        ' このコードは、破棄可能なパターンを正しく実装できるように Visual Basic によって追加されました。
        Public Sub Dispose() Implements IDisposable.Dispose
            ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region


        ''' <summary>
        ''' 自クラス初期化
        ''' </summary>
        ''' <param name="bInit">完全初期化時true   角度切替など現位置情報を保持したい場合false</param>
        Public Sub Init(ByVal bInit As Boolean)
            '''' RPIファイルクローズ
            Call RpiClose()
            ' 画像表示エリア
            mcls_Img.tProp_Paint_IsDefImgDspn = True											' デフォルト画像表示にON

            '''' 内部変数開放
            m_bIsRpi = False

            '''' 画像表示情報初期化
            If bInit Then
                m_DspAriaInf.DspPixelTotalOffset.X = 0
                m_DspAriaInf.DspPixelTotalOffset.Y = 0
                m_DspAriaInf.nDspFno = -1
                m_DspAriaInf.nDspFrameCnt = 0
                m_DspAriaInf.ImgNonAriaSize.Height = 0
                m_DspAriaInf.ImgNonAriaSize.Width = 0
            End If
            ' 画像バッファクリア
            m_DspAriaInf.nImgBufFno = -1
            Call Dispose_ImgBuf()


            '''' コントロール初期セット
            If Not mcntl_Item.vsBar Is Nothing Then
                mcntl_Item.vsBar.Enabled = False
            End If
            If Not mcntl_Item.hsBar Is Nothing Then
                mcntl_Item.hsBar.Enabled = False
            End If
        End Sub

        ''' <summary>
        ''' 内部画像バッファクリア
        ''' </summary>
        Private Sub Dispose_ImgBuf()
            For ii As Integer = 0 To m_DspAriaInf.ImgBuf.Length - 1
                Call Dispose_ImgBuf(ii)
            Next ii
        End Sub
        Private Sub Dispose_ImgBuf(ByVal Index As Integer)
            If m_DspAriaInf.ImgBuf.Length - 1 < Index Then Return
            If m_DspAriaInf.ImgBuf(Index) Is Nothing Then Return

            ' 開放
            m_DspAriaInf.ImgBuf(Index).Dispose()
            m_DspAriaInf.ImgBuf(Index) = Nothing
        End Sub



        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 公開 基本画像操作処理
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' RPIファイル アクセス開始
        ''' </summary>
        ''' <param name="strFPath">RPIファイルのフルパス</param>
        ''' <param name="bInit">完全初期化時true   角度切替など現位置情報を保持したい場合false</param>
        ''' <param name="dResX">横分解能 (0の場合は、RPIファイルを使用)</param>
        ''' <param name="dResY">縦分解能 (0の場合は、RPIファイルを使用)</param>
        ''' <returns></returns>
        Public Function RpiOpen(ByVal strFPath As String, Optional ByVal bInit As Boolean = True, Optional ByVal dResX As Single = 0, Optional ByVal dResY As Single = 0) As Boolean

            '''' RPIファイルの事前確認
            ' RPIファイルの存在確認
            Try
                Dim iof As New IO.FileInfo(strFPath)
                If Not iof.Exists() Then Return False
            Catch ex As Exception
                ' 引数のファイル名がNULLだった場合は、エラーとなる為
                Return False
            End Try

            '' RPI仮クラスでファイル読み込み可能かチェック
            ' 失敗した場合は、今開いているRPIをそのまま保持したい為
            Using wk As New tClass.tClass_ReadRPIFile
                If 0 >= wk.RpiOpen(strFPath) Then
                    Return False
                End If
            End Using


            '''' 正式にRPIファイル読込処理開始
            ' 一度 初期化
            Call Init(bInit)

            ' ファイルオープン
            If 0 >= mcls_Rpi.RpiOpen(strFPath) Then
                Call RpiClose()
                Return False
            End If

            ' フラグの変更
            m_bIsRpi = True
            mcls_Img.tProp_Paint_IsDefImgDspn = False											' デフォルト画像表示にOFF


            '''' 初期計算
            ' 画像表示クラス
            Dim rpiHead As tClass.tClass_ReadRPI.RPI_HEADER = mcls_Rpi.GetRpiHeader()
            ' 分解能外部指定
            dResX = CSng(IIf(dResX > 0, dResX, rpiHead.res_x))
            dResY = CSng(IIf(dResY > 0, dResY, rpiHead.res_y))
            Call mcls_Img.SetImgInf(rpiHead.width, rpiHead.height, dResX, dResY, 0, 0, 1, 1)   ' 画像表示枚数は仮でセット
            ' リサイズの処理 (分解能や画像枚数などでスクロール可能量などを計算)
            Call ResizePaintAria()
            ' 変更後のフレーム数を設定
            Call mcls_Img.ChangeImgCnt(1, m_DspAriaInf.nDspFrameCnt)
            ' 初期移動
            Call SetCommonScrollBar(bInit)								' スクロールバー量 定義変更
            Call SetNewImgPosition(m_DspAriaInf.DspPixelTotalOffset.Y, m_DspAriaInf.DspPixelTotalOffset.X, True, True)

            Return True
        End Function

        ''' <summary>
        ''' RPIファイル クローズ
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub RpiClose()
            '' フラグ変更
            m_bIsRpi = False
            '' クローズ処理
            Call mcls_Rpi.RpiClose()
        End Sub

        ''' <summary>
        ''' 再描画要求
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub RePaint()
            mcls_Img.Refresh()
        End Sub

        ''' <summary>
        ''' 縦スクロール
        ''' </summary>
        ''' <param name="nTotalOffsetY">全体先頭からのオフセット量</param>
        ''' <param name="bIsProgramable">プログラムで勝手に変えた場合true</param>
        ''' <remarks></remarks>
        Public Sub ScrollH(ByVal nTotalOffsetY As Integer, Optional ByVal bIsProgramable As Boolean = False)
            Call SetNewImgPosition(nTotalOffsetY, m_DspAriaInf.DspPixelTotalOffset.X, bIsProgramable)
        End Sub

        ''' <summary>
        ''' 横スクロール
        ''' </summary>
        ''' <param name="nTotalOffsetX">左端からのオフセット量</param>
        ''' <param name="bIsProgramable">プログラムで勝手に変えた場合true</param>
        ''' <remarks></remarks>
        Public Sub ScrollW(ByVal nTotalOffsetX As Integer, Optional ByVal bIsProgramable As Boolean = False)
            Call SetNewImgPosition(m_DspAriaInf.DspPixelTotalOffset.Y, nTotalOffsetX, bIsProgramable)
        End Sub

        ''' <summary>
        ''' イメージ再作り直し
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ReCreateImg()
            Call SetNewImgPosition(m_DspAriaInf.DspPixelTotalOffset.Y, m_DspAriaInf.DspPixelTotalOffset.X, False, True)
        End Sub

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 基本コントロール操作系
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' フレーム画像を内部バッファに取り込み
        ''' </summary>
        ''' <param name="nTotalOffsetY">0フレーム先頭からのオフセット量(表示座標系)</param>
        ''' <param name="bIsClearBuffer">今のバッファを使いまわす場合false</param>
        ''' <remarks></remarks>
        Private Sub CreateImgBuffer(ByVal nTotalOffsetY As Integer, Optional ByVal bIsClearBuffer As Boolean = False)
            Dim ii As Integer
            Dim nRecWk As Integer
            Dim imgwk As Bitmap

            Dim nOldImbBufFno(1) As Integer				' 旧内部バッファのIndex
            Dim nNewImbDspFno(1) As Integer				' 今回の表示エリアのIndex
            nOldImbBufFno(0) = m_DspAriaInf.nImgBufFno
            nOldImbBufFno(1) = nOldImbBufFno(0) + m_DspAriaInf.ImgBuf.Length - 1
            nNewImbDspFno(0) = GetOffset_TotalToOne_FrameNo(nTotalOffsetY)
            nNewImbDspFno(1) = nNewImbDspFno(0) + m_DspAriaInf.nDspFrameCnt - 1


            '''' 今回の表示画像は、内部バッファの画像のみで表示可能？
            If Not bIsClearBuffer Then
                If nOldImbBufFno(0) <= nNewImbDspFno(0) And nOldImbBufFno(0) <= nNewImbDspFno(1) And _
                   nOldImbBufFno(1) >= nNewImbDspFno(0) And nOldImbBufFno(1) >= nNewImbDspFno(1) Then
                    ' 全画像データあり？
                    '   通常ありえないはず。と思ったけど、下の範囲を求める時に、初期レコードの場合とかだと、最後の配列が画像無しとかありえる。
                    For ii = nNewImbDspFno(0) To nNewImbDspFno(1)
                        nRecWk = ii - nOldImbBufFno(0)
                        If m_DspAriaInf.ImgBuf(nRecWk) Is Nothing Then
                            Exit For
                        End If
                    Next ii

                    ' 内部バッファの画像のみで表示可能な為、何もしない
                    If ii = nNewImbDspFno(1) + 1 Then Return
                End If
            Else
                ' 今のバッファは強制クリア
                Call Dispose_ImgBuf()
            End If


            '''' 内部バッファ生成
            Dim OldImgBuf() As Bitmap = m_DspAriaInf.ImgBuf				' 旧バッファを格納
            Dim bOldImgBufRepeat(OldImgBuf.Length - 1) As Boolean		' 旧バッファの画像を使いまわす場合 true
            Dim NewImgBuf(m_nImgBufSp(0) + m_nImgBufSp(1) + m_DspAriaInf.nDspFrameCnt - 1) As Bitmap	' 新バッファエリアを確保
            Dim nNewImbBufFno(1) As Integer								' 今回の内部バッファのIndex
            nNewImbBufFno(0) = nNewImbDspFno(0) - m_nImgBufSp(0)
            nNewImbBufFno(1) = nNewImbDspFno(1) + m_nImgBufSp(1)
            If nNewImbBufFno(0) < 0 Then nNewImbBufFno(0) = 0
            If nNewImbBufFno(1) > mcls_Rpi.GetRpiHeader.nframe - 1 Then nNewImbBufFno(1) = mcls_Rpi.GetRpiHeader.nframe - 1


            '''' 画像読込
            Dim nNewBufIndex As Integer = 0
            For ii = nNewImbBufFno(0) To nNewImbBufFno(1)

                ' 旧バッファの配列値 算出 (-1=旧バッファ無いには対象画像無し)
                nRecWk = ii - nOldImbBufFno(0)
                If nRecWk < 0 Then nRecWk = -1
                If nRecWk > OldImgBuf.Length - 1 Then nRecWk = -1

                ' 旧バッファに対象フレームの画像有り？
                If nRecWk <> -1 AndAlso Not OldImgBuf(nRecWk) Is Nothing Then
                    ' 使いまわし
                    NewImgBuf(nNewBufIndex) = OldImgBuf(nRecWk)
                    bOldImgBufRepeat(nRecWk) = True

                Else

                    ' フレームデータ無しの部分
                    If ii > mcls_Rpi.GetRpiHeader.nframe Or ii < 0 Then
                        ' ダミーフレーム生成
                        NewImgBuf(nNewBufIndex) = New Bitmap(mcls_Rpi.GetRpiHeader.width, mcls_Rpi.GetRpiHeader.height)	' 尾端のちょっとの画像は、ダミーをセットしておく
                    Else
                        ' 新規読込
                        imgwk = mcls_Rpi.ReadFile_Img(ii + 1)			' RPIファイルから画像取り込み
                        RaiseEvent tEv_CreatingImg(ii + 1, imgwk)		' 外部に画像処理を依頼
                        NewImgBuf(nNewBufIndex) = imgwk					' ワークにセット
                    End If
                End If


                nNewBufIndex += 1
            Next ii


            '''' 旧バッファで使われなかった画像を開放
            For ii = 0 To m_DspAriaInf.ImgBuf.Length - 1
                If Not bOldImgBufRepeat(ii) Then Call Dispose_ImgBuf(ii)
                m_DspAriaInf.ImgBuf(ii) = Nothing
            Next ii


            '''' 新領域を確保して、画像をセット
            m_DspAriaInf.nImgBufFno = nNewImbBufFno(0)
            m_DspAriaInf.ImgBuf = NewImgBuf
        End Sub



        ''' <summary>
        ''' フレーム画像表示
        ''' </summary>
        ''' <param name="nTotalOffsetY">0フレーム先頭からのオフセット量(表示座標系)</param>
        ''' <param name="nTotalOffsetX">左端からのオフセット量(表示座標系)</param>
        ''' <param name="bIsProgramable">プログラムで勝手に変えた場合true</param>
        ''' <param name="bIsClearBuffer">今のバッファを使いまわす場合false</param>
        ''' <remarks></remarks>
        Private Sub SetNewImgPosition(ByVal nTotalOffsetY As Integer, Optional ByVal nTotalOffsetX As Integer = 0, Optional ByVal bIsProgramable As Boolean = False, Optional ByVal bIsClearBuffer As Boolean = False)
            Dim nNewDspFno As Integer			' 指定位置に対する 表示先頭フレームNo
            Dim nNewOneOffsetY As Integer		' 指定位置に対する 表示先頭フレームからのオフセット位置
            Call GetOffset_TotalToOne(nTotalOffsetY, nNewDspFno, nNewOneOffsetY)

            Try
                '''' 内部バッファ画像生成
                Call CreateImgBuffer(nTotalOffsetY, bIsClearBuffer)

                '''' 内部バッファ変数を更新
                mcls_Img.tProp_AriaInf_Offset = New Point(nTotalOffsetX * -1, nNewOneOffsetY * -1)	' 表示先頭フレームからのオフセット値をセット
                m_DspAriaInf.DspPixelTotalOffset.Y = nTotalOffsetY		' 先頭からのオフセット値
                m_DspAriaInf.DspPixelTotalOffset.X = nTotalOffsetX


                '''' 表示する対象の画像が変更された場合のみ
                If m_DspAriaInf.nDspFno <> nNewDspFno Or bIsClearBuffer Then

                    '' 表示する画像のみ取り出し
                    Dim DspImg(m_DspAriaInf.nDspFrameCnt - 1) As Bitmap
                    Dim nImgBufIndex As Integer = nNewDspFno - m_DspAriaInf.nImgBufFno 'GetOffset_TotalToOne_FrameNo(nTotalOffsetY) - m_DspAriaInf.nImgBufFno
                    For ii As Integer = 0 To DspImg.Length - 1
                        DspImg(ii) = m_DspAriaInf.ImgBuf(nImgBufIndex + ii)
                    Next ii


                    '' 画像表示クラスにフレームデータを渡す
                    Call mcls_Img.SetImage(CType(DspImg, Image()))

                    '' 表示先頭フレームが変更
                    m_DspAriaInf.nDspFno = nNewDspFno

                    '' 範囲が変更された
                    RaiseEvent tEv_ChangeDspFrameNo(nNewDspFno + 1, nNewDspFno + m_DspAriaInf.nDspFrameCnt)
                End If

                '' スクロールバー反映
                If bIsProgramable Then
                    If Not mcntl_Item.vsBar Is Nothing Then mcntl_Item.vsBar.ValueProgramable = nTotalOffsetY
                    If Not mcntl_Item.hsBar Is Nothing Then mcntl_Item.hsBar.ValueProgramable = nTotalOffsetX
                End If

            Catch ex As Exception
                'Call Paint_Stop()
                mcls_pLog.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("fno={0} offset={1} 表示異常", nNewDspFno, nTotalOffsetY))
            End Try

            Return
        End Sub

        ''' <summary>
        ''' フレーム画像表示ピクチャーボックスサイズ変更時の処理
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub ResizePaintAria()
            Try
                '' フレーム画像表示 サイズ登録
                'mcls_Img.tProp_AriaInf_Aria = mcntl_Item.picDspImg.ClientSize
                mcls_Img.SetAriaInf()
                mcls_Img.Calculation()			' 変更したサイズで再計算


                '' 非表示エリアサイズを算出
                ' 縦
                Dim nTotalPixelY As Integer = GetOffsetY_OneToTotal(mcls_Rpi.GetRpiHeader.nframe, 0)  'CInt(Math.Floor(mcls_Rpi.GetRpiHeader.nframe * mcls_Rpi.GetRpiHeader().height * mcls_Img.tProp_CalcAns.RateDsp.dH))
                m_DspAriaInf.ImgNonAriaSize.Height = nTotalPixelY - mcls_Img.tProp_AriaInf_Aria.Height
                If 0 > m_DspAriaInf.ImgNonAriaSize.Height Then m_DspAriaInf.ImgNonAriaSize.Height = 0
                ' 横
                Dim nTotalPixelX As Integer = GetOffsetX_OneToTotal(0)	'CInt(Math.Floor(mcls_Rpi.GetRpiHeader().width * mcls_Img.tProp_CalcAns.RateDsp.dW))
                m_DspAriaInf.ImgNonAriaSize.Width = nTotalPixelX - mcls_Img.tProp_AriaInf_Aria.Width
                If 0 > m_DspAriaInf.ImgNonAriaSize.Width Then m_DspAriaInf.ImgNonAriaSize.Width = 0


                '' 変更サイズ後で縦に何枚フレーム画像が表示できる？ (切り上げ)
                ' +1 するのがミソ。画素単位で移動させると、上下でフレーム途中の画像を表示する必要がある場合が出てくるため
                m_DspAriaInf.nDspFrameCnt = CInt(Math.Ceiling(mcls_Img.tProp_AriaInf.nAriaHeight / (mcls_Img.tProp_ImgInf.nOneImgH * mcls_Img.tProp_CalcAns.RateDsp.dH))) + 1
            Catch ex As Exception
            End Try
        End Sub

        ''' <summary>
        ''' スクロールバー 最大サイズ変更
        ''' </summary>
        ''' <param name="bInit">完全初期化時true</param>
        ''' <remarks></remarks>
        Private Sub SetCommonScrollBar(ByVal bInit As Boolean)
            ' 縦スクロールバー 有効化
            If Not mcntl_Item.vsBar Is Nothing Then
                If 0 >= m_DspAriaInf.ImgNonAriaSize.Height Then
                    mcntl_Item.vsBar.Enabled = False
                Else
                    ' エラーにならないように対策
                    If mcntl_Item.vsBar.Value > m_DspAriaInf.ImgNonAriaSize.Height Then
                        mcntl_Item.vsBar.Value = m_DspAriaInf.ImgNonAriaSize.Height
                    End If
                    ' サイズ変更によるスクロールバー値変更								↓ちょっとミソ。スクロールバーの移動量を大きくすると、割り切れない場合は、表示されないようになる為の処置。最後の数画素にゴミがつくけどね
                    mcntl_Item.vsBar.Maximum = m_DspAriaInf.ImgNonAriaSize.Height + mcntl_Item.vsBar.LargeChange

                    mcntl_Item.vsBar.Enabled = True
                End If
            End If


            ' 横スクロールバー 有効化
            If Not mcntl_Item.hsBar Is Nothing Then

                If 0 >= m_DspAriaInf.ImgNonAriaSize.Width Then
                    mcntl_Item.hsBar.Enabled = False
                Else

                    If mcntl_Item.hsBar.Value > m_DspAriaInf.ImgNonAriaSize.Width Then
                        mcntl_Item.hsBar.Value = m_DspAriaInf.ImgNonAriaSize.Width
                    End If
                    mcntl_Item.hsBar.Maximum = m_DspAriaInf.ImgNonAriaSize.Width

                    mcntl_Item.hsBar.Enabled = True
                End If
            End If

            ' 初期有効
            If bInit Then
                If Not mcntl_Item.vsBar Is Nothing Then mcntl_Item.vsBar.Value = 0
                If Not mcntl_Item.hsBar Is Nothing Then mcntl_Item.hsBar.Value = 0
            End If
        End Sub

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 内部 イベントのエントリー
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' ピクチャーボックスサイズ変更イベント
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        ''' <remarks></remarks>
        Public Sub ResizePaintAria(ByVal sender As Object, ByVal e As System.EventArgs)
            '' サイズチェック
            ' リモートデスクトップで入って、切断時に、なぜかresize通知が来て、サイズが0になっている対策
            If mcntl_Item.picDspImg.ClientSize.Width < 100 Or mcntl_Item.picDspImg.ClientSize.Height < 100 Then
                Return
            End If

            '' 今の左上のオフセット値を元画像でのPixelに変換
            ' 後で、サイズ変更後のオフセット値を求める為
            Dim nTotalOffset As Point
            If m_bIsRpi Then
                nTotalOffset = mcls_Img.GetSimplePoint_DspToImg(m_DspAriaInf.DspPixelTotalOffset)	' 表示座標系から元画像系に変換
            End If


            ' リサイズの処理
            Call ResizePaintAria()

            ' 変更後のフレーム数を設定
            Call mcls_Img.ChangeImgCnt(1, m_DspAriaInf.nDspFrameCnt)

            ' 描画している場合のみ
            If m_bIsRpi Then
                '' サイズ変更後のオフセットを設定する
                nTotalOffset = mcls_Img.GetSimplePoint_ImgToDsp(nTotalOffset)	' 元画像系から表示座標系に変換
                If nTotalOffset.Y > m_DspAriaInf.ImgNonAriaSize.Height Then nTotalOffset.Y = m_DspAriaInf.ImgNonAriaSize.Height ' ちょっとミソ。サイズを縮めた場合、一番最後のpixelを表示エリアの最後に持ってくるようにする
                If nTotalOffset.X > m_DspAriaInf.ImgNonAriaSize.Width Then nTotalOffset.X = m_DspAriaInf.ImgNonAriaSize.Width ' ちょっとミソ。サイズを縮めた場合、一番最後のpixelを表示エリアの最後に持ってくるようにする
                m_DspAriaInf.DspPixelTotalOffset.Y = nTotalOffset.Y
                m_DspAriaInf.DspPixelTotalOffset.X = nTotalOffset.X

                Call SetCommonScrollBar(False)								' スクロールバー量 定義変更
                Call SetNewImgPosition(m_DspAriaInf.DspPixelTotalOffset.Y, m_DspAriaInf.DspPixelTotalOffset.X, True, True)
            End If
        End Sub

        ''' <summary>
        ''' ユーザー初期描画
        ''' </summary>
        ''' <param name="e">グラフィックオブジェクト</param>
        ''' <remarks></remarks>
        Private Sub Ev_Paint_Start(ByVal e As System.Windows.Forms.PaintEventArgs)
            RaiseEvent tEv_Paint_Start(e)
        End Sub

        ''' <summary>
        ''' ユーザー最終描画
        ''' </summary>
        ''' <param name="e"></param>
        ''' <remarks></remarks>
        Private Sub Ev_Paint_End(ByVal e As System.Windows.Forms.PaintEventArgs)
            RaiseEvent tEv_Paint_End(e)
        End Sub

        ''' <summary>
        ''' オーバーレイクリック
        ''' </summary>
        ''' <remarks></remarks>s
        Private Sub Ev_OverLay_Click(ByVal sender As tClass_ImageDsp.OVERLAY_INF, ByVal e As Rectangle)
            RaiseEvent tEv_OverLay_Click(sender, e)
        End Sub
    End Class
End Namespace
