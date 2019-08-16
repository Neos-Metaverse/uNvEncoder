#include <d3d11.h>
#include <IUnityInterface.h>
#include <IUnityGraphicsD3D11.h>
#include "Common.h"


namespace uNvEncoder
{


extern IUnityInterfaces *g_unity;


IUnityInterfaces * GetUnity()
{
    return g_unity;
}


ID3D11Device * GetUnityDevice()
{
    return GetUnity()->Get<IUnityGraphicsD3D11>()->GetDevice();
}


}
