'*********************************************************************************
'	テキストファイルにログ出力を行う基底クラス
'	[Ver]
'		Ver.01	2007/03/08	vs2005 対応 (昔のtClass_Logを全面作り直し)
'
'	[メモ]
'		Logファイル書込みに特化したクラス
'*********************************************************************************
Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Imports System.IO                               ' IO操作

'#Const tDEBUG = False
''#Const VBC_VER = 1

Namespace tClass
    ''' <summary>
    ''' テキストファイルにログ出力を行う基底クラス
    ''' </summary>
    Public Class tClass_LogManager
        Implements IDisposable							' デストラクタ定義

        '********************************************************************
        '公開変数定義
        '********************************************************************
        ''' <summary>
        ''' ログ分類(iniファイルのOutModeで出力の有無を制御可能)
        ''' </summary>
        ''' <remarks></remarks>
        Public Enum LNO
            ''' <summary>無し</summary>
            NON = 0
            ''' <summary>☆ ガイダンス時に指定</summary>
            MSG
            ''' <summary>★ エラー時に指定</summary>
            ERR
            ''' <summary>○ 基本クラス等でのガイダンス時に指定</summary>
            INF
            ''' <summary>● 基本クラス等でのエラー時に指定</summary>
            WAR
        End Enum


        Private Const MAX_ROW_NUM As Integer = 20000		' 1ファイルあたりの行数
        Private Const MAX_LISTBOX As Integer = 200			' リストボックス表示行数
        Private Const MAX_QUE As Integer = 512				' ログファイル最大数


        '********************************************************************
        'メンバー変数定義
        '********************************************************************
        Private disposedValue As Boolean = False		' 重複する呼び出しを検出するには
#If VBC_VER >= 8.0 Then							' Generic に対応しているバージョン時
        Private mque_Log As tClass_Queue(Of String)			' ログ情報をセットするリスト要素(キュークラス)
#Else
        Private mque_Log As tClass_Queue					' ログ情報をセットするリスト要素(キュークラス)
