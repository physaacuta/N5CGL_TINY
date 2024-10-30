'/ ############################################################################################################################
'/ 並列操作専用クラス
'/ ############################################################################################################################
Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Option Explicit On

Imports System.Windows.Forms
Imports System.Drawing
Imports tClassLibrary.RpiPlugin

Namespace tClass
    Partial Public Class tClass_ViewRpiThread
        '===================================================
        ' 並列操作用 構造体
        ''' <summary>RPIファイルオープン</summary>
        Public Structure TYP_MULTI_EXEC_OPEN

            'Dim sFPath As String				' RPIファイルフルパス
            Dim sKizukenNo As String            ' 疵検管理No
        End Structure

        '===================================================
        ' 画像データ メインクラス
        ''' <summary>RPIファイル読込み結果</summary>
        Public Class CLS_IMG_INF
            ' IN
            Public nIndex As Integer            ' RPIファイル先頭からの位置 (1オリジン)
            Public nCh As Integer               ' チャンネル (0オリジン)
            Public nDspW As Integer             ' 表示 画像サイズ
            Public nDspH As Integer

            ' OUT
            ' Public pImgRawF As MemoryStream	' こいつが仮想メモリを使えないか？
            Public pImgRaw As Bitmap            ' 生画像 (使い終わったら、都度 開放必要)
            Public pImgDsp As Bitmap            ' 表示画像 (32ビットの表示画像サイズに変換済み) (使い終わったら、都度 開放必要)
            Public bImgDspSizeLimit As Boolean  ' 表示画像サイズが内部リミット上限の場合true

            '' 定数定義
            Public Enum EM_STATUS
                em_NON = 0                      ' 未生成
                em_NOMAL                        ' リストに登録されただけ
                em_OK                           ' データセットされた

            End Enum


            '' 管理データ
            Public emStatus As EM_STATUS        ' 自クラスの状態
            Public bCompleted As Integer
            Public bSyncExce As Boolean                             ' 非同期操作実行中 (実行中の場合true。実行完了時false)
            Public bDele As Boolean

            ''' <summary>
            ''' コンストラクタ
            ''' </summary>
            Public Sub New()
                nIndex = 0
                pImgRaw = Nothing
                pImgDsp = Nothing

                emStatus = EM_STATUS.em_NON
                bSyncExce = False
            End Sub

            Function IsDisposableBitmap(ByVal bmp As Bitmap) As Boolean
                If bmp Is Nothing Then Return False
                If bmp.Tag Is Nothing Then Return True
                If bmp.Tag.ToString.StartsWith("SHARED") Then Return False
                Return True
            End Function


            ''' <summary>
            ''' デストラクタ
            ''' </summary>
            Public Sub Dispose()

                If IsDisposableBitmap(pImgRaw) Then
                    pImgRaw.Dispose()
                End If
                pImgRaw = Nothing

                If IsDisposableBitmap(pImgDsp) Then
                    pImgDsp.Dispose()
                End If
                pImgDsp = Nothing

                Threading.Thread.VolatileWrite(bCompleted, -1)
                emStatus = EM_STATUS.em_NON
                bSyncExce = False
            End Sub
        End Class

        ''' <summary>
        ''' 並列操作クラス
        ''' </summary>
        ''' <remarks></remarks>
        Public Class MultiThreadDecoder
            Implements IDisposable                                                  ' デストラクタ定義

            '===================================================
            '' 公開イベント
            ''' <summary>
            ''' 生画像 に対する画像処理要求 (自クラス内で生成) (マルチスレッド化でコールするため、画面とは別スレッド)
            ''' </summary>
            ''' <param name="p">(in/out)修正対象の画像クラス情報</param>
            ''' <param name="img">(in/out)修正対象のRaw画像</param>
            ''' <remarks></remarks>
            Public Event tEv_CreatingImg(ByVal p As CLS_IMG_INF, ByRef img As Bitmap)

            ''' <summary>
            ''' 内蔵していない圧縮形式データの展開要求
            ''' </summary>
            ''' <param name="img">展開後ビットマップ</param>
            ''' <param name="byData">圧縮データ</param>
            ''' <param name="nDataSize">圧縮データのバイト数</param>
            ''' <param name="rpiHeader">現在のRPIヘッダ情報</param>
            ''' <remarks>imgに値が入っているなら、Disposeして上書きすること</remarks>
            Public Event tEv_DecodeImage(ByRef img As Bitmap, ByVal byData() As Byte, ByVal nDataSize As Integer, ByVal rpiHeader As tClass_ReadRPI.RPI_HEADER)




            '===================================================
            '' メンバークラス
            ' 自生成クラス
            Private mcls_WThread As New tClass_WorkThread                           ' ワーカースレッド本体


            ' 外部セットクラス
            Private mcls_pLog As tClass_LogManager                                  ' ログ管理クラス (ポインタ)
            Private mcls_pReadRpi As tClass.tClass_ReadRPI = Nothing                        ' RPIファイル読込クラス (実体なし)
            Private mque_pMultiExec As tClass_Queue(Of CLS_IMG_INF) = Nothing            ' 並列操作用キュー (実体なし)

            '' 変数
            Private m_sThreadName As String = ""                                    ' 自スレッド名称
            Private m_Resizer As IImageResize
            '' 操作中のRPIファイル
            Private m_cs As New Object                                              ' SyncLock同期オブジェクト(クリティカルセクション)
            Private m_sKizukenNo As String = ""                                     ' 疵検管理No

            Public m_decoders As New List(Of IRPIDecoder)
            Dim m_active_decoder As IRPIDecoder = Nothing

            '/ /////////////////////////////////////////////////////////////////////////////////
            '/ 初期セット
            '/ /////////////////////////////////////////////////////////////////////////////////
            ''' <summary>
            ''' ログクラスセット
            ''' </summary>
            Public Sub SetCls_Log(ByVal cls As tClass_LogManager)
                mcls_pLog = cls
            End Sub
            ''' <summary>
            ''' RPIファイル読込
            ''' </summary>
            Public Sub SetCls_ReadRpi(ByVal cls As tClass_ReadRPI)
                mcls_pReadRpi = cls
            End Sub
            ''' <summary>
            ''' 並列操作用キュー
            ''' </summary>
            Public Sub SetQue_MultiExec(ByVal que As tClass_Queue(Of CLS_IMG_INF))
                mque_pMultiExec = que
            End Sub


            ''' <summary>
            ''' 品質設定 (後で変更用)
            ''' </summary>
            ''' <param name="nQuarity">画像縮小品質 (0:低画質  1:高画質)</param>
            ''' <param name="bUseImgLibCLI">画像縮小方法 (false:API  true:ImgLibCLI)</param> 
            Public Sub SetParam_Quarity(ByVal nQuarity As Integer, ByVal bUseImgLibCLI As Boolean)

                If bUseImgLibCLI Then
                    m_Resizer = New tClass_ImgResize_IPP()
                Else
                    m_Resizer = New tClass_ImgResize_API()
                End If

                ' 縮小品質決定
                m_Resizer.Quality = nQuarity
            End Sub


            '/ /////////////////////////////////////////////////////////////////////////////////
            '/ 基本処理
            '/ /////////////////////////////////////////////////////////////////////////////////

            ''' <summary>
            ''' コンストラクタ
            ''' </summary>
            ''' <param name="sThreadName"></param>
            ''' <param name="nQuarity">画像縮小品質 (0:低画質  1:高画質)</param>
            ''' <param name="bUseImgLibCLI">画像縮小方法 (false:API  true:ImgLibCLI)</param>
            ''' <remarks></remarks>
            Public Sub New(ByVal sThreadName As String, Optional ByVal nQuarity As Integer = 0, Optional ByVal bUseImgLibCLI As Boolean = False)
                m_sThreadName = sThreadName
                SetParam_Quarity(nQuarity, bUseImgLibCLI)
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
                        For Each dec As IRPIDecoder In m_decoders
                            Dim dis As IDisposable = TryCast(dec, IDisposable)
                            If dis IsNot Nothing Then
                                dis.Dispose()
                            End If
                        Next

                    End If
                End If
                Me.disposedValue = True
            End Sub


            ' このコードは 、破棄可能なパターンを正しく実装できるように Visual Basic によって追加されました。
            Public Sub Dispose() Implements IDisposable.Dispose
                ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
                Dispose(True)
                GC.SuppressFinalize(Me)
            End Sub
#End Region


            ''' <summary>
            ''' スレッドスタート
            ''' </summary>
            Public Function ThreadStart(Optional ByVal priority As Threading.ThreadPriority = Threading.ThreadPriority.Normal) As Boolean
                '''' メインスレッド実行
                Return mcls_WThread.ThreadStart(AddressOf mainThread, priority, m_sThreadName, True)
            End Function

            ''' <summary>
            ''' スレッドストップ
            ''' </summary>
            Public Sub ThreadStop()
                '' インスタンス確認
                If mcls_WThread IsNot Nothing Then
                    '' メインスレッド終了
                    mcls_WThread.ThreadStop(2000)
                    mcls_WThread.Dispose()
                    mcls_WThread = Nothing
                End If
            End Sub


            '/ /////////////////////////////////////////////////////////////////////////////////
            '/ メイン処理
            '/ /////////////////////////////////////////////////////////////////////////////////
            Private Sub mainThread()
                '''' シグナルセット
                ' スレッド終了, キュー有り
                Dim EventAry As Threading.WaitHandle() = {mcls_WThread.g_evStop, mque_pMultiExec.g_evSem}


                '''' シグナル待ち
                Try
                    While (True)
                        '' シグナル待ち
                        Dim nEvtNo As Integer = Threading.WaitHandle.WaitAny(EventAry)
                        Select Case nEvtNo
                            Case 0                          ' 終了確認
                                Exit While
                            Case 1                          ' RPIファイル操作
                                Dim p As CLS_IMG_INF = mque_pMultiExec.GetItem()
                                If p IsNot Nothing Then
                                    ' フレーム生成
                                    ' 非同期操作開始
                                    p.bSyncExce = True
                                    System.Threading.Thread.VolatileWrite(p.bCompleted, 0)

                                    If RpiReadImg(p) Then
                                        p.bSyncExce = False
                                        System.Threading.Thread.VolatileWrite(p.bCompleted, 1)
                                    Else
                                        If p.bDele Then
                                            If Not mcls_pLog Is Nothing Then mcls_pLog.Write(tClass_LogManager.LNO.WAR, String.Format("[{0}] 要素削除 [{1}]", m_sThreadName, p.nIndex))
                                        End If
                                        p.Dispose()
                                    End If
                                End If

                            Case Else
                                'ありえない
                        End Select
                    End While
                Catch ex As Threading.ThreadAbortException
                    If Not mcls_pLog Is Nothing Then
                        mcls_pLog.Write(tClass_LogManager.LNO.ERR, String.Format("{0} ThreadAbortException. {1}", m_sThreadName, ex.ToString))
                    End If
                Catch ex As Exception
                    If Not mcls_pLog Is Nothing Then
                        mcls_pLog.Write(tClass_LogManager.LNO.ERR, String.Format("{0} Exception. {1}", m_sThreadName, ex.ToString))
                    End If
                End Try
            End Sub


            '/ /////////////////////////////////////////////////////////////////////////////////
            '/ RPIファイル操作 (排他)
            '/ /////////////////////////////////////////////////////////////////////////////////
            ''' <summary>
            ''' RPIファイルオープン
            ''' </summary>
            ''' <param name="p">情報</param>
            ''' <remarks></remarks>
            Public Sub RpiOpen(ByVal p As TYP_MULTI_EXEC_OPEN)
                '''' オープン
                SyncLock m_cs
                    m_sKizukenNo = p.sKizukenNo


                    'select decoder.
                    '最初はNothingとする
                    m_active_decoder = Nothing
                    Dim strCodec As String = tClass_ReadRPI.GetString(mcls_pReadRpi.GetRpiHeader.emCompType)
                    For Each dec As IRPIDecoder In m_decoders
                        If dec.IsCompressionSupported(strCodec) Then
                            m_active_decoder = dec
                        End If
                    Next
                End SyncLock

            End Sub
            ''' <summary>
            ''' RPIファイルクローズ
            ''' </summary>
            ''' <remarks></remarks>
            Public Sub RpiClose()
            End Sub

            ''' <summary>
            ''' エラー用のビットマップを読み込む。or 作成する
            ''' </summary>
            ''' <remarks>サイズは64KB程度なので、メモリ不足の心配はしなくてもよいかも..</remarks>
            Private Shared m_errorNotFound As Boolean = False
            Public Shared Function LoadErrorBitmap() As Bitmap
                Try
                    If Not m_errorNotFound Then
                        Return New Bitmap(".\フレームエラー.bmp")
                    End If

                Catch e As Exception
                    m_errorNotFound = True
                End Try

                Dim rbmp As New Bitmap(32, 32, System.Drawing.Imaging.PixelFormat.Format24bppRgb)
                For xx As Integer = 0 To 31
                    For yy As Integer = 0 To 31
                        rbmp.SetPixel(xx, yy, Color.Red)
                    Next
                Next

                Return rbmp
            End Function

            ''' <summary>
            ''' RPIファイル読込み ＆ 変換
            ''' </summary>
            ''' <param name="p">要求情報</param>
            ''' <returns> Falseを返したばあい、pをDisposeすること。</returns>
            ''' <remarks></remarks>
            Private Function RpiReadImg(ByVal p As CLS_IMG_INF) As Boolean

                SyncLock m_cs
                    '===================================================


                    ' 強制中断確認
                    If p.bDele Then Return False

                    '===================================================
                    ' 今回の画像圧縮データ部分だけ読み込む
                    If 1 > p.nIndex Then
                        Return False
                    End If

                    ' まだ、画像の位置が分からない。非同期リスト読込み中...
                    Dim nCnt As Integer = mcls_pReadRpi.EffectiveRpiFrameCount
                    While (nCnt < p.nIndex)
                        Threading.Thread.Sleep(50)
                        ' 強制中断確認
                        If p.bDele Then
                            Return False
                        End If
                        nCnt = mcls_pReadRpi.EffectiveRpiFrameCount       ' 今回はどう？
                    End While


                    Try
                        p.pImgRaw = DecompressFrame(p.nIndex)
                        If p.pImgRaw Is Nothing Then
                            p.pImgDsp = Nothing
                            p.pImgRaw = Nothing

                            p.emStatus = CLS_IMG_INF.EM_STATUS.em_OK
                            Return True
                        End If



                        ' 生画像生成イベントを起こす。
                        RaiseEvent tEv_CreatingImg(p, p.pImgRaw)


                        ' 強制中断確認
                        If p.bDele Then Return False


                        '===================================================
                        '''' 表示エリアに合わせた画像を生成（縮小）
                        Dim szNewSize As New Size(p.nDspW, p.nDspH)
                        p.pImgDsp = m_Resizer.ImgReSize(p.pImgRaw, szNewSize, p.bImgDspSizeLimit)

                    Catch ex As Exception

                        p.pImgDsp = LoadErrorBitmap()
                        p.pImgRaw = LoadErrorBitmap()

                        p.pImgDsp.Tag = ex
                        p.pImgRaw.Tag = ex

                    End Try


                    '===================================================
                    '' エンド処理
                    ' 強制中断確認

                    ' 非同期操作完了
                    p.emStatus = CLS_IMG_INF.EM_STATUS.em_OK

                    Return True
                    ' これがあれば、メモリはぜんぜん食わない。ただし、生画像を消すので、拡大不可能となる。
                    'p.pImgRaw.Dispose()
                    'p.pImgRaw = Nothing
                End SyncLock
            End Function

            ''' <summary>
            ''' フレームデータを読み込み、展開し、Bitmapをかえす。
            ''' </summary>
            ''' <param name="nFrame">展開対象フレーム</param>
            ''' <returns></returns>
            Public Function DecompressFrame(nFrame As Integer) As Bitmap

                ''Dim timerDecode As New System.Diagnostics.Stopwatch
                ''timerDecode.Start()
                Dim pImgRaw As Bitmap = Nothing

                '===================================================
                ' 圧縮データの読み込み
                Dim inf As tClass_ReadRPI.RPI_FRAME_INF = mcls_pReadRpi.GetRpiFrameInf(nFrame)
                Dim byCompressData(inf.img_size - 1) As Byte            ' 圧縮データ
                Dim img_size As Integer = mcls_pReadRpi.ReadFile_ImgByte_ActiveCh(nFrame, byCompressData)

                If img_size = 0 Then
                    Return Nothing
                End If


                '===================================================
                ' 生画像データに変換
                Dim bDecodeComplete As Boolean = False

                ' RpiOpen時点で、サポート対象画像であればm_active_decoderはセットされる
                If m_active_decoder IsNot Nothing Then
                    bDecodeComplete = m_active_decoder.Decompress(pImgRaw, byCompressData, mcls_pReadRpi)
                End If


                '内蔵していない圧縮方法はプラグイン側にまかせる
                If Not bDecodeComplete Then
                    RaiseEvent tEv_DecodeImage(pImgRaw, byCompressData, img_size, mcls_pReadRpi.GetRpiHeader)
                End If

                '結局、展開できなかった
                If pImgRaw Is Nothing Then
                    Dim strCodec As String = tClass_ReadRPI.GetString(mcls_pReadRpi.GetRpiHeader.emCompType)
                    If mcls_pLog IsNot Nothing Then
                        mcls_pLog.Write(tClass_LogManager.LNO.WAR, String.Format("[{0}] 圧縮形式不明 [{1}]", m_sThreadName, strCodec))
                    End If

                    Throw New InvalidOperationException("圧縮形式不明:" & strCodec)
                End If

                ''timerDecode.Stop()
                Return pImgRaw
            End Function

        End Class
    End Class

End Namespace