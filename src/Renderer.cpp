#include "Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image/stb_image.h"

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

void Renderer::DrawPixel(const Point& p, const wxColour& color, int thickness)
{
    m_DC->SetPen(wxPen(color, 1));

    if (thickness == 0)
        m_DC->DrawPoint(p.X, p.Y);
    else
    {
        // Draw thickness
		int startX = p.X;
		int endX = p.X;
		int startY = p.Y;
		int endY = p.Y;
		for (int i = thickness; i > 0; i--)
		{
			if ((startX == p.X) && (p.X - i >= 0))
				startX = p.X - i;
			if ((endX == p.X) && (p.X + i < m_Width))
				endX = p.X + i;
			if ((startY == p.Y) && (p.Y - i >= 0))
				startY = p.Y - i;
			if ((endY == p.Y) && (p.Y + i < m_Height))
				endY = p.Y + i;
		}

		for (int x = startX; x <= endX; x++)
		{
			for (int y = startY; y <= endY; y++)
				m_DC->DrawPoint(x, y);
		}
    }
}

void Renderer::DrawPixel(int x, int y, const wxColour& color, int thickness)
{
    Point p(x, y);
    DrawPixel(p, color, thickness);
}

void Renderer::DrawLine(const Point& p0, const Point& p1, const wxColour& color, int thickness)
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

    DrawPixel(Point(x1, y1), color, thickness);
 
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
 
            DrawPixel(Point(x1, y1), color, thickness);
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
 
            DrawPixel(Point(x1, y1), color, thickness);
        }
    }
}

void Renderer::DrawBackground(const Vec4& color)
{
    if (m_DC == NULL)
        return;
    
    wxColour col((unsigned char)color[0], (unsigned char)color[1], (unsigned char)color[2]);
    m_DC->SetBrush(wxBrush(col));
    m_DC->SetPen(wxPen(col, 1));

    wxRect windowRect(wxPoint(0, 0), wxSize(m_Width, m_Height));    

    m_DC->DrawRectangle(windowRect);
}

void Renderer::DrawBackgroundImage(const std::string& filename, bool stretch, 
    ImageInterpolationType interpolation)
{
    int width, height, numChannels;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &numChannels, 0);

    LOG_TRACE("Renderer::DrawBackgroundImage: Image Width: {0}", width);
    LOG_TRACE("Renderer::DrawBackgroundImage: Image Height: {0}", height);
    LOG_TRACE("Renderer::DrawBackgroundImage: Image Number of Channels: {0}", numChannels);
    
    if (data == NULL)
        return;
    
    if (stretch)
    {
        double cx = (double)m_Width / (double)width;    // Scale in X
        double cy = (double)m_Height / (double)height;  // Scale in Y
        for (int x = 0; x < m_Width; x++)
        {
            for (int y = 0; y < m_Height; y++)
            {
                if (interpolation == IMG_NEAREST_NEIGHBOUR)
                {
                    // Calculate Position in input image
                    double v = x / cx;
                    double w = y / cy;

                    // We'll pick the nearest neighbour to (v, w)
                    int vInt = (int)round(v);
                    int wInt = (int)round(w);

                    Vec4 colorVec = GetStbColor(data, numChannels, vInt, wInt, width);

                    wxColour color((unsigned char)colorVec[0], (unsigned char)colorVec[1], (unsigned char)colorVec[2]);
                    DrawPixel(x, y, color);
                }
                else if (interpolation == IMG_BILINEAR)
                {
                    // Calculate Position in input image
                    double v = x / cx;
                    double w = y / cy;

                    // 4 neighbours in input image
                    int x0 = (int)v;
                    int y0 = (int)w;
                    int x1 = MinInt(x0 + 1, m_Width);
                    int y1 = MinInt(y0 + 1, m_Height);

                    Vec4 color00 = GetStbColor(data, numChannels, x0, y0, width);
                    Vec4 color01 = GetStbColor(data, numChannels, x0, y1, width);
                    Vec4 color10 = GetStbColor(data, numChannels, x1, y0, width);
                    Vec4 color11 = GetStbColor(data, numChannels, x1, y1, width);

                    Vec4 color0 = color00 * (((double)x1 - v) / (double)(x1 - x0)) +
                        color10 * ((v - (double)x0) / (double)(x1 - x0));
                    Vec4 color1 = color01 * (((double)x1 - v) / (double)(x1 - x0)) +
                        color11 * ((v - (double)x0) / (double)(x1 - x0));
                    Vec4 color = color0 * (((double)y1 - w) / (double)(y1 - y0)) +
                        color1 * ((w - (double)y0) / (double)(y1 - y0));
                    
                    wxColour finalColor((unsigned char)color[0], (unsigned char)color[1], 
                        (unsigned char)color[2]);
                    DrawPixel(x, y, finalColor);
                }
            }
        }
    }
    else
    {
        for (int x = 0; x < m_Width; x++)
        {
            for (int y = 0; y < m_Height; y++)
            {
                int v = x % width;
                int w = y % height;
                
                Vec4 colorVec = GetStbColor(data, numChannels, v, w, width);

                wxColour color((unsigned char)colorVec[0], (unsigned char)colorVec[1], (unsigned char)colorVec[2]);
                DrawPixel(x, y, color);
            }
        }
    }

    stbi_image_free(data);
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

Vec4 Renderer::GetStbColor(unsigned char* data, unsigned numChannels, int x, int y, int width)
{
    Vec4 color;
    unsigned char* pixelOffset = data + (x + width * y) * numChannels;
    color[0] = pixelOffset[0];
    color[1] = pixelOffset[1];
    color[2] = pixelOffset[2];

    return color;
}
