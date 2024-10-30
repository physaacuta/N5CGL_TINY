'*********************************************************************************
'同期を行ったQueueの基底クラス
'	[Ver]
'		Ver.01    2006/03/04  vs2005 対応
'
'	[メモ]
'		セマフォーを使ったQueueクラス (vs2005専用)
'*********************************************************************************

Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

#If VBC_VER >= 8.0 Then
Namespace tClass
    ''' <summary>
    ''' 同期を行ったQueueの基底クラス [Generic対応]
    ''' </summary>
    ''' <typeparam name="T">[Generic] 要素の型 (インスタンスを生成する場合に (Of String) 等指定する)</typeparam>
    ''' <remarks>キューに溜まっている間 g_hEvent が シグナルとなっている</remarks>
    <System.Serializable()> _
    Public Class tClass_Queue(Of T)						' Genericに対応
        Implements IDisposable							' デストラクタ定義


        '********************************************************************
        '公開変数 定義
        '********************************************************************
        ''' <summary>
        ''' セマフォー
        ''' </summary>
        ''' <remarks>キューが有る:シグナル キュー無し:非シグナル</remarks>
        Public g_evSem As System.Threading.Semaphore		' セマフォー


        '********************************************************************
        'ローカル変数定義
        '********************************************************************
        Private disposedValue As Boolean = False			' 重複する呼び出しを検出するには
        Protected Friend my_pQue As Queue(Of T)				' 要素リスト

        Protected Friend my_cs As Object					' SyncLock同期オブジェクト
        Protected Friend my_nSemCount As Integer			' セマフォサイズ
        Protected Friend my_nMaxCount As Integer			' 瞬間最大登録件数


        '********************************************************************
        'プロパティ
        '********************************************************************
        ''' <summary>
        ''' 現在の登録件数
        ''' </summary>
        ''' <returns>現在の登録件数</returns>
        Public ReadOnly Property GetCount() As Integer
            Get
                Return my_pQue.Count
            End Get
        End Property

        ''' <summary>
        ''' 最大登録可能数
        ''' </summary>
        ''' <returns>最大登録可能数</returns>
        Public ReadOnly Property GetSemCount() As Integer
            Get
                Return my_nSemCount
            End Get
        End Property

        ''' <summary>
        ''' 今までの最大登録件数
        ''' </summary>
        ''' <returns>今までの最大登録件数</returns>
        Public ReadOnly Property GetMaxCount() As Integer
            Get
                Return my_nMaxCount
            End Get
        End Property
        ''' <summary>
        ''' 瞬間最大登録件数初期化
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ReSetMaxCount()
            my_nMaxCount = 0
        End Sub
        ''' <summary>
        ''' 残登録件数
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public ReadOnly Property GetFreeCount() As Integer
            Get
                Return GetSemCount() - GetCount()
            End Get
        End Property

        '********************************************************************
        'メソッド定義
        '********************************************************************
        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <param name="nSemCnt">最大登録可能数</param>
        ''' <remarks></remarks>
        Public Sub New(Optional ByVal nSemCnt As Integer = 256)
            '' 初期化
            my_nSemCount = nSemCnt
            my_nMaxCount = 0

            '' インスタンス生成
            my_pQue = New Queue(Of T)
            my_cs = New Object
            g_evSem = New Threading.Semaphore(0, my_nSemCount)
        End Sub

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: 明示的に呼び出されたときにアンマネージ リソースを解放します
                End If

                ' TODO: 共有のアンマネージ リソースを解放します
                g_evSem.Close()
                g_evSem = Nothing
                my_pQue.Clear()
                my_pQue = Nothing
                my_cs = Nothing
            End If
            Me.disposedValue = True
        End Sub

#Region " IDisposable Support "
        ' このコードは、破棄可能なパターンを正しく実装できるように Visual Basic によって追加されました。
        Public Sub Dispose() Implements IDisposable.Dispose
            ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region

        ''' <summary>
        ''' キューに要素を追加
        ''' </summary>
        ''' <param name="gnrT">[Generic] 追加する要素</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks>追加する要素がインスタンスの場合、使用後かならず開放を行う事</remarks>
        Public Overloads Function SetItem(ByVal gnrT As T) As Boolean
            SyncLock my_cs
                If my_pQue.Count() >= my_nSemCount Then Return False

                my_pQue.Enqueue(gnrT)		'キューにデータ追加
                If my_pQue.Count() >= my_nMaxCount Then my_nMaxCount = my_pQue.Count() '最大登録件数更新
                g_evSem.Release()			'セマフォー登録
                Return True
            End SyncLock
        End Function

        ''' <summary>
        ''' キューに要素を追加 (登録されるまでブロック)
        ''' </summary>
        ''' <param name="gnrT">[Generic] 追加する要素</param>
        ''' <param name="retryCnt">待ち回数</param>
        ''' <param name="waitTime">待ち時間</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Function SetItem(ByVal gnrT As T, ByVal retryCnt As Integer, Optional ByVal waitTime As Integer = 1) As Boolean
            '' 登録されるまで繰り返し
            For ii As Integer = 0 To retryCnt
                If setItem(gnrT) Then
                    Return True
                Else
                    '' 登録待ち
                    Threading.Thread.Sleep(waitTime)
                End If
            Next ii
            Return False
        End Function

        ''' <summary>
        ''' キューから要素を取得(セマフォーは減らない)
        ''' </summary>
        ''' <returns>要素</returns>
        ''' <remarks>インスタンスの場合は使用後,必ず開放を行う事</remarks>
        Public Overloads Function GetItem() As T
            SyncLock my_cs
                If my_pQue.Count() <= 0 Then
                    Return Nothing
                End If

                Return my_pQue.Dequeue()
            End SyncLock
        End Function

        ''' <summary>
        ''' キューから要素を取得 待ちバージョン (セマフォー減る)
        ''' </summary>
        ''' <param name="waitTime">待ち時間</param>
        ''' <returns>要素</returns>
        ''' <remarks></remarks>
        Public Overloads Function GetItem(ByVal waitTime As Integer) As T
            '' キューが取り出されるまで待機
            If Not g_evSem.WaitOne(waitTime, False) Then
                Return Nothing
            End If
            '' キュー取り出し
            Return GetItem()
        End Function

        ''' <summary>
        ''' キューから要素を全部取得
        ''' </summary>
        ''' <returns>要素配列</returns>
        ''' <remarks></remarks>
        Public Function GetAll() As T()
            SyncLock my_cs
                Dim cnt As Integer = my_pQue.Count
                If cnt <= 0 Then
                    Return Nothing
                End If

                ' セマフォー全解除
                Try
                    For ii As Integer = 0 To cnt - 1
                        g_evSem.WaitOne(0, False)
                    Next ii
                Catch ex As Exception
                End Try

                '' キュー内データを全て取得
                Dim gnrAryT() As T = my_pQue.ToArray
                my_pQue.Clear()
                Return gnrAryT
            End SyncLock
        End Function


        ''' <summary>
        ''' キューから全要素を開放 (Newしてセットしている場合は,メモリーリークしちゃうので注意？)
        ''' </summary>
        ''' <remarks>C++と動作が違うので注意</remarks>
        Public Sub Free()
            SyncLock my_cs
                Dim cnt As Integer = GetCount
                If cnt <= 0 Then
                    Return
                End If

                ' セマフォー全解除
                Try
                    For ii As Integer = 0 To cnt - 1
                        g_evSem.WaitOne(0, False)
                    Next ii
                Catch ex As Exception
                End Try
                my_pQue.Clear()
            End SyncLock
        End Sub

    End Class
End Namespace
#End If