#End If
        Private m_wThread As tClass_WorkThread				' 書込み用スレッド
        Private m_Encoding As System.Text.Encoding			' エンコーディング
        Private m_Stream As System.IO.StreamWriter			' ストリーム

        Private m_cntList As Windows.Forms.ListBox			' リストボックスコントロールオブジェクト
        Private m_bFileOut As Boolean						' ファイル出力 (true:ファイル出力有り)
        Private m_bListOut As Boolean						' リストボックスコントロール出力 (true:リスト出力有り)


        'ログファイル関連
        Private m_cBaseFolder As String						' ベースのフォルダ
        Private m_cBaseFile As String						' ベースのファイル名
        Private m_cNowFPath As String						' 現在出力中のファイルフルパス

        Private m_nNowFileNo As Integer						' ファイル数
        Private m_nNowRow As Integer						' 行数
        Private m_nMaxCycleCnt As Integer					' 最大サイクリックファイル件数
        Private m_nMaxBackCnt As Integer					' 最大バックアップ履歴数

        ' ListBox追加用
        Private Delegate Sub mDlgt_ListAdd(ByVal lst As Windows.Forms.ListBox, ByVal strLog() As String)	'リストボックス書き込み用非同期デリゲート


        '********************************************************************
        'プロパティ定義
        '********************************************************************
        ''' <summary>
        ''' リストボックスへの出力有無
        ''' </summary>
        ''' <value>リスト表示無しにする場合は、Nothingを指定する</value>
        ''' <remarks></remarks>
        Public WriteOnly Property tProp_SetListOut() As Windows.Forms.ListBox
            Set(ByVal value As Windows.Forms.ListBox)
                If value Is Nothing Then
                    m_bListOut = False
                Else
                    m_bListOut = True
                End If
                m_cntList = value
            End Set
        End Property
        ''' <summary>
        ''' ファイル出力有無
        ''' </summary>
        ''' <value>リスト表示無しにする場合は、Nothingを指定する</value>
        ''' <remarks></remarks>
        Public WriteOnly Property tProp_SetFileOut() As Boolean
            Set(ByVal value As Boolean)
                m_bFileOut = value
            End Set
        End Property
        ''' <summary>
        ''' 現在のファイルフルパスを出力
        ''' </summary>
        Public ReadOnly Property tProp_GetNowLogPath() As String
            Get
                Return m_cNowFPath
            End Get
        End Property

        '********************************************************************
        'メソッド定義
        '********************************************************************
        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <param name="filename">ファイル名 (通常タスク名)  (Nothing時ログ出力しない)</param>
        ''' <param name="folder">フォルダパス指定 (最後の\必須) (デフォルト指定の時は、NULLでOK)</param>	
        ''' <param name="list">ListBoxコントロール (Nothing時リスト出力しない)</param>
        ''' <param name="cycleCnt">繰り返しファイル数</param>
        ''' <param name="backCnt">バックアップ個数</param>				
        ''' <param name="encoding">出力エンコード</param>
        ''' <remarks>バイナリ出力の場合は,System.Text.Encoding.Unicode辺りを指定</remarks>
        Public Sub New(ByVal filename As String, Optional ByVal folder As String = "", Optional ByVal list As Windows.Forms.ListBox = Nothing, Optional ByVal cycleCnt As Integer = 3, Optional ByVal backCnt As Integer = 4, Optional ByVal encoding As System.Text.Encoding = Nothing)
            '' メンバ変数セット
            m_nMaxCycleCnt = cycleCnt
            m_nMaxBackCnt = backCnt
            If encoding Is Nothing Then
                'ウィンドウズとVSの文字形式が異なる為、明示的にエンコーディングを指定
                m_Encoding = System.Text.Encoding.GetEncoding("shift-jis")
            Else
                m_Encoding = encoding
            End If
            '' 出力するしない決定
            tProp_SetFileOut = CBool(IIf(filename = "", False, True))
            tProp_SetListOut = list

            '' バックアップ (ファイル出力有り時のみ)
            If m_bFileOut Then
                Dim ii As Integer
                Dim jj As Integer
                Dim cFileName As String
                Dim cFileNameNew As String

                ''フォルダ有無
                If folder = "" Then folder = ".\LOG\"
                If Not IO.Directory.Exists(folder) Then
                    IO.Directory.CreateDirectory(folder)
                End If
                m_cBaseFolder = folder
                m_cBaseFile = filename


                '' bk_xxx_@9_0.log ～ bk_xxx_@9_9.logを削除  (最古ファイル削除)
                For ii = 0 To m_nMaxCycleCnt - 1
                    cFileName = m_cBaseFolder & "bk_" & m_cBaseFile & "_@" & m_nMaxBackCnt & "_" & ii & ".log"
                    IO.File.Delete(cFileName)
                Next ii

                '' bk_xxx_@0_0.log ～ bk_xxx_@0_9.log ⇒ bk_xxx_@1_0.log ～ bk_xxx_@1_9.log に変更 (バックアップファイルリネーム)
                For ii = 0 To m_nMaxCycleCnt - 1
                    For jj = m_nMaxBackCnt To 1 Step -1
                        cFileName = m_cBaseFolder & "bk_" & m_cBaseFile & "_@" & jj - 1 & "_" & ii & ".log"		' 変換元
                        cFileNameNew = m_cBaseFolder & "bk_" & m_cBaseFile & "_@" & jj & "_" & ii & ".log"	   ' 変換先

                        IO.File.Delete(cFileNameNew)			' 移動先に既にファイルがあるとエラーになるから一度消す
                        If IO.File.Exists(cFileName) Then IO.File.Move(cFileName, cFileNameNew) '移動元のファイルがある場合のみ
                    Next jj
                Next ii

                '' xxx_0.log～xxx_9.log ⇒ bk_xxx_@0_0.log ～ bk_xxx_@0_9.log に変更 (最新ファイルをバックアップファイルにリネーム)
                For ii = 0 To m_nMaxCycleCnt - 1
                    cFileName = m_cBaseFolder & m_cBaseFile & "_" & ii & ".log"		  ' 変換元
                    cFileNameNew = m_cBaseFolder & "bk_" & m_cBaseFile & "_@0_" & ii & ".log"	   ' 変換先

                    IO.File.Delete(cFileNameNew)			' 移動先に既にファイルがあるとエラーになるから一度消す
                    If IO.File.Exists(cFileName) Then IO.File.Move(cFileName, cFileNameNew) '移動元のファイルがある場合のみ
                    'IO.File.Move(cFileName, cFileNameNew)
                Next ii


                '' ログファイルオープン
                cFileName = String.Format("{0}{1}_{2}.log", m_cBaseFolder, m_cBaseFile, m_nNowFileNo)
                m_Stream = New StreamWriter(cFileName, False, m_Encoding)
                m_Stream.AutoFlush = True
                m_cNowFPath = cFileName
            End If

            '''' インスタンス生成
