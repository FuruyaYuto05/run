#include "Model.h"
#include "ModelCommon.h"
#include "DirectXCommon.h"
#include "TextureManager.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstring>
#include <algorithm>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Animation.h"
#include "SrvManager.h"

void Model::Initialize(ModelCommon* modelCommon, const std::string& directoryPath, const std::string& filename) {
    modelCommon_ = modelCommon;

    // モデル読み込み
    if (filename.ends_with(".obj")) {
        LoadObjFile(directoryPath, filename);
    } else if (filename.ends_with(".gltf") || filename.ends_with(".glb")) {
        LoadGltfFile(directoryPath, filename);
    } else {
        assert(false);
    }

    // テクスチャ読み込みと番号取得
    TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);
    modelData_.material.textureIndex =
        TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData_.material.textureFilePath);

    // 頂点とマテリアルのデータ作成
    CreateVertexData();
    CreateIndexData();
    if (!modelData_.skinClusterData.empty() && !skeleton_.joints.empty()) {
        skinCluster_ = CreateSkinCluster(skeleton_, modelData_);
        isSkinned_ = true;
    } else {
        isSkinned_ = false;
    }

    CreateMaterialData();

    std::string logPath = "Model Loaded: " + filename + ", TexturePath: [" + modelData_.material.textureFilePath + "]\n";
    OutputDebugStringA(logPath.c_str());

    std::string logIndex = "Texture Index: " + std::to_string(modelData_.material.textureIndex) + "\n";
    OutputDebugStringA(logIndex.c_str());
}

void Model::CreateVertexData() {
    vertexResource_ = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());
    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
    std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
    vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

void Model::CreateIndexData()
{
    // OBJなど、まだIndexがないモデル用の保険
    if (modelData_.indices.empty()) {
        modelData_.indices.resize(modelData_.vertices.size());

        for (uint32_t index = 0; index < modelData_.indices.size(); ++index) {
            modelData_.indices[index] = index;
        }
    }

    indexResource_ = modelCommon_->GetDxCommon()->CreateBufferResource(
        sizeof(uint32_t) * modelData_.indices.size()
    );

    indexResource_->Map(
        0,
        nullptr,
        reinterpret_cast<void**>(&mappedIndex_)
    );

    std::memcpy(
        mappedIndex_,
        modelData_.indices.data(),
        sizeof(uint32_t) * modelData_.indices.size()
    );

    indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * modelData_.indices.size());
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
}

void Model::CreateMaterialData() {
    materialResource_ = modelCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
    materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    materialData_->enableLighting = true;
    materialData_->uvTransform = Math::MakeIdentity4x4();
    materialData_->shininess = 32.0f;
}

void Model::Draw() {
    ID3D12GraphicsCommandList* commandList = modelCommon_->GetDxCommon()->GetCommandList();

    if (isSkinned_) {
        // ==============================
        // スキニングモデル用
        // slot0: 頂点
        // slot1: Influence
        // ==============================
        D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
            vertexBufferView_,
            skinCluster_.influenceBufferView
        };

        commandList->IASetVertexBuffers(0, 2, vbvs);
    } else {
        // 通常モデル用
        // slot0だけ使う
        commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
    }
    // IndexBuffer
    commandList->IASetIndexBuffer(&indexBufferView_);

    // マテリアル
    commandList->SetGraphicsRootConstantBufferView(
        0,
        materialResource_->GetGPUVirtualAddress()
    );

    commandList->SetGraphicsRootDescriptorTable(
        2,
        TextureManager::GetInstance()->GetSrvHandleGPU(modelData_.material.textureFilePath)
    );

    if (isSkinned_) {
        // MatrixPalette
        commandList->SetGraphicsRootDescriptorTable(
            5,
            skinCluster_.paletteSrvHandle.second
        );
    }

    // Index描画
    commandList->DrawIndexedInstanced(
        UINT(modelData_.indices.size()),
        1,
        0,
        0,
        0
    );
}

// ※ LoadObjFile と LoadMaterialTemplateFile の中身は Object3d.cpp からそのままコピーしてください。
// ただし、メンバ変数名は modelData_ に書き換える必要があります。

// Model.cpp の末尾に追記

