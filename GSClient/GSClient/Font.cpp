#include "stdafx.h"
#include "Font.h"
#include "Mesh.h"
#include "Shader.h"
#include "ShaderHandler.h"

constexpr int maxNumTextCharacters = 1024; 
// the maximum number of characters you can render during a frame. 
// This is just used to make sure
// there is enough memory allocated for the text vertex buffer each frame

Font LoadFont(const string& fileName, int windowWidth, int windowHeight)
{
    std::wifstream fs;
    fs.open(fileName.c_str());

    Font font;
    std::wstring tmp;
    int startpos;

    // extract font name
    fs >> tmp >> tmp; // info face="Arial"
    startpos = tmp.find(L"\"") + 1;
    font.name = tmp.substr(startpos, tmp.size() - startpos - 1);

    // get font size
    fs >> tmp; // size=73
    startpos = tmp.find(L"=") + 1;
    font.size = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // bold, italic, charset, unicode, stretchH, smooth, aa, padding, spacing
    fs >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp; // bold=0 italic=0 charset="" unicode=0 stretchH=100 smooth=1 aa=1 

    // get padding
    fs >> tmp; // padding=5,5,5,5 
    startpos = tmp.find(L"=") + 1;
    tmp = tmp.substr(startpos, tmp.size() - startpos); // 5,5,5,5

    // get up padding
    startpos = tmp.find(L",") + 1;
    font.toppadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

    // get right padding
    tmp = tmp.substr(startpos, tmp.size() - startpos);
    startpos = tmp.find(L",") + 1;
    font.rightpadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

    // get down padding
    tmp = tmp.substr(startpos, tmp.size() - startpos);
    startpos = tmp.find(L",") + 1;
    font.bottompadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

    // get left padding
    tmp = tmp.substr(startpos, tmp.size() - startpos);
    font.leftpadding = std::stoi(tmp) / (float)windowWidth;

    fs >> tmp; // spacing=0,0

    // get lineheight (how much to move down for each line), and normalize (between 0.0 and 1.0 based on size of font)
    fs >> tmp >> tmp; // common lineHeight=95
    startpos = tmp.find(L"=") + 1;
    font.lineHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

    // get base height (height of all characters), and normalize (between 0.0 and 1.0 based on size of font)
    fs >> tmp; // base=68
    startpos = tmp.find(L"=") + 1;
    font.baseHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

    // get texture width
    fs >> tmp; // scaleW=512
    startpos = tmp.find(L"=") + 1;
    font.textureWidth = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // get texture height
    fs >> tmp; // scaleH=512
    startpos = tmp.find(L"=") + 1;
    font.textureHeight = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // get pages, packed, page id
    fs >> tmp >> tmp; // pages=1 packed=0
    fs >> tmp >> tmp; // page id=0

    // get texture filename
    std::wstring wtmp;
    fs >> wtmp; // file="Arial.png"
    startpos = wtmp.find(L"\"") + 1;
    font.fontImage = wtmp.substr(startpos, wtmp.size() - startpos - 1);

    // get number of characters
    fs >> tmp >> tmp; // chars count=97
    startpos = tmp.find(L"=") + 1;
    font.numCharacters = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // initialize the character list
    font.CharList = new FontChar[font.numCharacters];

    for (int c = 0; c < font.numCharacters; ++c)
    {
        // get unicode id
        fs >> tmp >> tmp; // char id=0
        startpos = tmp.find(L"=") + 1;
        font.CharList[c].id = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

        // get x
        fs >> tmp; // x=392
        startpos = tmp.find(L"=") + 1;
        font.CharList[c].u = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)font.textureWidth;

        // get y
        fs >> tmp; // y=340
        startpos = tmp.find(L"=") + 1;
        font.CharList[c].v = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)font.textureHeight;

        // get width
        fs >> tmp; // width=47
        startpos = tmp.find(L"=") + 1;
        tmp = tmp.substr(startpos, tmp.size() - startpos);
        font.CharList[c].width = (float)std::stoi(tmp) / (float)windowWidth;
        font.CharList[c].twidth = (float)std::stoi(tmp) / (float)font.textureWidth;

        // get height
        fs >> tmp; // height=57
        startpos = tmp.find(L"=") + 1;
        tmp = tmp.substr(startpos, tmp.size() - startpos);
        font.CharList[c].height = (float)std::stoi(tmp) / (float)windowHeight;
        font.CharList[c].theight = (float)std::stoi(tmp) / (float)font.textureHeight;

        // get xoffset
        fs >> tmp; // xoffset=-6
        startpos = tmp.find(L"=") + 1;
        font.CharList[c].xoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

        // get yoffset
        fs >> tmp; // yoffset=16
        startpos = tmp.find(L"=") + 1;
        font.CharList[c].yoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

        // get xadvance
        fs >> tmp; // xadvance=65
        startpos = tmp.find(L"=") + 1;
        font.CharList[c].xadvance = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

        // get page
        // get channel
        fs >> tmp >> tmp; // page=0    chnl=0
    }

    // get number of kernings
    fs >> tmp >> tmp; // kernings count=96
    startpos = tmp.find(L"=") + 1;
    font.numKernings = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // initialize the kernings list
    font.KerningsList = new FontKerning[font.numKernings];

    for (int k = 0; k < font.numKernings; ++k)
    {
        // get first character
        fs >> tmp >> tmp; // kerning first=87
        startpos = tmp.find(L"=") + 1;
        font.KerningsList[k].firstid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

        // get second character
        fs >> tmp; // second=45
        startpos = tmp.find(L"=") + 1;
        font.KerningsList[k].secondid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

        // get amount
        fs >> tmp; // amount=-1
        startpos = tmp.find(L"=") + 1;
        int t = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos));
        font.KerningsList[k].amount = (float)t / (float)windowWidth;
    }

    return font;
}

