#include <folly/dynamic.h>
#include <react/renderer/graphics/Transform.h>
#define _USE_MATH_DEFINES
#include <cmath>

namespace rnoh {
    facebook::react::Transform applyTransformOperation(facebook::react::Transform const &transform, std::string const &property, double value) {
        facebook::react::Transform operation;
        if (property == "translateX") {
            operation = facebook::react::Transform::Translate(value, 0, 0);
        } else if (property == "translateY") {
            operation = facebook::react::Transform::Translate(0, value, 0);
        } else if (property == "scale") {
            operation = facebook::react::Transform::Scale(value, value, 1);
        } else if (property == "scaleX") {
            operation = facebook::react::Transform::Scale(value, 1, 1);
        } else if (property == "scaleY") {
            operation = facebook::react::Transform::Scale(1, value, 1);
        } else if (property == "rotate") {
            operation = facebook::react::Transform::Rotate(0, 0, value);
        } else if (property == "rotateX") {
            operation = facebook::react::Transform::Rotate(value, 0, 0);
        } else if (property == "rotateY") {
            operation = facebook::react::Transform::Rotate(0, value, 0);
        } else if (property == "rotateZ") {
            operation = facebook::react::Transform::Rotate(0, 0, value);
        } else if (property == "perspective") {
            operation = facebook::react::Transform::Perspective(value);
        } else if (property == "skewX") {
            operation = facebook::react::Transform::Skew(value, 0);
        } else if (property == "skewY") {
            operation = facebook::react::Transform::Skew(0, value);
        } else {
            throw std::runtime_error("Unsupported transform property " + property);
        }
        return transform * operation;
    }

    double convertStringAngleValueToRadians(std::string angle) {
        auto unit = angle.substr(angle.length() - 3);
        auto value = std::stod(angle.substr(0, angle.length() - 3));
        if (unit == "rad") {
            return value;
        } else if (unit == "deg") {
            return (value * M_PI) / 180;
        }

        throw std::invalid_argument("Angle must be a value in radians or degrees");
    }

    folly::dynamic parseTransform(folly::dynamic &dynamicTransform){
        facebook::react::Transform resultTransform = facebook::react::Transform::Identity();
        for (auto &singleTransform : dynamicTransform) {
            for (auto &item : singleTransform.items()) {
                auto transformType = item.first.getString();
                if (transformType == "matrix") {
                    auto matrixTransform = facebook::react::Transform::Identity();
                    if (item.second.size() == 16) {
                        for (size_t i = 0; i < 16; ++i) {
                            matrixTransform.matrix[i] = item.second[i].getDouble();
                        }
                    }
                    resultTransform = resultTransform * matrixTransform;
                } else {
                    double transformValue;
                    if (item.second.isString()) {
                        try {
                            transformValue = convertStringAngleValueToRadians(item.second.getString());
                        } catch (std::invalid_argument &e) {
                            // ignore incorrect values
                            continue;
                        }
                    } else if (item.second.isDouble()) {
                        transformValue = item.second.getDouble();
                    } else {
                        // ignore incorrect values
                        continue;
                    }
                    resultTransform = applyTransformOperation(resultTransform, transformType, transformValue);
                }
            }
        }
        auto const &matrix = resultTransform.matrix;
        return folly::dynamic::array(matrix.begin(), matrix.end());
    }
}