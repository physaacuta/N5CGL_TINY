'*********************************************************************************
'　生産番号背景色設定クラス
'	[Ver]
'		Ver.01    2008/07/23  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On
Imports System.Collections
Imports tClassLibrary


Public Class clsProductBackColor

    Implements IDisposable                          ' デストラクタ定義


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const DSP_KIZU_LST As Integer = 5                        '表示する疵情報
    Private Const PAGE_MAX As Integer = 5                            '最大ページ番号
    Private Const MAX_PRODUCT_COLOR As Integer = 5                   '生産番号背景色数
    Private Const M_C_TOP As Integer = 0                             '表
    Private Const M_C_BOT As Integer = 1                             '裏
    Private Const MC_PNOBG_MAX As Integer = 120                      '背景色指定番号割付最大数 超えたら古い割付情報から開放

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバークラス
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass.tClass_LogManager                    ' ログ管理クラス
    Private disposedValue As Boolean = False                        ' 重複する呼び出しを検出するには

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_ProNo_BackColor(4) As System.Drawing.Color            '生産番号背景色
    Private m_dctProductNoBackColor As New Dictionary(Of String, Integer)     '生産番号の背景色セット
    Private m_lstData As New List(Of String)                        '割付した疵検管理NO開放用
    Private m_nPltPoint As Integer = 0                              '生産番号背景色の最新ポインタ
    Private m_NewRecNo As Integer = -1                              '疵情報の最新レコードＮＯ

    Private mPltNo As Integer


    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="clsLog">ログクラス</param>
    ''' <remarks></remarks>
    Public Sub New(ByRef clsLog As tClass.tClass_LogManager)

        'ログクラス
        mcls_Log = clsLog

        '背景色を固定でテーブルセット
        m_ProNo_BackColor(0) = Color.Cyan
        m_ProNo_BackColor(1) = Color.DodgerBlue
        m_ProNo_BackColor(2) = Color.Yellow
        m_ProNo_BackColor(3) = Color.GreenYellow
        m_ProNo_BackColor(4) = Color.Lime

    End Sub
#Region " IDisposable Support "
    ' このコードは、破棄可能なパターンを正しく実装できるように Visual Basic によって追加されました。
    Public Sub Dispose() Implements IDisposable.Dispose
        ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub
#End Region

    ' IDisposable
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        If Not Me.disposedValue Then
            If disposing Then
                ' TODO: 明示的に呼び出されたときにアンマネージ リソースを解放します
            End If

            '' TODO: 共有のアンマネージ リソースを解放します
            m_dctProductNoBackColor.Clear()
            m_dctProductNoBackColor = Nothing

            m_lstData = Nothing

        End If
        Me.disposedValue = True
    End Sub

    ''' <summary>
    ''' 画面起動時生産番号背景色割り振り処理
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ReadPnoBGinit()

        Dim ii As Integer
        Dim jj As Integer
        Dim n_cnt_buf As Integer
        Dim nRecNo(1) As Integer
        Dim nCnt(1) As Integer
        Dim typDefInf As TBL_HY_DEFLIST                    '疵情報一覧テーブル

        Try

            '疵情報一覧テーブル読み込み。
            typDefInf = CType(KizuLibCLI.KizuMod.ksMemRead(TBL_HY_DEFLIST_NAME, GetType(TBL_HY_DEFLIST)), TBL_HY_DEFLIST)

            '最新レコード
            nRecNo(M_C_TOP) = typDefInf.rec.head.nPos(M_C_TOP) - 1
            nRecNo(M_C_BOT) = typDefInf.rec.head.nPos(M_C_BOT) - 1

            '最古レコード
            nCnt(M_C_TOP) = nRecNo(M_C_TOP) + 1
            If nCnt(M_C_TOP) >= HY_DEFLIST_REC_MAX Then
                nCnt(M_C_TOP) = 0
            End If

            nCnt(M_C_BOT) = nRecNo(M_C_BOT) + 1
            If nCnt(M_C_BOT) >= HY_DEFLIST_REC_MAX Then
                nCnt(M_C_BOT) = 0
            End If

            '最古から番号割り振る
            n_cnt_buf = mPltNo
            For jj = 0 To 1
                For ii = 0 To HY_DEFLIST_REC_MAX - 1

                    If typDefInf.rec.data(jj).DfData(nCnt(jj)).cKanriNo <> "" And
                       False = m_dctProductNoBackColor.ContainsKey(typDefInf.rec.data(jj).DfData(nCnt(jj)).cKanriNo) Then
                        '疵検管理ＮＯに対して色配列の要素数を登録する()
                        m_dctProductNoBackColor.Add(typDefInf.rec.data(jj).DfData(nCnt(jj)).cKanriNo, n_cnt_buf)
                        '色配列の要素数を加算
                        n_cnt_buf += 1
                        '色は５色をサイクリックとして使用するため、要素数が５以上になったら０に戻す
                        If n_cnt_buf >= MAX_PRODUCT_COLOR Then n_cnt_buf = 0

                        '割付数が一定を超えたら古いものから開放していく
                        m_lstData.Add(typDefInf.rec.data(jj).DfData(nCnt(jj)).cKanriNo)
                        If MC_PNOBG_MAX < m_dctProductNoBackColor.Count Then
                            m_dctProductNoBackColor.Remove(m_lstData.Item(0))
                            m_lstData.RemoveAt(0)
                        End If

                    End If

                    nCnt(jj) += 1
                    If nCnt(jj) >= HY_DEFLIST_REC_MAX Then
                        nCnt(jj) = 0
                    End If

                Next ii
            Next jj
            mPltNo = n_cnt_buf

        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "生産番号背景色紐付けエラー [" & ex.Message & "]")
        End Try

    End Sub

    ''' <summary>
    ''' 疵検管理NOに割り振られた背景色を返す
    ''' </summary>
    ''' <param name="strProductNo">疵検管理NO</param>
    ''' <remarks></remarks>
    Public Function SetPNoBackColor(ByVal strProductNo As String) As System.Drawing.Color

        Dim bg_color As System.Drawing.Color

        Try
            bg_color = m_ProNo_BackColor(m_dctProductNoBackColor(strProductNo))
        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "生産番号背景色取得エラー [" & ex.Message & "]")
        End Try

        Return bg_color

    End Function

End Class
