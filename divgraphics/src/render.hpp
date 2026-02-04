#pragma once
#include <raylib.h>
#include <rlgl.h>

const bool FIX_ARTIFACTS_BY_STRECHING_TEXEL = true;

struct rVertex
{
    float x, y, z;
    Color col;
    float tx, ty;
};

struct rQuad
{
    rVertex v[4];
    Texture2D tex;
    int blend;
};

inline void RenderQuad(const rQuad *quad)
{

    rlCheckRenderBatchLimit(4); // Make sure there is enough free space on the batch buffer
    rlSetTexture(quad->tex.id);

    rlBegin(RL_QUADS);

    Color a = quad->v[1].col;
    Color b = quad->v[0].col;
    Color c = quad->v[3].col;
    Color d = quad->v[2].col;

    rlNormal3f(0.0f, 0.0f, 1.0f);

    rlColor4ub(a.r, a.g, a.b, a.a);
    rlTexCoord2f(quad->v[1].tx, quad->v[1].ty);
    rlVertex3f(quad->v[1].x, quad->v[1].y, quad->v[1].z);

    rlColor4ub(b.r, b.g, b.b, b.a);
    rlTexCoord2f(quad->v[0].tx, quad->v[0].ty);
    rlVertex3f(quad->v[0].x, quad->v[0].y, quad->v[0].z);

    rlColor4ub(c.r, c.g, c.b, c.a);
    rlTexCoord2f(quad->v[3].tx, quad->v[3].ty);
    rlVertex3f(quad->v[3].x, quad->v[3].y, quad->v[3].z);

    rlColor4ub(d.r, d.g, d.b, d.a);
    rlTexCoord2f(quad->v[2].tx, quad->v[2].ty);
    rlVertex3f(quad->v[2].x, quad->v[2].y, quad->v[2].z);

    rlEnd();
}



inline void RenderClipSize(Texture2D texture, float x, float y, float width, float height, Rectangle clip, bool flipX, bool flipY, Color color, int blend)
{

    rQuad quad;
    quad.tex = texture;
    quad.blend = blend;

    int widthTex = texture.width;
    int heightTex = texture.height;

    float left;
    float right;
    float top;
    float bottom;

    if (FIX_ARTIFACTS_BY_STRECHING_TEXEL)
    {
        left = (2 * clip.x + 1) / (2 * widthTex);
        right = left + (clip.width * 2 - 2) / (2 * widthTex);
        top = (2 * clip.y + 1) / (2 * heightTex);
        bottom = top + (clip.height * 2 - 2) / (2 * heightTex);
    }
    else
    {
        left = clip.x / widthTex;
        right = (clip.x + clip.width) / widthTex;
        top = clip.y / heightTex;
        bottom = (clip.y + clip.height) / heightTex;
    }

    if (flipX)
    {
        float tmp = left;
        left = right;
        right = tmp;
    }

    if (flipY)
    {
        float tmp = top;
        top = bottom;
        bottom = tmp;
    }

    float TempX1 = x;
    float TempY1 = y;
    float TempX2 = x + width;
    float TempY2 = y + height;

    quad.v[1].x = TempX1;
    quad.v[1].y = TempY1;
    quad.v[1].tx = left;
    quad.v[1].ty = top;

    quad.v[0].x = TempX1;
    quad.v[0].y = TempY2;
    quad.v[0].tx = left;
    quad.v[0].ty = bottom;

    quad.v[3].x = TempX2;
    quad.v[3].y = TempY2;
    quad.v[3].tx = right;
    quad.v[3].ty = bottom;

    quad.v[2].x = TempX2;
    quad.v[2].y = TempY1;
    quad.v[2].tx = right;
    quad.v[2].ty = top;

    quad.v[0].z = quad.v[1].z = quad.v[2].z = quad.v[3].z = 0.0f;
    quad.v[0].col = quad.v[1].col = quad.v[2].col = quad.v[3].col = color;

    RenderQuad(&quad);
}

inline void RenderTexturePivotVertices(Texture2D texture, int x_pivot, int y_pivot, float spin, Rectangle clip, bool flipx, bool flipy, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, Color c)
{

    rQuad quad;
    quad.tex = texture;

    int widthTex = texture.width;
    int heightTex = texture.height;

    float left, right, top, bottom;

    if (FIX_ARTIFACTS_BY_STRECHING_TEXEL)
    {

        left = (2 * clip.x + 1) / (2 * widthTex);
        right = left + (clip.width * 2 - 2) / (2 * widthTex);
        top = (2 * clip.y + 1) / (2 * heightTex);
        bottom = top + (clip.height * 2 - 2) / (2 * heightTex);
    }
    else
    {
        left = clip.x / widthTex;
        right = (clip.x + clip.width) / widthTex;
        top = clip.y / heightTex;
        bottom = (clip.y + clip.height) / heightTex;
    }

    if (flipx)
    {
        float tmp = left;
        left = right;
        right = tmp;
    }

    if (flipy)
    {
        float tmp = top;
        top = bottom;
        bottom = tmp;
    }

    quad.v[0].tx = left;
    quad.v[0].ty = top;
    quad.v[1].tx = right;
    quad.v[1].ty = top;
    quad.v[2].tx = right;
    quad.v[2].ty = bottom;
    quad.v[3].tx = left;
    quad.v[3].ty = bottom;

    quad.v[0].x = x1;
    quad.v[0].y = y1;
    quad.v[1].x = x2;
    quad.v[1].y = y2;
    quad.v[2].x = x3;
    quad.v[2].y = y3;
    quad.v[3].x = x4;
    quad.v[3].y = y4;

    quad.v[0].z = quad.v[1].z = quad.v[2].z = quad.v[3].z = 0.0f;
    quad.v[0].col = quad.v[1].col = quad.v[2].col = quad.v[3].col = c;

    RenderQuad(&quad);
}

