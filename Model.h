#pragma once
#include <string>
#include <vector>
#include <d3d12.h>
#include <wrl.h>
#include "Math.h"
#include <optional>
#include <map>
#include <cstdint>
#include <array>
#include <span>
#include <utility>



// 前方宣言
class ModelCommon;
struct aiNode;
struct Animation;

// 3Dモデル
class Model
{
public: // 構造体 (Object3dから引っ越し)
    struct VertexData {
        Math::Vector4 position;
        Math::Vector2 texcoord;
        Math::Vector3 normal;
    };

    struct Material {
        Math::Vector4 color;
        int32_t enableLighting;
        float padding[3];
        Math::Matrix4x4 uvTransform;
        float shininess;
        float paddingShininess[3];
    };

    struct MaterialData {
        std::string textureFilePath;
        uint32_t textureIndex = 0;
    };

    struct Node {
        Math::QuaternionTransform transform;
        Math::Matrix4x4 localMatrix;
        std::string name;
        std::vector<Node> children;
    };

    struct Joint {
        Math::QuaternionTransform transform; // Transform情報
        Math::Matrix4x4 localMatrix;         // localMatrix
        Math::Matrix4x4 skeletonSpaceMatrix; // skeletonSpaceでの変換行列
        std::string name;                     // 名前
        std::vector<int32_t> children;        // 子JointのIndexのリスト。いなければ空
        int32_t index;                        // 自身のIndex
        std::optional<int32_t> parent;        // 親JointのIndex。いなければnull
    };

    struct Skeleton {
        int32_t root;                         // RootJointのIndex
        std::map<std::string, int32_t> jointMap; // Joint名とIndexとの辞書
        std::vector<Joint> joints;            // 所属しているJoint
    };

    static const uint32_t kNumMaxInfluence = 4;

    struct VertexInfluence {
        std::array<float, kNumMaxInfluence> weights;
        std::array<int32_t, kNumMaxInfluence> jointIndices;
    };


    struct WellForGPU {
        Math::Matrix4x4 skeletonSpaceMatrix;
        Math::Matrix4x4 skeletonSpaceInverseTransposeMatrix;
    };

    struct SkinCluster {
        // JointごとのInverseBindPoseMatrix
        std::vector<Math::Matrix4x4> inverseBindPoseMatrices;

        // Influence用
        Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
        D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
        std::span<VertexInfluence> mappedInfluence;

        // MatrixPalette用
        Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
        std::span<WellForGPU> mappedPalette;
        std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;
    };

    struct VertexWeightData {
        float weight;
        uint32_t vertexIndex;
    };

    struct JointWeightData {
        Math::Matrix4x4 inverseBindPoseMatrix;
        std::vector<VertexWeightData> vertexWeights;
    };

    struct ModelData {
        std::vector<VertexData> vertices;
        std::vector<uint32_t> indices;
        MaterialData material;
        Node rootNode;
        std::map<std::string, JointWeightData> skinClusterData;
    };



private: // メンバ変数
    // モデル共通部のポインタ (新規追加)
    ModelCommon* modelCommon_ = nullptr;

    // Objファイルのデータ (Object3dから引っ越し)
    ModelData modelData_;

    Skeleton skeleton_;
    SkinCluster skinCluster_;

    // 頂点リソース関連 (Object3dから引っ越し)
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    VertexData* vertexData_ = nullptr;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

    // IndexBuffer
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
    uint32_t* mappedIndex_ = nullptr;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

    // マテリアルリソース関連 (Object3dから引っ越し)
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
    Material* materialData_ = nullptr;


    Node ReadNode(aiNode* node);

    Skeleton CreateSkeleton(const Node& rootNode);
    int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

    SkinCluster CreateSkinCluster(const Skeleton& skeleton, const ModelData& modelData);

    void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);

    bool isSkinned_ = false;

public: // メンバ関数
    // 初期化
    // Model.h
    void Initialize(ModelCommon* modelCommon, const std::string& directoryPath, const std::string& filename);

    void Update(Skeleton& skeleton);

    void Update(SkinCluster& skinCluster, const Skeleton& skeleton);

    void Draw();

    void UpdateSkeleton(const Animation& animation, float animationTime);

    float GetShininess() const { return materialData_->shininess; }
    void SetShininess(float shininess) { materialData_->shininess = shininess; }

private: // メンバ関数 (Object3dから引っ越し)
    void CreateVertexData();
    void CreateMaterialData();
    void LoadObjFile(const std::string& directoryPath, const std::string& filename);
    static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
    void LoadGltfFile(const std::string& directoryPath, const std::string& filename);
    void CreateIndexData();
};