void Model::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
    std::vector<Math::Vector4> positions;
    std::vector<Math::Vector3> normals;
    std::vector<Math::Vector2> texcoords;
    std::string line;

    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open());

    modelData_.vertices.clear();
    modelData_.indices.clear();
    modelData_.skinClusterData.clear();

    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;

        if (identifier == "v") {
            Math::Vector4 position;
            s >> position.x >> position.y >> position.z;
            position.w = 1.0f;
            positions.push_back(position);
        } else if (identifier == "vt") {
            Math::Vector2 texcoord;
            s >> texcoord.x >> texcoord.y;
            texcoords.push_back(texcoord);
        } else if (identifier == "vn") {
            Math::Vector3 normal;
            s >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        } else if (identifier == "f") {
            VertexData triangle[3];
            for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
                std::string vertexDefiniton;
                s >> vertexDefiniton;
                std::istringstream v(vertexDefiniton);
                uint32_t elementIndices[3];
                for (int32_t element = 0; element < 3; ++element) {
                    std::string index;
                    std::getline(v, index, '/');
                    elementIndices[element] = std::stoi(index);
                }
                Math::Vector4 position = positions[elementIndices[0] - 1];
                position.x *= -1.0f;
                Math::Vector2 texcoord = texcoords[elementIndices[1] - 1];
                texcoord.y = 1.0f - texcoord.y;
                Math::Vector3 normal = normals[elementIndices[2] - 1];

                triangle[faceVertex] = { position, texcoord, normal }; // normalも追加
            }
            modelData_.vertices.push_back(triangle[2]);
            modelData_.vertices.push_back(triangle[1]);
            modelData_.vertices.push_back(triangle[0]);
        } else if (identifier == "mtllib") {
            std::string materialFilename;
            s >> materialFilename;
            modelData_.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
        }
    }
}


void Model::LoadGltfFile(const std::string& directoryPath, const std::string& filename)
{
    Assimp::Importer importer;

    std::string filePath = directoryPath + "/" + filename;

    std::string baseDirectory = directoryPath;

    if (filename.find('/') != std::string::npos) {
        baseDirectory += "/" + filename.substr(0, filename.find_last_of('/'));
    }

    const aiScene* scene = importer.ReadFile(
        filePath.c_str(),
        aiProcess_Triangulate
    );

    assert(scene);
    assert(scene->HasMeshes());

    modelData_.rootNode = ReadNode(scene->mRootNode);

    skeleton_ = CreateSkeleton(modelData_.rootNode);

    modelData_.vertices.clear();
    modelData_.indices.clear();
    modelData_.skinClusterData.clear();

    // まずテクスチャが取れなかった場合の保険
    modelData_.material.textureFilePath = "resources/uvChecker.png";

    // materialからテクスチャパスを取得
    if (scene->HasMaterials()) {
        aiMaterial* material = scene->mMaterials[0];

        aiString textureFilePath;

        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath) == AI_SUCCESS ||
            material->GetTexture(aiTextureType_BASE_COLOR, 0, &textureFilePath) == AI_SUCCESS) {

            modelData_.material.textureFilePath =
                baseDirectory + "/" + textureFilePath.C_Str();
        }
    }

    for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
        aiMesh* mesh = scene->mMeshes[meshIndex];

        assert(mesh->HasPositions());
        assert(mesh->HasNormals());
        assert(mesh->HasTextureCoords(0));

        uint32_t vertexOffset = static_cast<uint32_t>(modelData_.vertices.size());

        // 頂点数分だけ先に確保
        modelData_.vertices.resize(modelData_.vertices.size() + mesh->mNumVertices);

        // 頂点解析
        for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
            aiVector3D& positionAssimp = mesh->mVertices[vertexIndex];
            aiVector3D& normalAssimp = mesh->mNormals[vertexIndex];
            aiVector3D& texcoordAssimp = mesh->mTextureCoords[0][vertexIndex];

            VertexData& vertex = modelData_.vertices[vertexOffset + vertexIndex];

            vertex.position = {
                -float(positionAssimp.x),
                 float(positionAssimp.y),
                 float(positionAssimp.z),
                 1.0f
            };

            vertex.texcoord = {
                float(texcoordAssimp.x),
                float(texcoordAssimp.y)
            };

            vertex.normal = {
                -float(normalAssimp.x),
                 float(normalAssimp.y),
                 float(normalAssimp.z)
            };
        }

        // Index解析
        for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
            aiFace& face = mesh->mFaces[faceIndex];

            assert(face.mNumIndices == 3);

            for (uint32_t element = 0; element < face.mNumIndices; ++element) {
                uint32_t vertexIndex = face.mIndices[element];

                modelData_.indices.push_back(vertexOffset + vertexIndex);
            }
        }

        // SkinCluster構築用データ取得
        for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
            aiBone* bone = mesh->mBones[boneIndex];

            std::string jointName = bone->mName.C_Str();

            JointWeightData& jointWeightData =
                modelData_.skinClusterData[jointName];

            // AssimpのOffsetMatrixはInverseBindPoseMatrixなので、一度逆行列にしてBindPoseMatrixにする
            aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix;
            bindPoseMatrixAssimp.Inverse();

            aiVector3D scale;
            aiQuaternion rotate;
            aiVector3D translate;

            bindPoseMatrixAssimp.Decompose(scale, rotate, translate);

            Math::Matrix4x4 bindPoseMatrix = Math::MakeAffineMatrix(
                { float(scale.x), float(scale.y), float(scale.z) },
                { float(rotate.x), -float(rotate.y), -float(rotate.z), float(rotate.w) },
                { -float(translate.x), float(translate.y), float(translate.z) }
            );

            jointWeightData.inverseBindPoseMatrix = Math::Inverse(bindPoseMatrix);

            for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
                jointWeightData.vertexWeights.push_back({
                    bone->mWeights[weightIndex].mWeight,
                    vertexOffset + bone->mWeights[weightIndex].mVertexId
                    });
            }
        }
    }
}

