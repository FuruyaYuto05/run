#include "ParticleManager.h"

#include "DirectXCommon.h"
#include "SrvManager.h"
#include "TextureManager.h"

#include <cassert>

ParticleManager* ParticleManager::GetInstance()
{
    static ParticleManager instance;
    return &instance;
}

void ParticleManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager)
{
    // ==============================
    // nullptrチェック
    // ==============================
    assert(dxCommon);
    assert(srvManager);

    // ==============================
    // DirectXCommonを保存
    // ==============================
    dxCommon_ = dxCommon;

    // ==============================
    // SrvManagerを保存
    // ==============================
    srvManager_ = srvManager;

    // ==============================
    // Particle描画に必要なものを作成
    // ==============================
    CreateRootSignature();
    CreateGraphicsPipeline();
    CreateVertexResource();
    CreateMaterialResource();
}

void ParticleManager::CreateParticleGroup(
    const std::string& name,
    const std::string& textureFilePath
)
{
    // ==============================
    // 登録済みチェック
    // 同じ名前のグループは作らない
    // ==============================
    assert(particleGroups_.find(name) == particleGroups_.end());

    // ==============================
    // テクスチャを読み込む
    // ==============================
    TextureManager::GetInstance()->LoadTexture(textureFilePath);

    // ==============================
    // 新しいパーティクルグループを作成
    // ==============================
    ParticleGroup particleGroup{};

    // ==============================
    // テクスチャ情報を記録
    // ==============================
    particleGroup.textureFilePath = textureFilePath;

    particleGroup.textureIndex =
        TextureManager::GetInstance()->GetTextureIndexByFilePath(textureFilePath);

    // ==============================
    // インスタンシング用リソースを作成
    // ==============================
    particleGroup.instancingResource =
        dxCommon_->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);

    // ==============================
    // インスタンシング用データを書き込めるようにする
    // ==============================
    particleGroup.instancingResource->Map(
        0,
        nullptr,
        reinterpret_cast<void**>(&particleGroup.instancingData)
    );

    // ==============================
    // 初期値を書き込む
    // ==============================
    for (uint32_t index = 0; index < kNumMaxInstance; ++index) {
        particleGroup.instancingData[index].WVP = Math::MakeIdentity4x4();
        particleGroup.instancingData[index].World = Math::MakeIdentity4x4();
        particleGroup.instancingData[index].color = { 1.0f, 1.0f, 1.0f, 1.0f };
    }

    // ==============================
    // SrvManagerからSRV番号を確保
    // ==============================
    particleGroup.instancingSrvIndex = srvManager_->Allocate();

    // ==============================
    // StructuredBuffer用SRV設定
    // ==============================
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    srvDesc.Buffer.NumElements = kNumMaxInstance;
    srvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);

    // ==============================
    // SRV生成
    // ==============================
    dxCommon_->GetDevice()->CreateShaderResourceView(
        particleGroup.instancingResource.Get(),
        &srvDesc,
        srvManager_->GetCPUDescriptorHandle(particleGroup.instancingSrvIndex)
    );

    // ==============================
    // コンテナに登録
    // ==============================
    particleGroups_.emplace(name, particleGroup);
}

