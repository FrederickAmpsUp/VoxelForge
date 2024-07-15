#include <vforge/xraw_file.hpp>
#include <vforge/vox_file.hpp>
#include <fstream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <unordered_map>
#include <variant>
#include <optional>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

namespace voxelforge::files {

std::shared_ptr<voxelforge::VoxelObject> load_xraw_file(const std::string& filename) {
    std::ifstream file(filename.c_str(), std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return nullptr;
    }

    std::string magic;
    magic.resize(4);

    file.read(magic.data(), magic.size());

    if (magic != "XRAW") {
        std::cerr << "Invalid XRAW file: " << filename << std::endl;
        return nullptr;
    }

    char colorDataType;
    file.read(&colorDataType, sizeof(colorDataType));

    char colorNumChannels;
    file.read(&colorNumChannels, sizeof(colorNumChannels));

    char colorBitsPerChannel;
    file.read(&colorBitsPerChannel, sizeof(colorBitsPerChannel));

    uint32_t dim[3];
    file.read((char *)dim, sizeof(dim));

    uint32_t numColorPalleteColors;
    file.read((char *)&numColorPalleteColors, sizeof(numColorPalleteColors));

    // end of header

    std::vector<uint8_t> voxelBuffer(dim[0] * dim[1] * dim[2]);
    file.read((char *)voxelBuffer.data(), voxelBuffer.size());

    std::vector<uint8_t> palleteBuffer(numColorPalleteColors * colorBitsPerChannel * colorNumChannels);
    file.read((char *)palleteBuffer.data(), palleteBuffer.size());

    auto world = std::make_shared<voxelforge::VoxelObject>(glm::uvec3(
        dim[0] / 16 + 1,
        dim[1] / 16 + 1,
        dim[2] / 16 + 1
    ));

    // TODO
}

struct _VOXFileChunk {
    _VOXFileChunk(std::ifstream& file) {
        this->id.resize(4);
        file.read(this->id.data(), 4);
        file.read((char *)&this->dataSize, sizeof(this->dataSize));
        file.read((char *)&this->numChildren, sizeof(this->numChildren));

        this->data.resize(this->dataSize);
        file.read(this->data.data(), this->dataSize);

        this->children.resize(this->numChildren);
        for (int i = 0; i < this->numChildren; i++) {
            this->children[i] = _VOXFileChunk(file);
        }
    }
    _VOXFileChunk() {}

    std::string id;
    int dataSize;
    int numChildren;

    std::vector<char> data;
    std::vector<_VOXFileChunk> children;
};

using _VOXFileDict = std::unordered_map<std::string, std::string>;

static std::string loadString(std::vector<char>& data, int& start) {
    int len = *(int*)&data[start];
    start += 4;
    int sData = start;
    start += len;
    return std::string(&data[sData], len);
}
static _VOXFileDict loadDict(std::vector<char>& data, int& start) {
    _VOXFileDict d;
    int n = *(int*)&data[start];
    start += 4;
    for (int i = 0; i < n; i++) {
        std::string key = loadString(data, start);
        std::string val = loadString(data, start);
        d[key] = val;
    }
    return d;
} 

struct _VOXFilenTRN {
    _VOXFilenTRN(std::vector<char> chunkData) {
        int i = 0;
        this->nodeID = *(int*)&chunkData[i];
        i += 4;
        this->attribs = loadDict(chunkData, i);
        this->childID = *(int*)&chunkData[i];
        i += 4;
        this->reserved = *(int*)&chunkData[i];
        i += 4;
        this->layerID = *(int*)&chunkData[i];
        i += 4;
        this->nFrames = *(int*)&chunkData[i];
        i += 4;
        this->frameAttribs.resize(this->nFrames);
        for (int f = 0; f < this->nFrames; ++f) {
            this->frameAttribs[f] = loadDict(chunkData, i);
        }
    }
    _VOXFilenTRN() {}

    int nodeID;
    _VOXFileDict attribs;
    int childID;
    int reserved;
    int layerID;
    int nFrames;
    std::vector<_VOXFileDict> frameAttribs;
};

struct _VOXFilenGRP {
    _VOXFilenGRP(std::vector<char> chunkData) {
        int i = 0;
        this->nodeID = *(int*)&chunkData[i];
        i += 4;
        this->attribs = loadDict(chunkData, i);
        this->nChildren = *(int*)&chunkData[i];
        i += 4;
        this->childIDs.resize(this->nChildren);
        for (int c = 0; c < this->nChildren; c++) {
            this->childIDs[c] = *(int*)&chunkData[i];
            i += 4;
        }
    }
    _VOXFilenGRP() {}
    
