'*********************************************************************************
'MailSlotの送受信用のCOMMON_QUE系統クラス
'[Ver]
' Ver.01    2017/1/18  
'
'[メモ]
'*********************************************************************************
Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Namespace tClass

    ''' <summary>
    ''' メールスロットデータの基本クラス
    ''' </summary>
    ''' <remarks></remarks>
    Public Class CommonQueBase
        Public Const SIZE_MSL As Integer = 256
        ''' <summary>
        ''' 実際の送受信データ（バイト配列）
        ''' </summary>
        Protected m_byAllData(MailSlot.MAX_MAILSLOT_SIZE - 1) As Byte

        Public nEventNo As Integer
        Public nLineNo As Integer

        ''' <summary>
        ''' 初期化。中身はカラ
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()
        End Sub
        Public Sub New(ev As Integer, line As Integer)
            nEventNo = ev
            nLineNo = line
        End Sub

        ''' <summary>
        ''' 初期化：受信したバイト・長さを読む。
        ''' </summary>
        ''' <param name="arr">受信データ</param>
        ''' <param name="length">受信長</param>
        ''' <remarks></remarks>
        Public Sub New(arr() As Byte, length As Integer)
            Read(arr, length)
        End Sub

        ''' <summary>
        ''' 受信したバイト・長さを読み、自身を書き換える。
        ''' </summary>
        ''' <param name="arr">受信データ</param>
        ''' <param name="length">受信長</param>
        ''' <remarks></remarks>
        Public Sub Read(arr() As Byte, length As Integer)
            ReDim m_byAllData(Math.Max(SIZE_MSL, length) - 1)
            Array.Copy(arr, m_byAllData, Math.Min(length, m_byAllData.Length))
            doRead()
        End Sub

        Protected Sub doRead()
            Using br As New BinaryReaderSP(m_byAllData)
                nEventNo = br.ReadInt32()
                nLineNo = br.ReadInt32()
                ParseBody(br)
            End Using
        End Sub

        ''' <summary>
        ''' 送信用のバイト配列を返す。
        ''' </summary>
        ''' <returns>256バイトの送信データ</returns>
        ''' <remarks>短いとNUL埋め。長いと切り詰め</remarks>
        Public Overridable Function ToByteArray() As Byte()
            Using bw As New BinaryWriterSP(m_byAllData)
                bw.Write(nEventNo)
                bw.Write(nLineNo)
                WriteBody(bw)
            End Using

            Dim arr(SIZE_MSL - 1) As Byte
            Array.Copy(m_byAllData, arr, Math.Min(arr.Length, m_byAllData.Length))
            Return arr
        End Function


        ''' <summary>
        ''' 継承者がバイト配列から、自分の変数を読みだす。
        ''' </summary>
        ''' <param name="br">バイナリリーダ</param>
        ''' <remarks></remarks>
        Protected Overridable Sub ParseBody(br As BinaryReaderSP)
        End Sub

        ''' <summary>
        ''' 継承者が自分の変数をバイト配列に書き出す。
        ''' </summary>
        ''' <param name="bw">バイナリライタ</param>
        Protected Overridable Sub WriteBody(bw As BinaryWriterSP)
        End Sub

        ''' <summary>
        ''' 受信はCommonQueBaseでやるが、実態はCommonQueMixのようなときに使用
        ''' </summary>
        ''' <typeparam name="T">継承者</typeparam>
        ''' <returns>継承クラス</returns>
        ''' <remarks>使用例：Dim que as CommonQueMix = quebase.CastAs(Of CommonQueMix)()</remarks>
        Public Function CastAs(Of T As {New, CommonQueBase})() As T
            Dim cls As New T()
            cls.Read(m_byAllData, m_byAllData.Length)
            Return cls
        End Function
    End Class

    ''' <summary>
    ''' COMMON_QUE_MIX
    ''' </summary>
    ''' <remarks></remarks>
    Public Class CommonQueMix
        Inherits CommonQueBase
        Const STR_LENGTH As Integer = 32

        Public ReadOnly idata(20 - 1) As Integer
        Public ReadOnly cdata(5 - 1) As String

        Public Sub New()
        End Sub
        Public Sub New(ByVal ev As Integer, Optional line As Integer = 0)
            MyBase.New(ev, line)
        End Sub

        ''アクセッサ。
        Public ReadOnly Property cdata1 As String
            Get
                Return cdata(0)
            End Get
        End Property
        Public ReadOnly Property cdata2 As String
            Get
                Return cdata(1)
            End Get
        End Property
        Public ReadOnly Property cdata3 As String
            Get
                Return cdata(2)
            End Get
        End Property
        Public ReadOnly Property cdata4 As String
            Get
                Return cdata(3)
            End Get
        End Property

        Protected Overrides Sub ParseBody(br As BinaryReaderSP)
            For i As Integer = 0 To idata.Length - 1
                idata(i) = br.ReadInt32()
            Next
            For i As Integer = 0 To cdata.Length - 1
                cdata(i) = br.ReadString(STR_LENGTH)
            Next
        End Sub

        Protected Overrides Sub WriteBody(bw As BinaryWriterSP)
            For i As Integer = 0 To idata.Length - 1
                bw.Write(idata(i))
            Next
            For i As Integer = 0 To cdata.Length - 1
                bw.WriteString(cdata(i), STR_LENGTH)
            Next
        End Sub

    End Class

    ''' <summary>
    ''' COMMON_QUE_MIXだけど、stringを結合して使いたいときに。
    ''' </summary>
    ''' <remarks></remarks>
    Public Class CommonQueMixSt
        Inherits CommonQueBase

        Public ReadOnly idata(20 - 1) As Integer
        Public cdata As String

        Public Sub New()
        End Sub
        Public Sub New(ByVal ev As Integer, Optional line As Integer = 0)
            MyBase.New(ev, line)
        End Sub

        Protected Overrides Sub ParseBody(br As BinaryReaderSP)
            For i As Integer = 0 To idata.Length - 1
                idata(i) = br.ReadInt32()
            Next
            cdata = br.ReadString(32 * 5)
        End Sub


        Protected Overrides Sub WriteBody(bw As BinaryWriterSP)
            For i As Integer = 0 To idata.Length - 1
                bw.Write(idata(i))
            Next
            bw.WriteString(cdata, 32 * 5)
        End Sub

    End Class

    ''' <summary>
    ''' COMMON_QUE_FL
    ''' </summary>
    Public Class CommonQueFL
        Inherits CommonQueBase
        Public ReadOnly idata(62) As Integer

        Public Sub New()
        End Sub
        Public Sub New(ByVal ev As Integer, Optional line As Integer = 0)
            MyBase.New(ev, line)
        End Sub
        Protected Overrides Sub ParseBody(br As BinaryReaderSP)
            For i As Integer = 0 To idata.Length - 1
                idata(i) = br.ReadInt32()
            Next
        End Sub

        Protected Overrides Sub WriteBody(bw As BinaryWriterSP)

            For i As Integer = 0 To idata.Length - 1
                bw.Write(idata(i))
            Next
        End Sub
    End Class
End Namespace
