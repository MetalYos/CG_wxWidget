#include "Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image/stb_image.h"

Renderer::Renderer(int width, int height)
	: m_Width(width), m_Height(height), m_DC(NULL), m_ZBuffer(nullptr)
{
}

Renderer::~Renderer()
{
    delete[] m_ZBuffer;
}

void Renderer::SetDeviceContext(wxDC* dc)
{
    m_DC = dc;
}

void Renderer::SetHeight(int height)
{
	m_Height = height;
    BuildToScreenMatrix();
}

int Renderer::GetHeight() const
{
	return m_Height;
}

void Renderer::SetWidth(int width)
{
	m_Width = width;
    BuildToScreenMatrix();
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
	
	return aspectRatio;
}

const Mat4& Renderer::GetToScreenMatrix() const
{
    return m_ToScreen;
}

void Renderer::DrawPixel(int x, int y, const wxColour& color, int thickness)
{
    m_DC->SetPen(wxPen(color, 1));

    if (thickness == 0)
        m_DC->DrawPoint(x, y);
    else
    {
        // Draw thickness
		int startX = x;
		int endX = x;
		int startY = y;
		int endY = y;
		for (int i = thickness; i > 0; i--)
		{
			if ((startX == x) && (x - i >= 0))
				startX = x - i;
			if ((endX == x) && (x + i < m_Width))
				endX = x + i;
			if ((startY == y) && (y - i >= 0))
				startY = y - i;
			if ((endY == y) && (y + i < m_Height))
				endY = y + i;
		}

		for (int xPix = startX; xPix <= endX; xPix++)
		{
			for (int yPix = startY; yPix <= endY; yPix++)
				m_DC->DrawPoint(xPix, yPix);
		}
    }
}

