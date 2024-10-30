'*******************************************************************************
'   ステータス情報表示
'
'   [Ver]
'       Ver.01  2013/12/19  初版
'
'   [メモ]
'       tMod_MemSet.MemSet関数を使用しているため
'       [Option Strict Off]とする(エラー回避)
'*******************************************************************************
Option Strict On

Imports KizuLibCLI.KizuMod

Imports KizuForm

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod


Imports System.Runtime.InteropServices

''' <summary>
''' ステータス情報表示
''' </summary>
''' <remarks></remarks>
Public Class clsDispStatus
    Implements IDisposable
    '///////////////////////////////////////////////////////////////////////////
    '構造体
    '///////////////////////////////////////////////////////////////////////////
    'ステータス情報表示コントロール
    Public Structure TYP_STATUS_DSP
        Dim lblGaidans As Label                             'ガイダンスメッセージ
        Dim lblKadou As Label                               '稼働状態
        Dim lblKiki As Label                                '機器状態
        Dim lblKensa As Label                               '検査状態
    End Structure


    '点滅管理用構造体
    Private Structure BLINK
		Dim cnt As Control
		Dim text As String
		Dim colorOrg As Color
		Public Sub New(ByVal cnt As Control, ByVal text As String, ByVal clr1 As Color)
			Me.cnt = cnt
			Me.text = text
			Me.colorOrg = clr1
		End Sub
	End Structure


    '///////////////////////////////////////////////////////////////////////////
    'メンバ変数
    '///////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False                        'リソース開放フラグ[True:開放済,False:未開放]

    Private mcls_Log As tClass_LogManager                           'ログ管理

    Private m_typStatusDsp As TYP_STATUS_DSP                        'ステータス表示

    Private m_bDBError As Boolean = False                           'DB接続異常[True:異常,False:正常]

    Private WithEvents m_tmBlk As New Windows.Forms.Timer           'BLINK用タイマー
	Private lstBlk As New List(Of BLINK)
	Private m_blkStatus As Boolean

	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	' 長周期ガイダンス表示
	Private m_strLongPdGuidanceMessage As String = ""               'メッセージ内容
	Private m_nLongPdGuidanceBackColor As Integer                   '背景色
	Private m_nLongPdGuidanceForeColor As Integer                   '文字色
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

	'///////////////////////////////////////////////////////////////////////////
	'プロパティ
	'///////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' DB接続状態
	''' </summary>
	''' <returns>True:接続 False:切断</returns>
	''' <remarks></remarks>
	Public Property DbStatus() As Boolean
        Get
            Return Not m_bDBError
        End Get
        Set(ByVal value As Boolean)
            m_bDBError = Not value
        End Set
	End Property

	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	''' <summary>
	''' 長周期ガイダンス表示 メッセージ内容
	''' </summary>
	Public WriteOnly Property LongPdGuidanceMessage() As String
		Set(value As String)
			m_strLongPdGuidanceMessage = value
		End Set
	End Property

	''' <summary>
	''' 長周期ガイダンス表示 背景色
	''' </summary>
	Public WriteOnly Property LongPdGuidanceBackColor() As Integer
		Set(value As Integer)
			m_nLongPdGuidanceBackColor = value
		End Set
	End Property

	''' <summary>
	''' 長周期ガイダンス表示 文字色
	''' </summary>
	Public WriteOnly Property LongPdGuidanceForeColor() As Integer
		Set(value As Integer)
			m_nLongPdGuidanceForeColor = value
		End Set
	End Property
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------


	'///////////////////////////////////////////////////////////////////////////
	'メンバ関数
	'///////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' コンストラクタ
	''' </summary>
	''' <param name="typStatusDsp">ステータス表示</param>
	''' <param name="clsLog">ログ管理</param>
	''' <remarks></remarks>
	Public Sub New(ByRef typStatusDsp As TYP_STATUS_DSP, ByRef clsLog As tClass_LogManager)
        Try
            mcls_Log = clsLog
			m_typStatusDsp = typStatusDsp

			' 点滅表示初期化
			InitBlink()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("ステータス情報表示生成異常[{0}]", ex.Message))
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
    ''' 表示初期化
    ''' </summary>
    ''' <remarks></remarks>
	Public Sub DspInit()
		m_tmBlk.Interval = 2000

		'ガイダンス情報表示
		ucStatusAria.InitLabel(m_typStatusDsp.lblGaidans)
		'稼動状態表示
		ucStatusAria.InitLabel(m_typStatusDsp.lblKadou)
		'検査状態表示
		ucStatusAria.InitLabel(m_typStatusDsp.lblKensa)
		'機器状態表示
        ucStatusAria.InitLabel(m_typStatusDsp.lblKiki)

    End Sub

    ''' <summary>
    ''' ステータス表示
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspStatus(ByVal nMen As Integer)
        Dim typHyStatus As TBL_HY_STATUS = Nothing
        Dim typNowSystem As KizuForm.TYP_STATUS_NOWSYSTEM = Nothing

        Try
            '初期化
            typHyStatus.status.initialize()
            typNowSystem.initialize()

            'ステータス情報読み込み[共有メモリ]
            typHyStatus = CType(ksMemRead(TBL_HY_STATUS_NAME, GetType(TBL_HY_STATUS)), TBL_HY_STATUS)

            'ステータスコピー
            Dim p As IntPtr
			Try
				p = Marshal.AllocHGlobal(SizeOf(typHyStatus.status))					'メモリ取得
				Marshal.StructureToPtr(typHyStatus.status, p, True)					'メモリ上にデータをセット
				typNowSystem = DirectCast(Marshal.PtrToStructure(p, typNowSystem.GetType), KizuForm.KizuCommon.TYP_STATUS_NOWSYSTEM)		'メモリからデータを取得し、構造体へセット
			Catch ex As Exception
				typNowSystem = Nothing
			Finally
				Marshal.FreeHGlobal(p)									'メモリ開放
			End Try

            'DB接続異常
            If m_bDBError Then
                typNowSystem.nCnt = 1
                typNowSystem.typErr(0).msg = "データベース接続異常が発生したため検査できません"
                typNowSystem.st_kensa(nMen) = CType(EM_DIV_KENSA.DIV_KENSA_NG, KizuForm.EM_DIV_KENSA)
                typNowSystem.st_kiki(nMen) = CType(EM_DIV_KIKI.DIV_KIKI_JYU, KizuForm.EM_DIV_KIKI)
            End If

            'ガイダンス表示
            ucStatusAria.DspGaidans(m_typStatusDsp.lblGaidans, nMen, typNowSystem)

			'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
			' 長周期ガイダンス表示
			Call DspLongPdGuidance(typNowSystem)
			'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

			'稼働状態
			ucStatusAria.DspKadou(m_typStatusDsp.lblKadou, CType(typHyStatus.emKadou, KizuForm.EM_DIV_KADOU), CType(typHyStatus.emUnten, KizuForm.EM_DIV_UNTEN))

            '機器状態
            ucStatusAria.DspKiki(m_typStatusDsp.lblKiki, typNowSystem.st_kiki(nMen))

            '検査状態
            ucStatusAria.DspKensa(m_typStatusDsp.lblKensa, typNowSystem.st_kensa(nMen))

            ' 点滅情報セット
            For i As Integer = 0 To lstBlk.Count - 1
                With lstBlk(i)
                    If .text = .cnt.Text Then
                        lstBlk(i) = New BLINK(.cnt, .text, .cnt.BackColor)
                    End If
                End With
            Next
            DispBlink()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("ステータス表示異常[{0}]", ex.Message))
        End Try
	End Sub

	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	''' <summary>
	'''  長周期ガイダンス表示
	''' </summary>
	Private Sub DspLongPdGuidance(ByVal typNowSystem As KizuForm.TYP_STATUS_NOWSYSTEM)

		Dim bError As Boolean = False
		If typNowSystem.nCnt > 0 Then
			bError = True
		End If

		If True = bError Then
			' 何か異常メッセージが表示中なら文字色を元に戻して終了。
			m_typStatusDsp.lblGaidans.ForeColor = Color.Black
			Exit Sub
		End If

		'共有メモリ読込
		Dim typLongPd As TBL_HY_LONGPD = Nothing
		typLongPd.initialize()
		typLongPd = CType(ksMemRead(TBL_HY_LONGPD_NAME, GetType(TBL_HY_LONGPD)), TBL_HY_LONGPD)

		If 1 = typLongPd.nGuidanceFlag Then
			m_typStatusDsp.lblGaidans.Text = m_strLongPdGuidanceMessage
			m_typStatusDsp.lblGaidans.BackColor = Color.FromArgb(m_nLongPdGuidanceBackColor)
			m_typStatusDsp.lblGaidans.ForeColor = Color.FromArgb(m_nLongPdGuidanceForeColor)
		Else
			m_typStatusDsp.lblGaidans.Text = ""
			m_typStatusDsp.lblGaidans.BackColor = g_ColorControl
			m_typStatusDsp.lblGaidans.ForeColor = Color.Black
		End If
	End Sub
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

	''' <summary>
	''' 点滅表示初期化
	''' </summary>
	''' <remarks></remarks>
	Private Sub InitBlink()
		lstBlk.Clear()

		lstBlk.Add(New BLINK(m_typStatusDsp.lblKiki, "重故障", g_ColorControl))
		lstBlk.Add(New BLINK(m_typStatusDsp.lblKiki, "軽故障", g_ColorControl))

		lstBlk.Add(New BLINK(m_typStatusDsp.lblKensa, "未検査", g_ColorControl))
		lstBlk.Add(New BLINK(m_typStatusDsp.lblKensa, "未保存", g_ColorControl))
		lstBlk.Add(New BLINK(m_typStatusDsp.lblKensa, "ﾒﾝﾃ中", g_ColorControl))
        lstBlk.Add(New BLINK(m_typStatusDsp.lblKensa, "縮退", g_ColorControl))
		lstBlk.Add(New BLINK(m_typStatusDsp.lblKensa, "ｴｯｼﾞ不正", g_ColorControl))

		m_tmBlk.Interval = 2000
		m_tmBlk.Start()

	End Sub


	''' <summary>
	''' 点滅表示
	''' </summary>
	''' <remarks></remarks>
	Private Sub DispBlink()

		If m_blkStatus Then
			' 消灯時
			m_blkStatus = False
			For Each blk As BLINK In lstBlk
				If blk.cnt.Text = blk.text Then
					blk.cnt.BackColor = g_ColorControl
					blk.cnt.ForeColor = Color.Black
				End If
			Next
		Else
			' 点灯時
			m_blkStatus = True
			For Each blk As BLINK In lstBlk
				If blk.cnt.Text = blk.text Then
					blk.cnt.BackColor = blk.colorOrg
					blk.cnt.ForeColor = Color.Yellow
				End If
			Next
		End If
	End Sub


	''' <summary>
	''' 表示の点滅用タイマー
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub m_tmBlk_tick(ByVal sender As System.Object, ByVal e As EventArgs) Handles m_tmBlk.Tick
		' 点滅状態を反転させる
		m_blkStatus = Not m_blkStatus

		' 点滅表示を行う
		DispBlink()
	End Sub

End Class