Model::Node Model::ReadNode(aiNode* node)
{
    Node result{};

    aiVector3D scale;
    aiQuaternion rotate;
    aiVector3D translate;

    node->mTransformation.Decompose(scale, rotate, translate);

    result.transform.scale = {
        float(scale.x),
        float(scale.y),
        float(scale.z)
    };

    result.transform.rotate = {
         float(rotate.x),
        -float(rotate.y),
        -float(rotate.z),
         float(rotate.w)
    };

    result.transform.translate = {
        -float(translate.x),
         float(translate.y),
         float(translate.z)
    };

    result.localMatrix = Math::MakeAffineMatrix(
        result.transform.scale,
        result.transform.rotate,
        result.transform.translate
    );

    result.name = node->mName.C_Str();

    result.children.resize(node->mNumChildren);

    for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
        result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
    }

    return result;
}

Model::Skeleton Model::CreateSkeleton(const Node& rootNode)
{
    Skeleton skeleton;

    skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

    for (const Joint& joint : skeleton.joints) {
        skeleton.jointMap.emplace(joint.name, joint.index);
    }

    Update(skeleton);

    return skeleton;
}

Model::SkinCluster Model::CreateSkinCluster(
    const Skeleton& skeleton,
    const ModelData& modelData
)
{
    SkinCluster skinCluster;

    DirectXCommon* dxCommon = modelCommon_->GetDxCommon();
    SrvManager* srvManager = modelCommon_->GetSrvManager();

    assert(srvManager);

    // palette用のResourceを確保
    skinCluster.paletteResource = dxCommon->CreateBufferResource(
        sizeof(WellForGPU) * skeleton.joints.size()
    );

    WellForGPU* mappedPalette = nullptr;

    skinCluster.paletteResource->Map(
        0,
        nullptr,
        reinterpret_cast<void**>(&mappedPalette)
    );

    skinCluster.mappedPalette = {
        mappedPalette,
        skeleton.joints.size()
    };

    // 初期値をIdentityにしておく
    for (WellForGPU& palette : skinCluster.mappedPalette) {
        palette.skeletonSpaceMatrix = Math::MakeIdentity4x4();
        palette.skeletonSpaceInverseTransposeMatrix = Math::MakeIdentity4x4();
    }

    // ==============================
    // MatrixPalette用SRVをSrvManagerから確保
    // ==============================
    uint32_t paletteSrvIndex = srvManager->Allocate();

    skinCluster.paletteSrvHandle.first =
        srvManager->GetCPUDescriptorHandle(paletteSrvIndex);

    skinCluster.paletteSrvHandle.second =
        srvManager->GetGPUDescriptorHandle(paletteSrvIndex);

    // palette用のSRVを作成
    D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
    paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
    paletteSrvDesc.Shader4ComponentMapping =
        D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    paletteSrvDesc.Buffer.FirstElement = 0;
    paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    paletteSrvDesc.Buffer.NumElements = UINT(skeleton.joints.size());
    paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);

    dxCommon->GetDevice()->CreateShaderResourceView(
        skinCluster.paletteResource.Get(),
        &paletteSrvDesc,
        skinCluster.paletteSrvHandle.first
    );

    // influence用のResourceを確保。頂点ごとにinfluence情報を追加できるようにする
    skinCluster.influenceResource = dxCommon->CreateBufferResource(
        sizeof(VertexInfluence) * modelData.vertices.size()
    );

    VertexInfluence* mappedInfluence = nullptr;

    skinCluster.influenceResource->Map(
        0,
        nullptr,
        reinterpret_cast<void**>(&mappedInfluence)
    );

    // 0埋め。weightを0にしておく
    std::memset(
        mappedInfluence,
        0,
        sizeof(VertexInfluence) * modelData.vertices.size()
    );

    skinCluster.mappedInfluence = {
        mappedInfluence,
        modelData.vertices.size()
    };

    // influence用のVBVを作成
    skinCluster.influenceBufferView.BufferLocation =
        skinCluster.influenceResource->GetGPUVirtualAddress();

    skinCluster.influenceBufferView.SizeInBytes =
        UINT(sizeof(VertexInfluence) * modelData.vertices.size());

    skinCluster.influenceBufferView.StrideInBytes =
        sizeof(VertexInfluence);

    // InverseBindPoseMatrixを格納する場所を作成
    skinCluster.inverseBindPoseMatrices.resize(skeleton.joints.size());

    std::generate(
        skinCluster.inverseBindPoseMatrices.begin(),
        skinCluster.inverseBindPoseMatrices.end(),
        []() {
            return Math::MakeIdentity4x4();
        }
    );

    // ModelDataを解析してInfluenceを埋める
    for (const auto& jointWeight : modelData.skinClusterData) {

        // modelDataに入っているJoint名が、Skeletonにあるか確認
        auto it = skeleton.jointMap.find(jointWeight.first);

        // Skeletonに存在しないJointなら飛ばす
        if (it == skeleton.jointMap.end()) {
            continue;
        }

        // JointごとのInverseBindPoseMatrixを保存
        skinCluster.inverseBindPoseMatrices[it->second] =
            jointWeight.second.inverseBindPoseMatrix;

        // そのJointが影響する頂点Weightを反映
        for (const auto& vertexWeight : jointWeight.second.vertexWeights) {

            VertexInfluence& currentInfluence =
                skinCluster.mappedInfluence[vertexWeight.vertexIndex];

            // 1頂点につき最大4つまでJointの影響を入れる
            for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {

                // 空いている場所に入れる
                if (currentInfluence.weights[index] == 0.0f) {
                    currentInfluence.weights[index] = vertexWeight.weight;
                    currentInfluence.jointIndices[index] = it->second;
                    break;
                }
            }
        }
    }
    return skinCluster;
}

