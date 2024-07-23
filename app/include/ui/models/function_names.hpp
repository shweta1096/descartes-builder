#pragma once

#include <QString>

#include <QtUtility/data/constexpr_qstring>

using ConstLatin1String = QtUtility::data::ConstLatin1String;

namespace processor_function {

constexpr ConstLatin1String PROCESSOR = ""; // the first input must be a function
constexpr ConstLatin1String SPLIT_DATA = "split_data";

} // namespace processor_function

namespace coder_function {

constexpr ConstLatin1String TRANSFORM_DATA = "xform_data";

} // namespace coder_function

namespace trainer_function {

constexpr ConstLatin1String BASIC_TRAINER = "basic_trainer";

} // namespace trainer_function