#pragma once

#include <vector>
#include <atomic>
#include <memory>
#include <d3d11.h>
#include <wrl/client.h>
#include "nvEncodeAPI.h"
#include "Common.h"


namespace uNvEncoder
{


struct NvencDesc
{
    ComPtr<ID3D11Device> d3d11Device; 
    uint32_t width = 1920; 
    uint32_t height = 1080;
    uint32_t frameRate = 60;
};


struct NvencEncodedData
{
    uint64_t index = 0;
    std::unique_ptr<uint8_t[]> buffer;
    uint32_t size = 0;
};


class Nvenc final
{
public:
    explicit Nvenc(const NvencDesc &desc);
    ~Nvenc();
    bool IsValid() const { return encoder_ != nullptr; }
    bool Encode(const ComPtr<ID3D11Texture2D> &source, bool forceIdrFrame);
    bool GetEncodedData(std::vector<NvencEncodedData> &data);
    const uint32_t GetWidth() const { return desc_.width; }
    const uint32_t GetHeight() const { return desc_.height; }
    const uint32_t GetFrameRate() const { return desc_.frameRate; }

private:
    bool LoadModule();
    void UnloadModule();

    void OpenEncodeSession();
    void InitializeEncoder();
    void DestroyEncoder();
    void CreateCompletionEvents();
    void DestroyCompletionEvents();
    void CreateBitstreamBuffers();
    void DestroyBitstreamBuffers();
    void CreateInputTextures();
    void RegisterResources();
    void UnregisterResources();

    void CopyToInputTexture(int index, const ComPtr<ID3D11Texture2D> &texture);
    bool EncodeInputTexture(int index, bool forceIdrFrame);
    void MapInputResource(int index);
    void UnmapInputResource(int index);
    bool WaitForCompletion(int index, DWORD duration);
    void EndEncode();
    void SendEOS();

    unsigned long GetResourceCount() const { return static_cast<unsigned long>(resources_.size()); }
    unsigned long GetInputIndex() const { return inputIndex_ % GetResourceCount(); }
    unsigned long GetOutputIndex() const { return outputIndex_ % GetResourceCount(); }

    const NvencDesc desc_;
    HMODULE module_ = nullptr;
    NV_ENCODE_API_FUNCTION_LIST nvenc_ = { 0 };
    void *encoder_ = nullptr;
    uint64_t inputIndex_ = 0U;
    uint64_t outputIndex_ = 0U;

    struct Resource
    {
        ComPtr<ID3D11Texture2D> inputTexture_ = nullptr;
        HANDLE inputTextureSharedHandle_ = nullptr;
        NV_ENC_REGISTERED_PTR registeredResource_ = nullptr;
        NV_ENC_INPUT_PTR inputResource_ = nullptr;
        NV_ENC_OUTPUT_PTR bitstreamBuffer_ = nullptr;
        void *completionEvent_ = nullptr;
        std::atomic<bool> isEncoding_ = false;
    };
    std::vector<Resource> resources_;
};


}