#ifndef TMVA_SOFIE_ROperator_Bitwise
#define TMVA_SOFIE_ROperator_Bitwise

#include "TMVA/SOFIE_common.hxx"
#include "TMVA/ROperator.hxx"
#include "TMVA/RModel.hxx"

#include <sstream>

namespace TMVA {
namespace Experimental {
namespace SOFIE {

enum EBitwiseOperator { And, Or, Xor, Nor };

template <typename T, EBitwiseOperator Op>
struct BitwiseOperatorTrait {};

template <typename T>
struct BitwiseOperatorTrait<T, And> {
   static const std::string Name() { return "And"; }
   static std::string Op(const std::string& t1, const std::string t2) { return t1 + " & " + t2; }
};

template <typename T>
struct BitwiseOperatorTrait<T, Or> {
   static const std::string Name() { return "Or"; }
   static std::string Op(const std::string& t1, const std::string t2) { return t1 + " | " + t2; }
};

template <typename T>
struct BitwiseOperatorTrait<T, Xor> {
   static const std::string Name() { return "Xor"; }
   static std::string Op(const std::string& t1, const std::string t2) { return t1 + " ^ " + t2; }
};

template <typename T>
struct BitwiseOperatorTrait<T, Nor> {
   static const std::string Name() { return "Nor"; }
   static std::string Op(const std::string& t1, const std::string t2) { return "(~(" + t1 + " | " + t2 + "))"; }
};

template<typename T, EBitwiseOperator Op>
class ROperator_Bitwise final : public ROperator {
private:
   std::string fNA;
   std::string fNB;
   std::string fNY;

   std::vector<size_t> fShapeA;
   std::vector<size_t> fShapeB;
   std::vector<size_t> fShapeY;

public:
   ROperator_Bitwise(){}
   ROperator_Bitwise(std::string nameA, std::string nameB, std::string nameY):
      fNA(UTILITY::Clean_name(nameA)), fNB(UTILITY::Clean_name(nameB)), fNY(UTILITY::Clean_name(nameY)){}

   // type of output given input
   std::vector<ETensorType> TypeInference(std::vector<ETensorType> input) override {
      return input;
   }

   // shape of output tensors given input tensors
   std::vector<std::vector<size_t>> ShapeInference(std::vector<std::vector<size_t>> input) override {
      // Assuming inputs have the same shape (no broadcasting)
      return std::vector<std::vector<size_t>>(1, input[0]);
   }

   void Initialize(RModel& model) override {
      if (!model.CheckIfTensorAlreadyExist(fNA)){
         throw std::runtime_error(std::string("TMVA SOFIE Bitwise Op Input Tensor ") + fNA + " is not found in model");
      }
      if (!model.CheckIfTensorAlreadyExist(fNB)) {
         throw std::runtime_error(std::string("TMVA SOFIE Bitwise Op Input Tensor ") + fNB + " is not found in model");
      }

      fShapeA = model.GetTensorShape(fNA);
      fShapeB = model.GetTensorShape(fNB);

      bool broadcast = !UTILITY::AreSameShape(fShapeA, fShapeB);
      if (broadcast) {
         fShapeY = UTILITY::UnidirectionalBroadcastShape(fShapeA, fShapeB);
         bool broadcastA = !UTILITY::AreSameShape(fShapeA, fShapeY);
         bool broadcastB = !UTILITY::AreSameShape(fShapeB, fShapeY);

         if (broadcastA) {
            if (model.IsInitializedTensor(fNA)) {
               auto data = model.GetInitializedTensorData(fNA);
               std::shared_ptr<void> broadcastedData(
                  UTILITY::UnidirectionalBroadcast<float>(static_cast<float *>(data.get()), fShapeA, fShapeY),
                  std::default_delete<float[]>());
               model.UpdateInitializedTensor(fNA, model.GetTensorType(fNA), fShapeY, broadcastedData);
            } else {
               std::string broadcastedA = "Broadcasted" + fNA;
               model.AddIntermediateTensor(broadcastedA, model.GetTensorType(fNA), fShapeY);
               fNA = broadcastedA;
            }
         }

         if (broadcastB) {
            if (model.IsInitializedTensor(fNB)) {
               auto data = model.GetInitializedTensorData(fNB);
               std::shared_ptr<void> broadcastedData(
                  UTILITY::UnidirectionalBroadcast<float>(static_cast<float *>(data.get()), fShapeB, fShapeY),
                  std::default_delete<float[]>());
               model.UpdateInitializedTensor(fNB, model.GetTensorType(fNB), fShapeY, broadcastedData);
            } else {
               std::string broadcastedB = "Broadcasted" + fNB;
               model.AddIntermediateTensor(broadcastedB, model.GetTensorType(fNB), fShapeY);
               fNB = broadcastedB;
            }
         }
      } else {
         fShapeY = fShapeA; // Or fShapeB, as both are the same
      }
      model.AddIntermediateTensor(fNY, model.GetTensorType(fNA), fShapeY);
   }

   std::string GenerateInitCode() override {
      std::stringstream out;
      return out.str();
   }

   std::string Generate(std::string OpName) override {
      OpName = "op_" + OpName;

      if (fShapeY.empty()) {
         throw std::runtime_error("TMVA SOFIE Binary Op called to Generate without being initialized first");
      }
      std::stringstream out;
      // Broadcast A if it's uninitialized
      if (!fNBroadcadstedA.empty()) {
         out << SP << "// Broadcasting uninitialized tensor " << fNA << "\n";
         out << SP << "{\n";
         out << SP << SP << "float* data = TMVA::Experimental::SOFIE::UTILITY::UnidirectionalBroadcast<float>(tensor_" << fNA << ", " << ConvertShapeToString(fShapeA) << ", " << ConvertShapeToString(fShapeY) << ");\n";
         out << SP << SP << "std::copy(data, data + " << length << ", tensor_" << fNBroadcadstedA << ");\n";
         out << SP << SP << "delete[] data;\n";
         out << SP << "}\n";
      }
      // Broadcast B if it's uninitialized
      if (!fNBroadcadstedB.empty()) {
         out << SP << "// Broadcasting uninitialized tensor " << fNB << "\n";
         out << SP << "{\n";
         out << SP << SP << "float* data = TMVA::Experimental::SOFIE::UTILITY::UnidirectionalBroadcast<float>(tensor_" << fNB << ", " << ConvertShapeToString(fShapeB) << ", " << ConvertShapeToString(fShapeY) << ");\n";
         out << SP << SP << "std::copy(data, data + " << length << ", tensor_" << fNBroadcadstedB << ");\n";
         out << SP << SP << "delete[] data;\n";
         out << SP << "}\n";
      }
      const std::string& nameA = fNBroadcadstedA.empty()? fNA : fNBroadcadstedA;
      const std::string& nameB = fNBroadcadstedB.empty()? fNB : fNBroadcadstedB;

      out << SP << "\n//------ " << BitwiseOperatorTrait<T, Op>::Name() << "\n";
      size_t length = ConvertShapeToLength(fShapeY);

      out << SP << "for (size_t id = 0; id < " << length << " ; id++){\n";
      out << SP << SP << "tensor_" << fNY << "[id] = "
          << BitwiseOperatorTrait<T, Op>::Op("tensor_" + fNA + "[id]", "tensor_" + fNB + "[id]") << ";\n";
      out << SP << "}\n";
      return out.str();
   }
};

}//SOFIE
}//Experimental
}//TMVA

#endif //TMVA_SOFIE_ROperator_Bitwise