void Renderer::DrawLine(const Vec4& p0, const Vec4& p1, const wxColour& color, int thickness)
{
    if (m_DC == NULL)
        return;

    int x1 = (int)p0[0];
    int y1 = (int)p0[1];
    int x2 = (int)p1[0];
    int y2 = (int)p1[1];

    int delta_x(x2 - x1);
    // if x1 == x2, then it does not matter what we set here
    signed char const ix((delta_x > 0) - (delta_x < 0));
    delta_x = std::abs(delta_x) << 1;
 
    int delta_y(y2 - y1);
    // if y1 == y2, then it does not matter what we set here
    signed char const iy((delta_y > 0) - (delta_y < 0));
    delta_y = std::abs(delta_y) << 1;

    DrawPixel(x1, y1, color, thickness);
 
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
 
            DrawPixel(x1, y1, color, thickness);
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
 
            DrawPixel(x1, y1, color, thickness);
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

    LOG_INFO("Loaded Background image with size: ({0}, {1})", width, height);
    LOG_INFO("Loaded Background image with Number of Channels: {0}", numChannels);
    
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

void Renderer::DrawEdge(const Vec4& p0, const Vec4& p1, const Mat4& objectToWorld, const Mat4& camTransform,
    const Mat4& viewTransform, const Mat4& projection, const wxColour& color, int thickness)
{
    // Get vertices positions in object space
    Vec4 pos1 = p0;
    Vec4 pos2 = p1;

    // Transform vertices from object space to NDC
    pos1 = pos1 * objectToWorld * camTransform * viewTransform * projection;
    pos2 = pos2 * objectToWorld * camTransform * viewTransform * projection;

    // Divide by w
    pos1 /= pos1[3];
    pos2 /= pos2[3];

    // Transform to screen space
    Vec4 pos1Pix = pos1 * m_ToScreen;
    Vec4 pos2Pix = pos2 * m_ToScreen;
    
    // Draw Edge
    DrawLine(pos1Pix, pos2Pix, color, thickness);
}

void Renderer::DrawPolygon(Polygon* poly, Model* model, const Mat4& objectToWorld, 
    const Mat4& camTransform, const Mat4& viewTransform, const Mat4& projection, const wxColour& color)
{
    for (unsigned int i = 0; i < poly->Vertices.size(); i++)
    {
        // Get vertices positions in object space
        Vec4 pos1 = model->VertexPositions[poly->Vertices[i]->PositionID];
        Vec4 pos2 = model->VertexPositions[poly->Vertices[(i + 1) % poly->Vertices.size()]->PositionID];

        DrawEdge(pos1, pos2, objectToWorld, camTransform, viewTransform, projection, color);
    }
}

void Renderer::InitZBuffer()
{
    delete[] m_ZBuffer;
    unsigned int size = (unsigned int)(m_Width * m_Height);
    m_ZBuffer = new double[size];
    for (unsigned int i = 0; i < size; i++)
        m_ZBuffer[i] = -std::numeric_limits<double>::max();
}

void Renderer::FillPolygon(Model* model, Polygon* p, const Mat4& camTransform,
    const Mat4& projection, const Vec4& color)
{
    Mat4 objectToWorld = model->GetObjectToWorldTransform();
    Mat4 viewTransform = model->GetViewTransform();

    // Build Edges and send to ScanConvert
    std::vector<Edge> poly;
    for (unsigned int i = 0; i < p->Vertices.size(); i++)
    {
        Vertex* v1 = p->Vertices[i];
        Vertex* v2 = p->Vertices[(i + 1) % p->Vertices.size()];
        // Get vertices positions and normals in object space
        Vec4 pos1 = model->VertexPositions[v1->PositionID];
        Vec4 pos2 = model->VertexPositions[v2->PositionID];
        Vec4 norm1 = model->VertexNormals[v1->NormalID];
        Vec4 norm2 = model->VertexNormals[v2->NormalID];

        // Transform vertices and normals from object space to Camera space
        Vec4 pos1VS = pos1 * objectToWorld * camTransform * viewTransform;
        Vec4 pos2VS = pos2 * objectToWorld * camTransform * viewTransform;
        Vec4 normal1VS = norm1 * objectToWorld * camTransform * viewTransform;
        Vec4 normal2VS = norm2 * objectToWorld * camTransform * viewTransform;

        // Transform vertices from Camera space to NDC
        Vec4 pos1Prj = pos1VS * projection;
        Vec4 pos2Prj = pos2VS * projection;

        // Divide by w
        pos1Prj /= pos1Prj[3];
        pos2Prj /= pos2Prj[3];

        // Transform to screen space
        Vec4 pos1Pix = pos1Prj * m_ToScreen;
        Vec4 pos2Pix = pos2Prj * m_ToScreen;

        // Build first Vertex
        DVertex dv1;
        dv1.Pixel = Point(pos1Pix);
        dv1.PosVS = pos1VS;
        dv1.NormalVS = normal1VS;
        dv1.Z = pos1Prj[2];
        dv1.Color = color;

        // Build second Vertex
        DVertex dv2;
        dv2.Pixel = Point(pos2Pix);
        dv2.PosVS = pos2VS;
        dv2.NormalVS = normal2VS;
        dv2.Z = pos2Prj[2];
        dv2.Color = color;

        poly.push_back({ dv1, dv2 });
    }

    Vec4 polyCenter = p->Center * objectToWorld * camTransform * viewTransform;
    Vec4 polyNormal = p->Normal * objectToWorld * camTransform * viewTransform;
    wxColour colorToSC((unsigned int)color[0], (unsigned int)color[1], (unsigned int)color[2]);
    ScanConvert(poly, colorToSC, polyCenter, polyNormal);
}

void Renderer::ScanConvert(std::vector<Edge>& poly, wxColour& color, const Vec4& polyCenter, const Vec4& polyNormal)
{
	assert(poly.size() > 2);
    
	// Sort edges according to the ymin value
	EdgeSorterY sorter;
	std::sort(poly.begin(), poly.end(), sorter);

	// find ymax of edges in poly
	int ymax = poly[0].A.Pixel.y;
	for (unsigned int i = 0; i < poly.size(); i++)
	{
		if (ymax <= poly[i].A.Pixel.y)
			ymax = poly[i].A.Pixel.y;
		if (ymax <= poly[i].B.Pixel.y)
			ymax = poly[i].B.Pixel.y;
	}
	
	int ymin = std::max(std::min(poly[0].A.Pixel.y, poly[0].B.Pixel.y), 0);

	// Iterate over scan lines from ymin to ymax
    std::vector<Edge> activeList;
	for (int y = ymin; y <= ymax; y++)
	{
		// Iterate over the edges in Poly
		for (auto it = poly.begin(); it != poly.end(); ++it)
		{
			Edge polyEdge = *it;
			int edgeYMin = (polyEdge.A.Pixel.y < polyEdge.B.Pixel.y) ? polyEdge.A.Pixel.y : polyEdge.B.Pixel.y;
			int edgeYMax = (polyEdge.A.Pixel.y < polyEdge.B.Pixel.y) ? polyEdge.B.Pixel.y : polyEdge.A.Pixel.y;

			if (activeList.size() == 0 && edgeYMin <= y)
			{
				activeList.push_back(polyEdge);
				continue;
			}

			// Iterate over the edges in active list
			for (unsigned int i = 0; i < activeList.size(); i++)
			{
				// If the poly edge is not in the active list and it's ymin is smaller
				// than the scanline, add it to the active list
				if ((polyEdge.A.Pixel != activeList[i].A.Pixel) || (polyEdge.B.Pixel != activeList[i].B.Pixel))
				{
					if (edgeYMin <= y)
					{
						activeList.push_back(polyEdge);
						break;
					}
				}
				else
					break;
			}
			for (unsigned int i = 0; i < activeList.size(); i++)
			{
				// If the poly edge is in the active list and it's ymax is smaller
				// than the scanline, remove it from the active list
				if (((polyEdge.A.Pixel == activeList[i].A.Pixel) && (polyEdge.B.Pixel == activeList[i].B.Pixel)) &&
					(edgeYMax < y))
					activeList.erase(activeList.begin() + i);
			}
		}

		EdgeComparer eComp;
		auto it = std::unique(activeList.begin(), activeList.end(), eComp);
		activeList.resize(std::distance(activeList.begin(), it));

		// Calculate points of intersections of A members with line Y = y
		std::vector<Intersection> intersections;
		for (Edge e : activeList)
		{
			int dy = e.B.Pixel.y - e.A.Pixel.y;
			if (dy == 0)
				continue;
			int dx = e.B.Pixel.x - e.A.Pixel.x;

			Intersection i;
			// Calculate X axis intersections
			int x = (int)floor((y * dx + (e.A.Pixel.x * e.B.Pixel.y - e.B.Pixel.x * e.A.Pixel.y)) / dy);
			i.x = x;

			// Calculate Z pos at intersections
			double z = e.A.Z - (e.A.Z - e.B.Z) * ((double)(e.A.Pixel.y - y) / (double)(e.A.Pixel.y - e.B.Pixel.y));
			i.z = z;

			intersections.push_back(i);
		}

		// Sort intersections by decreasing x values
		IntersectionsSorter sorter;
		std::sort(intersections.begin(), intersections.end(), sorter);
		
		IntersectionsComparer comp;
		auto it2 = std::unique(intersections.begin(), intersections.end(), comp);
		intersections.resize(std::distance(intersections.begin(), it2));

		// Draw line according to shading and zbuffer
		for (unsigned int i = 0; i < intersections.size(); i += 2)
		{
			// Get intersections x pixel pos
			int x0 = intersections[i].x;
			if ((i + 1) >= intersections.size())
				break;
			int x1 = intersections[i + 1].x;

			// Get intersections z pos
			double z0 = intersections[i].z;
			if ((i + 1) >= intersections.size())
				break;
			double z1 = intersections[i + 1].z;

			int x = x0;
			while (x <= x1)
			{
				// Caluclate zPos and pos at (x, y)
				double zp = z1 - (z1 - z0) * ((double)(x1 - x) / (double)(x1 - x0));

				// Compare z Pos to zBuffer, if z Pos > zBuffer,
				// Draw and update z buffer
				int index = std::max(0, std::min(x + m_Width * y, m_Width * m_Height - 1));
				if (zp > m_ZBuffer[index])
				{
					DrawPixel(x, y, color);
					m_ZBuffer[index] = zp;
				}
				x++;
			}
		}
	}
}