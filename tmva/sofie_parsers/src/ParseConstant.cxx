#include "TMVA/RModelParser_ONNX.hxx"
#include "TMVA/ROperator_Constant.hxx"
#include "onnx_proto3.pb.h"

namespace TMVA {
namespace Experimental {
namespace SOFIE {

// template<typename A>
ParserFuncSignature ParseConstant = [](RModelParser_ONNX &parser, const onnx::NodeProto &nodeproto) {
   ETensorType input_type;
// there is no input in the docs, only an attribute
   auto input_name = nodeproto.input(0);
   if (parser.IsRegisteredTensorType(input_name)) {
      input_type = parser.GetTensorType(input_name);
   } else {
      throw std::runtime_error("TMVA::SOFIE ONNX Parser Constant op has input tensor" + input_name +
                               " but its type is not yet registered");
   }   
   std::vector<float>attrInput={nodeproto.attribute(0).floats().begin(),nodeproto.attribute(0).floats().end()};
   // auto name=nodeproto.attribute(0).name();
   // A attrInput;
   // if(name=="sparse_value")attrInput=nodeproto.attribute(0).i();
   // else if(name=="value")attrInput=nodeproto.attribute(0).i();
   // else if(name=="value_float")attrInput=nodeproto.attribute(0).i();
   // else if(name=="value_floats")attrInput={nodeproto.attribute(0).floats().begin(), nodeproto.attribute(0).floats().end()}
   // else if(name=="value_int")attrInput=nodeproto.attribute(0).i();
   // else if(name=="value_ints")attrInput={nodeproto.attribute(0).ints().begin(), nodeproto.attribute(0).ints().end()};
   // else if(name=="value_string")attrInput=nodeproto.attribute(0).i();
   // else if(name=="value_strings")attrInput={nodeproto.attribute(0).strings().begin(), nodeproto.attribute(0).strings().end()};

   std::unique_ptr<ROperator> op;
   std::string output_name = nodeproto.output(0);

   switch (input_type) {
   case ETensorType::FLOAT: op.reset(new ROperator_Constant<float>(attrInput, input_name, output_name)); break;
   default:
      throw std::runtime_error("TMVA::SOFIE - Unsupported - Operator Constant does not yet support input type " +
                               std::to_string(static_cast<int>(input_type)));
   }

   if (!parser.IsRegisteredTensorType(output_name)) {
      parser.RegisterTensorType(output_name, input_type);
   }

   return op;
};

} // namespace SOFIE
} // namespace Experimental
} // namespace TMVA
