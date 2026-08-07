#include "Object3dCommon.h"

Object3dCommon* Object3dCommon::GetInstance() {
    static Object3dCommon instance;
    return &instance;
}
#include "DirectXCommon.h" // ← 追加: DirectXCommonの機能を使うために必要

// 引数で dxCommon を受け取るように変更
void Object3dCommon::Initialize(DirectXCommon* dxCommon)
{
    // 引数で受け取ってメンバ変数に記録する
    dxCommon_ = dxCommon;

    // グラフィックスパイプラインの生成()を呼び出す
    CreateGraphicsPipeline();
}

void Object3dCommon::CreateRootSignature()
{
    // --- main.cpp からルートシグネチャの作成処理をコピー ---
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
    descriptorRange[0].BaseShaderRegister = 0;
    descriptorRange[0].NumDescriptors = 1;
    descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_DESCRIPTOR_RANGE descriptorRangeForMatrixPalette[1] = {};
    descriptorRangeForMatrixPalette[0].BaseShaderRegister = 0; // t0
    descriptorRangeForMatrixPalette[0].NumDescriptors = 1;
    descriptorRangeForMatrixPalette[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRangeForMatrixPalette[0].OffsetInDescriptorsFromTableStart =
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // RootParmater作成
    // 平行光源用に rootParameters[3] まで確保
    D3D12_ROOT_PARAMETER rootParameters[6] = {};
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[0].Descriptor.ShaderRegister = 0;

    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[1].Descriptor.ShaderRegister = 0;

    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

    // 平行光源用
    rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[3].Descriptor.ShaderRegister = 1;

    rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[4].Descriptor.ShaderRegister = 2;

    rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[5].DescriptorTable.pDescriptorRanges = descriptorRangeForMatrixPalette;
    rootParameters[5].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForMatrixPalette);

    descriptionRootSignature.pParameters = rootParameters;
    descriptionRootSignature.NumParameters = _countof(rootParameters);

    // Samplerの設定
    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
    staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
    staticSamplers[0].ShaderRegister = 0;
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    descriptionRootSignature.pStaticSamplers = staticSamplers;
    descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
    HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, signatureBlob.GetAddressOf(), errorBlob.GetAddressOf());
    if (FAILED(hr)) {
        assert(false);
    }

    hr = dxCommon_->GetDevice()->CreateRootSignature(
        0,
        signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(),
        IID_PPV_ARGS(rootSignature_.GetAddressOf())
    );
    assert(SUCCEEDED(hr));

}

// ... (これまでの関数の実装) ...