    int nodeID;
    _VOXFileDict attribs;
    int nChildren;
    std::vector<int> childIDs;
};

struct _VOXFilenSHP {    
    _VOXFilenSHP(std::vector<char> chunkData) {
        int i = 0;
        this->nodeID = *(int*)&chunkData[i];
        i += 4;
        this->attribs = loadDict(chunkData, i);
        this->nModels = *(int*)&chunkData[i];
        i += 4;
        this->modelIDs.resize(this->nModels);
        this->modelAttribs.resize(this->nModels);
        for (int m = 0; m < this->nModels; m++) {
            this->modelIDs[m] = *(int*)&chunkData[i];
            i += 4;
            this->modelAttribs[m] = loadDict(chunkData, i);
        }
    }
    _VOXFilenSHP() {}

    int nodeID;
    _VOXFileDict attribs;
    int nModels;
    std::vector<int> modelIDs;
    std::vector<_VOXFileDict> modelAttribs;
};

using _VOXFileSceneNodeData = std::optional<std::variant<_VOXFilenTRN, _VOXFilenGRP, _VOXFilenSHP>>;

struct _VOXFileModelData {
    _VOXFileModelData(glm::uvec3 size, std::vector<glm::u8vec4> data) : size(size), data(data) {}
    _VOXFileModelData() {
        this->size = glm::uvec3(0);
        this->data.clear();
    }
    glm::uvec3 size;
    std::vector<glm::u8vec4> data;
    bool instanced = false;
};

struct _VOXFileSceneNode;
struct _VOXFileTransformNode;
struct _VOXFileGroupingNode;
struct _VOXFileShapeNode;
class _VOXFileSceneGraphVisitor;

struct _VOXFileSceneNode {
public:
    using Ptr = std::shared_ptr<_VOXFileSceneNode>;

    virtual void add_child(Ptr child) {
        this->children.push_back(child);
    }

    virtual void accept(_VOXFileSceneGraphVisitor *v) = 0;

    Ptr parent;
    std::vector<Ptr> children;
    _VOXFileDict attributes;
};

class _VOXFileSceneGraphVisitor {
public:
    void visit(std::shared_ptr<_VOXFileSceneNode> node) {
        if (!node) return;
        node->accept(this);
    }

    virtual void visit(_VOXFileTransformNode *node) = 0;
    virtual void visit(_VOXFileGroupingNode *node) = 0;
    virtual void visit(_VOXFileShapeNode *node) = 0;
};


struct _VOXFileTransformNode : public _VOXFileSceneNode {
public:
    std::vector<glm::mat4x4> transformFrames;
    int layer;

    virtual void accept(_VOXFileSceneGraphVisitor *v) override {
        v->visit(this);
    }
};

struct _VOXFileGroupingNode : public _VOXFileSceneNode {
public:
    virtual void accept(_VOXFileSceneGraphVisitor *v) override {
        v->visit(this);
    }
};

struct _VOXFileShapeNode : public _VOXFileSceneNode {
public:
    std::vector<int> modelIDs;
    std::vector<_VOXFileDict> modelAttributes;