// get the dxgi format equivilent of a wic format
DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID)
{
    if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFloat) return DXGI_FORMAT_R32G32B32A32_FLOAT;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAHalf) return DXGI_FORMAT_R16G16B16A16_FLOAT;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBA) return DXGI_FORMAT_R16G16B16A16_UNORM;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA) return DXGI_FORMAT_R8G8B8A8_UNORM;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppBGRA) return DXGI_FORMAT_B8G8R8A8_UNORM;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR) return DXGI_FORMAT_B8G8R8X8_UNORM;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102XR) return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;

    else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102) return DXGI_FORMAT_R10G10B10A2_UNORM;
    else if (wicFormatGUID == GUID_WICPixelFormat16bppBGRA5551) return DXGI_FORMAT_B5G5R5A1_UNORM;
    else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR565) return DXGI_FORMAT_B5G6R5_UNORM;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFloat) return DXGI_FORMAT_R32_FLOAT;
    else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayHalf) return DXGI_FORMAT_R16_FLOAT;
    else if (wicFormatGUID == GUID_WICPixelFormat16bppGray) return DXGI_FORMAT_R16_UNORM;
    else if (wicFormatGUID == GUID_WICPixelFormat8bppGray) return DXGI_FORMAT_R8_UNORM;
    else if (wicFormatGUID == GUID_WICPixelFormat8bppAlpha) return DXGI_FORMAT_A8_UNORM;

    else return DXGI_FORMAT_UNKNOWN;
}

