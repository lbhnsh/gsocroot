#include "TMVA/RModelParser_ONNX.hxx"
#include "TMVA/ROperator_TopK.hxx"
#include "onnx_proto3.pb.h"

namespace TMVA {
namespace Experimental {
namespace SOFIE {

ParserFuncSignature ParseTopK = [](RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto) {
   ETensorType input_type = ETensorType::UNDEFINED;

   auto input_name = nodeproto.input(0);
   if (parser.IsRegisteredTensorType(input_name)) {
      input_type = parser.GetTensorType(input_name);
   } else {
      throw std::runtime_error("TMVA::SOFIE ONNX Parser Shape op has input tensor" + input_name +
                               " but its type is not yet registered");
   }
   auto k=nodeproto.input(1);
   std::vector<int> kElem;
   kElem.push_back(std::stoi(k));

   std::unique_ptr<ROperator> op;

   std::string output_name = nodeproto.output(0);
   int attr_axis = -1;
   int attr_largest = 1;
   int attr_sorted = 1;

   for (int_t i = 0; i < nodeproto.attribute_size(); i++) {
      std::string attribute_name = nodeproto.attribute(i).name();
      if (attribute_name == "axis")
         attr_axis = nodeproto.attribute(i).i();
      if (attribute_name == "largest")
         attr_largest = nodeproto.attribute(i).i();
      if (attribute_name == "sorted")
         attr_sorted = nodeproto.attribute(i).i();
   }

   op.reset(new ROperator_TopK<float>(attr_axis, attr_largest, attr_sorted, kElem, input_name, output_name));

   if (!parser.IsRegisteredTensorType(output_name)) {
      parser.RegisterTensorType(output_name, input_type);
   }

   return op;
};

} // namespace SOFIE
} // namespace Experimental
} // namespace TMVA