inline void RenderNormal(Texture2D texture, float x, float y, int blend)
{

    rQuad quad;
    quad.tex = texture;
    quad.blend = blend;

    float u = 0.0f;
    float v = 0.0f;
    float u2 = 1.0f;
    float v2 = 1.0f;

    float fx2 = x + texture.width;
    float fy2 = y + texture.height;

    quad.v[1].x = x;
    quad.v[1].y = y;
    quad.v[1].tx = u;
    quad.v[1].ty = v;

    quad.v[0].x = x;
    quad.v[0].y = fy2;
    quad.v[0].tx = u;
    quad.v[0].ty = v2;

    quad.v[3].x = fx2;
    quad.v[3].y = fy2;
    quad.v[3].tx = u2;
    quad.v[3].ty = v2;

    quad.v[2].x = fx2;
    quad.v[2].y = y;
    quad.v[2].tx = u2;
    quad.v[2].ty = v;

    quad.v[0].z = quad.v[1].z = quad.v[2].z = quad.v[3].z = 0.0f;
    quad.v[0].col = quad.v[1].col = quad.v[2].col = quad.v[3].col = WHITE;

    RenderQuad(&quad);
}

inline void RenderTexturePivotRotateSize(Texture2D texture, int x_pivot, int y_pivot, float x, float y, float spin, float size, bool flipx, bool flipy, Color c)
{
    Rectangle clip;
    clip.x = 0;
    clip.y = 0;
    clip.width = texture.width;
    clip.height = texture.height;

    float TX1 = -x_pivot * size;
    float TY1 = -y_pivot * size;
    float TX2 = (clip.width - x_pivot) * size;
    float TY2 = (clip.height - y_pivot) * size;

    float angle = spin * DEG2RAD;

    float SinT = sinf(angle);
    float CosT = cosf(angle);

    RenderTexturePivotVertices(texture,
                               x_pivot, y_pivot,
                               spin,
                               clip,
                               flipx, flipy,
                               TX1 * CosT - TY1 * SinT + x, TX1 * SinT + TY1 * CosT + y,
                               TX2 * CosT - TY1 * SinT + x, TX2 * SinT + TY1 * CosT + y,
                               TX2 * CosT - TY2 * SinT + x, TX2 * SinT + TY2 * CosT + y,
                               TX1 * CosT - TY2 * SinT + x, TX1 * SinT + TY2 * CosT + y, c);
}

 
inline void RenderQuadUV(float x, float y, float w, float h,
                         float uv_x1, float uv_y1, float uv_x2, float uv_y2,
                         Texture2D tex, Color tint = WHITE)
{
    rlCheckRenderBatchLimit(4);
    rlSetTexture(tex.id);
    rlBegin(RL_QUADS);

    rlColor4ub(tint.r, tint.g, tint.b, tint.a);
    rlNormal3f(0.0f, 0.0f, 1.0f);

    // Top-right
    rlTexCoord2f(uv_x2, uv_y1);  
    rlVertex3f(x + w, y, 0.0f);

    // Top-left
    rlTexCoord2f(uv_x1, uv_y1);  
    rlVertex3f(x, y, 0.0f);

    // Bottom-left
    rlTexCoord2f(uv_x1, uv_y2);  
    rlVertex3f(x, y + h, 0.0f);

    // Bottom-right
    rlTexCoord2f(uv_x2, uv_y2); 
    rlVertex3f(x + w, y + h, 0.0f);

    rlEnd();
}

inline void RenderTexturePivotRotateSizeXY(Texture2D texture, int x_pivot, int y_pivot,
                                           Rectangle clip,
                                           float x, float y, float spin,
                                           float size_x, float size_y,
                                           bool flipx, bool flipy, Color c)
{

    float TX1 = -x_pivot * size_x;
    float TY1 = -y_pivot * size_y;
    float TX2 = (clip.width - x_pivot) * size_x;
    float TY2 = (clip.height - y_pivot) * size_y;

    float angle = spin * DEG2RAD;
    float SinT = sinf(angle);
    float CosT = cosf(angle);

    RenderTexturePivotVertices(texture,
                               x_pivot, y_pivot,
                               spin,
                               clip,
                               flipx, flipy,
                               TX1 * CosT - TY1 * SinT + x, TX1 * SinT + TY1 * CosT + y,
                               TX2 * CosT - TY1 * SinT + x, TX2 * SinT + TY1 * CosT + y,
                               TX2 * CosT - TY2 * SinT + x, TX2 * SinT + TY2 * CosT + y,
                               TX1 * CosT - TY2 * SinT + x, TX1 * SinT + TY2 * CosT + y, c);
}
