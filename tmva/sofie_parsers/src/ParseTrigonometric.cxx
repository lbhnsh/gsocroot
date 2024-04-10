#include "TMVA/RModelParser_ONNX.hxx"
#include "TMVA/ROperator_Trigonometric.hxx"
#include "onnx_proto3.pb.h"

namespace TMVA {
namespace Experimental {
namespace SOFIE {

template <EBasicBinaryOperator Op>
std::unique_ptr<ROperator> ParseTrigonometric(RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto) {
   ETensorType input_type;

   auto input_name = nodeproto.input(0);
   if (parser.IsRegisteredTensorType(input_name)) {
      input_type = parser.GetTensorType(input_name);
   } else {
      throw std::runtime_error("TMVA::SOFIE ONNX Parser Trigonometric op has input tensor" + input_name +
                               " but its type is not yet registered");
   }

   std::unique_ptr<ROperator> op;
   std::string output_name = nodeproto.output(0);

   switch (input_type) {
   case ETensorType::FLOAT: 
        op.reset(new ROperator_Trigonometric<float,Op>(input_name, output_name)); 
        break;
   default:
      throw std::runtime_error("TMVA::SOFIE - Unsupported - Operator Trigonometric Op does not yet support input type " +
                               std::to_string(static_cast<int>(input_type)));
   }

   if (!parser.IsRegisteredTensorType(output_name)) {
      parser.RegisterTensorType(output_name, input_type);
   }

   return op;
};

// ParseSin
ParserFuncSignature ParseSin = [](RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto) {
   return ParseTrigonometric<ETrigonometricOperator::Sin>(parser, nodeproto);
};

// ParseSinh
ParserFuncSignature ParseSinh = [](RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto) {
  return ParseTrigonometric<ETrigonometricOperator::Sinh>(parser, nodeproto);
};

// ParseAsin
ParserFuncSignature ParseAsin = [](RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto) {
  return ParseTrigonometric<ETrigonometricOperator::Asin>(parser, nodeproto);
};

// ParseAsinh
ParserFuncSignature ParseAsinh = [](RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto) {
  return ParseTrigonometric<ETrigonometricOperator::Asinh>(parser, nodeproto);
};

// ParseCos
ParserFuncSignature ParseCos = [](RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto) {
  return ParseTrigonometric<ETrigonometricOperator::Cos>(parser, nodeproto);
};

// ParseCosh
ParserFuncSignature ParseCosh = [](RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto) {
  return ParseTrigonometric<ETrigonometricOperator::Cosh>(parser, nodeproto);
};

// ParseAcos
ParserFuncSignature ParseAcos = [](RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto) {
  return ParseTrigonometric<ETrigonometricOperator::Acos>(parser, nodeproto);
};

// ParseAcosh
ParserFuncSignature ParseAcosh = [](RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto) {
  return ParseTrigonometric<ETrigonometricOperator::Acosh>(parser, nodeproto);
};

// ParseTan 
ParserFuncSignature ParseTan = [](RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto) {
  return ParseTrigonometric<ETrigonometricOperator::Tan>(parser, nodeproto);
};

// ParseTanh
ParserFuncSignature ParseTanh = [](RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto) {
  return ParseTrigonometric<ETrigonometricOperator::Tanh>(parser, nodeproto);
};

// ParseAtan
ParserFuncSignature ParseAtan = [](RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto) {
  return ParseTrigonometric<ETrigonometricOperator::Atan>(parser, nodeproto);
};

// ParseAtanh
ParserFuncSignature ParseAtanh = [](RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto) {
  return ParseTrigonometric<ETrigonometricOperator::Atanh>(parser, nodeproto);
};

} // namespace SOFIE
} // namespace Experimental
} // namespace TMVA
