#include "Renderer.h"

Renderer::Renderer(int width, int height)
	: m_Width(width), m_Height(height), m_DC(NULL)
{
}

void Renderer::SetDeviceContext(wxDC* dc)
{
    m_DC = dc;
}

void Renderer::SetHeight(int height)
{
	m_Height = height;
    BuildToScreenMatrix();
    LOG_TRACE("Renderer::SetHeight: Changed renderer height to: {0}", m_Height);
}

int Renderer::GetHeight() const
{
	return m_Height;
}

void Renderer::SetWidth(int width)
{
	m_Width = width;
    BuildToScreenMatrix();
    LOG_TRACE("Renderer::SetWidth: Changed renderer width to: {0}", m_Width);
}

int Renderer::GetWidth() const
{
	return m_Width;
}

double Renderer::GetAspectRatio() const
{
    double aspectRatio = 1.0;
	if (m_Height != 0)
		aspectRatio = (double)m_Width / (double)m_Height;
	
    LOG_TRACE("Renderer::GetAspectRatio: Calculated Aspect Ratio: {0}", aspectRatio);
	return aspectRatio;
}

const Mat4& Renderer::GetToScreenMatrix() const
{
    return m_ToScreen;
}

void Renderer::DrawLine(const Point& p0, const Point& p1, const wxColour& color)
{
    if (m_DC == NULL)
        return;

    int x1 = p0.X;
    int y1 = p0.Y;
    int x2 = p1.X;
    int y2 = p1.Y;

    int delta_x(x2 - x1);
    // if x1 == x2, then it does not matter what we set here
    signed char const ix((delta_x > 0) - (delta_x < 0));
    delta_x = std::abs(delta_x) << 1;
 
    int delta_y(y2 - y1);
    // if y1 == y2, then it does not matter what we set here
    signed char const iy((delta_y > 0) - (delta_y < 0));
    delta_y = std::abs(delta_y) << 1;
    
    m_DC->SetPen(wxPen(color, 1));

    m_DC->DrawPoint(x1, y1);
 
    if (delta_x >= delta_y)
    {
        // error may go below zero
        int error(delta_y - (delta_x >> 1));
 
        while (x1 != x2)
        {
            // reduce error, while taking into account the corner case of error == 0
            if ((error > 0) || (!error && (ix > 0)))
            {
                error -= delta_x;
                y1 += iy;
            }
            // else do nothing
 
            error += delta_y;
            x1 += ix;
 
            m_DC->DrawPoint(x1, y1);
        }
    }
    else
    {
        // error may go below zero
        int error(delta_x - (delta_y >> 1));
 
        while (y1 != y2)
        {
            // reduce error, while taking into account the corner case of error == 0
            if ((error > 0) || (!error && (iy > 0)))
            {
                error -= delta_y;
                x1 += ix;
            }
            // else do nothing
 
            error += delta_x;
            y1 += iy;
 
            m_DC->DrawPoint(x1, y1);
        }
    }
}

void Renderer::DrawBackgeound(const wxColour& color)
{
    if (m_DC == NULL)
        return;
    
    m_DC->SetBrush(wxBrush(color));
    m_DC->SetPen(wxPen(color, 1));

    wxRect windowRect(wxPoint(0, 0), wxSize(m_Width, m_Height));    

    m_DC->DrawRectangle(windowRect);
}

void Renderer::BuildToScreenMatrix()
{
    Mat4 result;
    result[0][0] = m_Width / 2.0;
    result[1][1] = m_Height / 2.0;
    result[3][0] = (m_Width - 1) / 2.0;
    result[3][1] = (m_Height - 1) / 2.0;

    m_ToScreen = result;
}