#If VBC_VER >= 8.0 Then							' Generic に対応しているバージョン時
            mque_Log = New tClass_Queue(Of String)(MAX_QUE)
#Else
            mque_Log = New tClass_Queue(MAX_QUE)
#End If
            m_wThread = New tClass_WorkThread

            '''' メインスレッド実行
            'ちょっと優先度を低めに設定 
            '(高めに設定すると キューにあまり溜まらず すごい回数のファイルオープンクローズが発生する為)
            m_wThread.ThreadStart(AddressOf mainThread, Threading.ThreadPriority.Lowest, "tClass_LogManager", True)
        End Sub

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: 明示的に呼び出されたときにアンマネージ リソースを解放します
                End If

                ' TODO: 共有のアンマネージ リソースを解放します
                Try
                    '' インスタンス確認
                    If Not m_wThread Is Nothing Then
                        '' メインスレッド終了
                        m_wThread.ThreadStop(500)
                        m_wThread.Dispose()
                        m_wThread = Nothing
                    End If
                    mque_Log.Dispose()
                    If Not m_Stream Is Nothing Then
                        m_Stream.Close()
                        m_Stream.Dispose()
                        m_Stream = Nothing
                    End If
                Catch ex As Exception
                Finally
                End Try
            End If
            Me.disposedValue = True
        End Sub

#Region " IDisposable Support "
        ''' <summary>
        ''' Disposeデストラクタ
        ''' </summary>
        ''' <remarks>使用後、必ずコールする事</remarks>
        Public Sub Dispose() Implements IDisposable.Dispose
            ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region

        ''' <summary>
        ''' Finalizeデストラクタ
        ''' </summary>
        ''' <remarks></remarks>
        Protected Overrides Sub Finalize()
            Dispose()
            MyBase.Finalize()
        End Sub

        ''' <summary>
        ''' シスログ書き込み
        ''' </summary>
        ''' <param name="no">出力LNO</param>
        ''' <param name="strMsg">メッセージ</param>
        ''' <remarks>非同期書き込み</remarks>
        Public Overloads Sub Write(ByVal no As LNO, ByVal strMsg As String)
            Dim strLog As String
            Dim strTemp As String

            '' シンボル付加
            Select Case no
                Case LNO.NON
                    strTemp = ""
                Case LNO.MSG
                    strTemp = "☆ "
                Case LNO.ERR
                    strTemp = "★ "
                Case LNO.INF
                    strTemp = "○ "
                Case LNO.WAR
                    strTemp = "● "
                Case Else
                    strTemp = ""
            End Select

            '' 日付付加
            strLog = String.Format("{0:yyyy/MM/dd HH:mm:ss.fff} {1}{2}", Now(), strTemp, strMsg)
            '' キューに登録
            If 0 = mque_Log.GetFreeCount() Then		'空き無しの為 ログ破棄
                mque_Log.Free()

                strLog = String.Format("{0:yyyy/MM/dd HH:mm:ss.fff} {1}{2}", Now(), "● ", "ログキューフル！！")
            End If

            '' 非同期キューに追加
            mque_Log.SetItem(strLog)
        End Sub


        ''' <summary>
        ''' メインスレッド
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub mainThread()
            Dim EventAry(1) As Threading.WaitHandle		'イベント
            Dim iRetc As Integer						'WaitAny 復帰情報

            '''' シグナルセット
            EventAry(0) = m_wThread.g_evStop			' スレッド終了
            EventAry(1) = mque_Log.g_evSem				' キュー有り

            Try
                While (True)
                    '' シグナル待ち
                    iRetc = Threading.WaitHandle.WaitAny(EventAry)

                    '' まず終了確認
                    Select Case iRetc
                        Case 0
                            tProp_SetListOut = Nothing		'最後の為 リスト表示は無効
                            '' 最後にキューに溜まっているログを強制的に吐き出す
                            LogSet()
                            Exit While

                        Case 1

                            ''Logファイルに追加
                            LogSet()
                        Case Else
                            'ありえない
                    End Select
                End While
            Catch ex As Threading.ThreadAbortException
            Catch ex As Exception
            End Try
        End Sub

        ''' <summary>
        ''' シスログに書き込み
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub LogSet()
            '' キューに溜まっている全てのデータ取得