// get a dxgi compatible wic format from another wic format
WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID)
{
    if (wicFormatGUID == GUID_WICPixelFormatBlackWhite) return GUID_WICPixelFormat8bppGray;
    else if (wicFormatGUID == GUID_WICPixelFormat1bppIndexed) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat2bppIndexed) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat4bppIndexed) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat8bppIndexed) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat2bppGray) return GUID_WICPixelFormat8bppGray;
    else if (wicFormatGUID == GUID_WICPixelFormat4bppGray) return GUID_WICPixelFormat8bppGray;
    else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayFixedPoint) return GUID_WICPixelFormat16bppGrayHalf;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFixedPoint) return GUID_WICPixelFormat32bppGrayFloat;
    else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR555) return GUID_WICPixelFormat16bppBGRA5551;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR101010) return GUID_WICPixelFormat32bppRGBA1010102;
    else if (wicFormatGUID == GUID_WICPixelFormat24bppBGR) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat24bppRGB) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppPBGRA) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppPRGBA) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat48bppRGB) return GUID_WICPixelFormat64bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat48bppBGR) return GUID_WICPixelFormat64bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRA) return GUID_WICPixelFormat64bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBA) return GUID_WICPixelFormat64bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppPBGRA) return GUID_WICPixelFormat64bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
    else if (wicFormatGUID == GUID_WICPixelFormat48bppBGRFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
    else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
    else if (wicFormatGUID == GUID_WICPixelFormat128bppPRGBAFloat) return GUID_WICPixelFormat128bppRGBAFloat;
    else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFloat) return GUID_WICPixelFormat128bppRGBAFloat;
    else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
    else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBE) return GUID_WICPixelFormat128bppRGBAFloat;
    else if (wicFormatGUID == GUID_WICPixelFormat32bppCMYK) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppCMYK) return GUID_WICPixelFormat64bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat40bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat80bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
    else if (wicFormatGUID == GUID_WICPixelFormat32bppRGB) return GUID_WICPixelFormat32bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppRGB) return GUID_WICPixelFormat64bppRGBA;
    else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBAHalf) return GUID_WICPixelFormat64bppRGBAHalf;
#endif

    else return GUID_WICPixelFormatDontCare;
}

// get the number of bits per pixel for a dxgi format
int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat)
{
    if (dxgiFormat == DXGI_FORMAT_R32G32B32A32_FLOAT) return 128;
    else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_FLOAT) return 64;
    else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_UNORM) return 64;
    else if (dxgiFormat == DXGI_FORMAT_R8G8B8A8_UNORM) return 32;
    else if (dxgiFormat == DXGI_FORMAT_B8G8R8A8_UNORM) return 32;
    else if (dxgiFormat == DXGI_FORMAT_B8G8R8X8_UNORM) return 32;
    else if (dxgiFormat == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM) return 32;

    else if (dxgiFormat == DXGI_FORMAT_R10G10B10A2_UNORM) return 32;
    else if (dxgiFormat == DXGI_FORMAT_B5G5R5A1_UNORM) return 16;
    else if (dxgiFormat == DXGI_FORMAT_B5G6R5_UNORM) return 16;
    else if (dxgiFormat == DXGI_FORMAT_R32_FLOAT) return 32;
    else if (dxgiFormat == DXGI_FORMAT_R16_FLOAT) return 16;
    else if (dxgiFormat == DXGI_FORMAT_R16_UNORM) return 16;
    else if (dxgiFormat == DXGI_FORMAT_R8_UNORM) return 8;
    else if (dxgiFormat == DXGI_FORMAT_A8_UNORM) return 8;
}

int LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC& resourceDescription, LPCWSTR filename, int& bytesPerRow)
{
    HRESULT hr;
    
    // we only need one instance of the imaging factory to create decoders and frames
    static IWICImagingFactory* wicFactory;

    // reset decoder, frame and converter since these will be different for each image we load
    IWICBitmapDecoder* wicDecoder = NULL;
    IWICBitmapFrameDecode* wicFrame = NULL;
    IWICFormatConverter* wicConverter = NULL;

    bool imageConverted = false;

    if (wicFactory == NULL)
    {
        // Initialize the COM library
        CoInitialize(NULL);

        // create the WIC factory
        hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&wicFactory)
        );
        if (FAILED(hr)) return 0;
    }

    // load a decoder for the image
    hr = wicFactory->CreateDecoderFromFilename(
        filename,                        // Image we want to load in
        NULL,                            // This is a vendor ID, we do not prefer a specific one so set to null
        GENERIC_READ,                    // We want to read from this file
        WICDecodeMetadataCacheOnLoad,    // We will cache the metadata right away, rather than when needed, which might be unknown
        &wicDecoder                      // the wic decoder to be created
    );
    if (FAILED(hr)) return 0;

    // get image from decoder (this will decode the "frame")
    hr = wicDecoder->GetFrame(0, &wicFrame);
    if (FAILED(hr)) return 0;

    // get wic pixel format of image
    WICPixelFormatGUID pixelFormat;
    hr = wicFrame->GetPixelFormat(&pixelFormat);
    if (FAILED(hr)) return 0;

    // get size of image
    UINT textureWidth, textureHeight;
    hr = wicFrame->GetSize(&textureWidth, &textureHeight);
    if (FAILED(hr)) return 0;

    // we are not handling sRGB types in this tutorial, so if you need that support, you'll have to figure
    // out how to implement the support yourself

    // convert wic pixel format to dxgi pixel format
    DXGI_FORMAT dxgiFormat = GetDXGIFormatFromWICFormat(pixelFormat);

    // if the format of the image is not a supported dxgi format, try to convert it
    if (dxgiFormat == DXGI_FORMAT_UNKNOWN)
    {
        // get a dxgi compatible wic format from the current image format
        WICPixelFormatGUID convertToPixelFormat = GetConvertToWICFormat(pixelFormat);

        // return if no dxgi compatible format was found
        if (convertToPixelFormat == GUID_WICPixelFormatDontCare) return 0;

        // set the dxgi format
        dxgiFormat = GetDXGIFormatFromWICFormat(convertToPixelFormat);

        // create the format converter
        hr = wicFactory->CreateFormatConverter(&wicConverter);
        if (FAILED(hr)) return 0;

        // make sure we can convert to the dxgi compatible format
        BOOL canConvert = FALSE;
        hr = wicConverter->CanConvert(pixelFormat, convertToPixelFormat, &canConvert);
        if (FAILED(hr) || !canConvert) return 0;

        // do the conversion (wicConverter will contain the converted image)
        hr = wicConverter->Initialize(wicFrame, convertToPixelFormat, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
        if (FAILED(hr)) return 0;

        // this is so we know to get the image data from the wicConverter (otherwise we will get from wicFrame)
        imageConverted = true;
    }

    int bitsPerPixel = GetDXGIFormatBitsPerPixel(dxgiFormat); // number of bits per pixel
    bytesPerRow = (textureWidth * bitsPerPixel) / 8; // number of bytes in each row of the image data
    int imageSize = bytesPerRow * textureHeight; // total image size in bytes

    // allocate enough memory for the raw image data, and set imageData to point to that memory
    *imageData = (BYTE*)malloc(imageSize);

    // copy (decoded) raw image data into the newly allocated memory (imageData)
    if (imageConverted)
    {
        // if image format needed to be converted, the wic converter will contain the converted image
        hr = wicConverter->CopyPixels(0, bytesPerRow, imageSize, *imageData);
        if (FAILED(hr)) return 0;
    }
    else
    {
        // no need to convert, just copy data from the wic frame
        hr = wicFrame->CopyPixels(0, bytesPerRow, imageSize, *imageData);
        if (FAILED(hr)) return 0;
    }

    // now describe the texture with the information we have obtained from the image
    resourceDescription = {};
    resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDescription.Alignment = 0; // may be 0, 4KB, 64KB, or 4MB. 0 will let runtime decide between 64KB and 4MB (4MB for multi-sampled textures)
    resourceDescription.Width = textureWidth; // width of the texture
    resourceDescription.Height = textureHeight; // height of the texture
    resourceDescription.DepthOrArraySize = 1; // if 3d image, depth of 3d image. Otherwise an array of 1D or 2D textures (we only have one image, so we set 1)
    resourceDescription.MipLevels = 1; // Number of mipmaps. We are not generating mipmaps for this texture, so we have only one level
    resourceDescription.Format = dxgiFormat; // This is the dxgi format of the image (format of the pixels)
    resourceDescription.SampleDesc.Count = 1; // This is the number of samples per pixel, we just want 1 sample
    resourceDescription.SampleDesc.Quality = 0; // The quality level of the samples. Higher is better quality, but worse performance
    resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // The arrangement of the pixels. Setting to unknown lets the driver choose the most efficient one
    resourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE; // no flags

    // return the size of the image. remember to delete the image once your done with it (in this tutorial once its uploaded to the gpu)
    return imageSize;
}

