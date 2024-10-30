'*********************************************************************************
'RPIファイルを読み込むクラス。
'	[Ver]
'		Ver.01    2006/03/04  vs2005 対応
'		Ver.02    2014/09/02  抽象クラス化
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Imports System.IO						' ファイルアクセス

Namespace tClass
    Public MustInherit Class tClass_ReadRPI
        Implements IDisposable													' デストラクタ定義


        '*****************************************************************************************************
        ' 公開定数・構造体定義


        ''' <summary>
        ''' サポート圧縮形式
        ''' </summary>
        ''' <remark> 追記する場合は、↓のGetCompTypeで出てきた値を手でコピー</remark>
        Public Enum EM_COMP_TYPE
            COMP_TYPE_UNKNOWN = 0												'不明
            COMP_TYPE_RAW = &H52415720											' BMP(BMPﾍｯﾀﾞｰ無)
            COMP_TYPE_BMP = &H424D5020											' BMP(BMPﾍｯﾀﾞｰ有)
            COMP_TYPE_JPEG = &H4A504547											' JPEG
            COMP_TYPE_LAC = &H4C414320                                          ' LAC 
            COMP_TYPE_NUC = &H4E554320                                          ' NUC 
            COMP_TYPE_LAC3 = &H4C414333                                          ' LAC 3
            COMP_TYPE_LAC4 = &H4C414334                                          ' LAC 4
            COMP_TYPE_LAC5 = &H4C414335                                          ' LAC 5
        End Enum

        ''' <summary>
        ''' 文字列から、圧縮形式ENUMへ変換する
        ''' </summary>
        ''' <param name="sComp">"JPEG"等の文字列</param>
        ''' <returns>圧縮形式</returns>
        Public Shared Function GetCompType(ByVal sComp As String) As EM_COMP_TYPE
            Dim sComp4 As String = Left(sComp + "    ", 4)
            Dim byComp() As Byte = System.Text.Encoding.ASCII.GetBytes(sComp4)
            Array.Reverse(byComp)
            Dim nVal As Integer = BitConverter.ToInt32(byComp, 0)
            Return CType(nVal, EM_COMP_TYPE)
        End Function

        ''' <summary>
        ''' 圧縮形式ENUMを、文字列へ変換する
        ''' </summary>
        ''' <remarks>  末尾にスペースがつく場合があります。 </remarks>
        Public Shared Function GetString(ByVal emComp As EM_COMP_TYPE) As String
            Dim byComp() As Byte = BitConverter.GetBytes(CType(emComp, Integer))
            Array.Reverse(byComp)
            Dim sComp As String = System.Text.Encoding.ASCII.GetString(byComp)
            Return sComp
        End Function

        '''' フレーム情報定義
        Public Enum EM_FRM_TYPE
            EDGE_PER_NON = 0											' 情報無し
            EDGE_PER_FRAME = 1											' フレーム単位のエッジ位置
            EDGE_PER_LINE = 2											' 長手ライン単位のエッジ位置
        End Enum

        ''' <summary>
        ''' RPIファイルの1フレーム情報
        ''' </summary>
        ''' <remarks></remarks>
        Structure RPI_FRAME_INF
            ''' <summary>単純な連番 (1～)</summary>
            Dim no As Integer

            ''' <summary>ファイル先頭アドレスからの該当フレーム画像先頭位置までの相対位置 (0オリジン)</summary>
            Dim img_pos As Long
            ''' <summary>フレーム画像のサイズ</summary>
            Dim img_size As Integer

            ''' <summary>ファイル先頭アドレスからの該当付加情報先頭位置までの相対位置 (0オリジン)</summary>
            Dim inf_pos As Long
        End Structure


        ''' <summary>
        ''' フレームの付加情報 (フレーム単位のエッジ位置)
        ''' </summary>
        ''' <remarks></remarks>
        Structure EDGE_PER_FRAME
            Dim edge_left As Integer									' エッジが無い場合は -1
            Dim edge_right As Integer
        End Structure


        ''' <summary>
        ''' フレームの付加情報 (長手ライン単位のエッジ位置)
        ''' </summary>
        ''' <remarks></remarks>
        Structure EDGE_PER_LINE
            Dim edge_left() As Integer									' エッジが無い場合は -1
            Dim edge_right() As Integer
        End Structure


        ''' <summary>
        ''' 各カメラ情報
        ''' </summary>
        Structure CAM_INFO
            ''' <summary>基準位置。ラインセンターからの各カメラ左端の位置(左方向は、マイナス値) [mm] (PARAM_CAMSETのXoffset値そのもの)</summary>
            Dim dBaseXoffset As Single
            ''' <summary>基準画素。結合後のフレーム左端から自カメラ画像左端までの距離</summary>
            Dim nBaseLeft As Integer
            ''' <summary>次カメラとの右側のオーバーラップ量</summary>
            Dim nOverLap As Integer
        End Structure


        ''' <summary>
        ''' RPI形式のヘッダー情報
        ''' </summary>
        ''' <remarks></remarks>
        Structure RPI_HEADER
            ''' <summary>非圧縮形式時=true  圧縮形式時=false</summary>
            Dim IsBmp As Boolean
            ''' <summary>分解能有り形式時 true</summary>
            Dim IsRes As Boolean
            ''' <summary>フレーム付加情報有り形式時 true</summary>
            Dim IsFrm As Boolean


            '''' 追加
            ''' <summary>TEXヘッダー情報有無</summary>
            Dim IsTHeader As Boolean
            ''' <summary>圧縮形式</summary>
            Dim emCompType As EM_COMP_TYPE


            '' ファイル内部
            ''' <summary>タイプ 4byte</summary>
            Dim type As String
            ''' <summary>ヘッダーサイズ</summary>
            Dim header_size As Integer

            ''' <summary>画像幅</summary>
            Dim width As Integer
            ''' <summary>画像高さ</summary>
            Dim height As Integer
            ''' <summary>画像のチャンネル数(グレー画像=1、RGBカラー=3、など)</summary>
            Dim nchannel As Integer
            ''' <summary>1画素のビット数(8など)</summary>
            Dim pixel_nbit As Integer
            ''' <summary>画像幅に相当する？？</summary>
            Dim pitch_nbyte As Integer
            ''' <summary>フレーム数</summary>
            Dim nframe As Integer
            ''' <summary>1画像のバイト数 (ギャップ無し) (BMP形式のみ)(複数チャンネルの場合は、全画像分のサイズ) </summary>
            Dim frame_nbyte As Integer
            ''' <summary>ファイル先頭から 最初の画像位置</summary>
            Dim frame_start_pos As Integer
            ''' <summary>1画像のバイト数 (ギャップ有り) (BMP形式のみ)</summary>
            Dim frame_pitch_nbyte As Integer

            ''' <summary>フレーム情報の種別(0:無し。1:フレーム単位。2:長手単位)</summary>
            Dim frame_info_type As EM_FRM_TYPE

            ''' <summary>横分解能 [mm/pixel] (無い場合は1固定)</summary>
            Dim res_x As Single
            ''' <summary>縦分解能 [mm/pixel] (無い場合は1固定)</summary>
            Dim res_y As Single

        End Structure



        ''' <summary>
        ''' TEX製RPI形式のヘッダー情報
        ''' </summary>
        ''' <remarks></remarks>
        Structure RPI_T_HEADER
            ''' <summary>この構造体のサイズ</summary>
            Dim nTHeaderSize As Integer
            ''' <summary>TEXヘッダーバージョン (今のところ1固定)</summary>
            Dim nTHeaderVer As Integer
            ''' <summary>システム略称</summary>
            Dim cSysName As String
            ''' <summary>疵検管理No</summary>
            Dim cKizukenNo As String
            ''' <summary>フレーム情報部の格納形式  (0:無し 1:T_RPI_FRAME_INFO_T1)</summary>
            Dim frame_info_type As Short
            ''' <summary>フレーム情報部の格納サイズ</summary>
            Dim frame_info_size As Short
            ''' <summary>コイル先頭のフレームNo</summary>
            Dim nFrameNumInit As Integer
            ''' <summary>このファイル先頭のフレームNo</summary>
            Dim nFrameNumStart As Integer
            ''' <summary>コイル単位でのファイル何個目(１コイルの中で、ファイル分割した時に加算。)(0:分割無し 1～:分割有りで且つ何個目。1コ目のときは1)</summary>
            Dim nFileNum As Integer
            ''' <summary>ラインセンター画素(連結画像左端からのオフセット値) [pixel]</summary>
            Dim nLineCenter As Integer
            ''' <summary>左端カメラセット番号 (1オリジン)</summary>
            Dim nStartCamset As Integer
            ''' <summary>幅方向フレーム結合数 (1オリジン)</summary>
            Dim nCamsetNum As Integer
            ''' <summary>カメラ角度 (1オリジン)</summary>
            Dim nCamAngle As Integer
            ''' <summary>表裏区分 (0:表 1:裏)</summary>
            Dim nMen As Integer
            ''' <summary>カメラ情報</summary>
            Dim caminfo() As CAM_INFO
            ''' <summary>先頭フレーム長手位置 [mm]</summary>
            Dim nStartLen As Integer
            ''' <summary>画像タイプ (0:グレー画像  1:カラー画像  2:色差画像)</summary>
            Dim nColorType As Integer
            ''' <summary>疵検タイプ (0:連続ライン  1:バッチライン)</summary>
            Dim nSysType As Integer
            ''' <summary>先後端位置フレームNo (このファイル先頭からのフレームNo (0オリジン))</summary>
            Dim nFTposCfNo() As Integer
            ''' <summary>先後端位置フレーム内pixel (そのフレーム上端からのピクセル位置 (0オリジン))</summary>
            Dim nFTposPixel() As Integer
            ''' <summary>フッター情報オフセット位置(このファイル先頭からフッターまでの位置)</summary>
            Dim nFooterPos As Long
            ''' <summary>フッター情報（1レコード分）のサイズ。個数はnframe(標準側)</summary>
            Dim nFooterSize As Integer

            Public Sub initialize()
                ReDim caminfo(16 - 1)               ' 20181102 カメラ数拡張 8→16
                ReDim nFTposCfNo(2 - 1)
                ReDim nFTposPixel(2 - 1)
            End Sub
        End Structure

        ''' <summary>
        ''' TEX製フレーム付加情報 タイプ①  128byte固定
        ''' </summary>
        ''' <remarks></remarks>
        Structure T_RPI_FRAME_INFO_T1
            ''' <summary>フレームNo</summary>
            Dim nFNo As Integer
            ''' <summary>ライン速度 [mm/sec]</summary>
            Dim nSpeed As Integer
            ''' <summary>縦分解能 [mm/pixel]</summary>
            Dim vRes As Single
            ''' <summary>コイル先端からの距離 [mm]</summary>
            Dim nFromLen As Integer
            ''' <summary>エッジ位置 [mm]</summary>
            Dim dEdgePos() As Single
            ''' <summary>左端からの左右エッジPixel位置 [pixel]</summary>
            Dim nEdgeIdx() As Integer
            ''' <summary>予備</summary>
            Dim nYobi() As Integer

            Public Sub initialize()
                ReDim dEdgePos(2 - 1)
                ReDim nEdgeIdx(2 - 1)
                ReDim nYobi(24 - 1)
            End Sub
        End Structure

        ''' <summary>
        ''' フッター情報 
        ''' </summary>
        Structure T_RPI_FOOTER_INFO
            ''' <summary>ファイル先頭アドレスからの該当フレーム情報先頭位置までの相対位置 (0オリジン)</summary>
            Dim pos As Long
            ''' <summary>フレーム情報部のサイズ</summary>
            Dim framesize As Integer
            ''' <summary>画像のサイズ</summary>
            Dim imgsize As Integer
        End Structure


        '*****************************************************************************************************
        ' メンバ・メソッド


        Protected m_nActiveCh As Integer										' 現在の選択チャンネル (0オリジン)
        Protected m_RpiHeader As New RPI_HEADER				' EPC製ヘッダー
        Protected m_RpiTHeader As New RPI_T_HEADER          ' TEX製ヘッダー

#Region " IDisposable Support "
        ' このコードは、破棄可能なパターンを正しく実装できるように Visual Basic によって追加されました。
        Public Sub Dispose() Implements IDisposable.Dispose
            ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region

        ''' <summary>
        ''' デストラクタ
        ''' </summary>
        ''' <param name="disposing"></param>
        ''' <remarks>スレッドの中断とか、ファイルの解放とかはこれをオーバーライドして実行する</remarks>
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        End Sub


        ''' <summary>
        ''' RPIヘッダをかえす
        ''' </summary>
        ''' <remarks>基本的にｵｰﾊﾞｰﾗｲﾄﾞする必要はないはず。</remarks>
        Public Overridable ReadOnly Property GetRpiHeader() As RPI_HEADER
            Get
                Return m_RpiHeader
            End Get
        End Property
        'テスト
        Public Property PropRpiHeader() As RPI_HEADER
            Get
                Return m_RpiHeader
            End Get
            Set(value As RPI_HEADER)
                m_RpiHeader = value
            End Set
        End Property


        ''' <summary>
        ''' T形式RPIヘッダをかえす
        ''' </summary>
        ''' <remarks>基本的にｵｰﾊﾞｰﾗｲﾄﾞする必要はないはず。</remarks>
        Public Overridable ReadOnly Property GetRpiTHeader() As RPI_T_HEADER
            Get
                Return m_RpiTHeader
            End Get
        End Property


        ''' <summary>
        ''' フレーム毎エッジ情報をかえす
        ''' </summary>
        Public Overridable Function GetEdgePerFrame(ByVal fno As Integer) As EDGE_PER_FRAME
            Return Nothing
        End Function

        ''' <summary>
        ''' RPIフレーム情報をかえす
        ''' </summary>
        ''' <param name="fno">フレームNo (1オリジン)</param>
        ''' <returns>RPIフレーム情報</returns>
        Public Overridable ReadOnly Property GetRpiFrameInf(ByVal fno As Integer) As RPI_FRAME_INF
            Get
                Dim r As New RPI_FRAME_INF
                r.img_pos = 0
                r.img_size = 0
                r.inf_pos = 0
                r.no = fno
                Return r
            End Get
        End Property


        ''' <summary>
        ''' いま読みこめる上限フレーム。
        ''' このフレーム以降は、まだ準備ができていない。
        ''' </summary>
        ''' <remarks>
        ''' 継承先によっては、設定することも可能（ReadRpiFrameなど）
        ''' 
        ''' </remarks>
        Public MustOverride Property EffectiveRpiFrameCount() As Integer


        ''' <summary>
        ''' いま読みこめる上限フレームをかえす。
        ''' </summary>
        Public Overridable ReadOnly Property GetRpiFrameCount() As Integer
            Get
                Return EffectiveRpiFrameCount
            End Get
        End Property

        ''' <summary>
        ''' 表示用の画像（圧縮）のバイナリ列を読み込む
        ''' </summary>
        ''' <param name="fno">フレーム番号（１オリジン）</param>
        ''' <param name="bWk">読み込む先のバイト配列。呼び出し側は十分大きなサイズを準備してね</param>
        Public MustOverride Function ReadFile_ImgByte_ActiveCh(ByVal fno As Integer, ByVal bWk() As Byte) As Integer


        ''' <summary>
        ''' フレーム毎　情報（T形式）を返す
        ''' </summary>
        ''' <param name="fno">フレーム番号（１オリジン）</param>
        Public Overridable ReadOnly Property GetRpiFrameAdd1(ByVal fno As Integer) As T_RPI_FRAME_INFO_T1
            Get
                Return Nothing
            End Get
        End Property

        ''' <summary>
        ''' 表示用の画像（圧縮）のバイナリ列を読み込み、かえす。
        ''' </summary>
        ''' <param name="fno">フレーム番号（１オリジン）</param>
        Public Overridable Function ReadFile_ImgByte(ByVal fno As Integer) As Byte()
            Return Nothing
        End Function


        ''' <summary>
        ''' 複数チャネルから構成されるとき、チャネルを指定。
        ''' </summary>
        ''' <remarks>基本的にｵｰﾊﾞｰﾗｲﾄﾞする必要はないはず。</remarks>
        Public Overridable Property PropRpiActiveCh() As Integer
            Get
                Return m_nActiveCh
            End Get
            Set(ByVal value As Integer)
                m_nActiveCh = value
            End Set
        End Property

    End Class
End Namespace