void ParticleManager::Update(const Math::Matrix4x4& viewProjectionMatrix)
{
    // ==============================
    // 全パーティクルグループを更新
    // ==============================
    for (auto& [name, particleGroup] : particleGroups_) {

        // 描画数をリセット
        particleGroup.numInstance = 0;

        // ==============================
        // パーティクルを1つずつ更新
        // ==============================
        for (std::list<Particle>::iterator particleIterator = particleGroup.particles.begin();
            particleIterator != particleGroup.particles.end();) {

            Particle& particle = *particleIterator;

            // ==============================
            // 寿命更新
            // ==============================
            particle.currentTime += 1.0f / 60.0f;

            // ==============================
            // 寿命が終わったら削除
            // ==============================
            if (particle.currentTime >= particle.lifeTime) {
                particleIterator = particleGroup.particles.erase(particleIterator);
                continue;
            }

            // ==============================
            // 移動
            // ==============================
            particle.transform.translate.x += particle.velocity.x * (1.0f / 60.0f);
            particle.transform.translate.y += particle.velocity.y * (1.0f / 60.0f);
            particle.transform.translate.z += particle.velocity.z * (1.0f / 60.0f);

            // ==============================
            // 最大数を超えたらGPUに書き込まない
            // ==============================
            if (particleGroup.numInstance < kNumMaxInstance) {

                // ==============================
                // World行列作成
                // ==============================
                Math::Matrix4x4 worldMatrix = Math::MakeAffineMatrix(
                    particle.transform.scale,
                    particle.transform.rotate,
                    particle.transform.translate
                );

                // ==============================
                // WVP作成
                // ==============================
                Math::Matrix4x4 worldViewProjectionMatrix =
                    Math::Multiply(worldMatrix, viewProjectionMatrix);

                // ==============================
                // GPU用データに書き込み
                // ==============================
                particleGroup.instancingData[particleGroup.numInstance].WVP =
                    worldViewProjectionMatrix;

                particleGroup.instancingData[particleGroup.numInstance].World =
                    worldMatrix;

                particleGroup.instancingData[particleGroup.numInstance].color =
                    particle.color;

                // ==============================
                // 描画数を増やす
                // ==============================
                particleGroup.numInstance++;
            }

            ++particleIterator;
        }
    }
}

void ParticleManager::Draw()
{
    // ==============================
    // コマンドリスト取得
    // ==============================
    ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

    // ==============================
    // RootSignature / PSO を設定
    // ==============================
    commandList->SetGraphicsRootSignature(rootSignature_.Get());
    commandList->SetPipelineState(graphicsPipelineState_.Get());

    // ==============================
    // 頂点バッファを設定
    // ==============================
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

    // ==============================
    // 三角形リスト
    // ==============================
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // ==============================
    // マテリアル
    // RootParameter[0]
    // ==============================
    commandList->SetGraphicsRootConstantBufferView(
        0,
        materialResource_->GetGPUVirtualAddress()
    );

    // ==============================
    // 全パーティクルグループを描画
    // ==============================
    for (auto& [name, particleGroup] : particleGroups_) {

        // 描画するParticleがないなら飛ばす
        if (particleGroup.numInstance == 0) {
            continue;
        }

        // ==============================
        // Instancing用SRV
        // RootParameter[1]
        // ==============================
        commandList->SetGraphicsRootDescriptorTable(
            1,
            srvManager_->GetGPUDescriptorHandle(particleGroup.instancingSrvIndex)
        );

        // ==============================
        // Texture用SRV
        // RootParameter[2]
        // ==============================
        commandList->SetGraphicsRootDescriptorTable(
            2,
            TextureManager::GetInstance()->GetSrvHandleGPU(particleGroup.textureFilePath)
        );

        // ==============================
        // DrawCall
        // 6頂点の板ポリゴンを numInstance 個描画
        // ==============================
        commandList->DrawInstanced(
            6,
            particleGroup.numInstance,
            0,
            0
        );
    }
}

void ParticleManager::Emit(
    const std::string& name,
    const Math::Vector3& position,
    uint32_t count
)
{
    // ==============================
    // 指定されたグループが存在するか確認
    // ==============================
    auto it = particleGroups_.find(name);
    assert(it != particleGroups_.end());

    ParticleGroup& particleGroup = it->second;

    // ==============================
    // 指定数だけパーティクルを追加
    // ==============================
    for (uint32_t index = 0; index < count; ++index) {

        Particle particle{};

        // ==============================
        // 大きさ
        // ==============================
        particle.transform.scale = {
            0.3f,
            0.3f,
            0.3f
        };

        // ==============================
        // 回転
        // ==============================
        particle.transform.rotate = {
            0.0f,
            0.0f,
            0.0f
        };

        // ==============================
        // 発生座標
        // ==============================
        particle.transform.translate = position;

        // ==============================
        // 速度
        // とりあえず上に少し動く
        // ==============================
        particle.velocity = {
            0.0f,
            0.03f,
            0.0f
        };

        // ==============================
        // 色
        // ==============================
        particle.color = {
            1.0f,
            1.0f,
            1.0f,
            1.0f
        };

        // ==============================
        // 寿命
        // ==============================
        particle.lifeTime = 1.0f;
        particle.currentTime = 0.0f;

        // ==============================
        // グループに追加
        // ==============================
        particleGroup.particles.push_back(particle);
    }
}