#If VBC_VER >= 8.0 Then							' Generic に対応しているバージョン時
            Dim objLog() As String		'ログ配列
            objLog = mque_Log.GetAll
            If objLog Is Nothing Then Exit Sub
#Else
            Dim o() As Object
            o = mque_Log.getAll()
            If o Is Nothing Then Exit Sub
            ''string配列 に変換
            Dim objLog(o.Length - 1) As String      'ログ配列
            For ii As Integer = 0 To o.Length - 1
                objLog(ii) = CStr(o(ii))
            Next ii
#End If


            '' リストボックス追加
            If m_bListOut Then
                Try
                    Dim args(1) As Object
                    args(0) = m_cntList
                    args(1) = objLog
                    m_cntList.BeginInvoke(New mDlgt_ListAdd(AddressOf lstWrite), args)
                Catch ex As Exception

                End Try
            End If

            '' ファイル出力
            If m_bFileOut Then WriteFile(objLog)
        End Sub


        ''' <summary>
        ''' ファイル出力
        ''' </summary>
        ''' <param name="strLog"></param>
        ''' <remarks></remarks>
        Private Sub WriteFile(ByVal strLog() As String)
            For ii As Integer = 0 To strLog.Length - 1
                'ファイル出力
                m_Stream.WriteLine(strLog(ii))
                m_nNowRow += 1

                'ファイル切り替え
                If m_nNowRow > MAX_ROW_NUM Then
                    m_Stream.Close()
                    m_Stream.Dispose()
                    m_Stream = Nothing

                    m_nNowRow = 0
                    m_nNowFileNo += 1
                    If m_nNowFileNo > m_nMaxCycleCnt Then m_nNowFileNo = 0
                    Dim cFileName As String = String.Format("{0}{1}_{2}.log", m_cBaseFolder, m_cBaseFile, m_nNowFileNo)
                    m_Stream = New StreamWriter(cFileName, False, m_Encoding)
                    m_Stream.AutoFlush = True
                    m_cNowFPath = cFileName
                End If
            Next ii
        End Sub


        '********************************************************************
        ' リストボックス追加用
        '********************************************************************
            ''' <summary>
        ''' 非同期書き込み
        ''' </summary>
        ''' <param name="lst">書き込み対象コントロール</param>
        ''' <param name="strLog">追加ログ配列</param>
        ''' <remarks></remarks>
        Private Sub lstWrite(ByVal lst As Windows.Forms.ListBox, ByVal strLog() As String)
            Dim ii As Integer
            Dim delCnt As Integer	'削除行数
            Dim addItem() As String
            Try
                '''' パフォーマンス対策 第1段
                If MAX_LISTBOX < strLog.Length Then		'最大表示行数より追加行数の方が多い場合
                    ReDim addItem(MAX_LISTBOX - 1)
                    Array.Copy(strLog, strLog.Length - MAX_LISTBOX, addItem, 0, MAX_LISTBOX)
                    delCnt = MAX_LISTBOX
                Else
                    addItem = strLog
                    If lst.Items.Count >= MAX_LISTBOX Then		'最大件数到達済み
                        delCnt = strLog.Length
                    Else										'最大件数未到達
                        delCnt = strLog.Length - (MAX_LISTBOX - lst.Items.Count)
                        If delCnt < 0 Then delCnt = 0
                    End If
                End If

                '''' パフォーマンス対策 第2段
                lst.BeginUpdate()
                'まず削除
                If delCnt = MAX_LISTBOX Then
                    lst.Items.Clear()
                Else
                    For ii = 1 To delCnt
                        lst.Items.RemoveAt(0)
                    Next
                End If
                '追加
                lst.Items.AddRange(addItem)
                '最新行を選択状態に変更
                lst.SelectedIndex = lst.Items.Count - 1
                lst.EndUpdate()
            Catch ex As Exception

            End Try
        End Sub

        ''' <summary>
        ''' メモ帳起動
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ShowEdit()
            Dim strCmd As String = "notepad.exe " & m_cNowFPath
            Shell(strCmd, AppWinStyle.NormalFocus, False)
        End Sub


    End Class
End Namespace