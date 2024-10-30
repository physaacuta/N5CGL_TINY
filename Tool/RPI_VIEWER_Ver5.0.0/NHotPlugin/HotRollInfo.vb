Option Explicit On
Option Strict On
Imports System.Drawing

Public Class RollPitch
    Public Name As String
    Public mmPitch As Integer

    Sub New(n As String, mm As Integer)
        Name = n
        mmPitch = mm
    End Sub
End Class

Public Class DrawHotRollProperties
    Inherits tClassLibrary.RpiPlugin.PeropertiesBase

    Public fontsize As Integer = 32
    Public linewidth As Integer = 2
    Public linestyle As Drawing2D.DashStyle = Drawing2D.DashStyle.Solid



    Const SECTION As String = "DrawRollPitch"

    Sub New()
        fontsize = tClassLibrary.tMod.ReadIniInt(SECTION, "FontSize", HotRollInfo.TASKINI_HOTPL, 32)
        linewidth = tClassLibrary.tMod.ReadIniInt(SECTION, "LineWidth", HotRollInfo.TASKINI_HOTPL, 2)
        linestyle = CType(tClassLibrary.tMod.ReadIniInt(SECTION, "LineStyle", HotRollInfo.TASKINI_HOTPL, 0), Drawing2D.DashStyle)
    End Sub

    Sub Save()
        tClassLibrary.tMod.WriteIni(SECTION, "FontSize", HotRollInfo.TASKINI_HOTPL, fontsize.ToString)
        tClassLibrary.tMod.WriteIni(SECTION, "LineWidth", HotRollInfo.TASKINI_HOTPL, linewidth.ToString)
        tClassLibrary.tMod.WriteIni(SECTION, "LineStyle", HotRollInfo.TASKINI_HOTPL, CInt(linestyle).ToString)
    End Sub
End Class


