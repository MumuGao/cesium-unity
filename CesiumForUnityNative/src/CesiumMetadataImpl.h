#pragma once

#include <CesiumGltf/AccessorView.h>
#include <CesiumGltf/ExtensionMeshPrimitiveExtFeatureMetadata.h>
#include <CesiumGltf/ExtensionModelExtFeatureMetadata.h>
#include <CesiumGltf/MetadataPropertyView.h>
#include <CesiumGltf/Model.h>

#include <DotNet/System/String.h>

#include <unordered_map>

namespace DotNet::CesiumForUnity {
class CesiumMetadata;
}

namespace DotNet::UnityEngine {
class Transform;
}

namespace CesiumForUnityNative {

using ValueType = std::variant<
    int8_t,
    uint8_t,
    int16_t,
    uint16_t,
    int32_t,
    uint32_t,
    int64_t,
    uint64_t,
    float,
    double,
    bool,
    std::string_view,
    CesiumGltf::MetadataArrayView<int8_t>,
    CesiumGltf::MetadataArrayView<uint8_t>,
    CesiumGltf::MetadataArrayView<int16_t>,
    CesiumGltf::MetadataArrayView<uint16_t>,
    CesiumGltf::MetadataArrayView<int32_t>,
    CesiumGltf::MetadataArrayView<uint32_t>,
    CesiumGltf::MetadataArrayView<int64_t>,
    CesiumGltf::MetadataArrayView<uint64_t>,
    CesiumGltf::MetadataArrayView<float>,
    CesiumGltf::MetadataArrayView<double>,
    CesiumGltf::MetadataArrayView<bool>,
    CesiumGltf::MetadataArrayView<std::string_view>>;

using PropertyType = std::variant<
    CesiumGltf::MetadataPropertyView<int8_t>,
    CesiumGltf::MetadataPropertyView<uint8_t>,
    CesiumGltf::MetadataPropertyView<int16_t>,
    CesiumGltf::MetadataPropertyView<uint16_t>,
    CesiumGltf::MetadataPropertyView<int32_t>,
    CesiumGltf::MetadataPropertyView<uint32_t>,
    CesiumGltf::MetadataPropertyView<int64_t>,
    CesiumGltf::MetadataPropertyView<uint64_t>,
    CesiumGltf::MetadataPropertyView<float>,
    CesiumGltf::MetadataPropertyView<double>,
    CesiumGltf::MetadataPropertyView<bool>,
    CesiumGltf::MetadataPropertyView<std::string_view>,
    CesiumGltf::MetadataPropertyView<CesiumGltf::MetadataArrayView<int8_t>>,
    CesiumGltf::MetadataPropertyView<CesiumGltf::MetadataArrayView<uint8_t>>,
    CesiumGltf::MetadataPropertyView<CesiumGltf::MetadataArrayView<int16_t>>,
    CesiumGltf::MetadataPropertyView<CesiumGltf::MetadataArrayView<uint16_t>>,
    CesiumGltf::MetadataPropertyView<CesiumGltf::MetadataArrayView<int32_t>>,
    CesiumGltf::MetadataPropertyView<CesiumGltf::MetadataArrayView<uint32_t>>,
    CesiumGltf::MetadataPropertyView<CesiumGltf::MetadataArrayView<int64_t>>,
    CesiumGltf::MetadataPropertyView<CesiumGltf::MetadataArrayView<uint64_t>>,
    CesiumGltf::MetadataPropertyView<CesiumGltf::MetadataArrayView<float>>,
    CesiumGltf::MetadataPropertyView<CesiumGltf::MetadataArrayView<double>>,
    CesiumGltf::MetadataPropertyView<CesiumGltf::MetadataArrayView<bool>>,
    CesiumGltf::MetadataPropertyView<
        CesiumGltf::MetadataArrayView<std::string_view>>>;

using AccessorType = std::variant<
    CesiumGltf::AccessorView<CesiumGltf::AccessorTypes::SCALAR<int8_t>>,
    CesiumGltf::AccessorView<CesiumGltf::AccessorTypes::SCALAR<uint8_t>>,
    CesiumGltf::AccessorView<CesiumGltf::AccessorTypes::SCALAR<int16_t>>,
    CesiumGltf::AccessorView<CesiumGltf::AccessorTypes::SCALAR<uint16_t>>,
    CesiumGltf::AccessorView<CesiumGltf::AccessorTypes::SCALAR<uint32_t>>,
    CesiumGltf::AccessorView<CesiumGltf::AccessorTypes::SCALAR<float>>>;

static std::string
getString(const ValueType& value, const std::string& defaultValue) {
  return std::visit(
      [defaultValue](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (CesiumGltf::IsMetadataNumeric<T>::value) {
          return std::to_string(arg);
        } else if constexpr (CesiumGltf::IsMetadataBoolean<T>::value) {
          return arg ? std::string("true") : std::string("false");
        } else if constexpr (CesiumGltf::IsMetadataString<T>::value) {
          return std::string(arg);
        } else {
          return defaultValue;
        }
      },
      value);
}

class CesiumMetadataImpl {
public:
  ~CesiumMetadataImpl();
  CesiumMetadataImpl(const DotNet::CesiumForUnity::CesiumMetadata& metadata){};
  void JustBeforeDelete(const DotNet::CesiumForUnity::CesiumMetadata& metadata);
  void loadMetadata(
      const CesiumGltf::Model& model,
      const CesiumGltf::ExtensionModelExtFeatureMetadata& modelMetadata);

  void loadMetadata(
      const DotNet::CesiumForUnity::CesiumMetadata& metadata,
      const DotNet::UnityEngine::Transform& transform,
      int triangleIndex);

  int getNumberOfProperties(
      const DotNet::CesiumForUnity::CesiumMetadata& metadata) {
    return _currentMetadataValues.size();
  }

  DotNet::System::String
  getKey(const DotNet::CesiumForUnity::CesiumMetadata& metadata, int index) {
    if (index < _currentMetadataValues.size()) {
      return DotNet::System::String(
          _currentMetadataValues[index].first.c_str());
    }
    return DotNet::System::String("");
  }

  DotNet::System::String getValueAsString(
      const DotNet::CesiumForUnity::CesiumMetadata& metadata,
      int index) {
    if (index < _currentMetadataValues.size()) {
      return DotNet::System::String(
          std::visit(
              [](auto value) { return getString(value, "not implemented"); },
              _currentMetadataValues[index].second)
              .c_str());
    }
    return DotNet::System::String("");
  }

private:
  std::unordered_map<std::string, std::unordered_map<std::string, PropertyType>>
      _featureTables;
  std::vector<std::pair<
      AccessorType,
      std::vector<std::pair<std::string, AccessorType>>>>
      _featureIDs;
  std::vector<std::pair<std::string, ValueType>> _currentMetadataValues;
};
} // namespace CesiumForUnityNative
