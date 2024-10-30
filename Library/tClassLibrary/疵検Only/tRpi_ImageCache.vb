
'/ ############################################################################################################################
'/ データ管理クラス
'/ ############################################################################################################################
Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Option Explicit On

Imports System.Windows.Forms
Imports System.Drawing
Imports tClassLibrary.RpiPlugin

Namespace tClass
    Partial Public Class tClass_ViewRpiThread

        ''' <summary>
        ''' データの管理クラス
        ''' </summary>
        ''' <remarks></remarks>
        Friend Class lCls_ImageCache
            ' メイン
            Public g_lstImg As New Dictionary(Of Integer, CLS_IMG_INF)              ' データ管理用リスト (ｺｲﾙ先頭からのﾌﾚｰﾑ位置(1ｵﾘｼﾞﾝ))

            ' 外部セットクラス
            Private mcls_pLog As tClass_LogManager                                  ' ログ管理クラス (ポインタ)
            Private mcls_pReadRpi As tClass.tClass_ReadRPI = Nothing                        ' RPIファイル読込クラス (実体なし)
            Private mcls_pImg As tClass_ImageDsp = Nothing                          ' 画像表示クラス  (実体なし)
            Private mque_pMultiExec As tClass_Queue(Of CLS_IMG_INF) = Nothing            ' 並列操作用キュー (実体なし)


            '===================================================
            ' 外部直接公開用


            '===================================================
            ' メンバー
            Private m_cs As New Object                                              ' SyncLock同期オブジェクト(クリティカルセクション)
            Private m_nListMaxNum As Integer = 20                                   ' 最大リスト数 (画面表示数+先読み数×2 よりも大きくすること)
            Private m_nResvNum() As Integer = {5, 10}                               ' 先読みフレーム数

            Private m_nDspImgSize As Size                                           ' 表示画像サイズ



            '/ /////////////////////////////////////////////////////////////////////////////////
            '/ 縮小画像と表示領域が1pixとかズレて、描画がbitbiltにならない対策

            ''' <summary>
            ''' フレームNOの表示領域を求めるメソッド
            ''' </summary>
            ''' <param name="RecX">幅方向のフレーム画像番号</param>
            ''' <param name="RecY">絶対フレームNo(0オリジン)</param>
            ''' <returns>表示座標系の、表示領域</returns>
            ''' <remarks></remarks>
            Public Delegate Function GetAnsOneImgSizeType(ByVal RecX As Integer, ByVal RecY As Integer) As Rectangle
            Private m_dgGetAnsOneImgSizeType As GetAnsOneImgSizeType = Nothing      '可変縮小サイズメソッド (nothingのままなら昔バージョン)

            ''' <summary>
            ''' フレームNO毎に縮小画素数かえるための関数を設定する。
            ''' </summary>
            ''' <param name="dgGetAnsOneImgSizeType">指定したNoのフレームの、表示領域を求めて返すメソッド</param>
            ''' <remarks>Nothingのときはm_nDspImgSizeで固定に縮小</remarks>
            Public Sub SetupImageSizeMethod(ByVal dgGetAnsOneImgSizeType As GetAnsOneImgSizeType)
                m_dgGetAnsOneImgSizeType = dgGetAnsOneImgSizeType
            End Sub






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
            ''' 画像表示
            ''' </summary>
            Public Sub SetCls_Img(ByVal cls As tClass_ImageDsp)
                mcls_pImg = cls
            End Sub
            ''' <summary>
            ''' 並列操作用キュー
            ''' </summary>
            Public Sub SetQue_MultiExec(ByVal que As tClass_Queue(Of CLS_IMG_INF))
                mque_pMultiExec = que
            End Sub
            ''' <summary>
            ''' 先読みフレーム数
            ''' </summary>
            Public Property tProp_ResvNum(ByVal nIndex As Integer) As Integer
                Get
                    Return m_nResvNum(nIndex)
                End Get
                Set(ByVal value As Integer)
                    m_nResvNum(nIndex) = value
                End Set
            End Property


            '/ /////////////////////////////////////////////////////////////////////////////////
            '/ 外部公開用
            '/ /////////////////////////////////////////////////////////////////////////////////

            ''' <summary>
            ''' 表示データが揃ったかチェック
            ''' </summary>
            ''' <param name="nFNoS">表示開始FNo (1オリジン)</param>
            ''' <param name="nFNoE">表示終了FNo (1オリジン)</param>
            Public Function CheckDispImg(ByVal nFNoS As Integer, ByVal nFNoE As Integer) As Boolean

                For ii As Integer = nFNoS To nFNoE
                    If 1 > ii Then Continue For

                    If Not g_lstImg.ContainsKey(ii) Then Continue For
                    If g_lstImg(ii).bCompleted = -1 Then Continue For



                    If System.Threading.Thread.VolatileRead(g_lstImg(ii).bCompleted) = 0 Then
                        Return False
                    End If
                Next
                Return True
            End Function

            ''' <summary>
            ''' 表示するからデータ揃えてね
            ''' </summary>
            ''' <param name="nFNoS">表示開始FNo (1オリジン)</param>
            ''' <param name="nFNoE">表示終了FNo (1オリジン)</param>
            ''' <param name="nMode">前回からの動作状況 (0:不明  1:先に進んだ  -1:戻った  2:大きく移動した)</param>
            ''' <returns>全部正常に揃っている場合 true</returns>
            ''' <remarks></remarks>
            Public Function Disp(ByVal nFNoS As Integer, ByVal nFNoE As Integer, Optional ByVal nMode As Integer = 0) As Boolean

                '===================================================
                '''' 前準備
                Dim nResvFNoS As Integer = nFNoS - m_nResvNum(0)
                Dim nResvFNoE As Integer = nFNoE + m_nResvNum(1)
                If nResvFNoS < 1 Then nResvFNoS = 1
                If nResvFNoE > mcls_pReadRpi.GetRpiHeader.nframe Then nResvFNoE = mcls_pReadRpi.GetRpiHeader.nframe

                If Not mcls_pLog Is Nothing Then mcls_pLog.Write(tClass_LogManager.LNO.WAR, String.Format("DISP開始 {0}～{1}⇔{2}～{3}", nResvFNoS, nFNoS, nFNoE, nResvFNoE))


                ' 範囲外のデータは すべて消す
                Call Delete(nResvFNoS, nResvFNoE, nMode)


                '===================================================
                '''' 表示エリアの画像取得 非同期依頼
                For ii As Integer = nFNoS To nFNoE
                    If g_lstImg.ContainsKey(ii) Then Continue For

                    '' バッファ内に画像が無いので、新規に取得
                    Call Add(ii)
                Next

                If Not mcls_pLog Is Nothing Then mcls_pLog.Write(tClass_LogManager.LNO.WAR, String.Format("DISP 待ち"))


                '===================================================
                '''' 表示エリアの画像だけは 非同期完了までブロック
                Dim bAllOk As Boolean = CheckDispImg(nFNoS, nFNoE)


                '===================================================
                '''' 先読みの画像取得 非同期依頼
                For ii As Integer = nFNoE + 1 To nResvFNoE
                    If g_lstImg.ContainsKey(ii) Then Continue For

                    '' バッファ内に画像が無いので、新規に取得
                    Call Add(ii)
                Next
                For ii As Integer = nFNoS - 1 To nResvFNoS Step -1
                    If g_lstImg.ContainsKey(ii) Then Continue For

                    '' バッファ内に画像が無いので、新規に取得
                    Call Add(ii)
                Next

                If Not mcls_pLog Is Nothing Then mcls_pLog.Write(tClass_LogManager.LNO.WAR, String.Format("DISP 完了"))
                Return bAllOk
            End Function


            ''' <summary>
            ''' 要素削除
            ''' </summary>
            ''' <param name="nFNoS">対象外の開始FNo (1オリジン)</param>
            ''' <param name="nFNoE">対象外終了FNo (1オリジン)</param>
            ''' <param name="nMode">前回からの動作状況 (0:不明  1:先に進んだ  -1:戻った  2:大きく移動した)</param>
            ''' <remarks></remarks>
            Public Sub Delete(ByVal nFNoS As Integer, ByVal nFNoE As Integer, ByVal nMode As Integer)

                '===================================================
                '''' 削除対象選択
                Dim nDelPer As Integer = 0                  ' 前
                Dim nDelNext As Integer = 0                 ' 先	

                Dim lstSyncDel As New List(Of Integer)          ' ここで削除するもの
                Dim lstSyncRemove As New List(Of Integer)       ' リストから削除するだけのもの
                Dim bDele As Boolean = False



                Dim nowImgDsp() As Image = mcls_pImg.GetImage()


                Dim v As Dictionary(Of Integer, CLS_IMG_INF).ValueCollection = g_lstImg.Values
                For Each p As CLS_IMG_INF In v
                    '' △ 今画面表示中の情報は開放させない
                    For ii As Integer = 0 To nowImgDsp.Length - 1
                        If nowImgDsp(ii) Is p.pImgDsp Or nowImgDsp(ii) Is p.pImgRaw Then
                            Continue For
                        End If
                    Next


                    '' 明らかに変な奴は削除
                    If p.emStatus = CLS_IMG_INF.EM_STATUS.em_NON Then
                        ' 同期削除
                        lstSyncDel.Add(p.nIndex)
                        Continue For
                    End If

                    '' 以降、普通のやつ
                    If p.nIndex >= nFNoS And p.nIndex <= nFNoE Then Continue For

                    '' 非同期実行中の削除対象のものは、並列スレッド内で処理を中断させる
                    If p.bSyncExce Then
                        p.bDele = True
                        lstSyncRemove.Add(p.nIndex)
                        If Not mcls_pLog Is Nothing Then mcls_pLog.Write(tClass_LogManager.LNO.WAR, String.Format("リスト削除 [{0}]", p.nIndex))
                        Continue For
                    Else
                        ' 同期削除
                        lstSyncDel.Add(p.nIndex)
                    End If


                    '' 既に完成しているアイテムは、ここで消す
                    If nDelPer = 0 OrElse nDelPer > p.nIndex Then nDelPer = p.nIndex
                    If nDelNext = 0 OrElse nDelNext < p.nIndex Then nDelNext = p.nIndex
                Next

                '===================================================
                '''' 削除
                If 1 = nMode Then               ' 先に進んだ = 前のやつを削除
                    If 0 <> nDelPer AndAlso g_lstImg.ContainsKey(nDelPer) Then
                        g_lstImg(nDelPer).Dispose()
                        g_lstImg.Remove(nDelPer)        ' リストから削除
                        bDele = True
                    End If
                ElseIf -1 = nMode Then          ' 前に戻った = 先のやつを削除
                    If 0 <> nDelNext AndAlso g_lstImg.ContainsKey(nDelNext) Then
                        g_lstImg(nDelNext).Dispose()
                        g_lstImg.Remove(nDelNext)       ' リストから削除
                        bDele = True
                    End If
                End If

                For ii As Integer = 0 To lstSyncDel.Count - 1
                    If g_lstImg.ContainsKey(lstSyncDel(ii)) Then
                        Dim p As CLS_IMG_INF = g_lstImg(lstSyncDel(ii))
                        If mcls_pLog IsNot Nothing Then mcls_pLog.Write(tClass_LogManager.LNO.WAR, String.Format("要素開放＆リスト削除 [{0}]", p.nIndex))
                        p.Dispose()                 ' 要素の開放
                        g_lstImg.Remove(lstSyncDel(ii))     ' リストから削除
                        bDele = True
                    End If
                Next

                For ii As Integer = 0 To lstSyncRemove.Count - 1
                    If g_lstImg.ContainsKey(lstSyncRemove(ii)) Then
                        g_lstImg.Remove(lstSyncRemove(ii))      ' リストから削除
                        ' スレッドに要素の開放依頼 は、同期とかが難しいからしない。生成と削除が別スレッドで同時実行されたら厄介。GCに後始末は任せる
                    End If
                Next

                If bDele Then GC.Collect()
            End Sub

            ''' <summary>
            ''' 要素完全削除
            ''' </summary>
            Public Sub Init()
                Call Delete(-1, -1, 0)
                ' 念のため
                g_lstImg.Clear()
            End Sub


            ''' <summary>
            ''' リストにこのフレームNOを追加する
            ''' </summary>
            ''' <param name="nIndex">ｺｲﾙ先頭からのﾌﾚｰﾑ位置(1ｵﾘｼﾞﾝ)</param>
            ''' <remarks></remarks>
            Public Sub Add(ByVal nIndex As Integer)
                '	SyncLock m_cs
                '''' すでに有る？
                If g_lstImg.ContainsKey(nIndex) Then Return

                If Not mcls_pLog Is Nothing Then mcls_pLog.Write(tClass_LogManager.LNO.INF, String.Format("要素追加 [{0}]", nIndex))

                '''' 新規追加
                Dim p As New CLS_IMG_INF
                p.nIndex = nIndex


                '' 縮小画像サイズを算出
                If m_dgGetAnsOneImgSizeType Is Nothing Then
                    ' 旧バージョン
                    p.nDspW = m_nDspImgSize.Width
                    p.nDspH = m_nDspImgSize.Height
                Else
                    ' 対策版
                    Dim rcRect As Rectangle = m_dgGetAnsOneImgSizeType(0, nIndex - 1)       ' 実体は、tClass_ImageDspBase.GetAnsOneImgSize
                    p.nDspW = rcRect.Width
                    p.nDspH = rcRect.Height
                End If



                p.emStatus = CLS_IMG_INF.EM_STATUS.em_NOMAL
                System.Threading.Thread.VolatileWrite(p.bCompleted, 0)
                p.bSyncExce = True
                p.bDele = False
                g_lstImg.Add(nIndex, p)     ' リストに追加

                '' 並列処理に読込み依頼
                mque_pMultiExec.SetItem(p)
            End Sub


            ''' <summary>
            ''' サイズ変更
            ''' </summary>
            ''' <param name="nW">新サイズ</param>
            ''' <param name="nH">新サイズ</param>
            ''' <remarks></remarks>
            Public Sub ChangeDspImgSize(ByVal nW As Integer, ByVal nH As Integer)
                m_nDspImgSize.Width = nW
                m_nDspImgSize.Height = nH
            End Sub
        End Class
    End Class
End Namespace
