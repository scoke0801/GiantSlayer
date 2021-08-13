#pragma once

constexpr int MAX_NUM_TEXT_CHARACTERS = 1024;
class CTextVertex;
struct FontChar
{
    // the unicode id
    int id;

    // these need to be converted to texture coordinates 
    // (where 0.0 is 0 and 1.0 is textureWidth of the font)
    float u; // u texture coordinate
    float v; // v texture coordinate
    float twidth; // width of character on texture
    float theight; // height of character on texture

    float width; // width of character in screen coords
    float height; // height of character in screen coords

    // these need to be normalized based on size of font
    float xoffset; // offset from current cursor pos to left side of character
    float yoffset; // offset from top of line to top of character
    float xadvance; // how far to move to right for next character
};
 
struct FontKerning
{
    int firstid; // the first character
    int secondid; // the second character
    float amount; // the amount to add/subtract to second characters x
};

struct Font
{
    std::wstring name; // name of the font
    std::wstring fontImage;
    int size; // size of font, lineheight and baseheight will be based on this as if this is a single unit (1.0)
    float lineHeight; // how far to move down to next line, will be normalized
    float baseHeight; // height of all characters, will be normalized
    int textureWidth; // width of the font texture
    int textureHeight; // height of the font texture
    int numCharacters; // number of characters in the font
    FontChar* CharList; // list of characters
    int numKernings; // the number of kernings
    FontKerning* KerningsList; // list to hold kerning values
    ID3D12Resource* textureBuffer; // the font texture resource
    D3D12_GPU_DESCRIPTOR_HANDLE srvHandle; // the font srv

    // these are how much the character is padded in the texture. We
    // add padding to give sampling a little space so it does not accidentally
    // padd the surrounding characters. We will need to subtract these paddings
    // from the actual spacing between characters to remove the gaps you would otherwise see
    float leftpadding;
    float toppadding;
    float rightpadding;
    float bottompadding;

    // this will return the amount of kerning we need to use for two characters
    float GetKerning(wchar_t first, wchar_t second)
    {
        for (int i = 0; i < numKernings; ++i)
        {
            if ((wchar_t)KerningsList[i].firstid == first && (wchar_t)KerningsList[i].secondid == second)
                return KerningsList[i].amount;
        }
        return 0.0f;
    }

    // this will return a FontChar given a wide character
    FontChar* GetChar(wchar_t c)
    {
        for (int i = 0; i < numCharacters; ++i)
        {
            if (c == (wchar_t)CharList[i].id)
                return &CharList[i];
        }
        return nullptr;
    }
};

Font LoadFont(const string& fileName, int width, int height);
int LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC& resourceDescription, LPCWSTR filename, int& bytesPerRow);

DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID);
WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID);
int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat);

constexpr int MAX_TEXT_NUM = 10;

class TextHandler
{
private:
    TextHandler() {};

    TextHandler(const TextHandler& other) = delete;
    TextHandler& operator=(const TextHandler& other) = delete;

public:
    static TextHandler& GetInstance() {
        static TextHandler self;
        return self;
    }

    void Load(const string& fileName);

    void Render(ID3D12GraphicsCommandList* pd3dCommandList, wstring text, int index, XMFLOAT2 pos, 
        XMFLOAT2 scale = XMFLOAT2(1.0f, 1.0f), XMFLOAT2 padding = XMFLOAT2(0.5f, 0.0f), XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

    bool InitVertexBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12DescriptorHeap* d3dDescripotrHeap, int handlePosition);

private:
    Font m_Font;
     
    ID3D12Resource* textVertexBuffer[MAX_TEXT_NUM];
    D3D12_VERTEX_BUFFER_VIEW textVertexBufferView[MAX_TEXT_NUM]; // a view for our text vertex buffer
    CTextVertex* textVBGPUAddress[MAX_TEXT_NUM]; // this is a pointer to each of the text constant buffers 
};