void ParticleManager::Finalize()
{
    particleGroups_.clear();
    materialResource_.Reset();
    vertexResource_.Reset();
    graphicsPipelineState_.Reset();
    rootSignature_.Reset();
    dxCommon_ = nullptr;
    srvManager_ = nullptr;
}


void ParticleManager::CreateRootSignature()
{
    // ==============================
    // RootSignature設定
    // ==============================
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
    descriptionRootSignature.Flags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // ==============================
    // Instancing用 StructuredBuffer SRV
    // VS : t0
    // ==============================
    D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
    descriptorRangeForInstancing[0].BaseShaderRegister = 0;
    descriptorRangeForInstancing[0].NumDescriptors = 1;
    descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart =
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // ==============================
    // Texture用 SRV
    // PS : t0
    // ==============================
    D3D12_DESCRIPTOR_RANGE descriptorRangeForTexture[1] = {};
    descriptorRangeForTexture[0].BaseShaderRegister = 0;
    descriptorRangeForTexture[0].NumDescriptors = 1;
    descriptorRangeForTexture[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRangeForTexture[0].OffsetInDescriptorsFromTableStart =
        D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    // ==============================
    // RootParameter
    // [0] Material b0
    // [1] Instancing t0
    // [2] Texture t0
    // ==============================
    D3D12_ROOT_PARAMETER rootParameters[3] = {};

    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[0].Descriptor.ShaderRegister = 0;

    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
    rootParameters[1].DescriptorTable.NumDescriptorRanges =
        _countof(descriptorRangeForInstancing);

    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRangeForTexture;
    rootParameters[2].DescriptorTable.NumDescriptorRanges =
        _countof(descriptorRangeForTexture);

    descriptionRootSignature.pParameters = rootParameters;
    descriptionRootSignature.NumParameters = _countof(rootParameters);

    // ==============================
    // Sampler
    // ==============================
    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
    staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
    staticSamplers[0].ShaderRegister = 0;
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    descriptionRootSignature.pStaticSamplers = staticSamplers;
    descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

    // ==============================
    // RootSignature作成
    // ==============================
    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3D12SerializeRootSignature(
        &descriptionRootSignature,
        D3D_ROOT_SIGNATURE_VERSION_1,
        signatureBlob.GetAddressOf(),
        errorBlob.GetAddressOf()
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        }
        assert(false);
    }

    hr = dxCommon_->GetDevice()->CreateRootSignature(
        0,
        signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature_)
    );
    assert(SUCCEEDED(hr));

}

