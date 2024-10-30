'*********************************************************************************
'RPIファイルを読み込むクラス。
'	[Ver]
'		Ver.01    2006/03/04  vs2005 対応
'		Ver.02    2014/09/02  抽象クラスから継承
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Imports System.IO                       ' ファイルアクセス
#Const READRPI_FILESTREAM = True

Namespace tClass


    ''' <summary>
    ''' 本当のRPIファイルから、ファイルを読むクラス
    ''' </summary>
    Public Class tClass_ReadRPIFile
        Inherits tClass_ReadRPI							' デストラクタ定義


        '*****************************************************************************************************
        ' 公開定数定義
        '''' ヘッダータイプ 
        ' EPC側でlongデータとしてセットしている為、反転している
        Public Const RPI_HEAD_TYPE_RWFL As String = "LFWR"				' フレーム情報無し形式
        Public Const RPI_HEAD_TYPE_RWFI As String = "IFWR"				' フレーム情報有り形式


        '*****************************************************************************************************
        ' メンバー定数定義
        '''' ヘッダーサイズによるヘッダー種別
        ' RWFL 形式
        Private Const RPI_HSIZE_BMP As Integer = 44						' BMP時のヘッダーサイズ
        Private Const RPI_HSIZE_BMP_RES As Integer = 52					' BMP(分解能有り)時のヘッダーサイズ
        Private Const RPI_HSIZE_JPG As Integer = 40						' JPG時のヘッダーサイズ
        Private Const RPI_HSIZE_JPG_RES As Integer = 48					' JPG(分解能有り)時のヘッダーサイズ (TEXヘッダーがあるかも知れない)

        Private Const RPI_HSIZE_BMP_MULTI_EXP As Integer = 60			' 多重露光 (まだとりあえず表示できるようにしただけ)

        ' RWFI
        Private Const RPI_HSIZE_FRM_BMP As Integer = 44					' BMP(フレーム情報有り)時のヘッダーサイズ
        Private Const RPI_HSIZE_FRM_BMP_RES As Integer = 52				' BMP(フレーム情報有り)(分解能有り)時のヘッダーサイズ
        Private Const RPI_HSIZE_FRM_JPG As Integer = 40					' JPG(フレーム情報有り)時のヘッダーサイズ
        Private Const RPI_HSIZE_FRM_JPG_RES As Integer = 48				' JPG(フレーム情報有り)(分解能有り)時のヘッダーサイズ


        ''''''''''''''''
        '' 共通ヘッダー部
        ''''''''''''''''
        Private Const RPI_POS_TYPE_POS As Integer = 0					' タイプ オフセット
        Private Const RPI_POS_TYPE_SIZE As Integer = 4					' タイプ 4byte
        Private Const RPI_POS_HEADERSIZE As Integer = 4					' ヘッダーサイズ

        '*****************************************************************************************************
        'グローバル変数定義


        '*****************************************************************************************************
        'ローカル変数定義
        Private m_IsAccess As Boolean                                       ' RPIファイルにアクセス可能か
        Private m_RpiFileSize As Long

        ' RPIファイルの読み込み用。
#If READRPI_FILESTREAM Then
        Private m_RpiFileStream As IO.FileStream 
#Else
        Private m_RpiFileStream As Win32AsyncFile
#End If



        Public m_RpiFrameInf As New Dictionary(Of Integer, RPI_FRAME_INF)   ' リスト (フレームNo, フレーム情報)
        Private mcls_WThread As tClass_WorkThread                           ' リスト生成用ワーカースレッド本体

        ' TEX用フレーム付加情報
        Private ReadOnly m_RpiFrameAdd1 As New Dictionary(Of Integer, T_RPI_FRAME_INFO_T1) ' フレーム付加情報


        '*****************************************************************************************************
        'プロパティ
        ''' <summary>
        ''' RPIフレームアクセス
        ''' </summary>
        ''' <param name="fno">フレームNo (1オリジン)</param>
        ''' <returns>RPIフレーム情報</returns>
        Public Overrides ReadOnly Property GetRpiFrameInf(ByVal fno As Integer) As RPI_FRAME_INF
            Get
                SyncLock m_RpiFrameInf
                    Return m_RpiFrameInf.Item(fno)
                End SyncLock
            End Get
        End Property
        ''' <summary>
        ''' RPIフレームアクセス (選択チャンネル考慮版)
        ''' </summary>
        ''' <param name="fno">フレームNo (1オリジン)</param>
        ''' <returns>RPIフレーム情報</returns>
        Public ReadOnly Property GetRpiFrameInf_ActiveCh(ByVal fno As Integer) As RPI_FRAME_INF
            Get
                SyncLock m_RpiFrameInf
                    Dim wk As RPI_FRAME_INF = m_RpiFrameInf.Item(fno)
                    wk.img_pos = wk.img_pos + wk.img_size * m_nActiveCh
                    Return wk
                End SyncLock
            End Get
        End Property
        ''' <summary>
        ''' RPI 生成済みフレームリストカウント (非同期読込みで使うのみ？)
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overrides Property EffectiveRpiFrameCount() As Integer
            Get
                SyncLock m_RpiFrameInf
                    Return m_RpiFrameInf.Count
                End SyncLock
            End Get
            Set(ByVal value As Integer)
                ''not supported,.
                Throw New NotSupportedException("tClass_ReadRPIFileは、フレーム数設定できません。")
            End Set
        End Property

        ''' <summary>
        ''' RPIファイルにアクセス可能か
        ''' </summary>
        ''' <returns>RPIアクセス</returns>
        Public ReadOnly Property IsAccess() As Boolean
            Get
                Return m_IsAccess
            End Get
        End Property
        ''' <summary>
        ''' Bitmap画像(true)？Jpeg画像(false)？
        ''' </summary>
        Public ReadOnly Property IsBmp() As Boolean
            Get
                Return m_RpiHeader.IsBmp
            End Get
        End Property
        ''' <summary>
        ''' TEXヘッダー有無
        ''' </summary>
        Public ReadOnly Property IsTHeader() As Boolean
            Get
                Return m_RpiHeader.IsTHeader
            End Get
        End Property
        ''' <summary>
        ''' 圧縮形式
        ''' </summary>
        Public ReadOnly Property IsCompType() As EM_COMP_TYPE
            Get
                Return m_RpiHeader.emCompType
            End Get
        End Property


        ' フレーム付加情報 ===========================================
        ''' <summary>
        ''' RPIフレーム付加情報アクセス.キャッシュになければファイルから読む
        ''' </summary>
        ''' <param name="fno">フレームNo (1オリジン)</param>
        ''' <returns>RPIフレーム付加情報</returns>
        ''' <remarks> 全域でFrameAddロックをとる</remarks>
        Public Overrides ReadOnly Property GetRpiFrameAdd1(ByVal fno As Integer) As T_RPI_FRAME_INFO_T1
            Get
                Dim wk As New T_RPI_FRAME_INFO_T1
                Try
                    SyncLock m_RpiFrameAdd1
                        If Not m_RpiFrameAdd1.TryGetValue(fno, wk) Then
                            If ReadFile_FrameInfo_T1_Async(fno, AddressOf OnFinishRead_FrameInfo_T1) Then
                                'とりあえず、ダミーデータをうめておき、ReadFileが複数実行されないようにする
                                wk.nFNo = -fno
                                m_RpiFrameAdd1.Add(fno, wk)
                            End If
                        End If
                    End SyncLock
                Catch ex As Exception
                End Try
                Return wk
            End Get
        End Property


        ''' <summary>
        ''' ファイルから読み込めた(完了通知）
        ''' </summary>
        ''' <param name="ms">メモリ</param>
        ''' <param name="read">読み込んだバイト数。</param>
        ''' <param name="o">fno. 読み込んだフレームNO（RPI相対）</param>
        Private Sub OnFinishRead_FrameInfo_T1(ms As MemoryStream, read As Integer, o As Object)
            Dim wk As T_RPI_FRAME_INFO_T1 = Parse_FrameInfo_T1(ms)
            Dim fno As Integer = CInt(o)

            '上で用意したダミーを上書き。
            SyncLock m_RpiFrameAdd1
                'このifはRpiClose直後にOnFinishがきたとき用
                If m_RpiFrameInf.Count <> 0 Then
                    m_RpiFrameAdd1(fno) = wk
                End If
            End SyncLock
        End Sub

        '********************************************************************
        'メソッド定義
        '********************************************************************
        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()

        End Sub

        ' IDisposable
        Protected Overrides Sub Dispose(ByVal disposing As Boolean)
            Call RpiClose()
        End Sub


        ''' <summary>
        ''' RPIファイル アクセス可能かチェック
        ''' </summary>
        ''' <param name="strFPath">RPIファイルのフルパス</param>
        Public Shared Function RpiOpenCheck(ByVal strFPath As String) As Boolean
            '''' RPIファイルの事前確認
            If String.IsNullOrEmpty(strFPath) Then
                Return False
            End If
            ' RPIファイルの存在確認
            Try
                If Not IO.File.Exists(strFPath) Then Return False
            Catch ex As Exception
                ' 引数のファイル名がNULLだった場合は、エラーとなる為
                Return False
            End Try

            '''''' ファイルオープン （読込専用）
            Try

                Using fs As IO.FileStream = New FileStream(strFPath, FileMode.Open, FileAccess.Read, FileShare.Delete Or FileShare.Read)

                    ' タイプ項目 読み込み
                    Dim wk(4 - 1) As Byte
                    fs.Position = 0
                    fs.Read(wk, 0, 4)
                    Dim type As String = tMod.tMod_Common.ByteToString(wk)
                    Return RPI_HEAD_TYPE_RWFL = type Or RPI_HEAD_TYPE_RWFI = type
                End Using

            Catch ex As Exception
                Return False
            End Try

        End Function


        Public Const READ_FROM_RPI_FILE As String = "[READ_RPI_FILE]"

        ''' <summary>
        ''' RPIファイル アクセス開始	
        ''' </summary>
        ''' <param name="strFPath">RPIファイルのフルパス</param>
        ''' <param name="bThreadRead">RPIファイルの非同期読込み</param>
        ''' <param name="sKanriNo">上書き用検査番号（規定はRPIファイルから）</param>
        ''' <returns>0～画像枚数 -1～:オープン失敗</returns>
        ''' <remarks></remarks>
        Public Function RpiOpen(ByVal strFPath As String, Optional ByVal bThreadRead As Boolean = False, Optional ByVal sKanriNo As String = READ_FROM_RPI_FILE) As Integer
            '''''' 初期化されているかチェック
            If 0 <> m_RpiFrameInf.Count Then
                Return -9
            End If

            '''''' ファイルオープン （読込専用）
            Try
#If READRPI_FILESTREAM Then
                'RpiViewerの読み方では、FileStreamのバッファが役に立つことは少ないので、小さくしておく。
                m_RpiFileStream = New FileStream(strFPath, FileMode.Open, FileAccess.Read, FileShare.Delete Or FileShare.Read, 32, True)
                m_RpiFileSize = m_RpiFileStream.Length
#Else
                m_RpiFileStream = Win32AsyncFile.OpenForRead(strFPath)
                m_RpiFileSize = m_RpiFileStream.GetFileSize
#End If
            Catch ex As Exception

                Return -1
            End Try


            '''''' RPIファイルからヘッダー情報取得
            If Not Read_Header() Then
                '' クローズ処理する事
                RpiClose()
                Return -2
            End If

            ''''''検査NOの上書き
            If sKanriNo <> READ_FROM_RPI_FILE Then
                m_RpiTHeader.cKizukenNo = sKanriNo
            End If


            '''''' 個々の画像情報のインデックスを作成
            If Not bThreadRead Then
                Dim nRetc As Integer = ListCreate()

                If 0 <> nRetc Then Return nRetc
            Else
                '' スレッドで非同期生成
                ThreadStart_RpiCreateList(Threading.ThreadPriority.Lowest)
            End If


            ''''''' ここまで来たら ファイルアクセス可能
            m_IsAccess = True
            Return m_RpiHeader.nframe
        End Function


        ''' <summary>
        ''' RPIファイル クローズ
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub RpiClose()
            '' チェック
            If m_RpiFileStream Is Nothing Then
                Return
            End If

            '' リスト非同期生成スレッド停止
            ThreadStop_RpiCreateList()

            '' リスト 開放
            m_RpiFrameInf.Clear()
            SyncLock m_RpiFrameAdd1
                m_RpiFrameAdd1.Clear()
            End SyncLock

            '' ファイル クローズ
            m_RpiFileStream.Close()
            m_RpiFileStream = Nothing

            m_IsAccess = False

        End Sub


        '*****************************************************************************************************
        ' ヘッダー生成


        ''' <summary>
        ''' RPIファイルからヘッダー情報を取得する
        ''' </summary>
        ''' <remarks></remarks>
        Private Function Read_Header() As Boolean
            ' まずはヘッダー部を初期化
            m_RpiHeader = Nothing

            Dim br As New BinaryReaderSP(ReadFile_Raw(0, 2048))

            '' 共通ヘッダーの部分を取得
            Dim type As String = br.ReadString(RPI_POS_TYPE_SIZE)
            Dim header_size As Integer = br.ReadInt32()
            m_RpiHeader.type = type
            m_RpiHeader.header_size = header_size

            ' ================================================
            '' タイプの判定
            If RPI_HEAD_TYPE_RWFL = type Then                   ' フレーム情報無し形式
                m_RpiHeader.IsFrm = False

                Select Case header_size
                    Case RPI_HSIZE_BMP
                        m_RpiHeader.IsBmp = True
                        m_RpiHeader.IsRes = False
                    Case RPI_HSIZE_BMP_RES
                        m_RpiHeader.IsBmp = True
                        m_RpiHeader.IsRes = True
                    Case RPI_HSIZE_JPG
                        m_RpiHeader.IsBmp = False
                        m_RpiHeader.IsRes = False
                    Case RPI_HSIZE_JPG_RES
                        m_RpiHeader.IsBmp = False
                        m_RpiHeader.IsRes = True
                    Case RPI_HSIZE_BMP_MULTI_EXP
                        m_RpiHeader.IsBmp = True
                        m_RpiHeader.IsRes = True

                    Case Else
                        Return False
                End Select

            ElseIf RPI_HEAD_TYPE_RWFI = type Then               ' フレーム情報有り形式
                m_RpiHeader.IsFrm = True

                Select Case header_size
                    Case RPI_HSIZE_FRM_BMP
                        m_RpiHeader.IsBmp = True
                        m_RpiHeader.IsRes = False
                    Case RPI_HSIZE_FRM_BMP_RES
                        m_RpiHeader.IsBmp = True
                        m_RpiHeader.IsRes = True
                    Case RPI_HSIZE_FRM_JPG
                        m_RpiHeader.IsBmp = False
                        m_RpiHeader.IsRes = False
                    Case RPI_HSIZE_FRM_JPG_RES
                        m_RpiHeader.IsBmp = False
                        m_RpiHeader.IsRes = True
                    Case Else
                        Return False
                End Select
            Else
                Return False
            End If

            ' ================================================
            '' TEX拡張ヘッダーの有無をチェック
            br.Position = m_RpiHeader.header_size
            Dim nTHeadPos As Integer = br.ReadInt32()      ' TEX製ヘッダーオフセット位置 (EPCヘッダーの次固定)

            ' 暫定対策
            If 2048 < nTHeadPos Then nTHeadPos = 0 ' EPCヘッダー直後にデータ有り

            If RPI_HEAD_TYPE_RWFL = type And m_RpiHeader.IsRes Then
                If 0 <> nTHeadPos Then
                    '' TEX製ヘッダーアリ
                    m_RpiHeader.IsTHeader = True

                    '' TEX製ヘッダー読み込み
                    m_RpiTHeader.initialize()
                    br.Position = nTHeadPos

                    m_RpiTHeader.nTHeaderSize = br.ReadInt32
                    m_RpiTHeader.nTHeaderVer = br.ReadInt32
                    m_RpiTHeader.cSysName = br.ReadString(20)
                    m_RpiTHeader.cKizukenNo = br.ReadString(20)
                    m_RpiTHeader.frame_info_type = br.ReadInt16
                    m_RpiTHeader.frame_info_size = br.ReadInt16
                    m_RpiTHeader.nFrameNumInit = br.ReadInt32
                    m_RpiTHeader.nFrameNumStart = br.ReadInt32
                    m_RpiTHeader.nFileNum = br.ReadInt32
                    m_RpiTHeader.nLineCenter = br.ReadInt32
                    m_RpiTHeader.nStartCamset = br.ReadInt32
                    m_RpiTHeader.nCamsetNum = br.ReadInt32
                    m_RpiTHeader.nCamAngle = br.ReadInt32
                    m_RpiTHeader.nMen = br.ReadInt32
                    For ii As Integer = 0 To m_RpiTHeader.nCamsetNum - 1
                        m_RpiTHeader.caminfo(ii).dBaseXoffset = br.ReadSingle()
                        m_RpiTHeader.caminfo(ii).nBaseLeft = br.ReadInt32()
                        m_RpiTHeader.caminfo(ii).nOverLap = br.ReadInt32()
                    Next
                    'br.Position = nTHeadPos + 180
                    If m_RpiTHeader.nTHeaderVer = 1 Then
                        br.Position = nTHeadPos + 180
                    Else
                        br.Position = nTHeadPos + 276
                    End If
                    m_RpiTHeader.nStartLen = br.ReadInt32
                    m_RpiTHeader.nColorType = br.ReadInt32
                    m_RpiTHeader.nSysType = br.ReadInt32
                    ' 先後端位置
                    m_RpiTHeader.nFTposCfNo(0) = br.ReadInt32()
                    m_RpiTHeader.nFTposCfNo(1) = br.ReadInt32()
                    m_RpiTHeader.nFTposPixel(0) = br.ReadInt32()
                    m_RpiTHeader.nFTposPixel(1) = br.ReadInt32()
                    ' フッター位置
                    'br.Position = nTHeadPos + 208
                    If m_RpiTHeader.nTHeaderVer = 1 Then
                        br.Position = nTHeadPos + 208
                    Else
                        br.Position = nTHeadPos + 304
                    End If
                    m_RpiTHeader.nFooterPos = br.ReadInt64()
                    m_RpiTHeader.nFooterSize = br.ReadInt32()

                End If
            End If

            ' ================================================
            '''' 圧縮形式特定
            ' 標準ヘッダーでもコーデックを見る

            br.Position = 8
            If False = m_RpiHeader.IsTHeader And m_RpiHeader.IsBmp Then         ' 基本タイプ
                m_RpiHeader.emCompType = EM_COMP_TYPE.COMP_TYPE_RAW
            Else                                                ' ヘッダーに圧縮形式の記載有り

                ' 圧縮形式の場合は、圧縮タイプをチェック
                Dim nCompType As Integer = br.ReadInt32
                m_RpiHeader.emCompType = CType(nCompType, EM_COMP_TYPE)  ' 圧縮形式特定

            End If


            m_RpiHeader.width = br.ReadInt32                    ' 画像幅
            m_RpiHeader.height = br.ReadInt32                ' 画像高さ
            m_RpiHeader.nchannel = br.ReadInt32         ' 画像のチャンネル数(グレー画像=1、RGBカラー=3、など)
            m_RpiHeader.pixel_nbit = br.ReadInt32           ' 1画素のビット数(8など)
            m_RpiHeader.pitch_nbyte = br.ReadInt32          ' 画像幅に相当する？？△
            m_RpiHeader.nframe = br.ReadInt32               ' フレーム数

            ' ================================================
            '' ヘッダー情報取得
            ' あまりよろしくないが、今後の拡張の為、べた打ち・直打ち にしておくよ
            If True = m_RpiHeader.IsBmp And False = m_RpiHeader.IsFrm Then          ' 非圧縮RPI (フレーム情報無し)
                m_RpiHeader.frame_nbyte = br.ReadInt32          ' 1画像のバイト数 (ギャップ無し) (BMP形式のみ)
                m_RpiHeader.frame_start_pos = br.ReadInt32      ' ファイル先頭から 最初の画像位置
                m_RpiHeader.frame_pitch_nbyte = br.ReadInt32    ' 1画像のバイト数 (ギャップ有り) (BMP形式のみ)



                ' 不要項目初期化
                m_RpiHeader.frame_info_type = EM_FRM_TYPE.EDGE_PER_NON


            ElseIf False = m_RpiHeader.IsBmp And False = m_RpiHeader.IsFrm Then     ' 圧縮RPI (フレーム情報無し) ※ TEX版RPIはこれのみ
                m_RpiHeader.frame_start_pos = br.ReadInt32         ' ファイル先頭から 最初の画像位置

                ' 不要項目初期化
                m_RpiHeader.frame_nbyte = -1
                m_RpiHeader.frame_pitch_nbyte = -1
                m_RpiHeader.frame_info_type = EM_FRM_TYPE.EDGE_PER_NON


                ' TEXヘッダーでわかるもの
                If m_RpiHeader.IsTHeader Then
                    If EM_COMP_TYPE.COMP_TYPE_RAW = m_RpiHeader.emCompType Or EM_COMP_TYPE.COMP_TYPE_BMP = m_RpiHeader.emCompType Then
                        m_RpiHeader.frame_nbyte = m_RpiHeader.width * m_RpiHeader.height * m_RpiHeader.nchannel
                        If EM_COMP_TYPE.COMP_TYPE_BMP = m_RpiHeader.emCompType Then m_RpiHeader.frame_nbyte += 1078
                        m_RpiHeader.frame_pitch_nbyte = tMod.GET_SIZE_JUST(12 + m_RpiHeader.frame_nbyte + m_RpiTHeader.frame_info_size, 512)
                    End If
                End If

            ElseIf True = m_RpiHeader.IsBmp And True = m_RpiHeader.IsFrm Then       ' 非圧縮RPI (フレーム情報有り)

                m_RpiHeader.frame_nbyte = br.ReadInt32()          ' 1画像のバイト数 (ギャップ無し) (BMP形式のみ)
                m_RpiHeader.frame_start_pos = br.ReadInt32()      ' ファイル先頭から 最初の画像位置
                m_RpiHeader.frame_info_type = CType(br.ReadInt32(), EM_FRM_TYPE) ' フレーム情報の種別

                ' 不要項目初期化
                m_RpiHeader.frame_pitch_nbyte = -1


            ElseIf False = m_RpiHeader.IsBmp And True = m_RpiHeader.IsFrm Then      ' 圧縮RPI (フレーム情報有り)
                m_RpiHeader.frame_start_pos = br.ReadInt32()      ' ファイル先頭から 最初の画像位置
                m_RpiHeader.frame_info_type = CType(br.ReadInt32(), EM_FRM_TYPE) ' フレーム情報の種別


                ' 不要項目初期化
                m_RpiHeader.pitch_nbyte = m_RpiHeader.width
                m_RpiHeader.frame_nbyte = -1
                m_RpiHeader.frame_pitch_nbyte = -1
                m_RpiHeader.frame_info_type = EM_FRM_TYPE.EDGE_PER_NON

            End If

            If m_RpiHeader.IsRes Then
                m_RpiHeader.res_x = br.ReadSingle       ' 横分解能 [mm/pixel]
                m_RpiHeader.res_y = br.ReadSingle           ' 縦分解能 [mm/pixel]
            Else
                m_RpiHeader.res_x = 1
                m_RpiHeader.res_y = 1
            End If
            'ReadFile_Int(m_RpiFile.Length - 4)
            'MsgBox("")
            Return True
        End Function


        '*****************************************************************************************************
        ' リスト生成

        Function CanCreateListImmidiately() As Boolean
            If m_RpiHeader.IsTHeader Then
                ' フッターからインデックスを生成可能
                If 0 <> m_RpiTHeader.nFooterPos AndAlso 0 <> m_RpiTHeader.nFooterSize Then
                    Return True
                End If
            End If
            If Not m_RpiHeader.IsFrm And m_RpiHeader.IsBmp Then
                '' フレーム付加情報無し& 非圧縮 (bmp)
                Return True
            End If
            Return False
        End Function

        ''' <summary>
        ''' リスト生成
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Private Function ListCreate() As Integer
            ' ================================================
            ' TEX製ヘッダーのみ
            If m_RpiHeader.IsTHeader Then
                If m_RpiHeader.IsFrm Then Return -99 ' サポート外
                'If m_RpiHeader.IsBmp Then Return -98	' BMP もサポートした！

                ' フッターからインデックスを生成可能
                If 0 <> m_RpiTHeader.nFooterPos AndAlso 0 <> m_RpiTHeader.nFooterSize Then
                    If ListCreate_Footer(CBool(m_RpiTHeader.frame_info_type)) Then
                        Return 0
                    Else
                        Return -19
                    End If
                End If



                If EM_COMP_TYPE.COMP_TYPE_RAW = m_RpiHeader.emCompType Or EM_COMP_TYPE.COMP_TYPE_BMP = m_RpiHeader.emCompType Then
                    '------------------------
                    ' 非圧縮形式時
                    If 0 = m_RpiTHeader.frame_info_type Then
                        If Not ListCreate_RAW() Then                    ' TEX製フレーム情報無し
                            Return -10
                        End If
                    ElseIf 1 = m_RpiTHeader.frame_info_type Then
                        If Not ListCreate_RAW_AddFrmInfoRead_T1() Then  ' TEX製フレーム付加情報１ 同時読込み
                            Return -12
                        End If
                    Else
                        Return -90
                    End If
                Else
                    '------------------------
                    ' 圧縮形式時
                    If 0 = m_RpiTHeader.frame_info_type Then
                        If Not ListCreate_JPG() Then                    ' フレーム情報無し
                            Return -11
                        End If
                    ElseIf 1 = m_RpiTHeader.frame_info_type Then
                        If Not ListCreate_JPG_AddFrmInfoRead_T1() Then  ' TEX製フレーム付加情報１ 同時読込み
                            Return -13
                        End If
                    Else
                        Return -90
                    End If
                End If

                Return 0
            End If


            ' ================================================
            ' 基本
            If Not m_RpiHeader.IsFrm Then					'' フレーム付加情報無し
                If m_RpiHeader.IsBmp Then							'' 非圧縮 (bmp)
                    If Not ListCreate_BMP() Then
                        Return -10
                    End If

                Else
                    If Not ListCreate_JPG() Then                    '' 圧縮 (jpeg, lac)
                        Return -11
                    End If
                End If

            Else											'' フレーム付加情報有り
                If m_RpiHeader.IsBmp Then							'' 非圧縮 (bmp)
                    If Not ListCreate_BMP_AddFrmInfo() Then
                        Return -12
                    End If

                Else
                    If Not ListCreate_JPG_AddFrmInfo() Then         '' 圧縮 (jpeg, lac)
                        Return -13
                    End If
                End If
            End If
            Return 0
        End Function

        ''' <summary>
        ''' BMP形式の リスト生成 (フレーム情報無し)
        ''' </summary>
        ''' <remarks></remarks>
        Private Function ListCreate_BMP() As Boolean
            Dim pos As Long = m_RpiHeader.frame_start_pos       ' 最初の画像位置

            SyncLock m_RpiFrameInf
                For ii As Integer = 1 To m_RpiHeader.nframe
                    ' データ生成
                    Dim inf As new RPI_FRAME_INF
                    inf.no = ii
                    inf.img_pos = pos                               ' ファイル先頭から画像までのオフセット
                    inf.img_size = CInt(m_RpiHeader.frame_nbyte / m_RpiHeader.nchannel)    ' 画像サイズ
                    inf.inf_pos = 0                                 ' 付加情報無しの為、0固定

                    ' データセット
                    m_RpiFrameInf.Add(ii, inf)

                    ' 次へ
                    pos += m_RpiHeader.frame_pitch_nbyte

                    ' 最低限のサイズチェック
                    If pos > m_RpiFileSize Then
                        Return False
                    End If
                Next ii
            End SyncLock

            Return True
        End Function

        ''' <summary>
        ''' BMP形式の リスト生成 (フレーム情報有り)
        ''' </summary>
        ''' <remarks></remarks>
        Private Function ListCreate_BMP_AddFrmInfo() As Boolean
            Dim pos As Long = m_RpiHeader.frame_start_pos		' 最初の画像位置

            For ii As Integer = 1 To m_RpiHeader.nframe
                Dim br As New BinaryReaderSP(ReadFile_Raw(pos, 8))
                ' データ生成
                Dim inf As RPI_FRAME_INF
                inf.no = ii
                inf.img_pos = pos + 8   ' (8=next=bmp_image)    ' ファイル先頭から画像までのオフセット
                inf.img_size = CInt(m_RpiHeader.frame_nbyte / m_RpiHeader.nchannel)       '  画像サイズ
                inf.inf_pos = inf.img_pos + m_RpiHeader.frame_nbyte     ' 付加情報のオフセット。(画像の後ろに情報がついている)

                ' データセット
                SyncLock m_RpiFrameInf
                    m_RpiFrameInf.Add(ii, inf)
                End SyncLock

                ' 次へ
                pos = br.ReadInt64

                ' 最低限のサイズチェック
                If pos > m_RpiFileSize Then
                    Return False
                End If
                ' スレッドが停止？
                If TreadStopCheck_RpiCreatList() Then Return True
            Next ii

            Return True
        End Function

        ''' <summary>
        ''' 圧縮形式の リスト生成 (フレーム情報無し)
        ''' </summary>
        ''' <remarks></remarks>
        Private Function ListCreate_JPG() As Boolean
            Dim pos As Long = m_RpiHeader.frame_start_pos		' 最初の画像位置 (各画像データのsize位置を示すようにする)
            Dim nFileLength As Long = m_RpiFileSize

            For ii As Integer = 1 To m_RpiHeader.nframe
                Dim br As New BinaryReaderSP(ReadFile_Raw(pos, 16))
                ' データ生成
                Dim inf As RPI_FRAME_INF
                inf.no = ii
                inf.img_pos = pos + 12   ' (12=size+next=jpeg_image)	' ファイル先頭から画像までのオフセット
                inf.img_size = br.ReadInt32 ' 画像サイズ
                inf.inf_pos = 0                                 ' 付加情報無しの為、0固定

                ' データセット
                SyncLock m_RpiFrameInf
                    m_RpiFrameInf.Add(ii, inf)
                End SyncLock

                ' 次へ
                pos = br.ReadInt64

                ' 最低限のサイズチェック (最後のフレーム時はチェックしない)
                If ii <> m_RpiHeader.nframe And pos >= nFileLength Then
                    ''△
                    'm_RpiHeader.nframe = ii
                    'Return True

                    Return False
                End If

                ' スレッドが停止？
                If TreadStopCheck_RpiCreatList() Then Return True
            Next ii

            Return True
        End Function

        ''' <summary>
        ''' JPG形式の リスト生成 (フレーム情報有り)
        ''' </summary>
        ''' <remarks></remarks>
        Private Function ListCreate_JPG_AddFrmInfo() As Boolean
            Dim pos As Long = m_RpiHeader.frame_start_pos		' 最初の画像位置 (各画像データのsize位置を示すようにする)

            For ii As Integer = 1 To m_RpiHeader.nframe
                ' データ生成
                Dim br As New BinaryReaderSP(ReadFile_Raw(pos, 16))

                Dim inf As RPI_FRAME_INF
                inf.no = ii
                inf.img_pos = pos + 12   ' (12=size+next=jpeg_image)	' ファイル先頭から画像までのオフセット
                inf.img_size = br.ReadInt32 ' 画像サイズ
                inf.inf_pos = inf.img_pos + inf.img_size                ' 付加情報のオフセット。(画像の後ろに情報がついている)


                ' データセット
                SyncLock m_RpiFrameInf
                    m_RpiFrameInf.Add(ii, inf)
                End SyncLock

                ' 次へ
                pos = br.ReadInt64

                ' 最低限のサイズチェック (最後のフレーム時はチェックしない)
                If ii <> m_RpiHeader.nframe And pos >= m_RpiFileSize Then
                    Return False
                End If

                ' スレッドが停止？
                If TreadStopCheck_RpiCreatList() Then Return True
            Next ii

            Return True
        End Function


        ''' <summary>
        ''' 圧縮形式の リスト生成 (フレーム情報有り:同時読込TEX形式T1) TEXヘッダーの場合のみ
        ''' </summary>
        ''' <remarks></remarks>
        Private Function ListCreate_JPG_AddFrmInfoRead_T1() As Boolean
            Dim pos As Long = m_RpiHeader.frame_start_pos		' 最初の画像位置 (各画像データのsize位置を示すようにする)

            For ii As Integer = 1 To m_RpiHeader.nframe
                ' データ生成
                Dim br As New BinaryReaderSP(ReadFile_Raw(pos, 16))

                Dim inf As RPI_FRAME_INF
                inf.no = ii
                inf.img_pos = pos + 12   ' (12=size+next=jpeg_image)	' ファイル先頭から画像までのオフセット
                inf.img_size = br.ReadInt32 ' 画像サイズ
                inf.inf_pos = inf.img_pos + inf.img_size                ' 付加情報のオフセット。(画像の後ろに情報がついている)


                ' データセット
                ' フレーム情報読込み & セット
                SyncLock m_RpiFrameInf
                    m_RpiFrameInf.Add(ii, inf)
                End SyncLock

                SyncLock m_RpiFrameAdd1
                    If Not m_RpiFrameAdd1.ContainsKey(ii) Then
                        m_RpiFrameAdd1.Add(ii, ReadFile_FrameInfo_T1(ii))
                    End If
                End SyncLock


                ' 次へ
                pos = br.ReadInt64

                ' 最低限のサイズチェック (最後のフレーム時はチェックしない)
                If ii <> m_RpiHeader.nframe And pos >= m_RpiFileSize Then
                    Return False
                End If

                ' スレッドが停止？
                If TreadStopCheck_RpiCreatList() Then Return True
            Next ii

            Return True
        End Function


        ''' <summary>
        ''' 非圧縮形式の リスト生成 (フレーム情報無し)
        ''' </summary>
        ''' <remarks></remarks>
        Private Function ListCreate_RAW() As Boolean
            Dim pos As Long = m_RpiHeader.frame_start_pos		' 最初の画像位置
            SyncLock m_RpiFrameInf

                For ii As Integer = 1 To m_RpiHeader.nframe
                    ' データ生成
                    Dim inf As RPI_FRAME_INF
                    inf.no = ii
                    inf.img_pos = pos + 12       ' (12=size+next=jpeg_image)			' ファイル先頭から画像までのオフセット
                    inf.img_size = CInt(m_RpiHeader.frame_nbyte / m_RpiHeader.nchannel)    ' 画像サイズ
                    inf.inf_pos = 0                                 ' 付加情報無しの為、0固定

                    ' データセット
                    m_RpiFrameInf.Add(ii, inf)

                    ' 次へ
                    pos += m_RpiHeader.frame_pitch_nbyte

                    ' 最低限のサイズチェック
                    If pos > m_RpiFileSize Then
                        Return False
                    End If
                Next ii
            End SyncLock


            Return True
        End Function

        ''' <summary>
        ''' 非圧縮形式の リスト生成 (フレーム情報有り:同時読込TEX形式T1) TEXヘッダーの場合のみ
        ''' </summary>
        ''' <remarks></remarks>
        Private Function ListCreate_RAW_AddFrmInfoRead_T1() As Boolean
            Dim pos As Long = m_RpiHeader.frame_start_pos		' 最初の画像位置

            For ii As Integer = 1 To m_RpiHeader.nframe
                ' データ生成
                Dim inf As RPI_FRAME_INF
                inf.no = ii
                inf.img_pos = pos + 12   ' (12=size+next=jpeg_image)	' ファイル先頭から画像までのオフセット
                inf.img_size = CInt(m_RpiHeader.frame_nbyte / m_RpiHeader.nchannel)       '  画像サイズ
                inf.inf_pos = inf.img_pos + m_RpiHeader.frame_nbyte     ' 付加情報のオフセット。(画像の後ろに情報がついている)


                ' データセット
                SyncLock m_RpiFrameInf
                    m_RpiFrameInf.Add(ii, inf)
                End SyncLock

                ' フレーム情報読込み & セット
                Try
                    SyncLock m_RpiFrameAdd1
                        If Not m_RpiFrameAdd1.ContainsKey(ii) Then
                            m_RpiFrameAdd1.Add(ii, ReadFile_FrameInfo_T1(ii))
                        End If
                    End SyncLock
                Catch ex As Exception
                End Try

                ' 次へ (512単位)
                pos += m_RpiHeader.frame_pitch_nbyte

                ' 最低限のサイズチェック
                If pos > m_RpiFileSize Then
                    Return False
                End If
            Next ii

            Return True
        End Function

        '*****************************************************************************************************
        ' フッターからインデックス生成

        ''' <summary>
        ''' フッターからインデックス生成
        ''' </summary>
        ''' <param name="bAddInf"></param>
        Private Function ListCreate_Footer(ByVal bAddInf As Boolean) As Boolean
            Dim br As New BinaryReaderSP(ReadFile_Raw(m_RpiTHeader.nFooterPos, m_RpiHeader.nframe * m_RpiTHeader.nFooterSize))

            'すぐにおわるはずなので、広範囲でLOCKする
            SyncLock m_RpiFrameInf
                For ii As Integer = 1 To m_RpiHeader.nframe
                    Dim wk As New T_RPI_FOOTER_INFO
                    br.Position = m_RpiTHeader.nFooterSize * (ii - 1)

                    wk.pos = br.ReadInt64
                    wk.framesize = br.ReadInt32
                    wk.imgsize = br.ReadInt32


                    ' データ生成
                    Dim inf As RPI_FRAME_INF
                    inf.no = ii
                    inf.img_pos = 12 + wk.pos   ' ファイル先頭から画像までのオフセット
                    inf.img_size = wk.imgsize      '  画像サイズ
                    If bAddInf Then
                        inf.inf_pos = inf.img_pos + inf.img_size        ' 付加情報のオフセット。(画像の後ろに情報がついている)
                    End If

                    ' データセット
                    m_RpiFrameInf.Add(ii, inf)
                Next
            End SyncLock
            Return True
        End Function





        '*****************************************************************************************************
        ' ファイル読込関係(基本・本体系)

        ''' <summary>
        ''' 表示用の画像（圧縮）のバイナリ列を読み込む
        ''' </summary>
        ''' <param name="fno">フレーム番号（１オリジン）</param>
        ''' <param name="bWk">読み込む先のバイト配列。呼び出し側は十分大きなサイズを準備してね</param>
        Public Overrides Function ReadFile_ImgByte_ActiveCh(ByVal fno As Integer, ByVal bWk() As Byte) As Integer
            'Dim inf As RPI_FRAME_INF = m_RpiFrameInf.Item(fno)
            Dim inf As RPI_FRAME_INF = GetRpiFrameInf_ActiveCh(fno)
            ' 読み込み位置チェック, フレーム数
            If inf.img_pos >= m_RpiFileSize Or fno > m_RpiHeader.nframe Then
                Return 0
            End If

            ' 画像取得

            Return doReadFile_Byte(inf.img_pos, bWk, inf.img_size)
        End Function


        ''' <summary>
        ''' RPIファイル から フレーム画像 を読み込む
        ''' </summary>
        ''' <param name="fno">フレームNo (1オリジン)</param>
        ''' <returns>バイトデータ</returns>
        ''' <remarks></remarks>
        Public Function ReadFile_Img(ByVal fno As Integer) As Drawing.Bitmap
            Dim inf As RPI_FRAME_INF = m_RpiFrameInf.Item(fno)

            ' 読み込み位置チェック, フレーム数
            If inf.img_pos >= m_RpiFileSize Or fno > m_RpiHeader.nframe Then
                Return Nothing
            End If

            Dim wk(inf.img_size - 1) As Byte

            ' 画像取得
            doReadFile_Byte(inf.img_pos, wk, inf.img_size)

            ' 画像形式判定 して イメージを生成
            If m_RpiHeader.IsBmp Then
                Return tClass.tClass_BmpJpeg.DataToBmp(m_RpiHeader.pitch_nbyte, m_RpiHeader.height, wk)
            Else
                Return tClass.tClass_BmpJpeg.DataToImage(wk)
            End If
        End Function

        ''' <summary>
        ''' RPIファイル から フレーム画像のバイナリデータ を読み込む
        ''' </summary>
        ''' <param name="fno">フレームNo (1オリジン)</param>
        ''' <returns>バイトデータ</returns>
        ''' <remarks></remarks>
        Public Overrides Function ReadFile_ImgByte(ByVal fno As Integer) As Byte()
            Dim inf As RPI_FRAME_INF = m_RpiFrameInf.Item(fno)

            ' 読み込み位置チェック, フレーム数
            If inf.img_pos >= m_RpiFileSize Or fno > m_RpiHeader.nframe Then
                Return Nothing
            End If

            ' 画像取得
            Dim wk(inf.img_size - 1) As Byte

            ' 画像取得
            doReadFile_Byte(inf.img_pos, wk, inf.img_size)

            Return wk
        End Function

        '*****************************************************************************************************
        ' ファイル読込関係(基本・ヘッダ系)
        ''' <summary>
        ''' フレーム付加情報の読み込み (フレーム単位のエッジ位置)
        ''' </summary>
        ''' <param name="fno">フレームNo (1オリジン)</param>
        ''' <returns>フレーム単位のエッジ位置</returns>
        ''' <remarks></remarks>
        Public Function ReadFile_FrameInfo_EdgePerFrame(ByVal fno As Integer) As EDGE_PER_FRAME
            Dim inf As RPI_FRAME_INF = m_RpiFrameInf.Item(fno)
            Dim br As New BinaryReaderSP(ReadFile_Raw(inf.inf_pos, 8))

            Dim wk As EDGE_PER_FRAME
            wk.edge_left = br.ReadInt32
            wk.edge_right = br.ReadInt32
            Return wk
        End Function

        ''' <summary>
        ''' フレーム付加情報の読み込み (長手ライン単位のエッジ位置)
        ''' </summary>
        ''' <param name="fno">フレームNo (1オリジン)</param>
        ''' <returns>長手ライン単位のエッジ位置</returns>
        ''' <remarks></remarks>
        Public Function ReadFile_FrameInfo_EdgePerLine(ByVal fno As Integer) As EDGE_PER_LINE
            Dim inf As RPI_FRAME_INF = m_RpiFrameInf.Item(fno)

            ' 領域確保
            Dim wk As New EDGE_PER_LINE
            ReDim wk.edge_left(m_RpiHeader.height - 1)
            ReDim wk.edge_right(m_RpiHeader.height - 1)

            ' バイナリデータ形式で取得して、一気にint配列に変換
            Dim size As Integer = 4 * m_RpiHeader.height  ' 片側のみのサイズ
            Dim buf As MemoryStream = ReadFile_Raw(inf.inf_pos, size * 2)
            Buffer.BlockCopy(buf.GetBuffer(), 0, wk.edge_left, 0, m_RpiHeader.height * 4)
            Buffer.BlockCopy(buf.GetBuffer(), m_RpiHeader.height * 4, wk.edge_right, 0, m_RpiHeader.height * 4)

            Return wk
        End Function

        ''' <summary>
        ''' フレーム付加情報の読み込み (TEX形式１)
        ''' </summary>
        ''' <param name="fno">フレームNo (1オリジン)</param>
        ''' <returns>フレーム単位のエッジ位置</returns>
        ''' <remarks></remarks>
        Public Function ReadFile_FrameInfo_T1(ByVal fno As Integer) As T_RPI_FRAME_INFO_T1

            '例外が飛ぶのはいや
            Dim inf As New RPI_FRAME_INF
            If Not m_RpiFrameInf.TryGetValue(fno, inf) Then
                Return Nothing
            End If

            Dim br As New BinaryReaderSP(ReadFile_Raw(inf.inf_pos, 64))

            Dim wk As New T_RPI_FRAME_INFO_T1
            wk.initialize()
            wk.nFNo = br.ReadInt32()
            wk.nSpeed = br.ReadInt32()
            wk.vRes = br.ReadSingle()
            wk.nFromLen = br.ReadInt32()
            wk.dEdgePos(0) = br.ReadSingle()
            wk.dEdgePos(1) = br.ReadSingle()
            wk.nEdgeIdx(0) = br.ReadInt32()
            wk.nEdgeIdx(1) = br.ReadInt32()
            Return wk
        End Function


        ''' <summary>
        ''' フレーム付加情報の読み込み (TEX形式１)
        ''' </summary>
        ''' <param name="fno">フレームNo (1オリジン)</param>
        ''' <returns>フレーム単位のエッジ位置</returns>
        ''' <remarks></remarks>
        Public Function ReadFile_FrameInfo_T1_Async(ByVal fno As Integer, cb As CallbackReadFile) As Boolean

            '例外が飛ぶのはいや
            Dim inf As New RPI_FRAME_INF
            If Not m_RpiFrameInf.TryGetValue(fno, inf) Then
                Return False
            End If

            If inf.inf_pos = 0 Then
                Return False
            End If

            Try
                ReadFile_RawAsync(inf.inf_pos, 64, cb, fno)
                Return True
            Catch e As System.IO.IOException
                Return False
            End Try

        End Function

        Public Function Parse_FrameInfo_T1(ms As MemoryStream) As T_RPI_FRAME_INFO_T1
            Dim br As New BinaryReaderSP(ms)

            Dim wk As New T_RPI_FRAME_INFO_T1
            wk.initialize()
            wk.nFNo = br.ReadInt32()
            wk.nSpeed = br.ReadInt32()
            wk.vRes = br.ReadSingle()
            wk.nFromLen = br.ReadInt32()
            wk.dEdgePos(0) = br.ReadSingle()
            wk.dEdgePos(1) = br.ReadSingle()
            wk.nEdgeIdx(0) = br.ReadInt32()
            wk.nEdgeIdx(1) = br.ReadInt32()
            Return wk
        End Function

        '*****************************************************************************************************
        ' ファイル読込関係(基本系)
        ''' <summary>
        ''' RPIファイルからlengthバイト読み込み、MemoryStreamとして返す。
        ''' </summary>
        ''' <param name="pos">ファイルの位置</param>
        ''' <param name="length">読み込むサイズ</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Private Function ReadFile_Raw(ByVal pos As Long, ByVal length As Integer) As MemoryStream
            Dim ms As New MemoryStream()
            ms.SetLength(length)

            doReadFile_Byte(pos, ms.GetBuffer(), length)
            Return ms
        End Function

        Public Delegate Sub CallbackReadFile(ms As MemoryStream, read As Integer, o As Object)

        Private Class Tuple3
            Public cb As CallbackReadFile
            Public ms As MemoryStream
            Public o As Object
            Sub New(cb As CallbackReadFile, ms As MemoryStream, o As Object)
                Me.cb = cb
                Me.ms = ms
                Me.o = o
            End Sub
        End Class

        Private Function ReadFile_RawAsync(ByVal pos As Long, ByVal length As Integer, cb As CallbackReadFile, o As Object) As Boolean
            Dim ms As New MemoryStream()
            ms.SetLength(length)

            Dim buf() As Byte = ms.GetBuffer()
            Dim ar As IAsyncResult
            SyncLock m_RpiFileStream
                m_RpiFileStream.Position = pos
                'ar = m_RpiFileStream.BeginRead()
                ar = m_RpiFileStream.BeginRead(buf, 0, length, AddressOf BeginRead_finish, New Tuple3(cb, ms, o))
            End SyncLock

            Return True
        End Function

        Private Sub BeginRead_finish(ar As IAsyncResult)
            Dim kv As Tuple3 = TryCast(ar.AsyncState, Tuple3)
            Dim read As Integer = m_RpiFileStream.EndRead(ar)
            kv.cb.Invoke(kv.ms, read, kv.o)
        End Sub

        ''' <summary>
        ''' RPIファイルからlengthバイト読む。スレッドセーフ
        ''' </summary>
        ''' <param name="pos"></param>
        ''' <param name="buf"></param>
        ''' <param name="readlen"></param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Private Function doReadFile_Byte(ByVal pos As Long, ByVal buf() As Byte, readlen As Integer) As Integer
#If READRPI_FILESTREAM Then
            'SyncLock m_RpiFileStream
            '    m_RpiFileStream.Position = pos
            '    Return m_RpiFileStream.Read(buf, 0, readlen)
            'End SyncLock


            Dim ar As IAsyncResult = doReadFile_Begin(pos, buf, readlen)
            ar.AsyncWaitHandle.WaitOne()
            Return m_RpiFileStream.EndRead(ar)
#Else
            Using ov As ReadOverlapped = m_RpiFileStream.IssureRead(pos, buf, readlen)
                Dim ret As Integer = ov.WaitResult()
                Return ret
            End Using
#End If
        End Function

        Private Function doReadFile_Begin(ByVal pos As Long, ByVal buf() As Byte, readlen As Integer) As IAsyncResult
            SyncLock m_RpiFileStream
                m_RpiFileStream.Position = pos
                'ar = m_RpiFileStream.BeginRead()
                Return m_RpiFileStream.BeginRead(buf, 0, readlen, Nothing, Nothing)
            End SyncLock
        End Function






        '*****************************************************************************************************
        '/ メイン処理・バックグラウンドで、フレームリンク情報を取得するとか。

        ''' <summary>
        ''' スレッドスタート
        ''' </summary>
        Private Function ThreadStart_RpiCreateList(Optional ByVal priority As Threading.ThreadPriority = Threading.ThreadPriority.Normal) As Boolean
            If Not mcls_WThread Is Nothing Then
                Call ThreadStop_RpiCreateList()
            End If

            '''' メインスレッド実行
            mcls_WThread = New tClass_WorkThread
            mcls_WThread.ThreadStart(AddressOf mainThread_RpiCreateList, priority, "ReadRpiCreatList", True)
            Return True
        End Function

        ''' <summary>
        ''' スレッドストップ
        ''' </summary>
        Private Sub ThreadStop_RpiCreateList()
            '' インスタンス確認
            If Not mcls_WThread Is Nothing Then
                '' メインスレッド終了
                mcls_WThread.ThreadStop(200)
                mcls_WThread.Dispose()
                mcls_WThread = Nothing
            End If
        End Sub

        ''' <summary>
        ''' スレッド終了チェック
        ''' </summary>
        ''' <returns>true:スレッド停止要求  false:run</returns>
        ''' <remarks></remarks>
        Private Function TreadStopCheck_RpiCreatList() As Boolean
            If mcls_WThread Is Nothing Then Return False

            '''' スレッド有り時
            If mcls_WThread.g_evStop.WaitOne(0, True) Then Return True
            Return False
        End Function


        ''' <summary>
        ''' 非同期 RPIリスト生成
        ''' </summary>
        Private Sub mainThread_RpiCreateList()
            Try
                Call ListCreate()
            Catch ex As Threading.ThreadAbortException
            Catch ex As Exception
            End Try
        End Sub
    End Class

End Namespace
