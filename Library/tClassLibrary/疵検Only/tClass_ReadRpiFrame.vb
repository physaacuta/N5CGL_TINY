'*********************************************************************************
' 全長ファイル群を読み込むクラス。　１フレーム＝１ファイルの構造を、RPIファイルに偽装する
'	[Ver]
'		Ver.01    2014/09/02  実装
'
'	[メモ]
'		何フレームまでファイルがあるかは、使う側が設定する(EffectiveRpiFrameCount)
'*********************************************************************************
Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Imports System.IO						' ファイルアクセス

Namespace tClass
    ''' <summary>
    '''  全長ファイル群を読み込むクラス。　１フレーム＝１ファイルの構造を、RPIファイルに偽装する
    ''' </summary>
    Public Class tClass_ReadRpiFrame
        Inherits tClass_ReadRPI


        Private m_PathFmt As String	' フレーム番号を与えてFormatすると、ファイル名になる文字列
        Protected m_nCanReadFrameNum As Integer = 0	' どこまでファイルがあるか


        ''' <summary>
        ''' 全長ファイル（１フレーム１ファイル）クラスを作成する
        ''' </summary>
        ''' <param name="pathfmt">フレーム番号を与えてFormatすると、ファイル名になる文字列</param>
        ''' <param name="rpiheader">RPIヘッダ</param>
        ''' <param name="trpiheader">T_RPIヘッダ</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal pathfmt As String, ByVal rpiheader As tClass_ReadRPI.RPI_HEADER, ByVal trpiheader As tClass_ReadRPI.RPI_T_HEADER)
            m_PathFmt = pathfmt
            m_RpiHeader = rpiheader
            m_RpiTHeader = trpiheader
        End Sub


        '************* 継承連中
        ''' <summary>
        ''' RPIヘッダをかえす
        ''' </summary>
        Public Overrides ReadOnly Property GetRpiHeader() As tClass_ReadRPI.RPI_HEADER
            Get
                '読み込み可能な数が増えた。ヘッダ側のフレーム数も増やしておく。
                If m_RpiHeader.nframe < m_nCanReadFrameNum Then
                    m_RpiHeader.nframe = m_nCanReadFrameNum
                End If
                Return m_RpiHeader
            End Get
        End Property

        ''' <summary>
        ''' いま読みこめる上限フレーム。
        ''' このフレーム以降は、まだ準備ができていない。
        ''' </summary>
        ''' <remarks>
        ''' このクラスは、このプロパティによって、フレームファイルの上限を知る
        ''' </remarks>
        Public Overrides Property EffectiveRpiFrameCount() As Integer
            Get
                Return m_nCanReadFrameNum
            End Get
            Set(ByVal value As Integer)
                m_nCanReadFrameNum = value
            End Set
        End Property

        ''' <summary>
        ''' 表示用の画像（圧縮）のバイナリ列を読み込む
        ''' </summary>
        ''' <param name="fno">フレーム番号（１オリジン）</param>
        ''' <param name="bWk">読み込む先のバイト配列。呼び出し側は十分大きなサイズを準備してね</param>
        Public Overrides Function ReadFile_ImgByte_ActiveCh(ByVal fno As Integer, ByVal bWk() As Byte) As Integer
            Dim frame_file As String = String.Format(m_PathFmt, fno)
            If frame_file = "NUL" Then Return 0

            Using fs As New FileStream(frame_file, FileMode.Open, FileAccess.Read, FileShare.Read)
                Dim nLength As Integer = CInt(fs.Length)
                fs.Read(bWk, 0, nLength)
                Return nLength
            End Using

        End Function

        ''' <summary>
        ''' 表示用の画像（圧縮）のバイナリ列を読み込み、かえす。
        ''' </summary>
        ''' <param name="fno">フレーム番号（１オリジン）</param>
        Public Overrides Function ReadFile_ImgByte(ByVal fno As Integer) As Byte()
            Dim frame_file As String = String.Format(m_PathFmt, fno)

            Using fs As New FileStream(frame_file, FileMode.Open, FileAccess.Read, FileShare.Read)
                Dim nLength As Integer = CInt(fs.Length)
                Dim bWk(nLength - 1) As Byte
                fs.Read(bWk, 0, nLength)
                Return bWk
            End Using
        End Function
    End Class




    ''' <summary>
    '''  全長ファイル群を読み込むクラス。ファイルは自動探索。１フレーム＝１ファイルの構造を、RPIファイルに偽装する.
    ''' </summary>
    ''' <remarks>定期的にフォルダを見ては、フレームファイルが増えてないかチェックします。(抜けファイルは無い前提)</remarks>
    Public Class tClass_ReadRpiFrameAuto
        Inherits tClass_ReadRpiFrame


        Private mcls_WThread As New tClass_WorkThread 'ファイル存在チェックスレッド

        Private m_PathFmt As String	' フレーム番号を与えてFormatすると、ファイル名になる文字列

        Private m_bInfiniteLoop As Boolean 'フレームがどこまで伸びるかわからない。



        ''' <summary>
        ''' 全長ファイル（１フレーム１ファイル）クラスを作成する
        ''' </summary>
        ''' <param name="pathfmt">フレーム番号を与えてFormatすると、ファイル名になる文字列</param>
        ''' <param name="rpiheader">RPIヘッダ</param>
        ''' <param name="trpiheader">T_RPIヘッダ</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal pathfmt As String, ByVal rpiheader As tClass_ReadRPI.RPI_HEADER, ByVal trpiheader As tClass_ReadRPI.RPI_T_HEADER)
            MyBase.New(pathfmt, rpiheader, trpiheader)
            mcls_WThread.ThreadStart(AddressOf mainThread_RpiCreateList, Threading.ThreadPriority.Lowest, "ReadRpiCreatList", True)
        End Sub

        '************* バックグラウンド関連

        ''' <summary>
        ''' バックグラウンドで、ファイルがどこまででできたか検知する
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ListCreate()
            Do While True ' m_RpiHeader.nframe <> nCanReadFrameNum

                'あたらしくできたファイルはあるかな？
                For ii As Integer = Math.Max(1, m_nCanReadFrameNum) To Integer.MaxValue	'm_RpiHeader.nframe
                    Try
                        If File.Exists(String.Format(m_PathFmt, ii)) Then
                            m_nCanReadFrameNum = ii
                        Else
                            Exit For
                        End If
                    Catch iex As IOException

                    End Try
                Next

                'もう全部のファイルを読み切ったかな？
                If Not m_bInfiniteLoop Then
                    If m_nCanReadFrameNum >= m_RpiHeader.nframe Then
                        Exit Do
                    End If
                End If

                'もう自分は死ぬべきかな？
                If Me.mcls_WThread.g_evStop.WaitOne(1000) Then
                    Exit Do
                End If
            Loop
        End Sub


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

        ''' <summary>
        ''' バックグラウンドスレッドを殺す
        ''' </summary>
        ''' <param name="disposing"></param>
        ''' <remarks></remarks>
        Protected Overrides Sub Dispose(ByVal disposing As Boolean)
            MyBase.Dispose(disposing)
            'バックグラウンドで動いているファイル検索スレッドを止める
            mcls_WThread.ThreadStop()
        End Sub


        ''' <summary>
        ''' いま読みこめる上限フレーム。
        ''' このフレーム以降は、まだ準備ができていない。
        ''' </summary>
        ''' <remarks>
        ''' このクラスは、このプロパティによって、フレームファイルの上限を知る
        ''' </remarks>
        Public Overrides Property EffectiveRpiFrameCount() As Integer
            Get
                Return m_nCanReadFrameNum
            End Get
            Set(ByVal value As Integer)
                m_bInfiniteLoop = False	'念のため。探索スレッドが動いていたら殺す
                m_nCanReadFrameNum = value
            End Set
        End Property
    End Class

End Namespace