void ParticleManager::CreateGraphicsPipeline()
{
    // ==============================
    // InputLayout
    // ==============================
    D3D12_INPUT_ELEMENT_DESC inputElementDesc[2] = {};

    inputElementDesc[0].SemanticName = "POSITION";
    inputElementDesc[0].SemanticIndex = 0;
    inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputElementDesc[1].SemanticName = "TEXCOORD";
    inputElementDesc[1].SemanticIndex = 0;
    inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDesc[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
    inputLayoutDesc.pInputElementDescs = inputElementDesc;
    inputLayoutDesc.NumElements = _countof(inputElementDesc);

    // ==============================
    // Blend 加算合成
    // ==============================
    D3D12_BLEND_DESC blendDesc{};
    blendDesc.RenderTarget[0].RenderTargetWriteMask =
        D3D12_COLOR_WRITE_ENABLE_ALL;

    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

    // ==============================
    // Rasterizer
    // ==============================
    D3D12_RASTERIZER_DESC rasterizerDesc{};
    rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

    // ==============================
    // Depth
    // 半透明なのでDepth書き込みはOFF
    // ==============================
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    // ==============================
    // Shader Compile
    // ==============================
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob =
        dxCommon_->CompileShader(L"resources/shaders/Particle.VS.hlsl", L"vs_6_0");

    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob =
        dxCommon_->CompileShader(L"resources/shaders/Particle.PS.hlsl", L"ps_6_0");

    assert(vertexShaderBlob);
    assert(pixelShaderBlob);



    // ==============================
    // PSO作成
    // ==============================
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
    pipelineStateDesc.pRootSignature = rootSignature_.Get();
    pipelineStateDesc.InputLayout = inputLayoutDesc;
    pipelineStateDesc.VS = {
        vertexShaderBlob->GetBufferPointer(),
        vertexShaderBlob->GetBufferSize()
    };
    pipelineStateDesc.PS = {
        pixelShaderBlob->GetBufferPointer(),
        pixelShaderBlob->GetBufferSize()
    };
    pipelineStateDesc.BlendState = blendDesc;
    pipelineStateDesc.RasterizerState = rasterizerDesc;
    pipelineStateDesc.DepthStencilState = depthStencilDesc;
    pipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    pipelineStateDesc.NumRenderTargets = 1;
    pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    pipelineStateDesc.PrimitiveTopologyType =
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipelineStateDesc.SampleDesc.Count = 1;
    pipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

    HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(
        &pipelineStateDesc,
        IID_PPV_ARGS(&graphicsPipelineState_)
    );
    assert(SUCCEEDED(hr));

}

void ParticleManager::CreateVertexResource()
{
    // ==============================
    // 6頂点の板ポリゴン
    // ==============================
    vertexResource_ =
        dxCommon_->CreateBufferResource(sizeof(ParticleVertexData) * 6);

    vertexBufferView_.BufferLocation =
        vertexResource_->GetGPUVirtualAddress();

    vertexBufferView_.SizeInBytes =
        sizeof(ParticleVertexData) * 6;

    vertexBufferView_.StrideInBytes =
        sizeof(ParticleVertexData);

    ParticleVertexData* vertexData = nullptr;

    vertexResource_->Map(
        0,
        nullptr,
        reinterpret_cast<void**>(&vertexData)
    );

    // 左下
    vertexData[0].position = { -0.5f, -0.5f, 0.0f, 1.0f };
    vertexData[0].texcoord = { 0.0f, 1.0f };

    // 左上
    vertexData[1].position = { -0.5f,  0.5f, 0.0f, 1.0f };
    vertexData[1].texcoord = { 0.0f, 0.0f };

    // 右下
    vertexData[2].position = { 0.5f, -0.5f, 0.0f, 1.0f };
    vertexData[2].texcoord = { 1.0f, 1.0f };

    // 左上
    vertexData[3].position = { -0.5f,  0.5f, 0.0f, 1.0f };
    vertexData[3].texcoord = { 0.0f, 0.0f };

    // 右上
    vertexData[4].position = { 0.5f,  0.5f, 0.0f, 1.0f };
    vertexData[4].texcoord = { 1.0f, 0.0f };

    // 右下
    vertexData[5].position = { 0.5f, -0.5f, 0.0f, 1.0f };
    vertexData[5].texcoord = { 1.0f, 1.0f };
}

void ParticleManager::CreateMaterialResource()
{
    // ==============================
    // Material用Resource作成
    // ==============================
    materialResource_ =
        dxCommon_->CreateBufferResource(sizeof(ParticleMaterial));

    // ==============================
    // 書き込み先取得
    // ==============================
    materialResource_->Map(
        0,
        nullptr,
        reinterpret_cast<void**>(&materialData_)
    );

    // ==============================
    // 初期値
    // ==============================
    materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    materialData_->uvTransform = Math::MakeIdentity4x4();
}
