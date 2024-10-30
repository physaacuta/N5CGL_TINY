'*********************************************************************************
'RPI画像表示に特化した基底クラス (並列処理で、画像読込み、表示エリアにリスケを行い、高速描画を行う)
'	[Ver]
'		Ver.01    2009/02/20  vs2005 対応
'		Ver.2	  2014/09/27  カラー画像対応
'
'	[メモ]
'		・横1フレーム 縦?フレーム 形式でPixel単位でずらして表示させる専用クラス
'		・基本的にこのクラスの中で描画はしない。(= 位置変更とかされても使うほうで描画をコールする事。それが繰り返し描画の減少へとつながる)
'*********************************************************************************
Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Option Explicit On

Imports System.Windows.Forms
Imports System.Drawing
Imports tClassLibrary.RpiPlugin


Namespace tClass

    Partial Public Class tClass_ViewRpiThread
        Implements IDisposable                                                  ' デストラクタ定義

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 外部イベント
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' 内蔵していない圧縮形式のための展開要求
        ''' </summary>
        ''' <param name="img">展開後ビットマップ</param>
        ''' <param name="byData">圧縮データ</param>
        ''' <param name="nDataSize">圧縮データのバイト数</param>
        ''' <param name="rpiHeader">現在のRPIヘッダ情報</param>
        Public Event tEv_DecodeImage(ByRef img As Bitmap, ByVal byData() As Byte, ByVal nDataSize As Integer, ByVal rpiHeader As tClass_ReadRPI.RPI_HEADER)

        ''' <summary>
        ''' 生画像 に対する画像処理要求 (自クラス内で生成) (マルチスレッド化でコールするため、画面とは別スレッド)
        ''' </summary>
        ''' <param name="p">(in/out)修正対象の画像クラス情報</param>
        ''' <param name="img">(in/out)修正対象のRaw画像</param>
        ''' <remarks></remarks>
        Public Event tEv_CreatingImg(ByVal p As CLS_IMG_INF, ByRef img As Bitmap)

        ''' <summary>
        ''' 表示先頭フレームNoが変更、又は、表示範囲が変更 (自クラス内で生成)
        ''' </summary>
        ''' <param name="nFrameNoStart">今回の表示先頭フレームNo(1オリジン)</param>
        ''' <param name="nFrameNoEnd">今回の表示後端フレームNo(1オリジン)</param>
        ''' <remarks></remarks>
        Public Event tEv_ChangeDspFrameNo(ByVal nFrameNoStart As Integer, ByVal nFrameNoEnd As Integer)

        ''' <summary>
        ''' サイズ変更 (自クラス内で生成)
        ''' </summary>
        ''' <param name="nOldClientSize">前回のクライアントサイズ</param>
        ''' <remarks></remarks>
        Public Event tEv_ReSize(ByVal nOldClientSize As Size)


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
        '/ RPIファイル並列操作 メンバー
        '/ /////////////////////////////////////////////////////////////////////////////////
        Private m_sThreadName As String = "tCls_ViewRpiTh"

        ' 自生成クラス
        Private mcls_Img As New tClass_ImageDsp                                 ' 画像表示クラス  (本体)
        Private mcls_Rpi As tClass_ReadRPI = New tClass_ReadRPIFile()           ' RPIファイル読込 (本体)
        Private mcls_Cache As New lCls_ImageCache                               ' データ管理クラス (本体)
        Private mcls_MultiExec As New List(Of MultiThreadDecoder)               ' 並列読込クラス (本体)
        Private mque_MultiExec As New tClass_Queue(Of CLS_IMG_INF)(1024)             ' 並列操作用キュー (本体) (※ ここの値は十分に大きくすること)

        Public ReadOnly Property MTDecoder As MultiThreadDecoder
            Get
                Return mcls_MultiExec(0)
            End Get
        End Property

        ' 外部セットクラス
        Private mcls_pLog As tClass_LogManager = Nothing                        ' ログ管理クラス (ポインタ)

        ' 内部状態管理
        Private m_bIsRpi As Boolean = False                                     ' ファイルアクセス準備完了か
        Private m_DspAriaInf As DSP_ARIA_INF                                    ' 画像表示情報
        Private mcntl_Item As CNTL_ITEM                                         ' 外部セットオブジェクト

        Const m_nScrollPitch As Integer = 1                                   ' 巨大なRPIファイルの場合の最小移動画素数 [表示座標系でのpixel]   ' スクロールバーの SmallChange, LargeChangeがちゃんと出来たため、ほとんど使う機会無し。無くても良いが一応残す方向


        ' 表示範囲決定 
        Private m_dLenOffset(1) As Double                                       ' RPIファイルの表示範囲 [mm] (先頭を0mmとしたときに、x～yを指定。アプリ側ではx=0mとして見える)
        Private m_nImgOffsetY As Integer = 0                                    ' 画像をズラス [生pixel] (ズラシた位置を0mmとする) 200だと黒エリアが200挿入される



        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 表示範囲対応
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' 表示範囲 [mm] の取得・設定
        ''' </summary>
        ''' <param name="nIndex">(0:開始位置 1:終了位置)</param>
        Public Property tProp_LenOff(ByVal nIndex As Integer) As Double
            Get
                Return m_dLenOffset(nIndex)
            End Get
            Set(ByVal value As Double)
                m_dLenOffset(nIndex) = value
            End Set
        End Property

        ''' <summary>
        ''' それぞれの範囲位置をそのまま取得
        ''' </summary>
        Public Function GetLenOff_Len(ByVal nIndex As Integer) As Double
            Return m_dLenOffset(nIndex)
        End Function
        Public Function GetLenOff_Dp(ByVal nIndex As Integer) As Integer
            Return GetPos_LenToDp_Y(m_dLenOffset(nIndex))
        End Function
        Public Function GetLenOff_Rp(ByVal nIndex As Integer) As Integer
            Return GetPos_LenToRp_Y(m_dLenOffset(nIndex))
        End Function


        ''' <summary>
        ''' 画像をズラス [生pixel]  の取得・設定 (ズラシた位置を0mmとする) 200だと黒エリアが200挿入される
        ''' </summary>
        Public Property tProp_ImgOffY() As Integer
            Get
                Return m_nImgOffsetY
            End Get
            Set(ByVal value As Integer)
                m_nImgOffsetY = value
            End Set
        End Property




        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 受け渡し用
        '/ /////////////////////////////////////////////////////////////////////////////////

        '===================================================
        ' メイン 構造体

        ''' <summary>
        ''' 使用コントロール 受け渡し構造体
        ''' </summary>
        ''' <remarks></remarks>
        Public Structure CNTL_ITEM
            ''' <summary>画像表示ピクチャー</summary>
            Dim picDspImg As PictureBox                                         ' 画像表示ピクチャー
            ''' <summary>縦スクロールバー (NULL許可)</summary>
            Dim vsBar As tCnt.tCnt_VScrollBar                                   ' 縦スクロールバー (NULL許可)
            ''' <summary>横スクロールバー (NULL許可)</summary>
            Dim hsBar As tCnt.tCnt_HScrollBar                                   ' 横スクロールバー (NULL許可)
        End Structure




        '===================================================
        ''' <summary>
        ''' 画像表示情報
        ''' </summary>
        ''' <remarks></remarks>
        Public Structure DSP_ARIA_INF
            Dim DspImgSize As tClass_ImageDspBase.SizeD                         ' 表示座標系での画像サイズ (1フレーム)
            Dim DspPixelTotalOffset As Point                                    ' 表示座標系での表示左上までのトータルオフセットpixel値
            Dim nDspFno As Integer                                              ' 表示先頭のフレームNo (0オリジン)
            Dim nDspFnoEnd As Integer                                           ' 表示尾短のフレームNo (0オリジン)
            Dim bDspAllOk As Boolean                                            ' 画面に表示するフレーム全部が正常に取得済み時true
            Dim nDspFrameCnt As Integer                                         ' 画面に何フレーム表示可能か？(切り上げ)
            Dim ImgNonAriaSize As Size                                          ' 表示エリアに表示されていない部分の画像サイズ (表示倍率に変換した後のPixel数) = スクロールバーの量となる
        End Structure




        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 内部クラス の 情報公開用 関係
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' 画像表示クラス
        ''' </summary>
        Public ReadOnly Property GetCls_ImgDsp() As tClass_ImageDsp
            Get
                Return mcls_Img
            End Get
        End Property
        'テスト
        Public Property PropCls_ImgDsp() As tClass_ImageDsp
            Get
                Return mcls_Img
            End Get
            Set(value As tClass_ImageDsp)
                mcls_Img = value
            End Set
        End Property

        ''' <summary>
        ''' RPIヘッダ取得
        ''' </summary>
        Public ReadOnly Property GetRpi_Header() As tClass_ReadRPI.RPI_HEADER
            Get
                Return mcls_Rpi.GetRpiHeader
            End Get
        End Property

        ''' <summary>
        ''' TEX製RPIヘッダ取得
        ''' </summary>
        Public ReadOnly Property GetRpi_THeader() As tClass_ReadRPI.RPI_T_HEADER
            Get
                Return mcls_Rpi.GetRpiTHeader
            End Get
        End Property

        ''' <summary>
        ''' RPIフレームアクセス
        ''' </summary>
        ''' <param name="fno">フレームNo (1オリジン)</param>
        ''' <returns>RPIフレーム情報</returns>
        Public ReadOnly Property GetRpi_FrameInf(ByVal fno As Integer) As tClass_ReadRPI.RPI_FRAME_INF
            Get
                Return mcls_Rpi.GetRpiFrameInf(fno)
            End Get
        End Property

        ''' <summary>
        ''' RPIクラス本体
        ''' </summary>
        Public ReadOnly Property GetRpi_Cls() As tClass_ReadRPI
            Get
                Return mcls_Rpi
            End Get
        End Property

        ''' <summary>
        ''' 画像情報情報取得
        ''' </summary>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public ReadOnly Property GetImg_ImgInf() As tClass_ImageDsp.IMG_INF
            Get
                Return mcls_Img.tProp_ImgInf
            End Get
        End Property
        ''' <summary>
        ''' 表示形式情報取得
        ''' </summary>
        Public ReadOnly Property GetImg_ImgKind() As tClass_ImageDsp.IMG_KIND
            Get
                Return mcls_Img.tProp_ImgKind
            End Get
        End Property
        ''' <summary>
        ''' 表示領域情報取得
        ''' </summary>
        Public ReadOnly Property GetImg_AriaInf() As tClass_ImageDsp.ARIA_INF
            Get
                Return mcls_Img.tProp_AriaInf
            End Get
        End Property
        ''' <summary>
        ''' 座標最終結果取得
        ''' </summary>
        Public ReadOnly Property GetImg_CalcAns() As tClass_ImageDsp.CALC_ANS
            Get
                Return mcls_Img.tProp_CalcAns
            End Get
        End Property

        ''' <summary>
        ''' 先読み情報
        ''' </summary>
        ''' <param name="nIndex">ファイル先頭からのフレームNo(1オリジン)</param>
        Public ReadOnly Property GetCls_Img_inf(ByVal nIndex As Integer) As CLS_IMG_INF
            Get
                Dim result As CLS_IMG_INF = Nothing
                mcls_Cache.g_lstImg.TryGetValue(nIndex, result)
                Return result
            End Get
        End Property


        ''' <summary>
        ''' 90°回転するかどうか。あるいは、長手を横表示するかどうか
        ''' </summary>
        Public Property PropRotate90() As Boolean
            Get
                Return mcls_Img.tProp_ImgKind_Rotate90
            End Get
            Set(ByVal value As Boolean)
                mcls_Img.tProp_ImgKind_Rotate90 = value
            End Set
        End Property


        ''' <summary>
        ''' 長手側スクロールバー
        ''' </summary>
        Private ReadOnly Property scrNagate() As ScrollBar
            Get
                If mcls_Img.tProp_ImgKind_Rotate90 Then
                    Return mcntl_Item.hsBar
                Else
                    Return mcntl_Item.vsBar
                End If
            End Get
        End Property

        ''' <summary>
        ''' 長手側スクロールバーのマウス操作上限
        ''' </summary>
        Private ReadOnly Property scrNagateMaximumVirtual() As Integer
            Get
                If mcls_Img.tProp_ImgKind_Rotate90 Then
                    Return mcntl_Item.hsBar.MaximumVirtual
                Else
                    Return mcntl_Item.vsBar.MaximumVirtual
                End If
            End Get
        End Property

        ''' <summary>
        ''' 長手側スクロールバー値の変更
        ''' </summary>
        Private WriteOnly Property scrNagateValueProgramable() As Integer
            Set(ByVal value As Integer)
                If mcls_Img.tProp_ImgKind_Rotate90 Then
                    mcntl_Item.hsBar.ValueProgramable = value
                Else
                    mcntl_Item.vsBar.ValueProgramable = value
                End If
            End Set
        End Property


        ''' <summary>
        ''' 幅スクロールバー
        ''' </summary>
        Private ReadOnly Property scrHaba() As ScrollBar
            Get
                If Not mcls_Img.tProp_ImgKind_Rotate90 Then
                    Return mcntl_Item.hsBar
                Else
                    Return mcntl_Item.vsBar
                End If
            End Get
        End Property

        ''' <summary>
        ''' 幅スクロールバーのマウス操作上限
        ''' </summary>
        Private ReadOnly Property scrHabaMaximumVirtual() As Integer
            Get
                If Not mcls_Img.tProp_ImgKind_Rotate90 Then
                    Return mcntl_Item.hsBar.MaximumVirtual
                Else
                    Return mcntl_Item.vsBar.MaximumVirtual
                End If
            End Get
        End Property

        ''' <summary>
        ''' 幅側スクロールバー値の変更
        ''' </summary>
        Private WriteOnly Property scrHabaValueProgramable() As Integer
            Set(ByVal value As Integer)
                If Not mcls_Img.tProp_ImgKind_Rotate90 Then
                    mcntl_Item.hsBar.ValueProgramable = value
                Else
                    mcntl_Item.vsBar.ValueProgramable = value
                End If
            End Set
        End Property

        ''' <summary>
        ''' 品質設定 (後で変更用)
        ''' </summary>
        ''' <param name="nQuarity">画像縮小品質 (0:低画質  1:高画質)</param>
        ''' <param name="bIsUseImgLibCLI">画像縮小方法 (false:API  true:ImgLibCLI)</param> 
        Public Sub SetParam_Quarity(ByVal nQuarity As Integer, ByVal bIsUseImgLibCLI As Boolean)
            For Each cls As MultiThreadDecoder In mcls_MultiExec
                cls.SetParam_Quarity(nQuarity, bIsUseImgLibCLI)
            Next
        End Sub

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
        ''' 画面表示 左右反転
        ''' </summary>
        ''' <param name="nMode">そのまま:0 反転:1</param>
        ''' <remarks></remarks>
        Public Sub SetProp_DSWS(ByVal nMode As Integer)
            ' デフォルトでは、画面左側は、DS
            If 0 = nMode Then mcls_Img.tProp_ImgKind_IsTurnLR = False
            If 1 = nMode Then mcls_Img.tProp_ImgKind_IsTurnLR = True
        End Sub
        Public Function GetProp_DSWS() As Boolean
            Return mcls_Img.tProp_ImgKind_IsTurnLR
        End Function
        ''' <summary>
        ''' 画面表示 上下反転
        ''' </summary>
        ''' <param name="nMode">そのまま:0 反転:1</param>
        ''' <remarks></remarks>
        Public Sub SetProp_TB(ByVal nMode As Integer)
            If 0 = nMode Then mcls_Img.tProp_ImgKind_IsTurnTB = False
            If 1 = nMode Then mcls_Img.tProp_ImgKind_IsTurnTB = True
        End Sub
        Public Function GetProp_TB() As Boolean
            Return mcls_Img.tProp_ImgKind_IsTurnTB
        End Function

        ''' <summary>
        ''' 画像拡大
        ''' </summary>
        ''' <param name="dRate">拡大倍率</param>
        ''' <remarks></remarks>
        Public Sub SetProp_Zoom(ByVal dRate As Double)
            mcls_Img.tProp_AriaInf_Zoom = New tClass_ImageDsp_Calc.RATE(dRate, dRate)
        End Sub
        ''' <summary>
        ''' 画像拡大
        ''' </summary>
        ''' <param name="dRateX">拡大倍率X</param>
        ''' <param name="dRateY">拡大倍率Y</param>
        ''' <remarks></remarks>
        Public Sub SetProp_Zoom(ByVal dRateX As Double, ByVal dRateY As Double)
            mcls_Img.tProp_AriaInf_Zoom = New tClass_ImageDsp_Calc.RATE(dRateX, dRateY)
        End Sub
        ''' <summary>
        ''' 画像表示状態
        ''' </summary>
        Public Sub SetProp_ImgKind_StrechType(Optional ByVal em As tClass_ImageDsp_Calc.EM_IMG_STRECH = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechWidth)
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
        ''' 画像表示状態 (全フレーム画像表示可能)
        ''' </summary>
        Public ReadOnly Property GetDspAriaInf_DspAllOk() As Boolean
            Get
                Return m_DspAriaInf.bDspAllOk
            End Get
        End Property

        ''' <summary>
        ''' フレーム付加情報取得 (フレーム単位のエッジ位置)
        ''' </summary>
        ''' <param name="fno">フレームNo (1オリジン)</param>
        ''' <returns>フレーム単位のエッジ位置</returns>
        ''' <remarks></remarks>
        Public Function ReadFile_FrameInfo_EdgePerFrame(ByVal fno As Integer) As tClass_ReadRPI.EDGE_PER_FRAME
            Return mcls_Rpi.GetEdgePerFrame(fno)
        End Function

        ''' <summary>
        ''' 先読みフレーム数
        ''' </summary>
        Public Property tProp_ResvNum(ByVal nIndex As Integer) As Integer
            Get
                Return mcls_Cache.tProp_ResvNum(nIndex)
            End Get
            Set(ByVal value As Integer)
                mcls_Cache.tProp_ResvNum(nIndex) = value
            End Set
        End Property



        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 共通公開処理関係
        '/ /////////////////////////////////////////////////////////////////////////////////


        ''' <summary>
        ''' 有効なフレーム数を変更する。
        ''' </summary>
        ''' <param name="nCount">フレーム数</param>
        ''' <returns>成功した場合True</returns>
        ''' <remarks>RpiFileの場合は失敗します。</remarks>
        Public Function SetEffectiveFrameCount(ByVal nCount As Integer) As Boolean
            Try
                mcls_Rpi.EffectiveRpiFrameCount = nCount
                ResizePaintAria(Nothing, Nothing)
            Catch e As Exception
                Return False
            End Try
            Return True
        End Function

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
            If nOneOffsetY < 0 Then
                nFrameNo = 0
            End If
        End Sub
        Public Function GetOffset_TotalToOne_FrameNo(ByVal nTotalOffsetY As Integer) As Integer
            Dim nWk As Integer = nTotalOffsetY - GetPos_RpToDp_Y(m_nImgOffsetY)
            'If m_nImgOffsetY <> 0 Then
            '	nWk += CInt(Math.Floor(m_DspAriaInf.DspImgSize.Height)) - CInt(Math.Floor(CDbl(GetPos_RpToDp_Y(m_nImgOffsetY) Mod m_DspAriaInf.DspImgSize.Height)))
            'End If
            Return CInt(Math.Floor(nWk / m_DspAriaInf.DspImgSize.Height))
        End Function
        Public Function GetOffset_TotalToOne_OffsetY(ByVal nTotalOffsetY As Integer) As Integer
            Dim nWk As Integer = nTotalOffsetY
            If m_nImgOffsetY <> 0 Then
                Dim nImgOff As Integer = GetPos_RpToDp_Y(m_nImgOffsetY)
                Dim nOff As Integer = nImgOff - nTotalOffsetY
                If 0 < nOff Then
                    Return CInt(Math.Floor(m_DspAriaInf.DspImgSize.Height)) - CInt(Math.Floor(nOff Mod m_DspAriaInf.DspImgSize.Height))
                End If
                nWk -= nImgOff
            End If

            Return CInt(Math.Floor(nWk Mod m_DspAriaInf.DspImgSize.Height))
        End Function

        ''' <summary>
        ''' 該当フレームNo、フレーム先頭からのオフセットから表示座標先頭から全体のオフセット量を算出
        ''' </summary>
        ''' <param name="nFrameNo">指定したフレームNo(0オリジン)</param>
        ''' <param name="nOneOffsetY">そのフレーム内でのオフセット量</param>
        ''' <returns>表示座標系 先頭からのトータルオフセット量</returns>
        ''' <remarks></remarks>
        Public Function GetOffsetY_OneToTotal(ByVal nFrameNo As Integer, ByVal nOneOffsetY As Integer) As Integer
            Return CInt(nFrameNo * m_DspAriaInf.DspImgSize.Height) + nOneOffsetY + GetPos_RpToDp_Y(m_nImgOffsetY)
        End Function
        Public Function GetOffsetX_OneToTotal(ByVal nOneOffsetX As Integer) As Integer
            Return CInt(m_DspAriaInf.DspImgSize.Width) + nOneOffsetX
        End Function





        ''' <summary>
        ''' [表示pixel]→[mm]
        ''' </summary>
        Public Overloads Function GetPos_DpToLen_Y(ByVal v As Integer) As Single
            Return CSng(v * mcls_Img.tProp_CalcAns.FinalRes.dH)
        End Function
        Public Overloads Function GetPos_DpToLen_X(ByVal v As Integer) As Single
            Return CSng(v * mcls_Img.tProp_CalcAns.FinalRes.dW)
        End Function


        ''' <summary>
        ''' [mm]→[表示pixel]
        ''' </summary>
        Public Overloads Function GetPos_LenToDp_Y(ByVal v As Double) As Integer
            Return CInt(v / mcls_Img.tProp_CalcAns.FinalRes.dH)
        End Function
        Public Overloads Function GetPos_LenToDp_Y(ByVal v As Integer) As Integer
            Return CInt(v / mcls_Img.tProp_CalcAns.FinalRes.dH)
        End Function
        Public Overloads Function GetPos_LenToDp_X(ByVal v As Double) As Integer
            Return CInt(v / mcls_Img.tProp_CalcAns.FinalRes.dW)
        End Function
        Public Overloads Function GetPos_LenToDp_X(ByVal v As Integer) As Integer
            Return CInt(v / mcls_Img.tProp_CalcAns.FinalRes.dW)
        End Function

        ''' <summary>
        ''' [元pixel]→[mm]
        ''' </summary>
        Public Overloads Function GetPos_RpToLen_Y(ByVal v As Integer) As Single
            Return CSng((v * mcls_Img.tProp_CalcAns.RateDsp.dH) * mcls_Img.tProp_CalcAns.FinalRes.dH)
            'Return CSng(v * (mcls_Img.tProp_CalcAns.RateDsp.dH * mcls_Img.tProp_CalcAns.RateDsp.dH))
        End Function
        Public Overloads Function GetPos_RpToLen_X(ByVal v As Integer) As Single
            Return CSng(v * (mcls_Img.tProp_CalcAns.RateDsp.dW * mcls_Img.tProp_CalcAns.FinalRes.dW))
        End Function

        ''' <summary>
        ''' [mm]→[元pixel]
        ''' </summary>
        Public Overloads Function GetPos_LenToRp_Y(ByVal v As Double) As Integer
            Return CInt(v / (mcls_Img.tProp_CalcAns.RateDsp.dH * mcls_Img.tProp_CalcAns.FinalRes.dH))
        End Function
        Public Overloads Function GetPos_LenToRp_Y(ByVal v As Integer) As Integer
            Return CInt(v / (mcls_Img.tProp_CalcAns.RateDsp.dH * mcls_Img.tProp_CalcAns.FinalRes.dH))
        End Function
        Public Overloads Function GetPos_LenToRp_X(ByVal v As Double) As Integer
            Return CInt(v / (mcls_Img.tProp_CalcAns.RateDsp.dW * mcls_Img.tProp_CalcAns.FinalRes.dW))
        End Function
        Public Overloads Function GetPos_LenToRp_X(ByVal v As Integer) As Integer
            Return CInt(v / (mcls_Img.tProp_CalcAns.RateDsp.dW * mcls_Img.tProp_CalcAns.FinalRes.dW))
        End Function


        ''' <summary>
        ''' [表示pixel]→[元pixel] 表示用の座標から元の画像座標に変換
        ''' </summary>
        Public Overloads Function GetPos_DpToRp(ByVal x As Integer, ByVal y As Integer) As Point
            Return mcls_Img.GetSimplePoint_DspToImg(x, y)
        End Function
        Public Overloads Function GetPos_DpToRp_X(ByVal x As Integer) As Integer
            Return CInt(x / mcls_Img.tProp_CalcAns.RateDsp.dW)
        End Function
        Public Overloads Function GetPos_DpToRp_Y(ByVal y As Integer) As Integer
            Return CInt(y / mcls_Img.tProp_CalcAns.RateDsp.dH)
        End Function

        ''' <summary>
        ''' [元pixel]→[表示pixel] 元の画像座標から表示用の座標に変換
        ''' </summary>
        Public Overloads Function GetPos_RpToDp(ByVal x As Integer, ByVal y As Integer) As Point
            Return mcls_Img.GetSimplePoint_ImgToDsp(x, y)
        End Function
        Public Overloads Function GetPos_RpToDp_X(ByVal x As Integer) As Integer
            Return CInt(x * mcls_Img.tProp_CalcAns.RateDsp.dW)
        End Function
        Public Overloads Function GetPos_RpToDp_Y(ByVal y As Integer) As Integer
            Return CInt(y * mcls_Img.tProp_CalcAns.RateDsp.dH)
        End Function



        ''' <summary>
        ''' [元pixel]→[表示pixel] 元の画像座標から表示用の座標に変換 (反転・オフセット等を考慮) 
        ''' </summary>
        ''' <param name="x">元画像X座標</param>
        ''' <param name="y">元画像Y座標</param>
        ''' <returns>表示座標</returns>
        ''' <remarks></remarks>
        Public Overloads Function GetPosition(ByVal x As Integer, ByVal y As Integer) As Point
            Return mcls_Img.GetPosition(x, y)
        End Function
        Public Overloads Function GetPosition(ByVal pos As Point) As Point
            Return mcls_Img.GetPosition(pos.X, pos.Y)
        End Function




        '''''''''位置に関する変換器。_Yは距離で_mmは位置。_mmにはオフセットが乗る。
        '''''''''1000pxって何mmなの→_Yで変換。先頭から10000pxは何mmの位置なの→_mmで変換。
        '先頭位置(dp=0, rp=0)のラベルを何ミリと表示するか。
        Private m_mmLabelOffsetY As Double
        Public Property tProp_LabelOffsetYmm As Double
            Get
                Return m_mmLabelOffsetY
            End Get
            Set(value As Double)
                m_mmLabelOffsetY = value
            End Set
        End Property

        ''' <summary>
        ''' [表示pixel]→[mm位置]
        ''' </summary>
        Public Overloads Function GetPos_DpToLen_mm(ByVal v As Integer) As Single
            Return CSng(v * mcls_Img.tProp_CalcAns.FinalRes.dH + m_mmLabelOffsetY)
        End Function


        ''' <summary>
        ''' [mm位置]→[表示pixel]
        ''' </summary>
        Public Overloads Function GetPos_LenToDp_mm(ByVal v As Double) As Integer
            Return CInt((v - m_mmLabelOffsetY) / mcls_Img.tProp_CalcAns.FinalRes.dH)
        End Function
        Public Overloads Function GetPos_LenToDp_mm(ByVal v As Integer) As Integer
            Return CInt((v - m_mmLabelOffsetY) / mcls_Img.tProp_CalcAns.FinalRes.dH)
        End Function

        ''' <summary>
        ''' [元pixel]→[mm位置]
        ''' </summary>
        Public Overloads Function GetPos_RpToLen_mm(ByVal v As Integer) As Single
            Return CSng((v * mcls_Img.tProp_CalcAns.RateDsp.dH) * mcls_Img.tProp_CalcAns.FinalRes.dH + m_mmLabelOffsetY)
        End Function

        ''' <summary>
        ''' [mm位置]→[元pixel]
        ''' </summary>
        Public Overloads Function GetPos_LenToRp_mm(ByVal v As Double) As Integer
            Return CInt((v - m_mmLabelOffsetY) / (mcls_Img.tProp_CalcAns.RateDsp.dH * mcls_Img.tProp_CalcAns.FinalRes.dH))
        End Function
        Public Overloads Function GetPos_LenToRp_mm(ByVal v As Integer) As Integer
            Return CInt((v - m_mmLabelOffsetY) / (mcls_Img.tProp_CalcAns.RateDsp.dH * mcls_Img.tProp_CalcAns.FinalRes.dH))
        End Function



        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 初期セット
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' ログクラスセット
        ''' </summary>
        Public Sub SetCls_Log(ByRef cls As tClass_LogManager)
            mcls_pLog = cls
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
        ''' <param name="nThreadNum">並列読込みするスレッド数</param>
        ''' <param name="nQuarity">画像縮小品質 (0:低画質  1:高画質)</param>
        ''' <param name="bIsUseImgLibCLI">画像縮小方法 (false:API  true:ImgLibCLI)</param>
        ''' <remarks></remarks>
        Public Sub Alloc(ByRef cntl As CNTL_ITEM, ByVal nThreadNum As Integer, Optional ByVal nQuarity As Integer = 0, Optional ByVal bIsUseImgLibCLI As Boolean = False)
            '''' コントロールを登録
            mcntl_Item = cntl

            ''''' 画像表示クラス の 準備 2
            ' 描画コントロールセット
            Call mcls_Img.AddHandler_PaintEvent(mcntl_Item.picDspImg)                           ' ペイントイベント登録 & ピクチャー登録
            Call mcls_Img.AddHandler_OverLayClick()                                             ' ピクチャークリックでオーバーレイをクリックしたか判断 登録


            '''' 内部イベントと紐付け
            ' コントロールのイベントと紐付け
            AddHandler mcntl_Item.picDspImg.Resize, AddressOf ResizePaintAria                  ' ペイントサイズ変更イベント登録
            'AddHandler mcntl_Item.picDspImg.SizeChanged, AddressOf ResizePaintAria
            ' 内部クラスとのイベント紐付け
            AddHandler mcls_Img.tEv_Paint_Start, AddressOf Ev_Paint_Start                       ' フレーム画像に独自描画
            AddHandler mcls_Img.tEv_Paint_End, AddressOf Ev_Paint_End                           ' フレーム画像に独自描画
            AddHandler mcls_Img.tEv_OverLay_Click, AddressOf Ev_OverLay_Click                   ' オーバーレイクリック


            '===================================================
            ' データ管理クラス
            mcls_Cache.SetCls_Log(mcls_pLog)
            mcls_Cache.SetCls_ReadRpi(mcls_Rpi)
            mcls_Cache.SetCls_Img(mcls_Img)
            mcls_Cache.SetQue_MultiExec(mque_MultiExec)
            mcls_Cache.tProp_ResvNum(0) = 10
            mcls_Cache.tProp_ResvNum(1) = 10

            ' 並列読込クラス
            For ii As Integer = 0 To nThreadNum - 1

                mcls_MultiExec.Add(New MultiThreadDecoder(String.Format("Loader{0}", ii), nQuarity, bIsUseImgLibCLI))
                mcls_MultiExec(ii).SetCls_Log(mcls_pLog)
                mcls_MultiExec(ii).SetCls_ReadRpi(mcls_Rpi)
                mcls_MultiExec(ii).SetQue_MultiExec(mque_MultiExec)
                InitDecoders(mcls_MultiExec(ii).m_decoders)
                'mcls_MultiExec(ii).ThreadStart(Threading.ThreadPriority.Lowest)
                AddHandler mcls_MultiExec(ii).tEv_DecodeImage, AddressOf Ev_DecodeImage         ' 内蔵していない圧縮の展開
                AddHandler mcls_MultiExec(ii).tEv_CreatingImg, AddressOf Ev_CreatingImg         ' 生画像 に対する画像処理要求 (自クラス内で生成) (マルチスレッド化でコールするため、画面とは別スレッド)

                mcls_MultiExec(ii).ThreadStart(Threading.ThreadPriority.Highest)
            Next


            '''' 自クラス初期化
            Call Init(True)

            '''' サイズ変更処理でデータを登録
            Call ResizePaintAria(Nothing, Nothing)

            '''' 再描画
            Call RePaint()
        End Sub


        Public m_decoder_types As New List(Of Type)
        Public Sub AddDecoders(decoders As IEnumerable(Of Type))
            m_decoder_types.AddRange(decoders)
        End Sub



        Private Sub InitDecoders(decoders As List(Of IRPIDecoder))
            decoders.Add(New tClass_RpiDecoder_LAC)
            decoders.Add(New tClass_RpiDecoder_BMP)
            decoders.Add(New tClass_RpiDecoder_Raw)
            decoders.Add(New tClass_RpiDecoder_Jpeg)
            For Each t As Type In m_decoder_types
                Dim obj As Object = Activator.CreateInstance(t)
                Dim decoder As IRPIDecoder = TryCast(obj, IRPIDecoder)
                decoders.Add(decoder)
            Next
        End Sub


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 初期化関連
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        Public Sub New()
            '''' 画像表示クラス の 準備 1
            mcls_Img.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechWidth    ' 表示エリアの幅に画像幅を合わせる
            mcls_Img.tProp_AriaInf_IsCenter = False                                             ' 疵中心位置移動無し
            mcls_Img.SetImgInf(1024, 1024, 1, 1, 0, 0)                                          ' とりあえず初期情報としてセット
            mcls_Img.tProp_Paint_IsDefImgDspn = True                                            ' デフォルト画像表示にON
            mcls_Img.tProp_Paint_IsPaint = True                                                 ' 画像表示をON
        End Sub

        ''' <summary>
        ''' デストラクタ
        ''' </summary>
#Region " IDisposable Support "
        Private disposedValue As Boolean = False        ' 重複する呼び出しを検出するには

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: 明示的に呼び出されたときにマネージ リソースを解放します
                End If

                '''' クラス解放
                If Not mcls_Rpi Is Nothing Then
                    mcls_Rpi.Dispose()
                    mcls_Rpi = Nothing
                End If


                Call mcls_Img.RemoveHandler_OverLayClick()      ' 動的ハンドル開放
                If Not mcls_Img Is Nothing Then
                    mcls_Img.Dispose()
                    mcls_Img = Nothing
                End If

                mcls_Cache.Delete(-1, -1, 0)
                If Not mcls_Cache Is Nothing Then
                    mcls_Cache.g_lstImg.Clear()
                    mcls_Cache = Nothing
                End If
                If Not mcls_MultiExec Is Nothing Then
                    For ii As Integer = 0 To mcls_MultiExec.Count - 1
                        mcls_MultiExec(ii).ThreadStop()
                        mcls_MultiExec(ii).Dispose()
                        mcls_MultiExec(ii) = Nothing
                    Next
                    mcls_MultiExec.Clear()
                    mcls_MultiExec = Nothing
                End If
                If Not mque_MultiExec Is Nothing Then
                    mque_MultiExec.Dispose()
                    mque_MultiExec = Nothing
                End If

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
        ''' <param name="bInit">完全初期化時true   表裏切替など現位置情報を保持したい場合false</param>
        Public Sub Init(ByVal bInit As Boolean)
            '''' RPIファイルクローズ
            Call RpiClose()
            ' 画像表示エリア
            mcls_Img.tProp_Paint_IsDefImgDspn = True                                            ' デフォルト画像表示にON

            '''' 内部変数開放
            m_bIsRpi = False

            '''' 画像表示情報初期化
            If bInit Then
                m_DspAriaInf.DspPixelTotalOffset.X = 0
                m_DspAriaInf.DspPixelTotalOffset.Y = 0
                m_DspAriaInf.nDspFno = -1
                m_DspAriaInf.nDspFnoEnd = -1
                m_DspAriaInf.bDspAllOk = False
                m_DspAriaInf.nDspFrameCnt = 0
                m_DspAriaInf.ImgNonAriaSize.Height = 0
                m_DspAriaInf.ImgNonAriaSize.Width = 0
            End If


            '''' コントロール初期セット
            If Not scrNagate Is Nothing Then
                scrNagate.Enabled = False
            End If
            If Not scrHaba Is Nothing Then
                scrHaba.Enabled = False
            End If
        End Sub



        ''' <summary>
        ''' RPIにみせかけた、フレームファイル群を開く
        ''' </summary>
        ''' <param name="strFormat">ファイルパスのフォーマット。"c:\test\frame_{1:d4}.jpg"とか。</param>
        ''' <param name="rpiheader">RPIだったらこんなヘッダがつくだろうな、という中身</param>
        ''' <param name="trpiheader">RPIだったらこんなTヘッダがつくはず、という中身</param>
        ''' <param name="bExecAutoFindThread"> ファイルを定周期で探索するか</param>
        ''' <param name="bInit">完全初期化時true   表裏切替など現位置情報を保持したい場合false</param>
        Public Function OpenRpiFrameFiles(ByVal strFormat As String, ByVal rpiheader As tClass.tClass_ReadRPI.RPI_HEADER, ByVal trpiheader As tClass.tClass_ReadRPI.RPI_T_HEADER, ByVal bExecAutoFindThread As Boolean, Optional ByVal bInit As Boolean = True) As Boolean

            ' 一度 初期化
            Call Init(bInit)

            If bExecAutoFindThread Then
                mcls_Rpi = New tClass_ReadRpiFrameAuto(strFormat, rpiheader, trpiheader)
            Else
                mcls_Rpi = New tClass_ReadRpiFrame(strFormat, rpiheader, trpiheader)
            End If

            mcls_Cache.SetCls_ReadRpi(mcls_Rpi)
            For ii As Integer = 0 To mcls_MultiExec.Count - 1
                mcls_MultiExec(ii).SetCls_ReadRpi(mcls_Rpi)
            Next

            AfterInit(0, 0)
            Call SetCommonScrollBar(bInit)
            Return True
        End Function


        ''' <summary>
        ''' RPIファイル アクセス開始
        ''' </summary>
        ''' <param name="strFPath">RPIファイルのフルパス</param>
        ''' <param name="bInit">完全初期化時true   表裏切替など現位置情報を保持したい場合false</param>
        ''' <param name="dResX">横分解能 (0の場合は、RPIファイルを使用)</param>
        ''' <param name="dResY">縦分解能 (0の場合は、RPIファイルを使用)</param>
        ''' <param name="sKanriNo">上書き用検査番号（規定はRPIファイルから）</param>
        Public Function RpiOpen(ByVal strFPath As String, Optional ByVal bInit As Boolean = True, Optional ByVal dResX As Single = 0, Optional ByVal dResY As Single = 0, Optional ByVal sKanriNo As String = tClass_ReadRPIFile.READ_FROM_RPI_FILE) As Boolean
            '===================================================
            ' RPIクラスでファイル読み込み可能かチェック
            If Not tClass_ReadRPIFile.RpiOpenCheck(strFPath) Then
                If Not mcls_pLog Is Nothing Then mcls_pLog.Write(tClass_LogManager.LNO.ERR, String.Format("{0} RPIファイル読み込み失敗1 {1}", m_sThreadName, strFPath))
                Return False
            End If


            '===================================================
            '''' 正式にRPIファイル読込処理開始
            ' 一度 初期化
            Call Init(bInit)
            ''''''''''''''''''''''''''''

            mcls_Rpi = New tClass_ReadRPIFile()
            mcls_Cache.SetCls_ReadRpi(mcls_Rpi)
            For ii As Integer = 0 To mcls_MultiExec.Count - 1
                mcls_MultiExec(ii).SetCls_ReadRpi(mcls_Rpi)
            Next

            ' ファイルオープン ※ true:非同期RPIリスト読込み
            If 0 >= CType(mcls_Rpi, tClass_ReadRPIFile).RpiOpen(strFPath, True, sKanriNo) Then
                Call RpiClose()
                Return False
            End If

            AfterInit(dResX, dResY)
            Call SetCommonScrollBar(bInit)                              ' スクロールバー量 定義変更
            Return True
        End Function


        ''' <summary>
        ''' 全長画像を開いたあとの処理・続き
        ''' </summary>
        ''' <param name="dResX">横分解能 (0の場合は、RPIファイルを使用)</param>
        ''' <param name="dResY">縦分解能 (0の場合は、RPIファイルを使用)</param>
        Private Function AfterInit(ByVal dResX As Single, ByVal dResY As Single) As Boolean
            ' 並列読込み用もオープン
            For ii As Integer = 0 To mcls_MultiExec.Count - 1
                Dim wk As New TYP_MULTI_EXEC_OPEN
                'wk.sFPath = strFPath
                wk.sKizukenNo = mcls_Rpi.GetRpiTHeader.cKizukenNo
                mcls_MultiExec(ii).RpiOpen(wk)
            Next

            ' フラグの変更
            m_bIsRpi = True
            mcls_Img.tProp_Paint_IsDefImgDspn = False                                           ' デフォルト画像表示にOFF


            '===================================================
            '''' 初期計算
            ' 画像表示クラス
            Dim rpiHead As tClass_ReadRPI.RPI_HEADER = mcls_Rpi.GetRpiHeader()

            '''' なぜここでこれをしている？？ これは、ヤメ。画面で以外を選択している場合、変になる
            'mcls_Img.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechWidth


            ' 分解能外部指定
            dResX = IIf(dResX > 0, dResX, rpiHead.res_x)
            dResY = IIf(dResY > 0, dResY, rpiHead.res_y)
            Call mcls_Img.SetImgInf(rpiHead.width, rpiHead.height, dResX, dResY, 0, 0, 1, 1) ' 画像表示枚数は仮でセット
            ' リサイズの処理 (分解能や画像枚数などでスクロール可能量などを計算)
            Call ResizePaintAria()

            'オフセット系統をクリア。
            m_mmLabelOffsetY = 0 'ラベルオフセット
            m_nImgOffsetY = 0    '画像オフセット

            '' 初期表示枚数が決定したため、その分だけは先読み依頼しておく
            'For ii As Integer = 1 To m_DspAriaInf.nDspFrameCnt
            '	Call mcls_List.Add(ii)
            'Next

            ' 初期移動
            'Call SetNewImgPosition(m_DspAriaInf.DspPixelTotalOffset.Y, m_DspAriaInf.DspPixelTotalOffset.X)
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
            If mcls_Rpi IsNot Nothing Then
                Call mcls_Rpi.Dispose()
            End If

            '' バッファ削除
            Call mcls_Cache.Init()
            '' 並列読込み用も閉じる
            For ii As Integer = 0 To mcls_MultiExec.Count - 1
                Call mcls_MultiExec(ii).RpiClose()
            Next
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
        ''' <param name="nScrollY">スクロールバー量[DP]</param>
        ''' <param name="bIsProgramable">プログラムで勝手に変えた場合true</param>
        ''' <remarks></remarks>
        Public Sub ScrollH(ByVal nScrollY As Integer, Optional ByVal bIsProgramable As Boolean = False)
            Dim nTotalOffsetY As Integer = nScrollY * m_nScrollPitch
            If nTotalOffsetY > scrNagateMaximumVirtual Then
                nTotalOffsetY = scrNagateMaximumVirtual
            End If
            Call SetNewImgPosition(nTotalOffsetY, m_DspAriaInf.DspPixelTotalOffset.X)

            If bIsProgramable AndAlso Not scrNagate Is Nothing Then scrNagateValueProgramable = nScrollY
        End Sub

        ''' <summary>
        ''' 横スクロール
        ''' </summary>
        ''' <param name="nTotalOffsetX">左端からのオフセット量[DP]</param>
        ''' <param name="bIsProgramable">プログラムで勝手に変えた場合true</param>
        ''' <remarks></remarks>
        Public Sub ScrollW(ByVal nTotalOffsetX As Integer, Optional ByVal bIsProgramable As Boolean = False)
            Call SetNewImgPosition(m_DspAriaInf.DspPixelTotalOffset.Y, nTotalOffsetX)

            If bIsProgramable AndAlso Not scrHaba Is Nothing Then scrHabaValueProgramable = nTotalOffsetX
        End Sub







        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 重要処理
        '/ /////////////////////////////////////////////////////////////////////////////////




        ''' <summary>
        ''' フレーム画像表示用のデータを描画クラスにセット
        ''' </summary>
        ''' <param name="nTotalOffsetY">0フレーム先頭からのオフセット量(表示座標系)</param>
        ''' <param name="nTotalOffsetX">左端からのオフセット量(表示座標系)</param>
        Private Sub SetNewImgPosition(ByVal nTotalOffsetY As Integer, Optional ByVal nTotalOffsetX As Integer = 0)

            '' 新表示フレームNo を決定
            Dim nNewDspFno As Integer           ' 指定位置に対する 表示先頭フレームNo (0オリジン)
            Dim nNewOneOffsetY As Integer       ' 指定位置に対する 表示先頭フレームからのオフセット位置
            Call GetOffset_TotalToOne(nTotalOffsetY, nNewDspFno, nNewOneOffsetY)

            ' マイナスの場合は、マイナスフレーム分も加算
            If nNewDspFno <= 0 Then
                'If nNewDspFno < 0 AndAlso nNewDspFno <= m_DspAriaInf.nDspFno * -1 Then

                'nNewOneOffsetY += CInt(nNewDspFno * m_DspAriaInf.DspImgSize.Height) * -1
            End If
            ' 尾端を飛びぬけて表示するときの対策
            If nNewDspFno > mcls_Rpi.GetRpiHeader.nframe Then
                nNewDspFno = mcls_Rpi.GetRpiHeader.nframe
            End If

            '' 上下反転時
            If mcls_Img.tProp_ImgKind.bIsTurnTB Then
                nNewDspFno = mcls_Rpi.GetRpiHeader.nframe - nNewDspFno - m_DspAriaInf.nDspFrameCnt        ' フレーム位置を逆から数える
                If nNewDspFno < 0 Then                                                              ' 先頭フレームが負の場合
                    nNewOneOffsetY += CInt(nNewDspFno * m_DspAriaInf.DspImgSize.Height) * -1    ' 負数分の表示オフセットを調整
                    nNewDspFno = 0                                                                  ' 0フレーム目に固定 △
                End If
            End If


            Try
                '''' 内部バッファ変数を更新
                mcls_Img.tProp_AriaInf_Offset = New Point(nTotalOffsetX * -1, nNewOneOffsetY * -1)  ' 表示先頭フレームからのオフセット値をセット
                m_DspAriaInf.DspPixelTotalOffset.Y = nTotalOffsetY  ' 先頭からのオフセット値
                m_DspAriaInf.DspPixelTotalOffset.X = nTotalOffsetX


                '''' 表示する対象の画像が変更された場合のみ
                If m_DspAriaInf.nDspFno <> nNewDspFno Or Not m_DspAriaInf.bDspAllOk Then
                    '' 表示対象画像取得
                    Dim nDspFnoE As Integer = nNewDspFno + m_DspAriaInf.nDspFrameCnt    ' 最終フレームNo (1オリジン)
                    If nDspFnoE > mcls_Rpi.GetRpiHeader.nframe Then nDspFnoE = mcls_Rpi.GetRpiHeader.nframe
                    Dim nSa As Integer = nNewDspFno - m_DspAriaInf.nDspFno
                    Dim nMode As Integer
                    If nSa = 0 Then
                        nMode = 0
                    ElseIf nSa > 0 And Math.Abs(nSa) < m_DspAriaInf.nDspFrameCnt Then
                        nMode = 1
                    ElseIf nSa < 0 And Math.Abs(nSa) < m_DspAriaInf.nDspFrameCnt Then
                        nMode = -1
                    Else
                        nMode = 2
                    End If

                    '' データセット
                    mcls_Cache.Disp(nNewDspFno + 1, nDspFnoE, nSa)
                    m_DspAriaInf.bDspAllOk = False
                    Call ReSetImg(nNewDspFno)


                    '' 表示先頭フレームが変更
                    m_DspAriaInf.nDspFno = nNewDspFno
                    m_DspAriaInf.nDspFnoEnd = nDspFnoE - 1

                    ' 範囲が変更された
                    RaiseEvent tEv_ChangeDspFrameNo(nNewDspFno + 1, nDspFnoE)
                End If

            Catch ex As Exception
                If Not mcls_pLog Is Nothing Then mcls_pLog.Write(tClass_LogManager.LNO.ERR, String.Format("fno={0} offset={1} 表示データセット異常", nNewDspFno, nTotalOffsetY))
            End Try
        End Sub

        ''' <summary>
        ''' 描画クラスに画像データをセットする
        ''' </summary>
        ''' <param name="nDspFnoS">開始フレームNo(0オリジン)</param>
        ''' <remarks></remarks>
        Public Sub ReSetImg(ByVal nDspFnoS As Integer)
            Try
                If Not m_DspAriaInf.bDspAllOk Then
                    Dim nDspFnoE As Integer = nDspFnoS + m_DspAriaInf.nDspFrameCnt  ' 最終フレームNo (1オリジン)
                    If nDspFnoE > mcls_Rpi.GetRpiHeader.nframe Then nDspFnoE = mcls_Rpi.GetRpiHeader.nframe

                    Dim bAllOk As Boolean = mcls_Cache.CheckDispImg(nDspFnoS + 1, nDspFnoE)

                    '' 画像表示クラスにフレームデータを渡す
                    Dim pImgWk(m_DspAriaInf.nDspFrameCnt - 1) As Image
                    For ii As Integer = nDspFnoS + 1 To nDspFnoE
                        If 0 = mcls_Cache.g_lstImg.Count Then Continue For
                        If Not mcls_Cache.g_lstImg(ii).bSyncExce Then
                            'If Not mcls_List.g_lstImg(ii).bImgDspSizeLimit And _
                            '  100 < Math.Abs(m_DspAriaInf.DspImgSize.Width - mcls_List.g_lstImg(ii).nDspW) Or _
                            '  100 < Math.Abs(m_DspAriaInf.DspImgSize.Height - mcls_List.g_lstImg(ii).nDspH) Then


                            'Debug.Print("画像サイズ={0},{1}, 表示={2}", mcls_List.g_lstImg(ii).nDspW, mcls_List.g_lstImg(ii).nDspH, m_DspAriaInf.DspImgSize.Width)

                            If mcls_Cache.g_lstImg(ii).bImgDspSizeLimit AndAlso Not mcls_Cache.g_lstImg(ii).pImgRaw Is Nothing Then
                                pImgWk(ii - nDspFnoS - 1) = mcls_Cache.g_lstImg(ii).pImgRaw
                            Else
                                pImgWk(ii - nDspFnoS - 1) = mcls_Cache.g_lstImg(ii).pImgDsp
                            End If
                        Else
                            pImgWk(ii - nDspFnoS - 1) = Nothing
                        End If
                    Next


                    'If bAllOk Then
                    Call mcls_Img.SetImage(pImgWk, False) ' 画像は、この中では開放しないため false
                    'End If

                    m_DspAriaInf.bDspAllOk = bAllOk
                End If
            Catch ex As Exception
            End Try
        End Sub

        ''' <summary>
        ''' イメージを全部作り直し
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ReCreateImg()
            m_DspAriaInf.bDspAllOk = False
            Dim pImgWk(m_DspAriaInf.nDspFrameCnt - 1) As Image
            Call mcls_Img.SetImage(pImgWk, False)
            Call mcls_Cache.Delete(-1, -1, 0)

            Call SetNewImgPosition(m_DspAriaInf.DspPixelTotalOffset.Y, m_DspAriaInf.DspPixelTotalOffset.X)
        End Sub


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ 公開 基本画像操作処理
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' フレーム画像表示ピクチャーボックスサイズ変更時の処理
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ResizePaintAria()
            Try
                mcls_Img.SetAriaInf()
                mcls_Img.Calculation()                                          ' 変更したサイズで仮計算

                ''高速表示のために、表示エリアの縮尺に画像を合わせておく
                m_DspAriaInf.DspImgSize.Width = mcls_Img.tProp_ImgInf.nOneImgW * mcls_Img.tProp_CalcAns.RateDsp.dW
                m_DspAriaInf.DspImgSize.Height = mcls_Img.tProp_ImgInf.nOneImgH * mcls_Img.tProp_CalcAns.RateDsp.dH

                '' 変更サイズ後で縦に何枚フレーム画像が表示できる？ (切り上げ)
                ' +1 するのがミソ。画素単位で移動させると、上下でフレーム途中の画像を表示する必要がある場合が出てくるため

                Dim pxAreaNagate As Integer = mcls_Img.tProp_AriaInf.nAriaHeight

                m_DspAriaInf.nDspFrameCnt = CInt(Math.Ceiling(pxAreaNagate / m_DspAriaInf.DspImgSize.Height)) + 1


                ' 変更後のフレーム数を設定
                Call mcls_Img.ChangeImgCnt(1, m_DspAriaInf.nDspFrameCnt)                                            ' 画像表示クラス
                Call mcls_Cache.ChangeDspImgSize(CInt(m_DspAriaInf.DspImgSize.Width), CInt(m_DspAriaInf.DspImgSize.Height))  ' RPIファイル読込み

                ' 縮小画像と表示領域が1pixとかズレて、描画がbitbiltにならない対策。コメント化で、旧バージョンで動作可能
                Call mcls_Cache.SetupImageSizeMethod(AddressOf mcls_Img.GetAnsOneImgSize)



                '' 非表示エリアサイズを算出
                If m_bIsRpi Then
                    Dim rpiHead As tClass_ReadRPI.RPI_HEADER = mcls_Rpi.GetRpiHeader()
                    ' 縦
                    Dim nTotalPixelY As Integer = GetOffsetY_OneToTotal(rpiHead.nframe, 0)
                    'Dim nTotalPixelY As Integer = GetOffsetY_OneToTotal(rpiHead.nframe, 0) + m_nImgOffsetY
                    m_DspAriaInf.ImgNonAriaSize.Height = CInt(Math.Ceiling(nTotalPixelY / m_nScrollPitch)) - pxAreaNagate
                    If 0 > m_DspAriaInf.ImgNonAriaSize.Height Then m_DspAriaInf.ImgNonAriaSize.Height = 0
                    ' 横
                    Dim nTotalPixelX As Integer = GetOffsetX_OneToTotal(0)
                    m_DspAriaInf.ImgNonAriaSize.Width = nTotalPixelX - mcls_Img.tProp_AriaInf_Aria.Width
                    If 0 > m_DspAriaInf.ImgNonAriaSize.Width Then m_DspAriaInf.ImgNonAriaSize.Width = 0
                End If
            Catch ex As Exception
            End Try
        End Sub

        ''' <summary>
        ''' スクロールバー 最大サイズ変更
        ''' </summary>
        ''' <param name="bInit">完全初期化時true</param>
        ''' <remarks></remarks>
        Private Sub SetCommonScrollBar(ByVal bInit As Boolean)
            ' 長手スクロールバー 有効化
            If Not scrNagate Is Nothing Then
                If 0 >= m_DspAriaInf.ImgNonAriaSize.Height Then
                    scrNagate.Enabled = False
                    scrNagate.Value = 0
                    scrNagate.Maximum = scrNagate.LargeChange
                Else

                    ' エラーにならないように対策
                    'If scrNagate.Value > m_DspAriaInf.ImgNonAriaSize.Height + 1 Then
                    '	scrNagate.Value = m_DspAriaInf.ImgNonAriaSize.Height + 1
                    'End If
                    ' エラーにならないように対策 (そもそもなぜこんなことをしているか要検討。必要ないのでは？)
                    If scrNagate.Value > m_DspAriaInf.ImgNonAriaSize.Height + 1 Then
                        If (m_DspAriaInf.ImgNonAriaSize.Height + 1) > scrNagate.Maximum Then
                            scrNagate.Value = scrNagate.Maximum
                        ElseIf (m_DspAriaInf.ImgNonAriaSize.Height + 1) < scrNagate.Minimum Then
                            scrNagate.Value = scrNagate.Minimum
                        Else
                            scrNagate.Value = m_DspAriaInf.ImgNonAriaSize.Height + 1
                        End If
                    End If

                    If m_dLenOffset(1) = 0.0 Then
                        ' サイズ変更によるスクロールバー値変更								↓ちょっとミソ。(maxval-LargeChange+1)が最大のvalueとなるための処置。
                        scrNagate.Maximum = m_DspAriaInf.ImgNonAriaSize.Height + scrNagate.LargeChange
                    Else
                        scrNagate.Maximum = GetLenOff_Dp(1) - mcls_Img.tProp_AriaInf.nAriaHeight + scrNagate.LargeChange
                    End If
                    If m_dLenOffset(0) = 0.0 Then
                        scrNagate.Minimum = 0
                    Else
                        scrNagate.Minimum = GetLenOff_Dp(0)      ' 先にminを変更すると、LargeChange の値より大きいときに、勝手にLargeChangeも変更されるための対策
                    End If

                    scrNagate.Enabled = True
                End If
            End If


            ' 幅スクロールバー 有効化
            If Not scrHaba Is Nothing Then

                If 0 >= m_DspAriaInf.ImgNonAriaSize.Width Then
                    scrHaba.Enabled = False
                    scrHaba.Maximum = 0
                Else

                    If scrHaba.Value > m_DspAriaInf.ImgNonAriaSize.Width + 1 Then
                        scrHaba.Value = m_DspAriaInf.ImgNonAriaSize.Width + 1
                    End If
                    scrHaba.Maximum = m_DspAriaInf.ImgNonAriaSize.Width + scrHaba.LargeChange '+ 1
                    scrHaba.Enabled = True
                End If
            End If

            ' 初期有効 △検討 上でした方がよいのでは？
            If bInit Then
                If Not scrNagate Is Nothing Then scrNagate.Value = scrNagate.Minimum '0
                If Not scrHaba Is Nothing Then scrHaba.Value = scrHaba.Minimum '0
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

            Dim oldSize As Size = mcls_Img.tProp_AriaInf_Aria       ' 前回値

            '' 今の左上のオフセット値を元画像でのPixelに変換
            ' 後で、サイズ変更後のオフセット値を求める為
            Dim dNowLenY As Double
            Dim dNowLenX As Double

            If m_bIsRpi Then
                dNowLenY = GetPos_DpToLen_Y(m_DspAriaInf.DspPixelTotalOffset.Y)
                dNowLenX = GetPos_DpToLen_X(m_DspAriaInf.DspPixelTotalOffset.X)
            End If

            ' リサイズの処理
            Call ResizePaintAria()


            '' 描画している場合のみ
            If m_bIsRpi Then
                Dim nTotalOffset As Point
                nTotalOffset.X = GetPos_LenToDp_X(dNowLenX)
                nTotalOffset.Y = GetPos_LenToDp_Y(dNowLenY)
                m_DspAriaInf.DspPixelTotalOffset.Y = nTotalOffset.Y
                m_DspAriaInf.DspPixelTotalOffset.X = nTotalOffset.X
                m_DspAriaInf.bDspAllOk = False

                Call SetCommonScrollBar(False)                              ' スクロールバー量 定義変更
                Call SetNewImgPosition(m_DspAriaInf.DspPixelTotalOffset.Y, m_DspAriaInf.DspPixelTotalOffset.X)
                If Not scrNagate Is Nothing Then scrNagateValueProgramable = m_DspAriaInf.DspPixelTotalOffset.Y
                If Not scrHaba Is Nothing Then scrHabaValueProgramable = m_DspAriaInf.DspPixelTotalOffset.X
            End If
            RaiseEvent tEv_ReSize(oldSize)

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
        ''' <remarks></remarks>
        Private Sub Ev_OverLay_Click(ByVal sender As tClass_ImageDsp.OVERLAY_INF, ByVal e As Rectangle)
            RaiseEvent tEv_OverLay_Click(sender, e)
        End Sub

        ''' <summary>
        ''' 内蔵していない画像の展開要求 (自クラス内で生成) (マルチスレッド化でコールするため、画面とは別スレッド)
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub Ev_DecodeImage(ByRef img As Bitmap, ByVal byData() As Byte, ByVal nDataSize As Integer, ByVal rpiHeader As tClass_ReadRPI.RPI_HEADER)
            RaiseEvent tEv_DecodeImage(img, byData, nDataSize, rpiHeader)
        End Sub

        ''' <summary>
        ''' 生画像 に対する画像処理要求 (自クラス内で生成) (マルチスレッド化でコールするため、画面とは別スレッド)
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub Ev_CreatingImg(ByVal p As CLS_IMG_INF, ByRef img As Bitmap)
            RaiseEvent tEv_CreatingImg(p, img)
        End Sub

    End Class
End Namespace