    virtual void accept(_VOXFileSceneGraphVisitor *v) override {
        v->visit(this);
    }
};

static std::vector<int32_t> parseBCDString(const std::string& input) {
    std::istringstream iss(input);
    std::vector<int32_t> numbers;
    int32_t number;

    while (iss >> number) {
        numbers.push_back(number);
        if (iss.peek() == ' ') {
            iss.ignore();
        }
    }

    return numbers;
}

static glm::mat3 byteToRotationMatrix(unsigned char byte) {
    // Extract the index of the non-zero entries
    int row1_index = (byte & 0b00000011); // bits 0-1
    int row2_index = (byte & 0b00001100) >> 2; // bits 2-3

    // Extract the signs (0: positive, 1: negative)
    int sign1 = (byte & 0b00010000) >> 4; // bit 4
    int sign2 = (byte & 0b00100000) >> 5; // bit 5
    int sign3 = (byte & 0b01000000) >> 6; // bit 6

    // Initialize the rotation matrix with zeros
    glm::mat3 rotationMatrix(0.0f);

    // Set the non-zero entries based on the indices and signs
    rotationMatrix[0][row1_index] = sign1 ? -1.0f : 1.0f;
    rotationMatrix[1][row2_index] = sign2 ? -1.0f : 1.0f;

    // Determine the remaining entry
    int row3_index = 3 - (row1_index + row2_index);
    rotationMatrix[2][row3_index] = sign3 ? -1.0f : 1.0f;

    return rotationMatrix;
}

static _VOXFileSceneNode::Ptr buildSceneGraph(const std::unordered_map<int, _VOXFileSceneNodeData>& nodes, int parentID, _VOXFileSceneNode::Ptr parent = nullptr) {
    if (nodes.find(parentID) == nodes.end()) return nullptr; // no root node
    _VOXFileSceneNodeData root = nodes.at(parentID);

    if (!root.has_value()) return nullptr; // no root node

    auto var = root.value();
    if (std::holds_alternative<_VOXFilenTRN>(var)) {
        const auto& node = std::get<_VOXFilenTRN>(var);

        std::shared_ptr<_VOXFileTransformNode> transform = std::make_shared<_VOXFileTransformNode>();
        transform->parent = parent;

        for (const auto& f : node.frameAttribs) {
            glm::mat4x4 m = glm::identity<glm::mat4x4>();

            if (f.find("_t") != f.end()) {
                std::string translation = f.at("_t");
                std::vector<int32_t> translationValues = parseBCDString(translation);
                if (translationValues.size() == 3) {
                    m = glm::translate(m, glm::vec3((float)translationValues[0] / 16.0f, (float)translationValues[2] / 16.0f, (float)translationValues[1] / 16.0f));
                }
            }
            if (f.find("_r") != f.end()) {
                std::string rotation = f.at("_r");
                std::cout << rotation << std::endl;
                uint8_t b = atoi(rotation.c_str());

                glm::mat3x3 r = byteToRotationMatrix(b);

                glm::mat4 transform = glm::mat4(1.0f);  // Identity matrix
                transform = glm::mat4(r);  // Copy rotation into top-left 3x3
                transform[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);  // Set bottom row to (0, 0, 0, 1)

                //m = transform * m;
            }

                // not to spec, but who cares? noone uses animation in this format anyway...
            transform->transformFrames.push_back(m);
        }

        _VOXFileSceneNode::Ptr child = buildSceneGraph(nodes, node.childID, transform);
        transform->add_child(child);
        transform->attributes = node.attribs;
        transform->layer = node.layerID;

        return transform;
    }
    if (std::holds_alternative<_VOXFilenGRP>(var)) {
        const auto& node = std::get<_VOXFilenGRP>(var);

        std::shared_ptr<_VOXFileGroupingNode> grouping = std::make_shared<_VOXFileGroupingNode>();
        grouping->parent = parent;

        for (int c : node.childIDs) {
            _VOXFileSceneNode::Ptr child = buildSceneGraph(nodes, c, grouping);
            grouping->add_child(child);
        }

        grouping->attributes = node.attribs;

        return grouping;
    }
    if (std::holds_alternative<_VOXFilenSHP>(var)) {
        const auto& node = std::get<_VOXFilenSHP>(var);

        std::shared_ptr<_VOXFileShapeNode> shape = std::make_shared<_VOXFileShapeNode>();
        shape->parent = parent;

        shape->modelIDs = node.modelIDs;
        shape->modelAttributes = node.modelAttribs;

        return shape;
    }
    return nullptr; // ideally unreachable
}

struct _VOXFileSceneGraphPrinter : public _VOXFileSceneGraphVisitor {
public:
    using _VOXFileSceneGraphVisitor::visit;

    virtual void visit(_VOXFileTransformNode *node) override {
        std::cout << "(transform: {";
        for (const auto& m : node->transformFrames) {
            std::cout << glm::to_string(m) << ", ";
        }
        std::cout << "\b\b}, child=";
        this->visit(node->children[0]);
        std::cout << ")";
    }

    virtual void visit(_VOXFileGroupingNode *node) override {
        std::cout << "(grouping: children={";

        for (const auto& c : node->children) {
            this->visit(c);
            std::cout << ", ";
        }
        std::cout << "\b\b})";
    }

    virtual void visit(_VOXFileShapeNode *node) override {
        std::cout << "(shape: {";

        for (int i = 0; i < node->modelIDs.size(); ++i) {
            std::cout << node->modelIDs[i] << ", ";
        }

        std::cout << "\b\b})";
    }
};

struct _VOXFileSceneGraphObjectExtractor : public _VOXFileSceneGraphVisitor {
public:
    using _VOXFileSceneGraphVisitor::visit;

    _VOXFileSceneGraphObjectExtractor(std::vector<_VOXFileModelData>& models) : models(models) {}

    virtual void visit(_VOXFileTransformNode *node) override {
        glm::mat4x4 m = node->transformFrames[0];
        glm::mat4x4 modelOld = this->modelMatrix;
        this->modelMatrix = m * this->modelMatrix;
        this->visit(node->children[0]);
        this->modelMatrix = modelOld;
    }

    virtual void visit(_VOXFileGroupingNode *node) override {
        for (const auto& c : node->children) {
            this->visit(c);
        }
    }

    virtual void visit(_VOXFileShapeNode *node) override {
        int modelID = node->modelIDs[0];
        _VOXFileModelData& model = this->models[modelID];
        glm::uvec3 szChunks = model.size / 16u + 1u;
        auto object = std::make_shared<voxelforge::VoxelObject>(szChunks, this->modelMatrix);

        model.instanced = true;

        for (const auto& vox : model.data) {
            std::shared_ptr<voxelforge::VoxelData> vd = std::make_shared<voxelforge::VoxelData>(glm::vec3(0.0), vox.w);
            object->set(glm::uvec3(vox.x, vox.y, vox.z), vd);
        }

        this->objects.push_back(object);
    }

