#include "TMVA/RModelParser_ONNX.hxx"
#include "TMVA/ROperator_BasicBitwise.hxx"
#include "onnx_proto3.pb.h"

namespace TMVA {
namespace Experimental {
namespace SOFIE {

template <EBitwiseOperator Op>
std::unique_ptr<ROperator> ParseBitwise(RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto)
{
   ETensorType input_type = ETensorType::UNDEFINED;

   for (int i = 0; i < 2; ++i) {
      auto input_name = nodeproto.input(i);
      if (parser.IsRegisteredTensorType(input_name)) {
         // According to ONNX, both inputs have the same type
         if (i == 0)
            input_type = parser.GetTensorType(input_name);
         else
            if (input_type != parser.GetTensorType(input_name)) {
               throw std::runtime_error("TMVA::SOFIE ONNX parser Bitwise op has input tensors of different types");
            }
      } else {
         throw std::runtime_error("TMVA::SOFIE ONNX Parser Bitwise op has input tensor " + input_name +
                                  " but its type is not yet registered");
      }
   }

   std::unique_ptr<ROperator> op;
   std::string output_name = nodeproto.output(0);

   switch (input_type) {
   case ETensorType::FLOAT:
      // Replace with appropriate type if required (e.g., uint8, int32, etc.)
      op.reset(new ROperator_Bitwise<float, Op>(nodeproto.input(0), nodeproto.input(1), output_name));
      break;
   default:
      throw std::runtime_error("TMVA::SOFIE - Unsupported - Bitwise Operator does not yet support input type " +
                               std::to_string(static_cast<int>(input_type)));
   }

   // Infer the output type
   if (!parser.IsRegisteredTensorType(output_name)) {
      parser.RegisterTensorType(output_name, input_type);
   }

   return op;
};

// Parse And
ParserFuncSignature ParseAnd = [](RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto) {
   return ParseBitwise<EBitwiseOperator::And>(parser, nodeproto);
};

// Parse Or
ParserFuncSignature ParseOr = [](RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto) {
   return ParseBitwise<EBitwiseOperator::Or>(parser, nodeproto);
};

// Parse Xor
ParserFuncSignature ParseXor = [](RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto) {
   return ParseBitwise<EBitwiseOperator::Xor>(parser, nodeproto);
};

// Parse Nor
ParserFuncSignature ParseNor = [](RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto) {
   return ParseBitwise<EBitwiseOperator::Nor>(parser, nodeproto);
};

} // namespace SOFIE
} // namespace Experimental
} // namespace TMVA
