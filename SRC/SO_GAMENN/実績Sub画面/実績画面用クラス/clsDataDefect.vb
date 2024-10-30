'*******************************************************************************
'　疵情報
'	[Ver]
'		Ver.01    2012/09/01  初版
'
'	[メモ]
'		DBから疵画像を取得保持する
'*******************************************************************************
Option Strict On

Imports tClassLibrary.tClass
Imports tClassLibrary.tMod
Imports System.Data.SqlClient

''' <summary>
''' 疵情報クラス
''' </summary>
''' <remarks></remarks>
Public Class clsDataDefect
    Implements IDisposable

    '///////////////////////////////////////////////////////////////////////////
    '定数定義
    '///////////////////////////////////////////////////////////////////////////
    Private Const MAX_IMAGE As Integer = 4                                      ' 疵画像数

    '---------------------------------------------------------------------------
    ' メンバ変数
    '---------------------------------------------------------------------------
    Private m_bIsDesposed As Boolean = False                                    ' リソース開放済みフラウグ True:開放済み
    Private mcls_Log As tClass_LogManager                                       ' ログ管理
    Private mcls_Param As clsParamManager                                       ' パラメータ管理
    Private m_strKanriNo As String                                              ' 管理No
    Private m_typDefect As DETECT_BASE_DATA                                     ' 疵情報
    Private m_aryImage() As Bitmap                                              ' 疵画像
    Private m_aryTitle() As String                                              ' 画像名
    Private m_bImgRead As Boolean                                               ' 画像読み込み

    '---------------------------------------------------------------------------
    ' プロパティ
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' 疵画像
    ''' </summary>
    ''' <returns>疵画像</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DefectImage() As Bitmap()
        Get
            Return m_aryImage
        End Get
    End Property

    ''' <summary>
    ''' 疵情報取得
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DefectData() As DETECT_BASE_DATA
        Get
            Return m_typDefect
        End Get
    End Property

    ''' <summary>
    ''' 疵情報の管理No
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property KanriNo() As String
        Get
            Return m_strKanriNo
        End Get
    End Property

    Public ReadOnly Property ImageTitle() As String()
        Get
            Return m_aryTitle
        End Get
    End Property

    ''' <summary>
    ''' 画像読み込み確認
    ''' </summary>
    ''' <returns></returns>
    Public ReadOnly Property IsImgRead() As Boolean
        Get
            Return m_bImgRead
        End Get
    End Property


    '---------------------------------------------------------------------------
    ' メソッド
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' 疵情報の作成
    ''' </summary>
    ''' <param name="param">パラメータ管理</param>
    ''' <param name="log">ログ管理</param>
    ''' <remarks></remarks>
    Public Sub New(ByRef param As clsParamManager, ByRef log As tClass_LogManager)
        Try
            mcls_Log = log
            mcls_Param = param

            ReDim m_aryImage(MAX_IMAGE - 1)
            ReDim m_aryTitle(m_aryImage.Length - 1)

            For ii As Integer = 0 To m_aryTitle.Length - 1
                Select Case CType(ii + 1, EM_IMAGE_NO)
                    Case EM_IMAGE_NO.S
                        m_aryTitle(ii) = DEFECT_IMG_NAME(0)
                    Case EM_IMAGE_NO.R1
                        m_aryTitle(ii) = DEFECT_IMG_NAME(1)
                    Case EM_IMAGE_NO.R2
                        m_aryTitle(ii) = DEFECT_IMG_NAME(2)
                    Case EM_IMAGE_NO.R3
                        m_aryTitle(ii) = DEFECT_IMG_NAME(3)
                    Case Else
                        m_aryTitle(ii) = String.Empty
                End Select
            Next

            DataInit()

        Catch ex As Exception
            log.Write(tClass_LogManager.LNO.ERR, String.Format("初期化異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' オブジェクトの破棄
    ''' </summary>
    ''' <param name="disposing">True:明示的な呼び出し False:暗黙の呼び出し</param>
    ''' <remarks></remarks>
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        Try
            ' TODO オブジェクト破棄
            If m_bIsDesposed = False Then
                For ii As Integer = 0 To m_aryImage.Length - 1
                    If Not m_aryImage(ii) Is Nothing Then
                        m_aryImage(ii).Dispose()
                        m_aryImage(ii) = Nothing
                    End If
                Next
            End If
            m_bIsDesposed = True                                            ' Dispose済みとする
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵情報オブジェクト破棄異常 [{0}]", ex.Message))
        End Try
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
    ''' 疵データセット
    ''' </summary>
    ''' <param name="strKanriNo"></param>
    ''' <param name="typData"></param>
    Public Sub SetDefect(ByVal strKanriNo As String, ByVal typData As DETECT_BASE_DATA)

        If strKanriNo <> m_strKanriNo Then
            ' 既存の管理No.と異なるデータをセットする場合は画像を再読み込みする
            m_bImgRead = False
        End If
        If typData.nKizuNo <> m_typDefect.nKizuNo Then
            ' 既存の疵No.と異なるデータをセットする場合は画像を再読み込みする
            m_bImgRead = False
        End If

        m_strKanriNo = strKanriNo       ' 管理No
        m_typDefect = typData           ' 疵情報

        If Not m_bImgRead Then
            ' 疵画像を読み込み
            Dim sw As New Stopwatch
            sw.Reset()
            sw.Start()
            Read(m_strKanriNo, m_typDefect.nKizuNo, m_typDefect.TorB)
            sw.Stop()
            'mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB読込み時間 [{0}]", sw.Elapsed))
        End If

    End Sub

    ''' <summary>
    ''' 疵画像読み込み
    ''' </summary>
    ''' <param name="strKizukenNo">管理No</param>
    ''' <param name="nKizuNo">疵No</param>
    ''' <param name="nMen">0:表 1:裏</param>
    ''' <returns>True:読み込み成功 False:読み込み失敗</returns>
    ''' <remarks></remarks>
    Private Function Read(ByVal strKizukenNo As String, ByVal nKizuNo As Integer, ByVal nMen As Integer) As Boolean
        Dim clsDB As tClass_SQLServer = Nothing
        Dim readSQL As SqlDataReader = Nothing
        Try
            Dim strTblName() As String = {DB_DEFECT_IMG_T, DB_DEFECT_IMG_B}
            If 0 > nMen Or strTblName.Length - 1 < nMen Then Return False

            '-------------------------------------------------------------------
            ' ＳＱＬ作成 (疵画像)
            '-------------------------------------------------------------------
            Dim strSQL As String = ""
            strSQL = ""
            strSQL &= "SELECT"

            strSQL &= " 正画像"
            strSQL &= " ,乱画像"
            strSQL &= " ,乱画像2"
            strSQL &= " ,乱画像3"

            'strSQL &= " *"
            strSQL &= " FROM"
            strSQL &= " " & strTblName(nMen)
            strSQL &= " WHERE"
            strSQL &= " 管理No='" & strKizukenNo & "'"
            strSQL &= " AND 疵No=" & nKizuNo

            '-------------------------------------------------------------------
            ' 実行
            '-------------------------------------------------------------------
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)

            Dim bDbRet As Boolean = clsDB.Open()
            If bDbRet = False Then                                              ' DB接続失敗
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵画像DB接続失敗"))
                Return False
            End If

            readSQL = clsDB.SelectExecute(strSQL)
            If readSQL Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵画像取得SQL実行失敗 SQL={0}", strSQL))
                Return False
            End If

            '-------------------------------------------------------------------
            ' 格納
            '-------------------------------------------------------------------
            With readSQL
                For ii As Integer = 0 To m_aryImage.Length - 1
                    m_aryImage(ii) = Nothing
                Next
                If readSQL.Read() Then                                   ' 疵画像はあれば格納する
                    For ii As Integer = 0 To m_aryImage.Length - 1
                        If Not .IsDBNull(ii) Then
                            m_aryImage(ii) = tClass_BmpJpeg.DataToImage(CType(.GetValue(ii), Byte()))
                            m_bImgRead = True
                        End If
                    Next
                End If
            End With

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵画像読み込み異常 [{0}]", ex.Message))
            Return False
        Finally
            If clsDB Is Nothing = False Then
                clsDB.Cancel()
                If Not readSQL Is Nothing Then
                    readSQL.Close()
                    Read = Nothing
                End If
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try

        Return True
    End Function


    ''' <summary>
    ''' 初期化処理
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DataInit()
        Try

            m_typDefect = Nothing

            For ii As Integer = 0 To m_aryImage.Length - 1
                If Not m_aryImage(ii) Is Nothing Then
                    m_aryImage(ii).Dispose()
                    m_aryImage(ii) = Nothing
                End If
            Next

            m_bImgRead = False

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵情報オブジェクト初期化異常 [{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' データコピー
    ''' </summary>
    ''' <param name="cls"></param>
    Public Sub DataCopy(ByVal cls As clsDataDefect)
        m_strKanriNo = cls.KanriNo                ' 管理No
        m_typDefect = cls.DefectData              ' 疵情報
        m_aryImage = cls.DefectImage              ' 疵画像
        m_bImgRead = cls.IsImgRead                ' 画像読み込み
    End Sub

End Class