void TextHandler::Load(const string& fileName)
{
    m_Font = LoadFont(fileName, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
}

void TextHandler::Render(ID3D12GraphicsCommandList* pd3dCommandList, wstring text, XMFLOAT2 pos,
    XMFLOAT2 scale, XMFLOAT2 padding, XMFLOAT4 color)
{
    // clear the depth buffer so we can draw over everything
    // pd3dCommandList->ClearDepthStencilView(dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    CShaderHandler::GetInstance().GetData("Text")->Render(pd3dCommandList, nullptr);

    // set the text pipeline state object
    //d3dCommandList->SetPipelineState(textPSO);

    // this way we only need 4 vertices per quad rather than 6 if we were to use a triangle list topology
    pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // set the text vertex buffer
    pd3dCommandList->IASetVertexBuffers(0, 1, &textVertexBufferView);

    // bind the text srv. We will assume the correct descriptor heap and table are currently bound and set
    //pd3dCommandList->SetGraphicsRootDescriptorTable(6, m_Font.srvHandle);

    int numCharacters = 0;

    float topLeftScreenX = (pos.x * 2.0f) - 1.0f;
    float topLeftScreenY = ((1.0f - pos.y) * 2.0f) - 1.0f;

    float x = topLeftScreenX;
    float y = topLeftScreenY;

    float horrizontalPadding = (m_Font.leftpadding + m_Font.rightpadding) * padding.x;
    float verticalPadding = (m_Font.toppadding + m_Font.bottompadding) * padding.y;

    // cast the gpu virtual address to a textvertex, so we can directly store our vertices there
    CTextVertex* vert = (CTextVertex*)textVBGPUAddress;

    wchar_t lastChar = -1; // no last character to start with

    for (int i = 0; i < text.size(); ++i)
    {
        wchar_t c = text[i];

        FontChar* fc = m_Font.GetChar(c);

        // character not in font char set
        if (fc == nullptr)
            continue;

        // end of string
        if (c == L'\0')
            break;

        // new line
        if (c == L'\n')
        {
            x = topLeftScreenX;
            y -= (m_Font.lineHeight + verticalPadding) * scale.y;
            continue;
        }

        // don't overflow the buffer. In your app if this is true, you can implement a resize of your text vertex buffer
        if (numCharacters >= maxNumTextCharacters)
            break;

        float kerning = 0.0f;
        if (i > 0)
            kerning = m_Font.GetKerning(lastChar, c);

        vert[numCharacters] = CTextVertex(color.x,
        //vert = CTextVertex(color.x,
            color.y,
            color.z,
            color.w,
            fc->u,
            fc->v,
            fc->twidth,
            fc->theight,
            x + ((fc->xoffset + kerning) * scale.x),
            y - (fc->yoffset * scale.y),
            fc->width * scale.x,
            fc->height * scale.y);

        numCharacters++;

        // remove horrizontal padding and advance to next char position
        x += (fc->xadvance - horrizontalPadding) * scale.x;

        lastChar = c;
    }

    // we are going to have 4 vertices per character (trianglestrip to make quad), and each instance is one character
    pd3dCommandList->DrawInstanced(4, numCharacters, 0, 0);
}

// create text vertex buffer committed resources
bool TextHandler::InitVertexBuffer(ID3D12Device* pd3dDevice,
    ID3D12GraphicsCommandList* pd3dCommandList, ID3D12DescriptorHeap* d3dDescripotrHeap, int handlePosition)
{
    TextHandler::Load("resources/Font/dotum.fnt");

    // Load the image from file
    D3D12_RESOURCE_DESC fontTextureDesc;
    int fontImageBytesPerRow;
    BYTE* fontImageData;
    auto fontImageName = L"Resources/Font/" + m_Font.fontImage;
    int fontImageSize = LoadImageDataFromFile(&fontImageData, fontTextureDesc, fontImageName.c_str(), fontImageBytesPerRow);
    
    // make sure we have data
    if (fontImageData <= 0)
    { 
        return false;
    }

    // create the font texture resource
    HRESULT hr = pd3dDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &fontTextureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&m_Font.textureBuffer));
    if (FAILED(hr))
    { 
        return false;
    }
    m_Font.textureBuffer->SetName(L"Font Texture Buffer Resource Heap");

    ID3D12Resource* fontTextureBufferUploadHeap;
    UINT64 fontTextureUploadBufferSize;
    pd3dDevice->GetCopyableFootprints(&fontTextureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &fontTextureUploadBufferSize);

    // create an upload heap to copy the texture to the gpu
    hr = pd3dDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE, // no flags
        &CD3DX12_RESOURCE_DESC::Buffer(fontTextureUploadBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&fontTextureBufferUploadHeap));
    if (FAILED(hr))
    { 
        return false;
    }
    fontTextureBufferUploadHeap->SetName(L"Font Texture Buffer Upload Resource Heap");
     
    // store font image in upload heap
    D3D12_SUBRESOURCE_DATA fontTextureData = {};
    fontTextureData.pData = &fontImageData[0]; // pointer to our image data
    fontTextureData.RowPitch = fontImageBytesPerRow; // size of all our triangle vertex data
    fontTextureData.SlicePitch = fontImageBytesPerRow * fontTextureDesc.Height; // also the size of our triangle vertex data

    // Now we copy the upload buffer contents to the default heap
    UpdateSubresources(pd3dCommandList, m_Font.textureBuffer, fontTextureBufferUploadHeap, 0, 0, 1, &fontTextureData);

    // transition the texture default heap to a pixel shader resource (we will be sampling from this heap in the pixel shader to get the color of pixels)
    pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Font.textureBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

    // create an srv for the font
    D3D12_SHADER_RESOURCE_VIEW_DESC fontsrvDesc = {};
    fontsrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    fontsrvDesc.Format = fontTextureDesc.Format;
    fontsrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    fontsrvDesc.Texture2D.MipLevels = 1;

    //pd3dDevice->CreateShaderResourceView(m_pd3dMirrorTex, &srvDesc, hDescriptor);
     
    // we need to get the next descriptor location in the descriptor heap to store this srv
    auto srvHandleSize = pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    m_Font.srvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(d3dDescripotrHeap->GetGPUDescriptorHandleForHeapStart(), handlePosition, srvHandleSize);
    //m_Font.srvHandle.ptr += gnCbvSrvDescriptorIncrementSize * handlePosition;

    CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(d3dDescripotrHeap->GetCPUDescriptorHandleForHeapStart(), handlePosition, srvHandleSize);
   // srvHandle.ptr += gnCbvSrvDescriptorIncrementSize * handlePosition;

    pd3dDevice->CreateShaderResourceView(m_Font.textureBuffer, &fontsrvDesc, srvHandle);

    // create text vertex buffer committed resources 
    {
        // create upload heap. We will fill this with data for our text
        ID3D12Resource* vBufferUploadHeap;
        hr = pd3dDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
            D3D12_HEAP_FLAG_NONE, // no flags
            &CD3DX12_RESOURCE_DESC::Buffer(maxNumTextCharacters * sizeof(CTextVertex)), // resource description for a buffer
            D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
            nullptr,
            IID_PPV_ARGS(&textVertexBuffer));
        if (FAILED(hr))
        { 
            return false;
        }
        textVertexBuffer->SetName(L"Text Vertex Buffer Upload Resource Heap");

        CD3DX12_RANGE readRange(0, 0);	// We do not intend to read from this resource on the CPU. (so end is less than or equal to begin)

        // map the resource heap to get a gpu virtual address to the beginning of the heap
        hr = textVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&textVBGPUAddress));
    }

     
    // set the text vertex buffer view for each frame
    {
        textVertexBufferView.BufferLocation = textVertexBuffer->GetGPUVirtualAddress();
        textVertexBufferView.StrideInBytes = sizeof(CTextVertex);
        textVertexBufferView.SizeInBytes = maxNumTextCharacters * sizeof(CTextVertex);
    }
     
}
