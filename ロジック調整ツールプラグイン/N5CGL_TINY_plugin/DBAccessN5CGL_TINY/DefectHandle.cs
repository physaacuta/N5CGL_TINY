using System;
using System.Collections.Generic;
using System.Text;

namespace DBAccessN5CGL_TINY
{
    public class DefectHandle
    {
        DateTime m_datetime;
        string m_filename;
        SwdData.InspSurface m_surface;
        int m_defectnum;

        public DefectHandle(DateTime datetime, string filename, SwdData.InspSurface surface, int defectnum)
        {
            m_datetime = datetime;
            m_filename = filename;
            m_surface = surface;
            m_defectnum = defectnum;
        }

        public DateTime DateTime
        {
            get { return m_datetime; }
        }

        public string FileName
        {
            get { return m_filename; }
        }

        public SwdData.InspSurface Surface
        {
            get { return m_surface; }
        }

        public int DefectNum
        {
            get { return m_defectnum; }
        }
    }
}