Public Class HotRollInfo
    Inherits List(Of RollPitch)
    Public Const TASKINI_HOTPL As String = ".\NHotPlugin.ini"
    Const SECTION As String = "HotRoll"

    Public Shared DrawProp As New DrawHotRollProperties

    Private Shared Function IniDouble(key As String, defv As Double) As Double
        Dim strD As String = tClassLibrary.tMod.tMod_IniFile.ReadIniStr(SECTION, key, TASKINI_HOTPL)
        If String.IsNullOrEmpty(strD) Then
            Return defv
        End If
        Dim result As Double
        If Double.TryParse(strD, result) Then
            Return result
        End If

        Return defv
    End Function



    Public Shared ReadOnly STDs As List(Of String)
    Shared Sub New()
        STDs = New List(Of String)
        STDs.Add("HM1")
        STDs.Add("HM2")
        STDs.Add("HM3")
        STDs.Add("RRM1")
        STDs.Add("RRM2")
        STDs.Add("RRM3")
        STDs.Add("RRM4")
        STDs.Add("RRM5")
        STDs.Add("R5")

        'STDs.Add("F1")
        'STDs.Add("F2")
        'STDs.Add("F3")
        'STDs.Add("F4")
        'STDs.Add("F5")
        'STDs.Add("F6")
        'STDs.Add("F7")

    End Sub

    Public Sub New()
        For Each std As String In STDs
            Dim len As Integer = tClassLibrary.tMod.tMod_IniFile.ReadIniInt(SECTION, std, TASKINI_HOTPL)
            Me.Add(New RollPitch(std, len))
        Next

    End Sub
    Public Sub New(coilno As String, errormsg As String)
        m_CoilNO = coilno
        m_errormsg = errormsg

        UpdateGlobalRollInfo(Me)
    End Sub

    Dim m_CoilNO As String
    Dim m_KKNO As String

    Public mmRollDiam(7) As Double
    Public mmCoilSpeed(7) As Double
    Public mmAtu(7) As Double
    Public Adv(7) As Double



    Public ReadOnly Property コイルNO As String
        Get
            Return m_CoilNO
        End Get
    End Property

    Public ReadOnly Property 管理NO As String
        Get
            Return m_KKNO
        End Get
    End Property



    Private Sub ReadFromDataRow(dr As DataRow)
        m_CoilNO = CStr(dr("コイルNo"))
        m_KKNO = CStr(dr("疵検管理No"))

        For ii As Integer = 1 To 7
            Dim sFnRoll As String = "F" & CStr(ii)

            mmRollDiam(ii) = CInt(dr("ワークロール直径" & sFnRoll))
            mmCoilSpeed(ii) = CInt(dr("コイル進行量" & sFnRoll)) / 1000
            mmAtu(ii) = CInt(dr(sFnRoll & "出側厚")) / 100
            Adv(ii) = CInt(dr("先進率" & sFnRoll)) / 10000
        Next

        mmAtu(0) = CInt(dr("バー厚")) / 100
        For ii As Integer = 1 To 7
            Dim sFnRoll As String = "F" & CStr(ii)

            '計算した、出側寒山ロール長さ
            Dim out_roll_cycle As Double = mmRollDiam(ii) * Math.PI * mmAtu(ii) / mmAtu(7)

            ' aX + b 形式の補正
            Dim adj_roll_cycle As Double = IniDouble("a" & sFnRoll, 1) * out_roll_cycle + IniDouble("b" & sFnRoll, 0)
            Dim nrollpitch As Integer = CInt(adj_roll_cycle)

            Me.Add(New RollPitch(sFnRoll, nrollpitch))
        Next
    End Sub

    Public m_errormsg As String

    ''' <summary>
    ''' コイルNOをキーに、巻取前DBにレコードを問い合わせる
    ''' </summary>
    ''' <param name="hotmaki">巻取前DBへの接続</param>
    ''' <param name="coilno">HOTコイルNO</param>
    ''' <param name="sDaysSt">検査管理NOの検索開始側</param>
    ''' <param name="sDaysEd">検査管理NOの検索終了側</param>
    ''' <returns>コイルが見つかれば、True</returns>
    Public Function ReadFromMakitori(hotmaki As tClassLibrary.tClass.tClass_SQLServer, coilno As String, sDaysSt As String, sDaysEd As String) As Boolean

        GlobalRollInfo = Nothing
        m_errormsg = ""
        Try
            Dim normalCoilNo As String = coilno.Substring(0, 6)
            Dim sql As String = String.Format("SELECT TOP(1) * FROM COIL_INF where コイルNo = '{0}' and 疵検管理NO BETWEEN '{1}' AND '{2}'", normalCoilNo, "24" & sDaysSt, "24" & sDaysEd)
            Using adapter As SqlClient.SqlDataAdapter = hotmaki.SelectAdapterEx(sql)
                Dim dt As New DataTable
                If adapter.Fill(dt) = 0 Then
                    Me.m_errormsg = String.Format("巻取前に該当するコイルがないです。#{sql}", sql)
                Else
                    ReadFromDataRow(dt.Rows(0))
                End If
            End Using

            UpdateGlobalRollInfo(Me)
            Return True

        Catch e As Exception
            Me.m_CoilNO = coilno
            m_errormsg = e.ToString()
            UpdateGlobalRollInfo(Me)
            Return False
        End Try

    End Function


    Shared Function ToInt4(s As String, start As Integer) As Integer
        Return CInt(s.Substring(start, 4))
    End Function

    Shared Function ToInt2(s As String, start As Integer) As Integer
        Return CInt(s.Substring(start, 2))
    End Function


    ''' <summary>
    ''' 疵検査管理NOの日付部分を、前後させる
    ''' </summary>
    ''' <param name="KanriNo">疵検査管理NO</param>
    ''' <param name="nDeltaSeconds">秒の差分</param>
    ''' <returns></returns>
    Public Shared Function GetKanriNoDatePart(KanriNo As String, nDeltaSeconds As Integer) As String
        Dim nOffset As Integer = 0
        If KanriNo.Length = 17 Then
            nOffset = 2
        ElseIf KanriNo.Length = 15 Then
            nOffset = 0
        Else
            Return String.Empty
        End If

        Dim dt As New DateTime(ToInt4(KanriNo, nOffset + 0), ToInt2(KanriNo, nOffset + 4), ToInt2(KanriNo, nOffset + 6), _
                               ToInt2(KanriNo, nOffset + 9), ToInt2(KanriNo, nOffset + 11), ToInt2(KanriNo, nOffset + 13))

        Return dt.AddSeconds(nDeltaSeconds).ToString("yyyyMMdd HHmmss")
    End Function

    ''' <summary>
    ''' グローバル変数。frmPopHotJumpとDrawHotRollを連携させるため。
    ''' </summary>
    Public Shared GlobalRollInfo As HotRollInfo
    Public Shared Event OnUpdateGlobalRollInfo As EventHandler(Of EventArgs)
    Private Sub UpdateGlobalRollInfo(ri As HotRollInfo)
        GlobalRollInfo = ri
        RaiseEvent OnUpdateGlobalRollInfo(Me, Nothing)
    End Sub

End Class
