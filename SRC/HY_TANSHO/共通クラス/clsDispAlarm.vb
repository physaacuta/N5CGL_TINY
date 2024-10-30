'*******************************************************************************
'   疵接近警告表示
'
'   [Ver]
'       Ver.01  2013/12/20  初版
'
'	[メモ]
'
'*******************************************************************************
Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod

''' <summary>
''' 疵接近警告表示
''' </summary>
''' <remarks></remarks>
Public Class clsDispAlarm
    Implements IDisposable
    '///////////////////////////////////////////////////////////////////////////
    '構造体
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' アラーム情報表示コントロール
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure TYP_ALARM_DSP
        'ラベルは左側から順に格納する
        Dim lblLamp As Label()                                                  '疵接近ランプ
        'イニシャライズ
        Public Sub initialize()
			ReDim lblLamp(ALARM_COL_NUM - 1)
        End Sub
    End Structure

    '///////////////////////////////////////////////////////////////////////////
    'メンバ変数
    '///////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False                                    'リソース開放フラグ[True:開放済,False:未開放]

    Private mcls_Log As tClass_LogManager                                       'ログ管理

    Private m_typAlarmDsp As TYP_ALARM_DSP                                      'アラーム情報表示コントロール

    Private m_ColorArmKei As Color                                              '軽故障ランプ表示色
    Private m_ColorArmJyu As Color                                              '重故障ランプ表示色

    '///////////////////////////////////////////////////////////////////////////
    'プロパティ
    '///////////////////////////////////////////////////////////////////////////
#Region "プロパティ"

    ''' <summary>
    ''' 軽故障ランプ表示色
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Property ColorArmKei() As Color
        Get
            Return m_ColorArmKei
        End Get
        Set(ByVal value As Color)
            m_ColorArmKei = value
        End Set
    End Property

    ''' <summary>
    ''' 重故障ランプ表示色
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Property ColorArmJyu() As Color
        Get
            Return m_ColorArmJyu
        End Get
        Set(ByVal value As Color)
            m_ColorArmJyu = value
        End Set
    End Property

#End Region

    '///////////////////////////////////////////////////////////////////////////
    'メンバ関数
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="typAlarmDsp">アラーム情報表示コントロール</param>
    ''' <param name="clsLog">ログ管理</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal typAlarmDsp As TYP_ALARM_DSP, ByRef clsLog As tClass_LogManager)
        Try
            m_typAlarmDsp = typAlarmDsp
            mcls_Log = clsLog
            m_ColorArmKei = Color.LimeGreen
            m_ColorArmJyu = Color.Red
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵接近警告表示生成異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' デストラクタ
    ''' </summary>
    ''' <param name="disposing">True:明示的な呼び出し False:暗黙の呼び出し</param>
    ''' <remarks></remarks>
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        Try
            If Not m_bIsDesposed Then
                'オブジェクト破棄
            End If
            m_bIsDesposed = True                                                'Dispose済みとする
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("オブジェクト破棄異常[{0}]", ex.Message))
        End Try
    End Sub
#Region " IDisposable Support "
    'このコードは、破棄可能なパターンを正しく実装できるように Visual Basic によって追加されました。
    Public Sub Dispose() Implements IDisposable.Dispose
        'このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub
#End Region

    ''' <summary>
    ''' 疵接近ランプ表示
    ''' </summary>
    ''' <param name="nDspPos">出力位置</param>
    ''' <param name="nMen">面[0:表,1:裏]</param>
    ''' <param name="bDSPos">DS位置[True:左,False:右]</param>
    ''' <param name="lblLamp">表示ランプ</param>
    ''' <remarks></remarks>
    Public Sub DspDefLamp(ByVal nDspPos As Integer, ByVal nMen As Integer, ByVal bDSPos As Boolean, Optional ByVal lblLamp() As Label = Nothing)
        Dim lstLamp As List(Of Label) = Nothing
        Dim typHyAlarm As TBL_HY_ALARM = Nothing
        Dim nCol As Integer

        Try
            '警報出力中情報読み込み[共有メモリ]
            typHyAlarm = CType(ksMemRead(TBL_HY_ALARM_NAME, GetType(TBL_HY_ALARM)), TBL_HY_ALARM)

            '表示ランプ設定
            lstLamp = New List(Of Label)
            If lblLamp Is Nothing Then
                For ii As Integer = 0 To m_typAlarmDsp.lblLamp.Length - 1
                    lstLamp.Add(m_typAlarmDsp.lblLamp(ii))
                Next
            Else
                For ii As Integer = 0 To lblLamp.Length - 1
                    lstLamp.Add(lblLamp(ii))
                Next
            End If

            '疵接近ランプ表示
			For ii As Integer = 0 To ALARM_COL_NUM - 1
				If bDSPos Then			'左から
					nCol = ii
				Else					'右から
					nCol = (ALARM_COL_NUM - ii) - 1
				End If
				lstLamp(ii).Text = ""
                lstLamp(ii).BackColor = Color.White
                If 0 < typHyAlarm.a_mode.kendai(nDspPos).torb(nMen).row(nCol) Then
                    If 2 = typHyAlarm.a_mode.kendai(nDspPos).torb(nMen).row(nCol) Then
                        '重疵
                        lstLamp(ii).BackColor = m_ColorArmJyu
                    ElseIf 1 = typHyAlarm.a_mode.kendai(nDspPos).torb(nMen).row(nCol) Then
                        '軽疵
                        lstLamp(ii).BackColor = m_ColorArmKei
                    End If
                End If
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵接近ランプ表示異常[{0}]", ex.Message))
        Finally
            If Not lstLamp Is Nothing Then
                lstLamp.Clear()
                lstLamp = Nothing
            End If
        End Try
    End Sub
End Class
