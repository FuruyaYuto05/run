#pragma once
#include <string>
#include <vector>
#include "Math.h"
#include <d3d12.h>
#include <wrl.h>



// ↓ ここにあった VertexData は削除します（クラスの中に引っ越すため）

// 前方宣言
class Object3dCommon;
class Model;
class Camera;

// 3Dオブジェクト
class Object3d
{
public: // 構造体

    // ↓ 追加：main.cppやクラスの外にあったものをインナークラスとして実装
    struct VertexData {
        Math::Vector4 position;
        Math::Vector2 texcoord;
        Math::Vector3 normal; // スライド通り normal も有効化
    };

    struct Material {
        Math::Vector4 color;
        int32_t enableLighting;
        float padding[3];
        Math::Matrix4x4 uvTransform;
    };

    struct MaterialData
    {
        std::string textureFilePath;
        uint32_t textureIndex = 0;
    };

    struct ModelData {
        std::vector<VertexData> vertices; // ↑で定義した VertexData をここで使う
        MaterialData material;
    };

    struct TransformationMatrix {
        Math::Matrix4x4 WVP;
        Math::Matrix4x4 World;
        Math::Matrix4x4 WorldInverseTranspose;
    };

    struct CameraForGPU {
        Math::Vector3 worldPosition;
    };

    struct DirectionalLight {
        Math::Vector4 color;     // ライトの色
        Math::Vector3 direction; // ライトの向き
        float intensity;         // 輝度
    };

public: // メンバ関数
    // 初期化
    void Initialize(Object3dCommon* object3dCommon);

    void Update();
    void Draw();

    void SetModel(Model* model) { model_ = model; }

    void SetModel(const std::string& filePath);

    // .mtlファイルの読み取り
    //static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

    // .objファイルの読み取り
    //void LoadObjFile(const std::string& directoryPath, const std::string& filename);



private: // メンバ変数
    Object3dCommon* object3dCommon = nullptr;

    Model* model_ = nullptr;

    Math::Transform transform;

    // ==============================
    // このObject3dが参照するカメラ
    // ==============================
    Camera* camera_ = nullptr;

    Math::Matrix4x4 localMatrix_ = Math::MakeIdentity4x4();

    // 読み込んだモデルデータを保持する変数
    //ModelData modelData;

    //Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

    //// バッファリソース内のデータを指すポインタ
    //VertexData* vertexData = nullptr;

    //// バッファリソースの使い道を補足するバッファビュー
    //D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

    //Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
    //Material* materialData = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;
    TransformationMatrix* transformationMatrixData = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
    DirectionalLight* directionalLightData = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource;
    CameraForGPU* cameraData = nullptr;


    //// 頂点データ作成
    //void CreateVertexData();
    //// マテリアルデータの作成
    //void CreateMaterialData();

    void CreateTransformationMatrixData();

    void CreateDirectionalLightData();
    void CreateCameraData();

public:
    // --- 追加：setter ---
    void SetScale(const Math::Vector3& scale) { transform.scale = scale; }
    void SetRotate(const Math::Vector3& rotate) { transform.rotate = rotate; }
    void SetTranslate(const Math::Vector3& translate) { transform.translate = translate; }
    void SetLocalMatrix(const Math::Matrix4x4& localMatrix) { localMatrix_ = localMatrix; }

    // --- 追加：getter ---
    const Math::Vector3& GetScale() const { return transform.scale; }
    const Math::Vector3& GetRotate() const { return transform.rotate; }
    const Math::Vector3& GetTranslate() const { return transform.translate; }
    DirectionalLight& GetDirectionalLight() { return *directionalLightData; }
    Model* GetModel() const { return model_; }

    // ==============================
    // このObject3dが使うカメラを設定する
    // ==============================
    void SetCamera(Camera* camera) { camera_ = camera; }
};
