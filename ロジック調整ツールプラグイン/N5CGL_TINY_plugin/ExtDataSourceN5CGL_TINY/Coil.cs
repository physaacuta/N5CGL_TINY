using System;
using System.Collections.Generic;
using System.Text;

namespace ExtDataSourceN5CGL_TINY
{
    class Coil : SwdOnlineIF.IProduct
    {
        DBAccessN5CGL_TINY.CoilListItem m_coil_base;
        int[] m_types;


        internal Coil(DBAccessN5CGL_TINY.CoilListItem coil_base, int[] types)
        {
            m_coil_base = coil_base;
            m_types = types;
        }

        internal DBAccessN5CGL_TINY.CoilListItem CoilBase
        {
            get { return m_coil_base; }
        }

        #region IProduct メンバ

        /// <summary>
        /// コイル一覧に表示するデータを取得
        /// </summary>
        /// <remarks>DataSource.csのListColumnTitleに定義した項目と合わせる</remarks>
        public string[] ListRowText
        {
            get
            {
                System.Collections.Generic.List<string> text = new List<string>();

                text.Add(this.ProductionDateTime.ToString("yyyy/MM/dd HH:mm:ss"));
                text.Add(this.ProductNum);
                text.Add(this.CoilBase.InspLevelUp);
                text.Add(this.CoilBase.InspLevelDown);
                text.Add(this.CoilBase.PrimaryContext);
                text.Add(this.CoilBase.SurfProcDig1);
                text.Add(this.CoilBase.NextProcSched);
                text.Add(this.CoilBase.TotalNDefectsUp.ToString());
                text.Add(this.CoilBase.TotalNDefectsDn.ToString());
                foreach (int type in m_types)
                {
                    int n_up = m_coil_base.GetNDefectsOnType(SwdData.InspSurface.Up, type);
                    text.Add(0 == n_up ? "" : n_up.ToString());
                    int n_dn = m_coil_base.GetNDefectsOnType(SwdData.InspSurface.Down, type);
                    text.Add(0 == n_dn ? "" : n_dn.ToString());
                }
                text.Add(this.ProductWidth.ToString());
                text.Add((this.CoilBase.LengthAfterCut / 1000.0F).ToString());
                text.Add(this.ProductThickness.ToString());
                text.Add(m_coil_base.SurfaceUp);
                text.Add(m_coil_base.SurfaceDown);

                return text.ToArray();
            }
        }

        public string ProductNum
        {
            get { return m_coil_base.CoilNum; }
        }

        public DateTime ProductionDateTime
        {
            get { return m_coil_base.DateTime; }
        }

        /// <summary>
        /// カット前(入側コイル)の長さ[m]
        /// </summary>
        public float ProductLength
        {
            get { return m_coil_base.LengthBase / 1000.0F; }
        }

        public float ProductWidth
        {
            get { return m_coil_base.Width; }
        }

        public float ProductThickness
        {
            get { return m_coil_base.Thickness; }
        }

        #endregion
    }
}