void Model::Update(Skeleton& skeleton)
{
    // 親が必ず自分より若いindexになるようにCreateJointで追加しているので、
    // 先頭から順番に更新すれば親のskeletonSpaceMatrixは更新済みになる
    for (Joint& joint : skeleton.joints) {

        joint.localMatrix = Math::MakeAffineMatrix(
            joint.transform.scale,
            joint.transform.rotate,
            joint.transform.translate
        );

        if (joint.parent) {
            joint.skeletonSpaceMatrix = Math::Multiply(
                joint.localMatrix,
                skeleton.joints[*joint.parent].skeletonSpaceMatrix
            );
        } else {
            joint.skeletonSpaceMatrix = joint.localMatrix;
        }
    }
}

void Model::Update(SkinCluster& skinCluster, const Skeleton& skeleton)
{
    for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {

        assert(jointIndex < skinCluster.inverseBindPoseMatrices.size());
        assert(jointIndex < skinCluster.mappedPalette.size());

        skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix =
            Math::Multiply(
                skinCluster.inverseBindPoseMatrices[jointIndex],
                skeleton.joints[jointIndex].skeletonSpaceMatrix
            );

        skinCluster.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix =
            Math::Transpose(
                Math::Inverse(skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix)
            );
    }
}

void Model::ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime)
{
    for (Joint& joint : skeleton.joints) {
        if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
            const NodeAnimation& rootNodeAnimation = it->second;

            joint.transform.translate = CalculateValue(rootNodeAnimation.translate, animationTime);
            joint.transform.rotate = CalculateValue(rootNodeAnimation.rotate, animationTime);
            joint.transform.scale = CalculateValue(rootNodeAnimation.scale, animationTime);
        }
    }
}

int32_t Model::CreateJoint(
    const Node& node,
    const std::optional<int32_t>& parent,
    std::vector<Joint>& joints)
{
    Joint joint{};

    joint.name = node.name;
    joint.localMatrix = node.localMatrix;
    joint.skeletonSpaceMatrix = Math::MakeIdentity4x4();
    joint.transform = node.transform;
    joint.index = int32_t(joints.size());
    joint.parent = parent;

    joints.push_back(joint);

    for (const Node& child : node.children) {
        int32_t childIndex = CreateJoint(child, joint.index, joints);
        joints[joint.index].children.push_back(childIndex);
    }

    return joint.index;
}

Model::MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
    MaterialData materialData;
    std::string line;
    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open());
    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;
        if (identifier == "map_Kd" && materialData.textureFilePath.empty()) {
            std::string textureFilename;
            s >> textureFilename;
            materialData.textureFilePath = directoryPath + "/" + textureFilename;
        }
    }
    return materialData;
}

void Model::UpdateSkeleton(const Animation& animation, float animationTime)
{
    // 2. AnimationをSkeletonに適用
    ApplyAnimation(skeleton_, animation, animationTime);

    // 3. Skeletonの行列を更新
    Update(skeleton_);

    // 4. SkinClusterのMatrixPaletteを更新
    Update(skinCluster_, skeleton_);
}
