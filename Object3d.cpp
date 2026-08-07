#include "Object3d.h"
#include "Object3dCommon.h" 
#include <fstream>
#include <sstream>
#include <cassert>
#include "DirectXCommon.h"
#include "TextureManager.h"
#include "Model.h"
#include "ModelManager.h"
#include "Camera.h"

void Object3d::Initialize(Object3dCommon* object3dCommon)
{
    // ==============================
    // Object3dCommonを保存する
    // ==============================
    this->object3dCommon = object3dCommon;

    // ==============================
    // Object3dCommonに登録されている
    // デフォルトカメラを使う
    // ==============================
    this->camera_ = object3dCommon->GetDefaultCamera();

    // ==============================
    // Object3dのTransform初期化
    // ==============================
    transform = {
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f }
    };

    CreateTransformationMatrixData();

    CreateDirectionalLightData();
    CreateCameraData();
}



void Object3d::CreateTransformationMatrixData() {
    // 座標変換行列用のリソースを作る
    transformationMatrixResource = object3dCommon->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));

    // 書き込むためのアドレスを取得
    transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));

    // 単位行列で初期化しておく
    transformationMatrixData->WVP = Math::MakeIdentity4x4();
    transformationMatrixData->World = Math::MakeIdentity4x4();
    transformationMatrixData->WorldInverseTranspose = Math::MakeIdentity4x4();
}

void Object3d::CreateDirectionalLightData() {
    // 平行光源用のリソースを作る
    directionalLightResource = object3dCommon->GetDxCommon()->CreateBufferResource(sizeof(DirectionalLight));

    // 書き込むためのアドレスを取得
    directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

    // デフォルト値の設定（白、下向き、輝度1.0など）
    directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    directionalLightData->direction = { 0.0f, -1.0f, 0.0f };
    directionalLightData->intensity = 1.0f;
}

void Object3d::CreateCameraData() {
    cameraResource = object3dCommon->GetDxCommon()->CreateBufferResource(sizeof(CameraForGPU));
    cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));
    cameraData->worldPosition = { 0.0f, 0.0f, 0.0f };
}


// ... (既存のインクルード) ...

void Object3d::Update() {
    // ==============================
    // Transform から WorldMatrix を作る
    // ==============================
    Math::Matrix4x4 worldMatrix =
        Math::MakeAffineMatrix(
            transform.scale,
            transform.rotate,
            transform.translate
        );

    // ==============================
    // localMatrix を掛けた WorldMatrix を作る
    // ==============================
    Math::Matrix4x4 worldMatrixWithLocal =
        Math::Multiply(localMatrix_, worldMatrix);

    // ==============================
    // WVP行列を入れる変数
    // ==============================
    Math::Matrix4x4 worldViewProjectionMatrix;

    // ==============================
    // カメラがセットされている場合
    // CameraのViewProjectionを使う
    // ==============================
    if (camera_) {
        worldViewProjectionMatrix =
            Math::Multiply(
                worldMatrixWithLocal,
                camera_->GetViewProjectionMatrix()
            );
    } else {
        // ==============================
        // カメラがない場合はWorldだけ使う
        // 一応描画できるようにしておく
        // ==============================
        worldViewProjectionMatrix = worldMatrixWithLocal;
    }

    // ==============================
    // GPUに送る行列を書き込む
    // ==============================
    transformationMatrixData->WVP = worldViewProjectionMatrix;
    transformationMatrixData->World = worldMatrixWithLocal;

    transformationMatrixData->WorldInverseTranspose =
        Math::Transpose(Math::Inverse(worldMatrixWithLocal));

    if (camera_) {
        const Math::Matrix4x4& cameraWorld = camera_->GetWorldMatrix();
        cameraData->worldPosition = { cameraWorld.m[3][0], cameraWorld.m[3][1], cameraWorld.m[3][2] };
    }
}

void Object3d::Draw() {
    ID3D12GraphicsCommandList* commandList = object3dCommon->GetDxCommon()->GetCommandList();

    //// 1. VertexBufferViewを設定
    //commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

    //// 2. マテリアルCBufferの場所を設定
    //commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

    // 3. 座標変換行列CBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());

    //// 4. SRVのDescriptorTableの先頭を設定 (テクスチャ)
    //commandList->SetGraphicsRootDescriptorTable(2, object3dCommon->GetDxCommon()->GetSRVGPUDescriptorHandle(modelData.material.textureIndex));

    // 5. 平行光源CBufferの場所を設定
    // (※ rootParameters の 3番目として追加されている想定です)
    commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(4, cameraResource->GetGPUVirtualAddress());

    if (model_) {
        model_->Draw();
    }

    //// 6. 描画！ (DrawCall)
    //commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

}

void Object3d::SetModel(const std::string& filePath) {
    model_ = ModelManager::GetInstance()->FindModel(filePath);
}
