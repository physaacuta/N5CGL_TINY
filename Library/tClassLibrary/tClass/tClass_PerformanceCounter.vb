'*********************************************************************************
'高分解能パフォーマンスカウンタでの時刻測定
'	[Ver]
'		Ver.01    2005/12/23  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)
Imports System.Runtime.InteropServices			' DllImport

Namespace tClass
    ''' <summary>
    ''' 高分解能パフォーマンスカウンタでの時刻測定
    ''' </summary>
    ''' <remarks>ハードの対応が必要</remarks>
    Public Class tClass_PerformanceCounter
        '********************************************************************
        'API定義
        '********************************************************************
        ''' <summary>
        ''' 高分解能パフォーマンスカウンタ値取得
        ''' </summary>
        ''' <param name="lpPerformanceCount">カウンタ値</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        <DllImport("KERNEL32.DLL", EntryPoint:="QueryPerformanceCounter")> _
        Private Shared Function QueryPerformanceCounter( _
            ByRef lpPerformanceCount As Int64) _
            As Boolean
        End Function
        ''' <summary>
        ''' 現在の周波数
        ''' </summary>
        ''' <param name="lpPerformanceCount">周波数</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        <DllImport("KERNEL32.DLL", EntryPoint:="QueryPerformanceFrequency")> _
        Private Shared Function QueryPerformanceFrequency( _
            ByRef lpPerformanceCount As Int64) _
            As Boolean
        End Function

        '********************************************************************
        'メンバー変数定義
        '********************************************************************
        Private m_pf As Int64				' 周波数
        Private m_pc As Int64				' いろいろ使う測定点

        '' 共通
        Private m_min As Double				' 最小値	[ms]
        Private m_max As Double				' 最大値	[ms]
        Private m_ave As Double				' 平均時間	[ms]
        Private m_bFast As Boolean			' 一回実行後 True
        Private m_Cycle As Double			' 間隔 [ms]

        '********************************************************************
        'プロパティ
        '********************************************************************
        ''' <summary>
        ''' 平均時間 [ms] 
        ''' </summary>
        ''' <returns>平均時間 [ms] </returns>
        Public ReadOnly Property GetAveTime() As Double
            Get
                Return m_ave
            End Get
        End Property

        ''' <summary>
        ''' 間隔 [ms]
        ''' </summary>
        ''' <returns>間隔 [ms]</returns>
        Public ReadOnly Property GetCycleTime() As Double
            Get
                Return m_Cycle
            End Get
        End Property

        ''' <summary>
        ''' 最小時間 [ms]
        ''' </summary>
        ''' <returns>間隔 [ms]</returns>
        Public ReadOnly Property getMinTime() As Double
            Get
                Return m_min
            End Get
        End Property

        ''' <summary>
        ''' 最大時間 [ms]
        ''' </summary>
        ''' <returns>間隔 [ms]</returns>
        Public ReadOnly Property getMaxTime() As Double
            Get
                Return m_max
            End Get
        End Property


        '********************************************************************
        'メソッド定義
        '********************************************************************
        ''' <summary>
        ''' デフォルト コンストラクタ
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()
            '' 周波数取得
            QueryPerformanceFrequency(m_pf)
            m_pc = 0
            '' 共通用
            m_min = 9999.9			' 最小値	[ms]
            m_max = 0				' 最大値	[ms]
            m_ave = 0				' 平均時間	[ms]
            m_bFast = False			' 一回実行後 True
            m_Cycle = 0
        End Sub

        ''' <summary>
        ''' 2点間時間測定初期化
        ''' </summary>
        ''' <param name="intervalCnt">平均算出間隔</param>
        ''' <remarks></remarks>
        Public Sub initAve(ByVal intervalCnt As Integer)
            m_pc = 0

            m_ave = 0.0								'// 平均時間	[ms] 
            m_min = 9999.0							'// 最小値
            m_max = 0.0								'// 最大値
            m_bFast = False
            m_Cycle = 0
        End Sub

        ''' <summary>
        ''' 2点間時間測定開始
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub CounterStart()
            QueryPerformanceCounter(m_pc)
        End Sub

        ''' <summary>
        ''' 2点間時間測定完了
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub CounterEnd()
            '' 時間差算出
            Dim pc2 As Int64
            QueryPerformanceCounter(pc2)
            m_Cycle = (pc2 - m_pc) / m_pf * 1000.0		' [s] → [ms] 

            ' 最大最小時間算出
            If m_min > m_Cycle Then m_min = m_Cycle
            If m_max < m_Cycle Then m_max = m_Cycle

            ' 平均時間算出 (簡易算出)
            If m_bFast Then
                m_ave = m_ave * 0.8 + m_Cycle * 0.2
            Else
                m_ave = m_Cycle
                m_bFast = True
            End If
        End Sub

        ''' <summary>
        ''' 間隔測定初期化
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub initCycle()
            m_pc = 0
            m_Cycle = 0.0								'// 間隔 [ms]

            m_ave = 0.0								'// 平均時間	[ms] 
            m_min = 9999.0								'// 最小値
            m_max = 0.0								'// 最大値
            m_bFast = False
        End Sub

        ''' <summary>
        ''' 間隔測定
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub CounterCycle()
            '' 時間差算出
            Dim pc2 As Int64
            QueryPerformanceCounter(pc2)
            m_Cycle = (pc2 - m_pc) / m_pf * 1000.0		' [s] → [ms] 
            m_pc = pc2

            ' 最大最小時間算出
            If m_min > m_Cycle Then m_min = m_Cycle
            If m_max < m_Cycle Then m_max = m_Cycle

            ' 平均時間算出 (簡易算出)
            If m_bFast Then
                m_ave = m_ave * 0.8 + m_Cycle * 0.2
            Else
                m_ave = m_Cycle
                m_bFast = True
            End If
        End Sub
    End Class
End Namespace