    std::vector<_VOXFileModelData>& models;
    std::vector<std::shared_ptr<voxelforge::VoxelObject>> objects;
    glm::mat4x4 modelMatrix = glm::identity<glm::mat4x4>();
};

MagicaVoxelVOX::MagicaVoxelVOX(const char *filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string magic;
    magic.resize(4);

    file.read(magic.data(), magic.size());

    if (magic != "VOX ") {
        std::cerr << "Invalid VOX file: " << filename << std::endl;
        return;
    }

    int vers;
    file.read((char *)&vers, sizeof(int));

    _VOXFileChunk mainChunk = _VOXFileChunk(file);
    if (mainChunk.id != "MAIN") {
        std::cerr << "Invalid VOX file: " << filename << " (missing MAIN chunk)" << std::endl;
        return;
    }

    std::vector<_VOXFileModelData> models;
    std::unordered_map<int, _VOXFileSceneNodeData> sceneGraphData;

    glm::vec4 palette[256];

    for (int i = 0; i < mainChunk.children.size(); ++i) {
        const auto& chunk = mainChunk.children[i];
        if (chunk.id == "SIZE") {
            glm::uvec3 size;
            size.x = *(int*)&chunk.data[0];
            size.z = *(int*)&chunk.data[4];
            size.y = *(int*)&chunk.data[8];

            std::cout << glm::to_string(size) << std::endl;

            i++;
            const auto& xyziChunk = mainChunk.children[i];
            if (xyziChunk.id != "XYZI") {
                std::cerr << "Invalid VOX file: " << filename << " (expected XYZI after SIZE chunk)";
                return;
            }

            auto& model = models.emplace_back(size, std::vector<glm::u8vec4>{});
            
            int nVoxels = *(int*)&xyziChunk.data[0];
            for (int j = 0; j < nVoxels; j++) {
                uint8_t x = xyziChunk.data[4 + 4 * j];
                uint8_t z = xyziChunk.data[5 + 4 * j];
                uint8_t y = xyziChunk.data[6 + 4 * j];
                uint8_t i = xyziChunk.data[7 + 4 * j];
                model.data.push_back(glm::u8vec4(x, y, z, i));
            }
        }
        if (chunk.id == "RGBA") {
            for (int i = 0; i < 255; i++) {
                uint8_t r = chunk.data[i * 4 + 0];
                uint8_t g = chunk.data[i * 4 + 1];
                uint8_t b = chunk.data[i * 4 + 2];
                uint8_t a = chunk.data[i * 4 + 3];
                glm::vec4 mat = glm::vec4(r, g, b, a) / 256.0f;
                palette[i + 1] = mat;
            }
        }
        if (chunk.id == "nTRN") {
            _VOXFilenTRN trnNode(chunk.data);
            int i = trnNode.nodeID;
            
            sceneGraphData[i] = trnNode;
        }
        if (chunk.id == "nGRP") {
            _VOXFilenGRP grpNode(chunk.data);
            int i = grpNode.nodeID;

            sceneGraphData[i] = grpNode;
        }
        if (chunk.id == "nSHP") {
            _VOXFilenSHP shpNode(chunk.data);
            int i = shpNode.nodeID;

            sceneGraphData[i] = shpNode;
        }
    }

        // TODO: better root node picking function. this area is completely undocumented in the file format.
    _VOXFileSceneNode::Ptr sceneGraph = buildSceneGraph(sceneGraphData, 0);

    _VOXFileSceneGraphObjectExtractor gen(models);
    gen.visit(sceneGraph);

    this->world = std::make_shared<voxelforge::VoxelWorld>();

    for (auto& object : gen.objects) {
        for (int i = 0; i < 256; i++) {
            object->setMaterial(i, palette[i]);
        }
        this->world->addObject(object);
    }

    for (auto& model : models) {
            // the model is referenced by an nSHP chunk, it doesn't draw at the root
        if (model.instanced) continue;
        std::shared_ptr<voxelforge::VoxelObject> obj = std::make_shared<voxelforge::VoxelObject>(model.size / 16u + 1u);
        for (int i = 0; i < 256; i++) {
            obj->setMaterial(i, palette[i]);
        }
        for (auto v : model.data) {
            std::shared_ptr<voxelforge::VoxelData> vox = std::make_shared<voxelforge::VoxelData>(glm::vec3(0.0), v.w);
            obj->set(glm::uvec3(v.x, v.y, v.z), vox);
        }
        this->world->addObject(obj);
    }
}
}