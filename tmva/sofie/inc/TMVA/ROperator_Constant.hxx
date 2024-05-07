#ifndef TMVA_SOFIE_ROPERATOR_Constant
#define TMVA_SOFIE_ROPERATOR_Constant

#include "TMVA/SOFIE_common.hxx"
#include "TMVA/ROperator.hxx"
#include "TMVA/RModel.hxx"

#include <sstream>

namespace TMVA{
namespace Experimental{
namespace SOFIE{

template <typename T>
class ROperator_Constant final : public ROperator
{

private:

   std::string fNX;
   std::string fNY;
   // A fAttrValue_;
   std::vector<float> fAttrValue_;
   std::vector<size_t> fShape;

public:
   ROperator_Constant(){}
   ROperator_Constant(std::vector<float> attrInput, std::string nameX, std::string nameY):
      fAttrValue_(attrInput), fNX(UTILITY::Clean_name(nameX)), fNY(UTILITY::Clean_name(nameY)){}

   std::vector<ETensorType> TypeInference(std::vector<ETensorType> input){
      return input;
   }

   std::vector<std::vector<size_t>> ShapeInference(std::vector<std::vector<size_t>> input){
      auto ret = input; //suggest copy to compiler
      return ret;
   }

   void Initialize(RModel& model){
       //input must be a graph input, or already initialized intermediate tensor
      if (model.CheckIfTensorAlreadyExist(fNX) == false){
        throw std::runtime_error("TMVA SOFIE Constant Op Input Tensor is not found in model");
      }
      fShape = model.GetTensorShape(fNX);
      model.AddIntermediateTensor(fNY, model.GetTensorType(fNX), fShape);
      
      float* dataPtr = fAttrValue_.data(); // Get a pointer to the vector's data

    // Create a shared_ptr<float> to manage the attribute data
      std::shared_ptr<float> attrData(dataPtr, [](float* ptr) {
        // No deletion needed here since we don't own the data
        // The vector fAttrValue_ manages the data's lifetime
      });

      model.UpdateInitializedTensor(fNX, model.GetTensorType(fNX), fShape, attrData);
   }
   
   std::string Generate(std::string OpName){
      OpName = "op_" + OpName;
      if (fShape.empty()) {
         throw std::runtime_error("TMVA SOFIE Constant operator called to Generate without being initialized first");
      }
      std::stringstream out;
      size_t length = ConvertShapeToLength(fShape);
      out << "\n//------ Constant\n";
      out << SP << "for (int id = 0; id < " << length << " ; id++){\n";
      out << SP << SP << "tensor_" << fNY << "[id] = tensor_" << fNX << "[id];\n";
      out << SP << "}\n";
      return out.str();
   }
};

}//SOFIE
}//Experimental
}//TMVA


#endif //TMVA_SOFIE_ROPERATOR_Constant
