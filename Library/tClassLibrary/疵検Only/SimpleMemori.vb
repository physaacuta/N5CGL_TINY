'*********************************************************************************
'ピクチャーボックスに目盛り（スケール）を表示する
'	[Ver]
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports System.Windows.Forms
Imports System.Drawing

Imports tClassLibrary.tClass


Namespace tClass
    Public Class SimpleMemori
        Private m_MemoriInf As tClass_ImageRawCutDsp.MEMORI_INF           ' メモリ情報
        Public Property MemoriInf() As tClass_ImageRawCutDsp.MEMORI_INF
            Get
                Return m_MemoriInf
            End Get
            Set(ByVal value As tClass_ImageRawCutDsp.MEMORI_INF)
                m_MemoriInf = value
            End Set
        End Property


        Private mcls_pCalc As tClass_ImageDsp_Calc  ' 画像計算クラス (ポインタ)

        Public zoom_modifier_x As Double = 1.0
        Public zoom_modifier_y As Double = 1.0

        Private m_pic As Size
        Public Sub New(pic As Size, pCalc As tClass_ImageDsp_Calc)
            m_pic = pic
            mcls_pCalc = pCalc
        End Sub



        Structure dp_mm
            Public dp As Integer
            Public mm As Double
            Public Sub New(d As Integer, m As Double)
                dp = d
                mm = m
            End Sub
        End Structure

        Shared Function Round2(v As Double) As Double
            Dim lv As Double = Math.Log10(v)
            Dim lv_exp As Double = Math.Floor(lv)
            Dim fv_frac As Double = v / Math.Pow(10, lv_exp)
            Dim nv_frac2 As Integer = CInt(fv_frac * 10)

            Dim nfrac As Integer = 1
            If nv_frac2 > 92 Then
                nfrac = 100
            ElseIf nv_frac2 > 30 Then
                nfrac = (nv_frac2 \ 10) * 10
            ElseIf nv_frac2 > 27 Then
                nfrac = 30
            ElseIf nv_frac2 > 22 Then
                nfrac = 25
            ElseIf nv_frac2 > 17 Then
                nfrac = 20
            ElseIf nv_frac2 > 12 Then
                nfrac = 15
            Else
                nfrac = 10
            End If

            Return (nfrac / 10) * Math.Pow(10, lv_exp)
        End Function

        Function DecideMemoriSize_Width(init_dp As Integer, percent As Integer) As dp_mm
            Dim CalcAns As tClass_ImageDsp_Calc.CALC_ANS = mcls_pCalc.tProp_CalcAns
            Dim dp_w As Double = init_dp * percent / 100 - 10
            If dp_w < 0 Then dp_w = 10

            Dim mmInit As Double = dp_w * zoom_modifier_x * CalcAns.FinalRes.dW
            Dim mmLast As Double = Round2(mmInit)
            Dim pxLast As Double = ((mmLast / zoom_modifier_x) / CalcAns.FinalRes.dW)

            Return New dp_mm(CInt(pxLast), mmLast)
        End Function

        Function DecideMemoriSize_Length(init_dp As Integer, percent As Integer) As dp_mm
            Dim CalcAns As tClass_ImageDsp_Calc.CALC_ANS = mcls_pCalc.tProp_CalcAns
            Dim dp_w As Double = init_dp * percent / 100 - 10
            If dp_w < 0 Then dp_w = 10

            Dim mmInit As Double = dp_w * zoom_modifier_y * CalcAns.FinalRes.dH
            Dim mmLast As Double = Round2(mmInit)
            Dim pxLast As Double = ((mmLast / zoom_modifier_y) / CalcAns.FinalRes.dH)

            Return New dp_mm(CInt(pxLast), mmLast)
        End Function

        Public Sub DrawMemori(g As Graphics, Bsh As SolidBrush, Pen As Pen, Fnt As Font)
            Dim CalcAns As tClass_ImageDsp_Calc.CALC_ANS = mcls_pCalc.tProp_CalcAns


            Dim nPlsLineBot As Integer = m_pic.Height - 20
            Dim nPlsLineLeft As Integer = 15

            Dim mmNaga As New dp_mm(m_MemoriInf.nInteralY, m_MemoriInf.nInteralY * zoom_modifier_y * CalcAns.FinalRes.dH)
            Dim mmHaba As New dp_mm(m_MemoriInf.nInteralX, m_MemoriInf.nInteralX * zoom_modifier_x * CalcAns.FinalRes.dW)

            If m_MemoriInf.nInteralX < 0 Then
                mmHaba = DecideMemoriSize_Width(IIf(mcls_pCalc.tProp_ImgKind_Rotate90, m_pic.Height, m_pic.Width), -m_MemoriInf.nInteralX)
            End If
            If m_MemoriInf.nInteralY < 0 Then
                mmNaga = DecideMemoriSize_Length(IIf(mcls_pCalc.tProp_ImgKind_Rotate90, m_pic.Width, m_pic.Height), -m_MemoriInf.nInteralY)
            End If


            Dim mmX As dp_mm = IIf(mcls_pCalc.tProp_ImgKind_Rotate90, mmNaga, mmHaba)
            Dim mmY As dp_mm = IIf(mcls_pCalc.tProp_ImgKind_Rotate90, mmHaba, mmNaga)

            Dim strX As String = String.Format("{0:0.0}", mmX.mm)
            If mmX.mm > 490 Then
                strX = String.Format("{0:0}", mmX.mm)
            End If

            Dim strY As String = String.Format("{0:0.0}", mmY.mm)
            If mmY.mm > 490 Then
                strY = String.Format("{0:0}", mmY.mm)
            End If
            ' Y軸
            g.DrawString(strY, Fnt, Bsh, nPlsLineLeft - 10, nPlsLineBot - CInt(mmY.dp) - 15)
            g.DrawLine(Pen, nPlsLineLeft, nPlsLineBot, nPlsLineLeft, nPlsLineBot - CInt(mmY.dp))

            ' X軸
            g.DrawString(strX, Fnt, Bsh, nPlsLineLeft + CInt(mmX.dp) - 10, nPlsLineBot + 3)
            g.DrawLine(Pen, nPlsLineLeft, nPlsLineBot, nPlsLineLeft + CInt(mmX.dp), nPlsLineBot)

            ' 原点表示
            g.DrawString("0mm", Fnt, Bsh, nPlsLineLeft - 10, nPlsLineBot + 3)
        End Sub

    End Class
End Namespace