void Object3dCommon::CreateGraphicsPipeline()
{
    // 最初にルートシグネチャの作成を呼び出す
    CreateRootSignature();

    // -------------------------------------------------------------
    // ① 通常の Object3D 用パイプライン生成（元のコードを維持）
    // -------------------------------------------------------------

    // 【Object3D用】インプットレイアウト
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[5] = {};

    // POSITION
    inputElementDesc[0].SemanticName = "POSITION";
    inputElementDesc[0].SemanticIndex = 0;
    inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDesc[0].InputSlot = 0;
    inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    // TEXCOORD
    inputElementDesc[1].SemanticName = "TEXCOORD";
    inputElementDesc[1].SemanticIndex = 0;
    inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDesc[1].InputSlot = 0;
    inputElementDesc[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    // NORMAL
    inputElementDesc[2].SemanticName = "NORMAL";
    inputElementDesc[2].SemanticIndex = 0;
    inputElementDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDesc[2].InputSlot = 0;
    inputElementDesc[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    // WEIGHT
    inputElementDesc[3].SemanticName = "WEIGHT";
    inputElementDesc[3].SemanticIndex = 0;
    inputElementDesc[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDesc[3].InputSlot = 1;
    inputElementDesc[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    // INDEX
    inputElementDesc[4].SemanticName = "INDEX";
    inputElementDesc[4].SemanticIndex = 0;
    inputElementDesc[4].Format = DXGI_FORMAT_R32G32B32A32_SINT;
    inputElementDesc[4].InputSlot = 1;
    inputElementDesc[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    D3D12_INPUT_LAYOUT_DESC inputLayOutDesc{};
    inputLayOutDesc.pInputElementDescs = inputElementDesc;
    inputLayOutDesc.NumElements = _countof(inputElementDesc);

    D3D12_BLEND_DESC blendDesc{};
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    D3D12_RASTERIZER_DESC rastrizeDesc{};
    rastrizeDesc.CullMode = D3D12_CULL_MODE_BACK;
    rastrizeDesc.FillMode = D3D12_FILL_MODE_SOLID;

    // 通常の Object3D 用シェーダーコンパイル
    auto vertexShaderBlob = dxCommon_->CompileShader(L"resources/shaders/Object3d.VS.hlsl", L"vs_6_0");
    auto pixelShaderBlob = dxCommon_->CompileShader(L"resources/shaders/Object3d.PS.hlsl", L"ps_6_0");

    // アニメーション用のシェーダーコンパイル
    //auto vertexShaderBlob = dxCommon_->CompileShader(L"resources/shaders/SkinningObject3d.VS.hlsl", L"vs_6_0");

    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
    graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();
    graphicsPipelineStateDesc.InputLayout = inputLayOutDesc;
    graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };
    graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };
    graphicsPipelineStateDesc.BlendState = blendDesc;
    graphicsPipelineStateDesc.RasterizerState = rastrizeDesc;

    graphicsPipelineStateDesc.NumRenderTargets = 1;
    graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    graphicsPipelineStateDesc.SampleDesc.Count = 1;
    graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

    D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
    graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // 通常のパイプラインステートを作成
    HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(
        &graphicsPipelineStateDesc,
        IID_PPV_ARGS(graphicsPipelineState_.GetAddressOf())
    );
    assert(SUCCEEDED(hr));


    // -------------------------------------------------------------
    // ② Skybox専用パイプライン生成（専用レイアウトを設定）
    // -------------------------------------------------------------

    // Skybox用の新しく作ったシェーダーをコンパイル
    auto skyboxVSBlob = dxCommon_->CompileShader(L"resources/shaders/Skybox.VS.hlsl", L"vs_6_0");
    auto skyboxPSBlob = dxCommon_->CompileShader(L"resources/shaders/Skybox.PS.hlsl", L"ps_6_0");

    // 【修正点】Skybox専用のInputLayout（位置座標 POSITION のみ、要素数は1つだけ）
    D3D12_INPUT_ELEMENT_DESC skyboxInputElementDesc[1] = {};
    skyboxInputElementDesc[0].SemanticName = "POSITION";
    skyboxInputElementDesc[0].SemanticIndex = 0;
    skyboxInputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    skyboxInputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    D3D12_INPUT_LAYOUT_DESC skyboxInputLayoutDesc{};
    skyboxInputLayoutDesc.pInputElementDescs = skyboxInputElementDesc;
    skyboxInputLayoutDesc.NumElements = _countof(skyboxInputElementDesc); // 要素数 1

    // 記述子を一旦通常のベースからコピー
    D3D12_GRAPHICS_PIPELINE_STATE_DESC skyboxPsoDesc = graphicsPipelineStateDesc;

    // Skybox専用のInputLayoutとシェーダーを設定
    skyboxPsoDesc.InputLayout = skyboxInputLayoutDesc; // 👈 流用せず専用のものをセット！
    skyboxPsoDesc.VS = { skyboxVSBlob->GetBufferPointer(), skyboxVSBlob->GetBufferSize() };
    skyboxPsoDesc.PS = { skyboxPSBlob->GetBufferPointer(), skyboxPSBlob->GetBufferSize() };

    // 深度設定（スライドの指示通り、LESS_EQUALかつZERO）
    D3D12_DEPTH_STENCIL_DESC skyboxDepthStencilDesc{};
    skyboxDepthStencilDesc.DepthEnable = true;
    skyboxDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; // 書き込みをOFFにして無駄を省く
    skyboxDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // 1.0の描画を許容する

    skyboxPsoDesc.DepthStencilState = skyboxDepthStencilDesc;

    // Skybox用のパイプラインステートを作成
    hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(
        &skyboxPsoDesc,
        IID_PPV_ARGS(&graphicsPipelineStateSkybox_)
    );
    assert(SUCCEEDED(hr));
}

// ↓ここから追加
void Object3dCommon::SetCommonDrawSetting()
{
    // コマンドリストの取得
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    // ルートシグネチャをセットするコマンド
    commandList->SetGraphicsRootSignature(rootSignature_.Get());
    // グラフィックスパイプラインステートをセットするコマンド
    commandList->SetPipelineState(graphicsPipelineState_.Get());
    // プリミティブトポロジーをセットするコマンド
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
// ↑ここまで追加

